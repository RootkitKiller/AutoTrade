#include <iostream>
#include "ExchangeFac.h"
#include "GateioExchange.h"
#include "BiboxExchange.h"
#include "HuobiproExchange.h"

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
    auto p_pair_list_A=exchange_A->print_market_list();
    auto p_pair_list_B=exchange_B->print_market_list();

    std::sort(p_pair_list_A->begin(),p_pair_list_A->end());
    std::sort(p_pair_list_B->begin(),p_pair_list_B->end());
    std::vector<std::string> replace_pair_vec;  
//    std::vector<std::string> replace_pair_vec{"EOS_BTC","EOS_ETH","ETH_BTC","LTC_ETH","LTC_BTC"\
                ,"ETC_ETH","ETC_BTC","BCH_ETH","BCH_ETC"};
    std::set_intersection(p_pair_list_A->begin(),p_pair_list_A->end(),\
                p_pair_list_B->begin(),p_pair_list_B->end(),std::back_inserter(replace_pair_vec));
    for(auto pair_str:replace_pair_vec){

        //获得两个平台的卖一价和买一价
        auto depth_pair_A =exchange_A->print_pair_depth(pair_str);
        auto depth_pair_B =exchange_B->print_pair_depth(pair_str);
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
