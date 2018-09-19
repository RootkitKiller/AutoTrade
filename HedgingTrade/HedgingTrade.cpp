//
// Created by mooninwater on 2018/9/19.
//


#include "HedgingTrade.h"
#include <iostream>
#include <thread>

void HedgingTrade::compare_price(const Depth &asks_depth, const Depth &bids_depth) {
    //收益率计算，不考虑手续费
    auto earn_yield=(bids_depth.rate-asks_depth.rate)*100/asks_depth.rate;
    std::cout<<"当前利率: "<<earn_yield<<"% "<<std::endl;
    if(earn_yield>earn_rate){
        std::cout<<"买入交易所的卖一数: "<<asks_depth.number<<std::endl;
        std::cout<<"买入交易所的卖一价: "<<asks_depth.rate<<std::endl;
        std::cout<<"卖出交易所的买一数: "<<bids_depth.number<<std::endl;
        std::cout<<"卖出交易所的买一价: "<<bids_depth.rate<<std::endl;
        std::cout<<"毛利率: "<<earn_yield<<"%"<<std::endl;
        std::cout<<std::endl;
    }
}

void HedgingTrade::thread_single(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second,
                                 std::string pair) {
    while(true) {
        //获得两个平台的卖一价和买一价
        auto depth_pair_A = exc_first->print_pair_depth(pair);
        auto depth_pair_B = exc_second->print_pair_depth(pair);
        if (depth_pair_A.first->size() == 0 || depth_pair_B.first->size() == 0)
            return;
        auto asks_pair_A = depth_pair_A.first->back();    //卖一价
        auto bids_pair_A = depth_pair_A.second->front();  //买一价
        auto asks_pair_B = depth_pair_B.first->back();    //卖一价
        auto bids_pair_B = depth_pair_B.second->front();  //买一价
        //交易所的卖一大于买一
        //如果A交易所的卖一价 小于 B交易所的买一价，则根据深度吃掉A交易所的卖一/吃掉自身余额（规避风险，吃单共分成10次），反之亦然
        std::cout << "交易对: " << pair << std::endl;
        std::cout << "卖一价: ";
        std::cout.width(15);
        std::cout << asks_pair_A.rate << std::endl;
        std::cout << "买一价: ";
        std::cout.width(15);
        std::cout << bids_pair_B.rate << std::endl;

        std::cout << "卖一价: ";
        std::cout.width(15);
        std::cout << asks_pair_B.rate << std::endl;
        std::cout << "买一价: ";
        std::cout.width(15);
        std::cout << bids_pair_A.rate << std::endl;
        if (asks_pair_A.rate < bids_pair_B.rate) {
            //收益率计算，不考虑手续费
            compare_price(asks_pair_A, bids_pair_B);
        }
        if (asks_pair_B.rate < bids_pair_A.rate) {
            compare_price(asks_pair_B, bids_pair_A);
        }
    }
}

void HedgingTrade::auto_trade(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second) {
    /*auto p_pair_list_A=exc_first->print_market_list();
    auto p_pair_list_B=exc_second->print_market_list();

    std::sort(p_pair_list_A->begin(),p_pair_list_A->end());
    std::sort(p_pair_list_B->begin(),p_pair_list_B->end());
    std::vector<std::string> replace_pair_vec;
//    std::vector<std::string> replace_pair_vec{"EOS_BTC","EOS_ETH","ETH_BTC","LTC_ETH","LTC_BTC"\
                ,"ETC_ETH","ETC_BTC","BCH_ETH","BCH_ETC"};
    std::set_intersection(p_pair_list_A->begin(),p_pair_list_A->end(),\
                p_pair_list_B->begin(),p_pair_list_B->end(),std::back_inserter(replace_pair_vec));
    for(auto pair_str:replace_pair_vec){

        //获得两个平台的卖一价和买一价
        auto depth_pair_A =exc_first->print_pair_depth(pair_str);
        auto depth_pair_B =exc_second->print_pair_depth(pair_str);
        if(depth_pair_A.first->size()==0||depth_pair_B.first->size()==0)
            continue;
        auto asks_pair_A=depth_pair_A.first->back();    //卖一价
        auto bids_pair_A=depth_pair_A.second->front();  //买一价
        auto asks_pair_B=depth_pair_B.first->back();    //卖一价
        auto bids_pair_B=depth_pair_B.second->front();  //买一价
        //交易所的卖一大于买一
        //如果A交易所的卖一价 小于 B交易所的买一价，则根据深度吃掉A交易所的卖一/吃掉自身余额（规避风险，吃单共分成10次），反之亦然
        std::cout<<"交易对: "<<pair_str<<std::endl;
        std::cout<<"卖一价: ";
        std::cout.width(15);
        std::cout<<asks_pair_A.rate<<std::endl;
        std::cout<<"买一价: ";
        std::cout.width(15);
        std::cout<<bids_pair_B.rate<<std::endl;

        std::cout<<"卖一价: ";
        std::cout.width(15);
        std::cout<<asks_pair_B.rate<<std::endl;
        std::cout<<"买一价: ";
        std::cout.width(15);
        std::cout<<bids_pair_A.rate<<std::endl;
        if(asks_pair_A.rate<bids_pair_B.rate){
            //收益率计算，不考虑手续费
            compare_price(asks_pair_A,bids_pair_B);
        }
        if(asks_pair_B.rate<bids_pair_A.rate){
            compare_price(asks_pair_B,bids_pair_A);
        }
    }*/
    std::thread single_monitor(&HedgingTrade::thread_single,this,exc_first,exc_second,pair_str);
    single_monitor.detach();

    std::cout<<"当前交易所:"<<exc_first->get_exchange_name()<<"     "<<exc_second->get_exchange_name()<<std::endl;
    std::cout<<pair_str<<" 交易对正在监控~~~~~~"<<std::endl;
    std::cout<<std::endl;
}
