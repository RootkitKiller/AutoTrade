//
// Created by mooninwater on 2018/9/15.
//

#ifndef AUTOTRACE_GATEIOEXCHANGE_H
#define AUTOTRACE_GATEIOEXCHANGE_H

#include <string>
#include <map>
#include <vector>
#include "ExchangeFac.h"

class GateioExchange :public ExchangeFac{
public:
    explicit GateioExchange(const std::string ApiKey,const std::string SerKey){
        rest_addr="https://data.gateio.io";
        exchange_name="gate.io";
        p_client=std::make_shared<http_client>(U("https://data.gateio.io"));
        current_pair_rate=0;
        AccessKeyId=ApiKey;
        Secret_Key=SerKey;
        p_pair_list=std::make_shared<std::vector<std::string>>();
        p_asks_depth=std::make_shared<std::vector<Depth>>();
        p_bids_depth=std::make_shared<std::vector<Depth>>();
    };

    virtual void get_market_list();                                                 //获取交易所支持的交易对
    virtual std::shared_ptr<std::vector<std::string>> print_market_list();          //输出交易所支持的交易对

    virtual double print_pair_rate(const std::string pair_str);                     //获取某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
                   print_pair_depth(const std::string pair_str);                    //输出某个交易对的深度

    virtual void send_to_market(const Trade& trade_data);                           //发送交易

private:
    std::string rest_addr;                                                          //Rest API 地址
    std::string exchange_name;                                                      //交易所名称
    std::shared_ptr<http_client> p_client;
    std::string AccessKeyId;
    std::string Secret_Key;
    std::shared_ptr<std::vector<Depth>> p_asks_depth;                               //当前交易对的买盘深度
    std::shared_ptr<std::vector<Depth>> p_bids_depth;                               //当前交易对的卖盘深度
    double current_pair_rate;                                                       //当前交易对的价格
    std::shared_ptr<std::vector<std::string>> p_pair_list;                          //交易所支持的交易对
};


#endif //AUTOTRACE_GATEIOEXCHANGE_H
