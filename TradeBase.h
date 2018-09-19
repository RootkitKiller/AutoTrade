//
// Created by mooninwater on 2018/9/19.
//

#ifndef AUTOTRACE_TRADEBASE_H
#define AUTOTRACE_TRADEBASE_H


#include "ExchangeFac.h"

class TradeBase {
protected:
    double earn_rate;                                 // 设置多少收益率可以进行搬砖 示例:0.05(5%)
public:
    virtual void auto_trade(std::shared_ptr<ExchangeFac> exc_first,\
                    std::shared_ptr<ExchangeFac> exc_second)=0;                     //自动交易方法
    virtual ~TradeBase()= default;

};


#endif //AUTOTRACE_TRADEBASE_H
