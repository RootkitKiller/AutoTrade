//
// Created by mooninwater on 2018/9/19.
//

#ifndef AUTOTRACE_HEDGINGTRADE_H
#define AUTOTRACE_HEDGINGTRADE_H

#include "../TradeBase.h"

class HedgingTrade : public TradeBase {
public:
    HedgingTrade(const std::string pair,double rate):pair_str(pair){
        earn_rate=rate;
    }
    virtual void auto_trade(std::shared_ptr<ExchangeFac> exc_first,\
                    std::shared_ptr<ExchangeFac> exc_second);

private:
    std::string pair_str;                                                   //当前交易的交易对

    void compare_price(const Depth &asks_depth,const Depth &bids_depth);    //根据卖一价和买一价对比利润

    void thread_single(std::shared_ptr<ExchangeFac> exc_first,\
                    std::shared_ptr<ExchangeFac> exc_second,std::string pair);
};


#endif //AUTOTRACE_HEDGINGTRADE_H
