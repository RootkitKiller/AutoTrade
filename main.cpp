#include <iostream>
#include "ExchangeFac.h"
#include "GateioExchange.h"
#include "BiboxExchange.h"
#include "HuobiproExchange.h"

//策略一：对比两个交易所差价最大的交易对，输出彼此交易价格，输出利率
void find_pair(std::shared_ptr<ExchangeFac> exchange_A,std::shared_ptr<ExchangeFac> exchange_B){
    auto p_pair_list_A=exchange_A->print_market_list();
    auto p_pair_list_B=exchange_B->print_market_list();

    std::sort(p_pair_list_A->begin(),p_pair_list_A->end());
    std::sort(p_pair_list_B->begin(),p_pair_list_B->end());
    
    std::vector<std::string> replace_pair_vec{"EOS_BTC","EOS_ETH","ETH_BTC","BTC_ETH","LTC_ETH","LTC_BTC"\
                ,"ETC_ETH","ETC_BTC","BCH_ETH","BCH_ETC"};
    //std::set_intersection(p_pair_list_A->begin(),p_pair_list_A->end(),\
                //p_pair_list_B->begin(),p_pair_list_B->end(),std::back_inserter(replace_pair_vec));
    for(auto pair_str:replace_pair_vec){
        auto pair_rate_A=exchange_A->print_pair_rate(pair_str);
        auto pair_rate_B=exchange_B->print_pair_rate(pair_str);
	if(pair_rate_A==0||pair_rate_B==0)
	     continue;
        auto get_rate=(pair_rate_A>pair_rate_B?(pair_rate_A-pair_rate_B)*100/pair_rate_B:\
                (pair_rate_B-pair_rate_A)*100/pair_rate_A);
        if(get_rate>1){
            std::cout<<"第一个交易所: "<<pair_str<<"  "<<pair_rate_A<<std::endl;
            std::cout<<"第二个交易所: "<<pair_str<<"  "<<pair_rate_B<<std::endl;
            std::cout<<"盈利率: "<<get_rate<<std::endl;
        }
    }
}
void print_pair(std::shared_ptr<std::vector<std::string>> p_list ){
    for(auto pair_str:*p_list){
        std::cout<<pair_str<<std::endl;
    }
}

int main() {

    std::shared_ptr<ExchangeFac> exchange_A=std::make_shared<GateioExchange>\
            ("Your ApiKey","Your SecrectKey");
    //auto p_pair_list=exchange_A->print_market_list();
    //std::cout<<"gateio支持的交易对"<<std::endl;
    //print_pair(exchange_A->print_market_list());

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
    //std::cout<<"Bibox支持的交易对"<<std::endl;
    //print_pair(exchange_B->print_market_list());
    find_pair(exchange_A,exchange_B);
    

    return 0;
}
