//
// Created by mooninwater on 2018/9/17.
//

#include "BiboxExchange.h"

void BiboxExchange::get_market_list(json::value json_result,std::shared_ptr<std::vector<std::string>> p_pair_list) {
    if(json_result["result"].is_array()==false)
	    return;
    auto list_array=json_result["result"].as_array();
    for(auto pair:list_array){
        p_pair_list->push_back(pair["pair"].as_string());
    }
}

std::shared_ptr<std::vector<std::string>> BiboxExchange::print_market_list() {

    http_request requests(methods::GET);
    requests.set_request_uri("/v1/mdata?cmd=pairList");
    auto p_pair_list=std::make_shared<std::vector<std::string>>();
    callFunction get_result=std::bind(&BiboxExchange::get_market_list,this,std::placeholders::_1,p_pair_list);
    HttpRequest::send_request(rest_addr,requests,get_result);
    return p_pair_list;
}

void BiboxExchange::get_pair_rate(json::value json_result,double current_pair_rate) {

    if(json_result["result"]["last"].is_string()==true){
        auto rate_str=json_result["result"]["last"].as_string();
        current_pair_rate=atof(rate_str.c_str());
    }else{
        current_pair_rate=0;
    }
}

double BiboxExchange::print_pair_rate(const std::string pair_str) {

    http_request requests(methods::GET);
    std::string fullstr="/v1/mdata?cmd=market&pair="+pair_str;
    requests.set_request_uri(fullstr);
    double current_pair_rate=0;
    callFunction get_result=std::bind(&BiboxExchange::get_pair_rate,this,std::placeholders::_1,current_pair_rate);
    HttpRequest::send_request(rest_addr,requests,get_result);
    return current_pair_rate;
}

void BiboxExchange::get_pair_depth(json::value json_result,std::shared_ptr<std::vector<exc_trade::Depth>> p_asks_depth,\
                        std::shared_ptr<std::vector<exc_trade::Depth>> p_bids_depth) {
    auto json_res=json_result["result"];

    if(json_res["asks"].is_array()== true &&
            json_res["bids"].is_array()== true) {
        auto asks_array = json_res["asks"].as_array();
        auto bids_array = json_res["bids"].as_array();
        for (auto iter_asks_value:asks_array) {
            auto asks_depth = exc_trade::Depth(atof(iter_asks_value["price"].as_string().c_str()),
                             atof(iter_asks_value["volume"].as_string().c_str()));
            p_asks_depth->push_back(asks_depth);
        }
	    std::reverse(p_asks_depth->begin(),p_asks_depth->end());
        for (auto iter_bids_value:bids_array) {
            auto bids_depth = exc_trade::Depth(atof(iter_bids_value["price"].as_string().c_str()),
                             atof(iter_bids_value["volume"].as_string().c_str()));
            p_bids_depth->push_back(bids_depth);
        }
    }
}

std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>, std::shared_ptr<std::vector<exc_trade::Depth>>>
BiboxExchange::print_pair_depth(const std::string pair_str) {

    std::string fullstr="v1/mdata?cmd=depth&pair="+pair_str;
    fullstr+="&size=10";

    http_request requests(methods::GET);
    requests.set_request_uri(fullstr);
    auto p_asks_depth=std::make_shared<std::vector<exc_trade::Depth>>();
    auto p_bids_depth=std::make_shared<std::vector<exc_trade::Depth>>();

    callFunction get_result=std::bind(&BiboxExchange::get_pair_depth,this,std::placeholders::_1,p_asks_depth,p_bids_depth);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return std::make_pair<std::shared_ptr<std::vector<exc_trade::Depth>>, std::shared_ptr<std::vector<exc_trade::Depth>>> \
                    (std::move(p_asks_depth),std::move(p_bids_depth));

}

void BiboxExchange::get_trade_result(json::value json_result) {
    std::cout<<json_result<<std::endl;
}

void BiboxExchange::send_to_market(const exc_trade::Trade &trade_data) {

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
    //std::cout<<pBuffer<<std::endl;


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

void BiboxExchange::get_balance(json::value json_result,json::value m_balance ) {
    m_balance=json_result;
    //std::cout<<json_result<<std::endl;
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
    //std::cout<<pBuffer<<std::endl;


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
    json::value m_balance;
    callFunction get_result=std::bind(&BiboxExchange::get_balance,this,std::placeholders::_1,m_balance);
    HttpRequest::send_request(rest_addr,curr_request,get_result);

    delete[] pEncode_buffer;
    // 4 解析结果并返回
    if(m_balance["result"].is_array()==true){
        auto result=m_balance["result"][0]["result"];
        auto symbol_list=result["assets_list"];
        if(symbol_list.is_array()== true){
                        for(auto iter=symbol_list.as_array().begin();iter!=symbol_list.as_array().end();iter++){
                if((*iter)["coin_symbol"].as_string()==symbol){
                    return atof((*iter)["balance"].as_string().c_str());
                }
            }
            return 0;
        }else{
            return 0;
        }
    }else{
        return 0;
    }

}
