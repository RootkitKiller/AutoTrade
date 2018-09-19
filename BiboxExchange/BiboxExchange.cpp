//
// Created by mooninwater on 2018/9/17.
//

#include "BiboxExchange.h"

void BiboxExchange::get_market_list(json::value json_result) {
    auto list_array=json_result["result"].as_array();
    for(auto pair:list_array){
        p_pair_list->push_back(pair["pair"].as_string());
    }
}

std::shared_ptr<std::vector<std::string>> BiboxExchange::print_market_list() {
    /*
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
    }).wait();*/
    http_request requests(methods::GET);
    requests.set_request_uri("/v1/mdata?cmd=pairList");

    callFunction get_result=std::bind(&BiboxExchange::get_market_list,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);
    return p_pair_list;
}

void BiboxExchange::get_pair_rate(json::value json_result) {

    if(json_result["result"]["last"].is_string()==true){
        auto rate_str=json_result["result"]["last"].as_string();
        current_pair_rate=atof(rate_str.c_str());
    }else{
        current_pair_rate=0;
    }
}

double BiboxExchange::print_pair_rate(const std::string pair_str) {

    /*
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
    }).wait();*/
    http_request requests(methods::GET);
    std::string fullstr="/v1/mdata?cmd=market&pair="+pair_str;
    requests.set_request_uri(fullstr);

    callFunction get_result=std::bind(&BiboxExchange::get_pair_rate,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);
    return current_pair_rate;
}

void BiboxExchange::get_pair_depth(json::value json_result) {
    auto json_res=json_result["result"];
    p_asks_depth.reset(new std::vector<Depth>);
    p_bids_depth.reset(new std::vector<Depth>);
    if(json_res["asks"].is_array()== true &&
            json_res["bids"].is_array()== true) {
        auto asks_array = json_res["asks"].as_array();
        auto bids_array = json_res["bids"].as_array();
        for (auto iter_asks_value:asks_array) {
            auto asks_depth = Depth(atof(iter_asks_value["price"].as_string().c_str()),
                             atof(iter_asks_value["volume"].as_string().c_str()));
            p_asks_depth->push_back(asks_depth);
        }
        for (auto iter_bids_value:bids_array) {
            auto bids_depth = Depth(atof(iter_bids_value["price"].as_string().c_str()),
                             atof(iter_bids_value["volume"].as_string().c_str()));
            p_bids_depth->push_back(bids_depth);
        }
    }
}

std::pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>>
BiboxExchange::print_pair_depth(const std::string pair_str) {

    std::string fullstr="v1/mdata?cmd=depth&pair="+pair_str;
    fullstr+="&size=10";
    /*
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
     */
    http_request requests(methods::GET);
    requests.set_request_uri(fullstr);

    callFunction get_result=std::bind(&BiboxExchange::get_pair_depth,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return std::make_pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>> \
                    (std::move(p_asks_depth),std::move(p_bids_depth));

}

void BiboxExchange::get_trade_result(json::value json_result) {
    std::cout<<json_result<<std::endl;
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

    auto cmd_str_data="["+cmd_data.serialize();
    cmd_str_data=cmd_str_data+"]";
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_md5(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(cmd_str_data.c_str()), cmd_str_data.size());
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
    //request_body["cmds"]=cmd_data;
    request_body["cmds"]=json::value::string(cmd_str_data);
    request_body["apikey"]=json::value::string(AccessKeyId);
    request_body["sign"]=json::value::string(pBuffer);
    curr_request.set_body(request_body);



    callFunction get_result=std::bind(&BiboxExchange::get_trade_result,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,curr_request,get_result);

    delete[] pEncode_buffer;
}

void BiboxExchange::get_balance(json::value json_result) {
    std::cout<<json_result<<std::endl;
}

double BiboxExchange::print_balance(const std::string symbol) {

    //1 构建请求参数
    json::value body;
    body["select"]=json::value::number(1);

    json::value cmd_data;
    cmd_data["cmd"]=json::value::string("transfer/assets");

    cmd_data["body"]=body;

    auto cmd_str_data="["+cmd_data.serialize();
    cmd_str_data=cmd_str_data+"]";
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_md5(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(cmd_str_data.c_str()), cmd_str_data.size());
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


    //3 获取资产详细
    std::string trade_uri="/v1/transfer";
    http_request curr_request(methods::POST);
    curr_request.set_request_uri(trade_uri);
    json::value request_body;
    //request_body["cmds"]=cmd_data;
    request_body["cmds"]=json::value::string(cmd_str_data);
    request_body["apikey"]=json::value::string(AccessKeyId);
    request_body["sign"]=json::value::string(pBuffer);
    curr_request.set_body(request_body);

    callFunction get_result=std::bind(&BiboxExchange::get_balance,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,curr_request,get_result);

    delete[] pEncode_buffer;
    return 0;
}
