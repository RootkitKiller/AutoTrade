//
// Created by mooninwater on 2018/9/17.
//

#include "BiboxExchange.h"

std::shared_ptr<std::vector<std::string>> BiboxExchange::print_market_list() {
    p_client->request(methods::GET,"/v1/mdata?cmd=pairList").then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            auto list_array=json_result["result"].as_array();
            for(auto pair:list_array){
                //std::cout<<pair["id"]<<std::endl;
                //std::cout<<pair["pair"]<<std::endl;
                p_pair_list->push_back(pair["pair"].as_string());
            }
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();
    return p_pair_list;
}

double BiboxExchange::print_pair_rate(const std::string pair_str) {
    std::string fullstr="/v1/mdata?cmd=market&pair="+pair_str;
    p_client->request(methods::GET,fullstr).then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            if(json_result["result"]["last"].is_string()==true){
	    	auto rate_str=json_result["result"]["last"].as_string();
	    	current_pair_rate=atof(rate_str.c_str());
	    }else{
		current_pair_rate=0;
	    }
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();
    return current_pair_rate;
}

std::pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>>
BiboxExchange::print_pair_depth(const std::string pair_str) {

    std::string fullstr="v1/mdata?cmd=depth&pair="+pair_str;
    fullstr+="&size=10";
    p_client->request(methods::GET,fullstr).then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_ret=previousTask.get();
            auto json_result=json_ret["result"];
	    p_asks_depth.reset(new std::vector<Depth>);
            p_bids_depth.reset(new std::vector<Depth>);
            if(json_result["asks"].is_array()== true &&
               json_result["bids"].is_array()== true) {
                auto asks_array = json_result["asks"].as_array();
                auto bids_array = json_result["bids"].as_array();
                for (auto iter_asks_value:asks_array) {
                    auto obj = Depth(atof(iter_asks_value["price"].as_string().c_str()),
                                     atof(iter_asks_value["volume"].as_string().c_str()));
                    p_asks_depth->push_back(obj);
                    //std::cout<<iter_asks_value[0].as_string()<<std::endl;
                }
                for (auto iter_bids_value:bids_array) {
                    auto obj = Depth(atof(iter_bids_value["price"].as_string().c_str()),
                                     atof(iter_bids_value["volume"].as_string().c_str()));
                    p_bids_depth->push_back(obj);
                }
            }
        }
        catch (http_exception const & e){
            std::cout << pair_str<<" 交易对发生异常，检查是否存在该交易对"<<e.what() << std::endl;
        }
    }).wait();
    return std::make_pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>> \
                    (std::move(p_asks_depth),std::move(p_bids_depth));

}

void BiboxExchange::send_to_market(const Trade &trade_data) {

    //1 构建请求参数
    json::value body;
    body["pair"]=json::value::string(trade_data.pair_str);
    body["account_type"]=json::value::number(0);
    body["order_type"]=json::value::number(2);
    body["order_side"]=json::value::number((trade_data.trade_mode)==0?1:2);
    body["pay_bix"]=json::value::number(0);
    body["price"]=json::value::number(trade_data.trade_rate);
    body["amount"]=json::value::number(trade_data.trade_number);
    body["money"]=json::value::number(trade_data.trade_number*trade_data.trade_rate);
    json::value cmd_data;
    cmd_data["cmd"]=json::value::string("orderpending/trade");
    static int rand_number=0;
    cmd_data["index"]=json::value::number(rand_number);
    rand_number++;
    cmd_data["body"]=body;

    auto cmd_str_data=cmd_data.serialize();
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_md5(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(cmd_str_data.c_str()), cmd_str_data.size());        // input is OK; &input is WRONG !!!
    unsigned char* pEncode_buffer = new unsigned char[EVP_MAX_MD_SIZE];
    uint32_t buffer_length=0;
    HMAC_Final(&ctx, pEncode_buffer, &buffer_length);
    HMAC_CTX_cleanup(&ctx);

    //std::cout<<pEncode_buffer<<std::endl;
    //encode to degist
    char buf[33];
    for (int i=0; i<16; i++)
        sprintf(buf+i*2, "%02x", pEncode_buffer[i]);
    buf[32]=0;
    std::string pBuffer(buf);
    std::cout<<pBuffer<<std::endl;


    //3 买入/卖出
    std::string trade_uri="/v1/orderpending";
    http_request curr_request(methods::POST);
    curr_request.set_request_uri(trade_uri);
    json::value request_body;
    request_body["cmds"]=cmd_data;
    request_body["apikey"]=json::value::string(AccessKeyId);
    request_body["sign"]=json::value::string(pBuffer);
    curr_request.set_body(request_body.serialize());

    p_client->request(curr_request).then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            std::cout<<json_result<<std::endl;
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();

    delete[] pEncode_buffer;
}
