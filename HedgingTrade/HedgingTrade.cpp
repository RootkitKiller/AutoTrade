//
// Created by mooninwater on 2018/9/19.
//


#include "HedgingTrade.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <algorithm> 

bool HedgingTrade::compare_price(const exc_trade::Depth &asks_depth, const exc_trade::Depth &bids_depth) {
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
    std::cout.width(10);
    std::cout<<exchange->get_exchange_name()<<"（交易所）";
    std::cout<<"    "<<pair<<"（交易对）";
    std::cout.width(15);
    std::cout<<buy_rate;
    std::cout<<"（买一价）";
    std::cout.width(15);
    std::cout<<sell_rate;
    std::cout<<"（卖一价）"<<std::endl;
}

void HedgingTrade::trade(std::shared_ptr<ExchangeFac> exchange_buy, exc_trade::Depth& asks_1,
                         std::shared_ptr<ExchangeFac> exchange_sell, exc_trade::Depth& bids_1, std::string pair) {
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
	if(base_num==0||op_num==0){
		return;
	}
    auto buy_num=(can_buy_num>op_num)?op_num:can_buy_num;   //比较两个交易所的余额，得出购买数2
    auto number=(trade_num>buy_num)?buy_num:trade_num;      //number为交易数量 A交易所买入 B交易所卖出
    
    //3 对冲交易
    exchange_buy->send_to_market(exc_trade::Trade(pair,exc_trade::BUY,number,asks_1.rate));
    exchange_sell->send_to_market(exc_trade::Trade(pair,exc_trade::SELL,number,bids_1.rate));
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
        if(thread_count>100000)
            break;
        auto depth_pair_A = exc_first->print_pair_depth(pair);
        auto depth_pair_B = exc_second->print_pair_depth(pair);
        if (depth_pair_A.first->size() == 0 || depth_pair_B.first->size() == 0) {
            std::cout<<"自动交易：深度"<<depth_pair_A.first->size()<<std::endl;
            std::cout<<"自动交易：深度"<<depth_pair_B.first->size()<<std::endl;
	    sleep(10);
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
            	trade(exc_first,asks_pair_A,exc_second,bids_pair_B,pair);
		}
        }
        if (asks_pair_B.rate < bids_pair_A.rate) {
            if(compare_price(asks_pair_B, bids_pair_A)==true){
		        trade(exc_second,asks_pair_B,exc_first,bids_pair_A,pair);
		    }
        }
	sleep(10);
    }
}

void HedgingTrade::find_pair(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second) {

    //1 发现两个交易所 相同的交易对

    std::vector<std::string> replace_vec;
    auto pair_list_first=exc_first->print_market_list();
    auto pair_list_second=exc_second->print_market_list();
    sort(pair_list_first->begin(),pair_list_first->end());
    sort(pair_list_second->begin(),pair_list_second->end());
	
    set_intersection(pair_list_first->begin(),pair_list_first->end(),\
                       pair_list_second->begin(),pair_list_second->end(),std::back_inserter(replace_vec));

    size_t thread_count=0;
    while (true){
	    thread_count++;
        if(thread_count>100000)
            break;
        //2 对比差价，查看是否满足搬砖条件
        for(auto pair:replace_vec){

            std::cout<<pair<<std::endl;
		    auto depth_pair_A = exc_first->print_pair_depth(pair);
            auto depth_pair_B = exc_second->print_pair_depth(pair);
            if (depth_pair_A.first->size() == 0 || depth_pair_B.first->size() == 0) {
		        std::cout<<"交易对发现：获取到的深度"<<depth_pair_A.first->size()<<std::endl;
                std::cout<<"交易对发现：获取到的深度"<<depth_pair_B.first->size()<<std::endl;
		        sleep(10);
		        continue;
            }
            auto asks_pair_A = depth_pair_A.first->back();    //卖一价
            auto bids_pair_A = depth_pair_A.second->front();  //买一价
            auto asks_pair_B = depth_pair_B.first->back();    //卖一价
            auto bids_pair_B = depth_pair_B.second->front();  //买一价
		    if (asks_pair_A.rate < bids_pair_B.rate) {
                auto earn_num=(bids_pair_B.rate-asks_pair_A.rate)*100/asks_pair_A.rate;
                if(earn_num>2){
                    std::cout<<"发现高利润键值对: "<<pair<<" "<<exc_first->get_exchange_name()<<"  卖一价: "<<asks_pair_A.rate\
                                   <<" 卖一数: "<<asks_pair_A.number<<"  "<< exc_second->get_exchange_name()<<" 买一价: "<<\
                                   bids_pair_B.rate<<"买一数: "<<bids_pair_B.number<<std::endl;
                    std::cout<<"利润率: "<<earn_num<<"%"<<std::endl;
                }
            }
            if (asks_pair_B.rate < bids_pair_A.rate) {
                auto earn_num=(bids_pair_A.rate-asks_pair_B.rate)*100/asks_pair_B.rate;
                if(earn_num>2){
                    std::cout<<"发现高利润键值对: "<<pair<<" "<<exc_second->get_exchange_name()<<"  卖一价: "<<asks_pair_B.rate\
                                   <<" 卖一数: "<<asks_pair_B.number<<"  "<< exc_first->get_exchange_name()<<" 买一价: "<<\
                                   bids_pair_A.rate<<"买一数: "<<bids_pair_A.number<<std::endl;
                    std::cout<<"利润率: "<<earn_num<<"%"<<std::endl;
                }
            }
	        std::cout<<"高利润交易对 正在发现~~~~~"<<std::endl;
		    sleep(10);
        }
    }
}

void HedgingTrade::auto_trade(std::shared_ptr<ExchangeFac> exc_first, std::shared_ptr<ExchangeFac> exc_second) {

    std::thread single_monitor(&HedgingTrade::thread_single,this,exc_first,exc_second,pair_str);
    std::thread pair_monitor(&HedgingTrade::find_pair,this,exc_first,exc_second);

    std::cout<<"当前交易所:"<<exc_first->get_exchange_name()<<"     "<<exc_second->get_exchange_name()<<std::endl;
    std::cout<<pair_str<<" 交易对正在监控~~~~~~"<<std::endl;

    single_monitor.join();
    pair_monitor.join();

    std::cout<<std::endl;
}
