//
// Created by mooninwater on 2018/9/19.
//


#include "HedgingTrade.h"
#include <iostream>
#include <thread>
#include <unistd.h>

bool HedgingTrade::compare_price(const Depth &asks_depth, const Depth &bids_depth) {
    //收益率计算，不考虑手续费
    auto earn_yield=(bids_depth.rate-asks_depth.rate)*100/asks_depth.rate;
    std::cout<<"当前利率: "<<earn_yield<<"% "<<std::endl;
    if(earn_yield>earn_rate){
        return true;
    }else{
        return false;
    }
}

void HedgingTrade::print_log(std::shared_ptr<ExchangeFac> exchange,std::string pair, double buy_rate, double sell_rate) {

    std::cout<<exchange->get_exchange_name()<<"（交易所）";
    std::cout<<"    "<<pair<<"（交易对）";
    std::cout.width(15);
    std::cout<<buy_rate;
    std::cout<<"（买一价）";
    std::cout.width(15);
    std::cout<<sell_rate;
    std::cout<<"（卖一价）"<<std::endl;
}

void HedgingTrade::trade(std::shared_ptr<ExchangeFac> exchange_buy, Depth& asks_1,
                         std::shared_ptr<ExchangeFac> exchange_sell, Depth& bids_1, std::string pair) {
    //1 分割基准币与操作币
    auto find_iter=std::find(pair.begin(),pair.end(),'_');
    //auto find_index=pair.find("_");
    std::string op_symbol(pair.begin(),find_iter);
    std::string base_symbol(find_iter+1,pair.end());
    //2 计算吃单数（买一、卖一、操作币余额、基准币购买数）的最小值
    auto trade_num=(asks_1.number>bids_1.number)?bids_1.number:asks_1.number;
    // 获取A交易所基准币的余额，获取B交易所操作币的余额
    auto base_num=exchange_buy->print_balance(base_symbol);    //比较两个交易所的卖一数与买一数，得出购买数1
    auto can_buy_num=base_num/asks_1.rate;
    auto op_num=exchange_sell->print_balance(op_symbol);
    auto buy_num=(can_buy_num>op_num)?op_num:can_buy_num;   //比较两个交易所的余额，得出购买数2
    auto number=(trade_num>buy_num)?buy_num:trade_num;      //number为交易数量 A交易所买入 B交易所卖出

    //3 对冲交易
    //exchange_buy->send_to_market(Trade(pair,exc_trade::BUY,number,asks_1.rate));
    //exchange_sell->send_to_market(Trade(pair,exc_trade::SELL,number,bids_1.rate));
    std::cout<<"可吃单数: "<<number<<std::endl;
    std::cout<<"吃单价: "<<asks_1.rate<<std::endl;
    std::cout<<"卖出价: "<<bids_1.rate<<std::endl;
    std::cout<<"收益: "<<(bids_1.rate-asks_1.rate)*number<<" "<<op_symbol<<std::endl;
}

void HedgingTrade::thread_single(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second,
                                 std::string pair) {
    size_t thread_count=0;
    while(true) {
        thread_count++;
        if(thread_count>10000)
            break;
        //获得两个平台的卖一价和买一价、价格，多个线程可能会修改交易对的深度信息，需要加锁
        exc_first->get_mutex().lock();
        exc_second->get_mutex().lock();

        auto depth_pair_A = exc_first->print_pair_depth(pair);
        auto depth_pair_B = exc_second->print_pair_depth(pair);
        if (depth_pair_A.first->size() == 0 || depth_pair_B.first->size() == 0) {
            std::cout<<depth_pair_A.first->size()<<std::endl;
            std::cout<<depth_pair_B.first->size()<<std::endl;
            //解锁
            exc_first->get_mutex().unlock();
            exc_second->get_mutex().unlock();
            continue;
        }
        auto asks_pair_A = depth_pair_A.first->back();    //卖一价
        auto bids_pair_A = depth_pair_A.second->front();  //买一价
        auto asks_pair_B = depth_pair_B.first->back();    //卖一价
        auto bids_pair_B = depth_pair_B.second->front();  //买一价
        print_log(exc_first,pair,bids_pair_A.rate,asks_pair_A.rate);
        print_log(exc_second,pair,bids_pair_B.rate,asks_pair_B.rate);
        std::cout<<std::endl;


        if (asks_pair_A.rate < bids_pair_B.rate) {
            //收益率计算，不考虑手续费
            if(compare_price(asks_pair_A, bids_pair_B)==true){
            }
        }
        if (asks_pair_B.rate < bids_pair_A.rate) {
            compare_price(asks_pair_B, bids_pair_A);
        }
        exc_first->get_mutex().unlock();
        exc_second->get_mutex().unlock();

        sleep(10);
    }
}

void HedgingTrade::auto_trade(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second) {

    std::thread single_monitor(&HedgingTrade::thread_single,this,exc_first,exc_second,pair_str);
    single_monitor.join();

    std::cout<<"当前交易所:"<<exc_first->get_exchange_name()<<"     "<<exc_second->get_exchange_name()<<std::endl;
    std::cout<<pair_str<<" 交易对正在监控~~~~~~"<<std::endl;
    std::cout<<std::endl;
}
