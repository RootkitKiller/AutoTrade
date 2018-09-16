//
// Created by mooninwater on 2018/9/15.
//

#include "GateioExchange.h"

std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
        GateioExchange::print_pair_depth(const std::string pair_str) {
    std::string fullstr="/api2/1/orderBook/"+pair_str;
    p_client->request(methods::GET,fullstr).then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            //std::cout<<atof(json_result["last"].as_string().c_str())<<std::endl;
            auto asks_array =json_result["asks"].as_array();
            auto bids_array =json_result["bids"].as_array();
            for(auto iter_asks_value:asks_array){
                auto obj=Depth(atof(iter_asks_value[0].as_string().c_str()),atof(iter_asks_value[1].as_string().c_str()));
                p_asks_depth->push_back(obj);
                //std::cout<<iter_asks_value[0].as_string()<<std::endl;
            }
            for(auto iter_bids_value:bids_array){
                auto obj=Depth(atof(iter_bids_value[0].as_string().c_str()),atof(iter_bids_value[1].as_string().c_str()));
                p_bids_depth->push_back(obj);
            }
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();
    //return std::pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>>();
    return std::make_pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>> \
                    (std::move(p_asks_depth),std::move(p_bids_depth));
}


void GateioExchange::get_market_list() {
    p_client->request(methods::GET,"/api2/1/pairs").then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            auto json_array=json_result.as_array();
            for(auto value:json_array) {
                //std::cout<<value.as_string()<<std::endl;
                p_pair_list->push_back(value.as_string());
            }
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();

}


std::shared_ptr<std::vector<std::string>> GateioExchange::print_market_list() {
    return p_pair_list;
}

double GateioExchange::print_pair_rate(const std::string pair_str) {
    // /api2/1/ticker/BTC_ETH
    std::string fullstr="/api2/1/ticker/"+pair_str;
    p_client->request(methods::GET,fullstr).then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            //std::cout<<atof(json_result["last"].as_string().c_str())<<std::endl;
            std::string rate_str =json_result["last"].as_string();
            current_pair_rate=atof(rate_str.c_str());
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();

    return current_pair_rate;
}

void GateioExchange::send_to_market(const Trade &trade_data) {
    //接口说明
    /*通过以下API，用户可以使用程序控制自动进行账号资金查询，下单交易，取消挂单。
    请注意：请在您的程序中设置的HTTP请求头参数 Content-Type 为 application/x-www-form-urlencoded
    用户首先要通过这个链接获取API接口身份认证用到的Key和Secret。 然后在程序中用Secret作为密码，
    通过SHA512加密方式签名需要POST给服务器的数据得到Sign，
    并在HTTPS请求的Header部分传回Key和Sign。请参考以下接口说明和例子程序进行设置。
    */
    //1 构建请求参数
    json::value request_body;
    request_body["currencyPair"]=json::value::string(trade_data.pair_str);
    request_body["rate"]=json::value::string(std::to_string(trade_data.trade_rate));
    request_body["amount"]=json::value::string(std::to_string(trade_data.trade_number));

    //2 给交易签名 SHA512 格式：key=value&key=value&key=value
    std::string be_sign_data="currencyPair=";
    be_sign_data+=request_body["currencyPair"].as_string();
    be_sign_data+="&rate=";
    be_sign_data+=request_body["rate"].as_string();
    be_sign_data+="&amount=";
    be_sign_data+=request_body["amount"].as_string();
    //std::cout<<be_sign_data<<std::endl;

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, Secret_Key.c_str(), strlen(Secret_Key.c_str()), EVP_sha512(), NULL);
    HMAC_Update(&ctx, (unsigned char*)(be_sign_data.c_str()), be_sign_data.size());        // input is OK; &input is WRONG !!!
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
    std::string trade_uri;
    trade_data.trade_mode==0?trade_uri="/api2/1/private/buy":trade_uri="/api2/1/private/sell";
    http_request curr_request(methods::POST);
    curr_request.headers().add("Content-Type","application/x-www-form-urlencoded");
    curr_request.headers().add("KEY",AccessKeyId);
    curr_request.headers().add("SIGN",pBuffer);
    curr_request.set_request_uri(trade_uri);
    curr_request.set_body(be_sign_data);

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
