#include <iostream>
#include "ExchangeFac.h"
#include "GateioExchange.h"
#include "BiboxExchange.h"
#include "HuobiproExchange.h"

void print_pair(std::shared_ptr<std::vector<std::string>> p_list ){
    for(auto pair_str:*p_list){
        std::cout<<pair_str<<std::endl;
    }
}

int main() {

    std::shared_ptr<ExchangeFac> exchange_A=std::make_shared<GateioExchange>\
            ("Your ApiKey","Your SecrectKey");
    //auto p_pair_list=exchange_A->print_market_list();
    std::cout<<"gateio支持的交易对"<<std::endl;
    print_pair(exchange_A->print_market_list());

    /*
    auto pair_rate=exchange_A->print_pair_rate((*p_pair_list)[0]);
    std::cout<<"btc/usdt行情  "<<(*p_pair_list)[0]<<"  "<<pair_rate<<std::endl;

    std::cout<<"买入1.3btc  通过交易对 btc/usdt"<<std::endl;
    exchange_A->send_to_market(Trade((*p_pair_list)[0],0,1.3,pair_rate));

    auto depth_pair =exchange_A->print_pair_depth((*p_pair_list)[0]);
    std::cout<<"买盘深度"<<std::endl;
    for(auto asks_value:*(depth_pair.first)){
        std::cout.width(7);
        std::cout<<asks_value.rate<<"  "<<asks_value.number<<std::endl;
    }
    std::cout<<"卖盘深度"<<std::endl;
    for(auto bids_value:*(depth_pair.second)){
        std::cout.width(7);
        std::cout<<bids_value.rate<<"  "<<bids_value.number<<std::endl;
    }*/


    std::shared_ptr<ExchangeFac> exchange_B=std::make_shared<BiboxExchange>\
            ("Your ApiKey","Your SecrectKey");
    //auto p_pair_list_b=exchange_B->print_market_list();
    std::cout<<"Bibox支持的交易对"<<std::endl;
    print_pair(exchange_B->print_market_list());

    return 0;
}