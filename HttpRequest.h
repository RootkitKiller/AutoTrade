//
// Created by mooninwater on 2018/9/19.
//

#ifndef AUTOTRACE_HTTPREQUEST_H
#define AUTOTRACE_HTTPREQUEST_H

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

class HttpRequest{
public:
    static void send_request(std::string url,http_request requests,std::function<void(json::value)> callback ) {
        http_client client(U(url));
        client.request(requests).then([&](http_response response)-> pplx::task<json::value>{
            if(response.status_code() == status_codes::OK){
                return response.extract_json();
            }
            return pplx::task_from_result(json::value());
        }).then([&](pplx::task<json::value> previousTask){
            try{

                auto json_data=previousTask.get();
                callback(json_data);
            }
            catch (http_exception const & e){
                std::cout << e.what() << std::endl;
            }
        }).wait();
    }
};


#endif //AUTOTRACE_HTTPREQUEST_H
