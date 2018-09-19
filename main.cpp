#include <iostream>
#include "ExchangeFac.h"
#include "HuobiproExchange/HuobiproExchange.h"
#include "GateioExchange/GateioExchange.h"
#include "BiboxExchange/BiboxExchange.h"
#include "TradeBase.h"
#include "HedgingTrade/HedgingTrade.h"

//策略一：对比两个交易所差价最大的交易对，输出彼此交易价格，输出利率
void compare_price(const Depth &asks_depth,const Depth &bids_depth){
    //收益率计算，不考虑手续费
    auto earn_yield=(bids_depth.rate-asks_depth.rate)*100/asks_depth.rate;
    if(earn_yield>1){
        std::cout<<"买入交易所的卖一数: "<<asks_depth.number<<std::endl;
        std::cout<<"买入交易所的卖一价: "<<asks_depth.rate<<std::endl;
        std::cout<<"卖出交易所的买一数: "<<bids_depth.number<<std::endl;
        std::cout<<"卖出交易所的买一价: "<<bids_depth.rate<<std::endl;
	std::cout<<"毛利率: "<<earn_yield<<"%"<<std::endl;
	std::cout<<std::endl;
    }
}
void find_pair(std::shared_ptr<ExchangeFac> exchange_A,std::shared_ptr<ExchangeFac> exchange_B){

}
void print_pair(std::shared_ptr<std::vector<std::string>> p_list ){
    for(auto pair_str:*p_list){
        std::cout<<pair_str<<std::endl;
    }
}

int main() {

    //1 初始化各个交易所
    //2 按照指定的策略进行交易
    std::shared_ptr<ExchangeFac> exchange_A=std::make_shared<GateioExchange>\
            ("Your ApiKey","Your SecrectKey");
    /*
    auto p_pair_list=exchange_A->print_market_list();
    std::cout<<"gateio支持的交易对"<<std::endl;
    print_pair(exchange_A->print_market_list());
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
    //find_pair(exchange_A,exchange_B);

    //exchange_B->send_to_market(Trade("EOS_ETH",0,1.3,0.035));

    std::shared_ptr<TradeBase> hedg_trade=std::make_shared<HedgingTrade>("EOS_ETH",5);
    hedg_trade->auto_trade(exchange_A,exchange_B);

    return 0;
}
