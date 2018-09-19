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

    //std::shared_ptr<TradeBase> hedg_trade=std::make_shared<HedgingTrade>("EOS_ETH",5);
    //hedg_trade->auto_trade(exchange_A,exchange_B);

    return 0;
}
