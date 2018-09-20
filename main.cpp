#include <iostream>
#include "ExchangeFac.h"
#include "HuobiproExchange/HuobiproExchange.h"
#include "GateioExchange/GateioExchange.h"
#include "BiboxExchange/BiboxExchange.h"
#include "TradeBase.h"
#include "HedgingTrade/HedgingTrade.h"

int main() {

    //1 初始化各个交易所
    //2 按照指定的策略进行交易
    std::shared_ptr<ExchangeFac> exchange_A=std::make_shared<GateioExchange>\
        ("","");
auto balance_eth=exchange_A->print_balance("ETH");
auto balance_eos=exchange_A->print_balance("EOS");
std::cout<<balance_eth<<"  "<<balance_eos<<std::endl;


std::shared_ptr<ExchangeFac> exchange_B=std::make_shared<BiboxExchange>\
       ("","");
auto bibox_eth=exchange_B->print_balance("ETH");
auto bibox_eos=exchange_B->print_balance("EOS");
std::cout<<bibox_eth<<"  "<<bibox_eos<<std::endl;
    //std::shared_ptr<TradeBase> hedg_trade=std::make_shared<HedgingTrade>("EOS_ETH",5);
    //hedg_trade->auto_trade(exchange_A,exchange_B);
auto depth_pair=exchange_B->print_pair_depth("EOS_ETH");
auto rate= depth_pair.first->back().rate;
auto rate_2 = depth_pair.first->front().rate;
std::cout<<"front 卖"<<rate_2<<std::endl;
std::cout<<"back 卖"<<rate<<std::endl;
auto buy_rate_1=depth_pair.second->front().rate;
auto buy_rate_2=depth_pair.second->back().rate;
std::cout<<"买一 front"<<buy_rate_1<<"买一 back"<<buy_rate_2<<std::endl;   
//exchange_B->send_to_market(Trade("EOS_ETH",exc_trade::BUY,0.2,rate));
    return 0;
}
