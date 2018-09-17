//
// Created by mooninwater on 2018/9/17.
//

#ifndef AUTOTRACE_BIBOXEXCHANGE_H
#define AUTOTRACE_BIBOXEXCHANGE_H

#include "ExchangeFac.h"

class BiboxExchange : public ExchangeFac {
public:
    explicit BiboxExchange(const std::string ApiKey,const std::string SerKey){
        rest_addr="https://api.bibox.com";
        exchange_name="Bibox";
        p_client=std::make_shared<http_client>("https://api.bibox.com");
        AccessKeyId=ApiKey;
        Secret_Key=SerKey;
        p_asks_depth=std::make_shared<std::vector<Depth>>();
        p_bids_depth=std::make_shared<std::vector<Depth>>();
        current_pair_rate=0;
        p_pair_list=std::make_shared<std::vector<std::string>>();
    }
    virtual std::shared_ptr<std::vector<std::string>> print_market_list();    //输出交易对列表

    virtual double print_pair_rate(const std::string pair_str);               //输出某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
                    print_pair_depth(const std::string pair_str);             //输出某个交易对的深度

    virtual void send_to_market(const Trade & trade_data);                    //发送交易接口
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


#endif //AUTOTRACE_BIBOXEXCHANGE_H
