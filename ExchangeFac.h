//
// Created by mooninwater on 2018/9/15.
//

#ifndef AUTOTRACE_EXCHANGEFAC_H
#define AUTOTRACE_EXCHANGEFAC_H


#include <memory>
#include <string>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

namespace exc_trade {
    const bool SELL = false;
    const bool BUY = true;
}

struct Trade{
    std::string pair_str;
    bool trade_mode;
    double trade_number;
    double trade_rate;

    Trade(std::string pair_str_arg,bool trade_mode_arg,double trade_number_arg,double trade_rate_arg):\
            pair_str(pair_str_arg),trade_mode(trade_mode_arg),trade_number(trade_number_arg),trade_rate(trade_rate_arg){}
};
struct Depth{
    double rate;
    double number;
    Depth(double rate_arg,double number_arg):rate(rate_arg),number(number_arg){}
};
class ExchangeFac {
public:

    virtual std::shared_ptr<std::vector<std::string>> print_market_list()=0;    //输出交易对列表

    virtual double print_pair_rate(const std::string pair_str)=0;               //输出某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
                    print_pair_depth(const std::string pair_str)=0;             //输出某个交易对的深度

    virtual void send_to_market(const Trade & trade_data)=0;                    //发送交易接口

    virtual ~ExchangeFac()  = default;
};


#endif //AUTOTRACE_EXCHANGEFAC_H
