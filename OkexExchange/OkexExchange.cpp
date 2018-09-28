//
// Created by mooninwater on 2018/9/21.
//

#include "OkexExchange.h"
#include "openssl/md5.h"

void OkexExchange::get_market_list(json::value json_result, std::shared_ptr<std::vector<std::string>> p_pair_list) {
    return;
}
//okex未发现获取支持的交易对列表接口，空缺中~~~~~~~
std::shared_ptr<std::vector<std::string>> OkexExchange::print_market_list() {
    auto p_pair_list=std::make_shared<std::vector<std::string>>();
    return std::shared_ptr<std::vector<std::string>>();
}

void OkexExchange::get_pair_rate(json::value json_result, double current_pair_rate) {
    // json_result["ticker"]["last"]
    if(json_result["ticker"]["last"].is_string()==true){
        auto rate_str=json_result["ticker"]["last"].as_string();
        current_pair_rate=atof(rate_str.c_str());
    }
    //std::cout<<json_result<<std::endl;
}

double OkexExchange::print_pair_rate(const std::string pair_str) {
    //GET https://www.okex.com/api/v1/ticker.do?symbol=ltc_btc
    http_request requests(methods::GET);
    auto new_str=pair_str;
    new_str=pair_tolower(new_str);
    std::string fullstr="/api/v1/ticker.do?symbol="+new_str;
    requests.set_request_uri(fullstr);
    double current_pair_rate=0;
    auto get_result=std::bind(&OkexExchange::get_pair_rate,this,std::placeholders::_1,current_pair_rate);
    HttpRequest::send_request(rest_addr,requests,get_result);
    return current_pair_rate;
}


void OkexExchange::get_pair_depth(json::value json_result, std::shared_ptr<std::vector<exc_trade::Depth>> p_asks_depth,
                                  std::shared_ptr<std::vector<exc_trade::Depth>> p_bids_depth) {

    if(json_result["asks"].is_array()== true &&
            json_result["bids"].is_array()== true) {
        auto asks_array = json_result["asks"].as_array();
        auto bids_array = json_result["bids"].as_array();
        for (auto iter_asks_value:asks_array) {
            auto asks_depth = exc_trade::Depth(atof(iter_asks_value[0].as_string().c_str()),
                                               atof(iter_asks_value[1].as_string().c_str()));
            p_asks_depth->push_back(asks_depth);
        }
        for (auto iter_bids_value:bids_array) {
            auto bids_depth = exc_trade::Depth(atof(iter_bids_value[0].as_string().c_str()),
                                               atof(iter_bids_value[1].as_string().c_str()));
            p_bids_depth->push_back(bids_depth);
        }
    }
}

std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>, std::shared_ptr<std::vector<exc_trade::Depth>>>
OkexExchange::print_pair_depth(const std::string pair_str) {

    http_request requests(methods::GET);
    auto new_str=pair_str;
    new_str=pair_tolower(new_str);

    std::string fullstr="/api/v1/depth.do?symbol="+new_str;
    requests.set_request_uri(fullstr);
    auto p_asks_depth=std::make_shared<std::vector<exc_trade::Depth>>();
    auto p_bids_depth=std::make_shared<std::vector<exc_trade::Depth>>();

    auto get_result=std::bind(&OkexExchange::get_pair_depth,this,std::placeholders::_1,p_asks_depth,p_bids_depth);
    HttpRequest::send_request(rest_addr,requests,get_result);

    return std::make_pair<std::shared_ptr<std::vector<exc_trade::Depth>>, std::shared_ptr<std::vector<exc_trade::Depth>>>\
            (std::move(p_asks_depth),std::move(p_bids_depth));
}

void OkexExchange::get_trade_result(json::value json_result) {

}

