//
// Created by mooninwater on 2018/9/15.
//

#ifndef AUTOTRACE_EXCHANGEFAC_H
#define AUTOTRACE_EXCHANGEFAC_H


#include <memory>
#include <string>
#include <vector>
#include <mutex>

namespace exc_trade {
    const bool BUY = false;
    const bool SELL = true;
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
protected:
    std::string rest_addr;                                                          //Rest API 地址
    std::string exchange_name;                                                      //交易所名称

    std::string AccessKeyId;                                                        //apikey
    std::string Secret_Key;                                                         //apikey 密码
    std::shared_ptr<std::vector<Depth>> p_asks_depth;                               //当前交易对的买盘深度
    std::shared_ptr<std::vector<Depth>> p_bids_depth;                               //当前交易对的卖盘深度
    double current_pair_rate;                                                       //当前交易对的价格
    std::shared_ptr<std::vector<std::string>> p_pair_list;                          //交易所支持的交易对

    std::mutex mut_price_depth;                                                     //互斥量 给修改当前交易对价格深度加锁，防止同时修改

public:

    virtual std::shared_ptr<std::vector<std::string>> print_market_list()=0;        //输出交易对列表

    virtual double print_pair_rate(const std::string pair_str)=0;                   //输出某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
                    print_pair_depth(const std::string pair_str)=0;                 //输出某个交易对的深度

    virtual void send_to_market(const Trade & trade_data)=0;                        //发送交易接口
    virtual double print_balance(const std::string symbol)=0;                         //获取代币余额
    std::string get_exchange_name(){                                                //获取交易所名称
        return exchange_name;
    }
    std::mutex& get_mutex(){
        return mut_price_depth;
    }
    virtual ~ExchangeFac()  = default;
};


#endif //AUTOTRACE_EXCHANGEFAC_H
