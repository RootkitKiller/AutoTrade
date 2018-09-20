//
// Created by mooninwater on 2018/9/15.
//

#include "GateioExchange.h"

void GateioExchange::get_pair_depth(json::value json_result) {

    p_asks_depth.reset(new std::vector<Depth>);
    p_bids_depth.reset(new std::vector<Depth>);
    if(json_result["asks"].is_array()== true &&
       json_result["bids"].is_array()== true) {
        auto asks_array = json_result["asks"].as_array();
        auto bids_array = json_result["bids"].as_array();
        for (auto iter_asks_value:asks_array) {
            auto asks_depth = Depth(atof(iter_asks_value[0].as_string().c_str()),
                             atof(iter_asks_value[1].as_string().c_str()));
            p_asks_depth->push_back(asks_depth);
        }
        for (auto iter_bids_value:bids_array) {
            auto bids_depth = Depth(atof(iter_bids_value[0].as_string().c_str()),
                             atof(iter_bids_value[1].as_string().c_str()));
            p_bids_depth->push_back(bids_depth);
        }
    }
}

std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
        GateioExchange::print_pair_depth(const std::string pair_str) {

    std::string fullstr="/api2/1/orderBook/"+pair_str;
    http_request requests(methods::GET);
    requests.set_request_uri(fullstr);

    callFunction get_result=std::bind(&GateioExchange::get_pair_depth,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return std::make_pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>> \
                    (std::move(p_asks_depth),std::move(p_bids_depth));
}

void GateioExchange::get_market_list(json::value json_result) {

    auto json_array=json_result.as_array();
    for(auto value:json_array) {
        p_pair_list->push_back(value.as_string());
    }
}

std::shared_ptr<std::vector<std::string>> GateioExchange::print_market_list() {

    http_request requests(methods::GET);
    requests.set_request_uri("/api2/1/pairs");

    callFunction get_result=std::bind(&GateioExchange::get_market_list,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return p_pair_list;
}

void GateioExchange::get_pair_rate(json::value json_result) {

    if(json_result["last"].is_string()==true){
        std::string rate_str =json_result["last"].as_string();
        current_pair_rate=atof(rate_str.c_str());
    }else
        current_pair_rate=0;
}

double GateioExchange::print_pair_rate(const std::string pair_str) {
    // /api2/1/ticker/BTC_ETH

    http_request requests(methods::GET);
    std::string fullstr="/api2/1/ticker/"+pair_str;
    requests.set_request_uri(fullstr);

    callFunction get_result=std::bind(&GateioExchange::get_pair_rate,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return current_pair_rate;
}

void GateioExchange::get_trade_result(json::value json_result) {
    std::cout<<json_result<<std::endl;
}

void GateioExchange::send_to_market(const Trade &trade_data) {

    //1 给交易签名 SHA512 格式：key=value&key=value&key=value
    std::string be_sign_data="currencyPair=";
    be_sign_data+=trade_data.pair_str;
    be_sign_data+="&rate=";
    be_sign_data+=std::to_string(trade_data.trade_rate);
    be_sign_data+="&amount=";
    be_sign_data+=std::to_string(trade_data.trade_number);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_sha512(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(be_sign_data.c_str()), be_sign_data.size());
    unsigned char* pEncode_buffer = new unsigned char[EVP_MAX_MD_SIZE];
    uint32_t buffer_length=0;
    HMAC_Final(&ctx, pEncode_buffer, &buffer_length);
    HMAC_CTX_cleanup(&ctx);

    //encode to degist
    char buf[129];
    for (int i=0; i<64; i++)
        sprintf(buf+i*2, "%02x", pEncode_buffer[i]);
    buf[128]=0;
    std::string pBuffer(buf);


    //3 买入/卖出
    std::string trade_uri;
    trade_data.trade_mode==exc_trade::BUY?trade_uri="/api2/1/private/buy":trade_uri="/api2/1/private/sell";
    http_request curr_request(methods::POST);
    curr_request.headers().add("Content-Type","application/x-www-form-urlencoded");
    curr_request.headers().add("KEY",AccessKeyId);
    curr_request.headers().add("SIGN",pBuffer);
    curr_request.set_request_uri(trade_uri);
    curr_request.set_body(be_sign_data);

    callFunction get_result=std::bind(&GateioExchange::get_trade_result,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,curr_request,get_result);

    delete[] pEncode_buffer;
}

void GateioExchange::get_balance(json::value json_result) {
    m_balance=json_result;
    //std::cout<<json_result<<std::endl;
}

double GateioExchange::print_balance(const std::string symbol) {
    // /api2/1/private/balances
    //1 给交易签名 SHA512 格式：key=value&key=value&key=value
    std::string be_sign_data="";
    //2 签名过程
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_sha512(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(be_sign_data.c_str()), be_sign_data.size());
    unsigned char* pEncode_buffer = new unsigned char[EVP_MAX_MD_SIZE];
    uint32_t buffer_length=0;
    HMAC_Final(&ctx, pEncode_buffer, &buffer_length);
    HMAC_CTX_cleanup(&ctx);

    //encode to degist
    char buf[129];
    for (int i=0; i<64; i++)
        sprintf(buf+i*2, "%02x", pEncode_buffer[i]);
    buf[128]=0;
    std::string pBuffer(buf);
    //std::cout<<pBuffer<<std::endl;

    //3 买入/卖出

    http_request curr_request(methods::POST);
    curr_request.headers().add("Content-Type","application/x-www-form-urlencoded");
    curr_request.headers().add("KEY",AccessKeyId);
    curr_request.headers().add("SIGN",pBuffer);
    curr_request.set_request_uri("/api2/1/private/balances");
    curr_request.set_body(be_sign_data);

    callFunction get_result=std::bind(&GateioExchange::get_balance,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,curr_request,get_result);

    delete[] pEncode_buffer;
    //4 请求会阻塞到完成 校验回调函数的结果
    if(m_balance["result"].as_string()=="true"){
        if(m_balance["available"].is_array()== true){
	        return 0;
        }else{
            auto balance_list=m_balance["available"];
            if(balance_list.has_field(symbol)== false){
		        return 0;
            }else{
                return atof(balance_list[symbol].as_string().c_str());
            }
        }
    }else{
        return 0;
    }

}
