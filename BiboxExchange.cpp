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
    return std::pair<std::shared_ptr<std::vector<Depth>>, std::shared_ptr<std::vector<Depth>>>();
}

void BiboxExchange::send_to_market(const Trade &trade_data) {

}