void OkexExchange::send_to_market(const exc_trade::Trade &trade_data) {
    //1 构建待签名字符串,并签名
    //POST https://www.okex.com/api/v1/trade.do
    /*
    api_key	String	是	用户申请的apiKey
    symbol	String	是	币对如ltc_btc
    type	String	是	买卖类型：限价单(buy/sell) 市价单(buy_market/sell_market)
    price	Double	否	下单价格 市价卖单不传price
    amount	Double	否	交易数量 市价买单不传amount,市价买单需传price作为买入总金额
    sign	String	是	请求参数的签名
     */

    std::string sign_params="amount="+std::to_string(trade_data.trade_number);
    sign_params+="&api_key=";
    sign_params+=AccessKeyId;
    sign_params+="&price=";
    sign_params+=std::to_string(trade_data.trade_rate);
    auto newpair=trade_data.pair_str;
    newpair=pair_tolower(newpair);
    sign_params+="&symbol=";
    sign_params+=newpair;
    sign_params+="&type=";
    sign_params+=(trade_data.trade_mode==exc_trade::BUY)?"buy":"sell";
    sign_params+="&secret_key=";
    sign_params+=Secret_Key;

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx,sign_params.c_str(),sign_params.size());
    unsigned char* pEncode_buffer = new unsigned char[EVP_MAX_MD_SIZE];
    MD5_Final(pEncode_buffer,&ctx);

    char buf[33];
    for (int i=0; i<16; i++)
        sprintf(buf+i*2, "%02x", pEncode_buffer[i]);
    buf[32]=0;
    std::string pBuffer(buf);
    pBuffer=pair_toupper(pBuffer);

    //2 构建请求request
    http_request requests(methods::POST);
    json::value body;
    body["amount"]=json::value::string(std::to_string(trade_data.trade_number));
    body["api_key"]=json::value::string(AccessKeyId);
    body["price"]=json::value::string(std::to_string(trade_data.trade_rate));
    body["symbol"]=json::value::string(trade_data.pair_str);
    body["type"]=json::value::string((trade_data.trade_mode==exc_trade::BUY)?"buy":"sell");
    body["sign"]=json::value::string(pBuffer);
    requests.headers().add("Content-Type","application/x-www-form-urlencoded");
    requests.set_body(body);
    requests.set_request_uri("/api/v1/trade.do");
    //3 发送请求
    auto get_result=std::bind(&OkexExchange::get_trade_result,this,std::placeholders::_1);
    HttpRequest::send_request(rest_addr,requests,get_result);

    delete[] pEncode_buffer;

}

void OkexExchange::get_balance(json::value json_result, json::value m_balance) {
    m_balance=json_result;
}

double OkexExchange::print_balance(const std::string symbol) {
    //1 构造签名数据
    //POST https://www.okex.com/api/v1/userinfo.do
    /*
    api_key	String	是	用户申请的apiKey
    sign	String	是	请求参数的签名
    */
    std::string sign_params="api_key="+AccessKeyId;
    sign_params+="&secret_key=";
    sign_params+=Secret_Key;
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx,sign_params.c_str(),sign_params.size());
    unsigned char* pEncode_buffer = new unsigned char[EVP_MAX_MD_SIZE];
    MD5_Final(pEncode_buffer,&ctx);
    char buf[33];
    for(int i=0;i<16;i++){
        sprintf(buf+i*2, "%02x", pEncode_buffer[i]);
    }
    buf[32]=0;
    std::string pBuffer(buf);
    pBuffer=pair_toupper(pBuffer);

    //2 构造请求request
    http_request requests(methods::POST);
    requests.headers().add("Content-Type","application/x-www-form-urlencoded");
    json::value body;
    body["api_key"]=json::value::string(AccessKeyId);
    body["sign"]=json::value::string(pBuffer);
    requests.set_body(body);
    requests.set_request_uri("/api/v1/userinfo.do");

    //3 发送交易
    json::value balance_list;
    auto get_result=std::bind(&OkexExchange::get_balance,this,std::placeholders::_1,balance_list);
    HttpRequest::send_request(rest_addr,requests,get_result);

    //4 解析该token的余额
    auto newsymbol=symbol;
    newsymbol=pair_tolower(newsymbol);
    if(balance_list["result"]==true){
        auto balance_str=balance_list["info"]["funds"]["free"][newsymbol].as_string();
        return atof(balance_str.c_str());
    }else
        return 0;
}
