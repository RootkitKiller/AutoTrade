//
// Created by mooninwater on 2018/9/15.
//

#include "HuobiproExchange.h"
#include "hmac-sha256.h"
#include "base64.h"


std::shared_ptr<std::vector<std::string>> HuobiproExchange::print_market_list() {
    //1 加密请求的参数，参数如下：
    // GET\n
    // api.huobi.pro\n
    // /v1/order/orders\n
    // AccessKeyId=e2xxxxxx-99xxxxxx-84xxxxxx-7xxxx&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=2017-05-11T15%3A19%3A30&order-id=1234567890
    std::string src_sign_arg1="GET\n";
    src_sign_arg1+="api.huobi.pro\n";
    src_sign_arg1+="/v1/common/currencys\n";
    std::string src_sign_arg2="AccessKeyId=";
    src_sign_arg2+=AccessKeyId;
    src_sign_arg2+="&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp=";
    src_sign_arg2+=get_utctime();
    //2 签名请求信息
    std::string src_sign_arg=src_sign_arg1+src_sign_arg2;
    hmac_sha256 hmac;
    hmac_sha256_initialize(&hmac, (uint8_t*)(Secret_Key.c_str()), 32);
    hmac_sha256_update(&hmac, (uint8_t*)(src_sign_arg.c_str()), src_sign_arg.size());
    hmac_sha256_finalize(&hmac, NULL, 0);
    std::string str=escapeURL(base64_encode(hmac.digest, 32));
    src_sign_arg2+="&Signature=";
    src_sign_arg2+=str;

    std::string fulladdr="/v1/common/symbols?";
    fulladdr+=src_sign_arg2;

    //std::cout<<fulladdr<<std::endl;
    //web::http::client::http_client_config cfg;
    //cfg.set_proxy(web::web_proxy("127.0.0.1:1086"));

    p_client->request(methods::GET,"/v1/common/symbols").then([&](http_response response)-> pplx::task<json::value>{
        if(response.status_code() == status_codes::OK){
            return response.extract_json();
        }
        return pplx::task_from_result(json::value());
    }).then([&](pplx::task<json::value> previousTask){
        try{
            auto json_result=previousTask.get();
            std::cout<<json_result<<std::endl;
            //auto json_array=json_result.as_array();
            //for(auto value:json_array) {
            //std::cout<<value.as_string()<<std::endl;
            //p_pair_list->push_back(value.as_string());
            //}
        }
        catch (http_exception const & e){
            std::cout << e.what() << std::endl;
        }
    }).wait();

    return std::shared_ptr<std::vector<std::string>>();
}

double HuobiproExchange::print_pair_rate(const std::string pair_str) {
    return 0;
}

std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>,std::shared_ptr<std::vector<exc_trade::Depth>>> \
        HuobiproExchange::print_pair_depth(const std::string pair_str) {
    return std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>, std::shared_ptr<std::vector<exc_trade::Depth>>>();
}

void HuobiproExchange::send_to_market(const exc_trade::Trade &trade_data) {

}

