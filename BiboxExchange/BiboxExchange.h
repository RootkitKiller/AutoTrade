//
// Created by mooninwater on 2018/9/17.
//

#ifndef AUTOTRACE_BIBOXEXCHANGE_H
#define AUTOTRACE_BIBOXEXCHANGE_H

#include "../ExchangeFac.h"
#include "../HttpRequest.h"

class BiboxExchange : public ExchangeFac {
public:
    explicit BiboxExchange(const std::string ApiKey,const std::string SerKey){
        rest_addr="https://api.bibox.com";
        exchange_name="Bibox";
        AccessKeyId=ApiKey;
        Secret_Key=SerKey;
    }
    virtual std::shared_ptr<std::vector<std::string>> print_market_list();      //输出交易对列表

    virtual double print_pair_rate(const std::string pair_str);                 //输出某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>,std::shared_ptr<std::vector<exc_trade::Depth>>> \
                    print_pair_depth(const std::string pair_str);               //输出某个交易对的深度

    virtual void send_to_market(const exc_trade::Trade &trade_data);            //发送交易接口
    virtual double print_balance(const std::string symbol);

private:
    void get_market_list(json::value json_result,std::shared_ptr<std::vector<std::string>> p_pair_list);
    void get_pair_rate(json::value json_result,double &current_pair_rate);
    void get_pair_depth(json::value json_result,std::shared_ptr<std::vector<exc_trade::Depth>> p_asks_depth,\
                        std::shared_ptr<std::vector<exc_trade::Depth>> p_bids_depth);
    void get_trade_result(json::value json_result);
    void get_balance(json::value json_result,json::value &m_balance);

    typedef  std::function<void(json::value json_result)> callFunction;
};


#endif //AUTOTRACE_BIBOXEXCHANGE_H
