//
// Created by mooninwater on 2018/9/15.
//

#ifndef AUTOTRACE_GATEIOEXCHANGE_H
#define AUTOTRACE_GATEIOEXCHANGE_H

#include <string>
#include <map>
#include "../ExchangeFac.h"
#include "../HttpRequest.h"

class GateioExchange :public ExchangeFac{
public:
    explicit GateioExchange(const std::string ApiKey,const std::string SerKey){
        rest_addr="https://data.gateio.io";
        exchange_name="gate.io";
        AccessKeyId=ApiKey;
        Secret_Key=SerKey;
    };

    virtual std::shared_ptr<std::vector<std::string>> print_market_list();          //输出交易所支持的交易对

    virtual double print_pair_rate(const std::string pair_str);                     //获取某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<exc_trade::Depth>>,std::shared_ptr<std::vector<exc_trade::Depth>>> \
                   print_pair_depth(const std::string pair_str);                    //输出某个交易对的深度

    virtual void send_to_market(const exc_trade::Trade &trade_data);                //发送交易
    virtual double print_balance(const std::string symbol);

private:
    void get_market_list(json::value json_result,std::shared_ptr<std::vector<std::string>> p_pair_list );
    void get_pair_rate(json::value json_result, double current_pair_rate);
    void get_pair_depth(json::value json_result,std::shared_ptr<std::vector<exc_trade::Depth>> p_asks_depth,\
                        std::shared_ptr<std::vector<exc_trade::Depth>> p_bids_depth);
    void get_trade_result(json::value json_result);
    void get_balance(json::value json_result,json::value m_balance);

    typedef  std::function<void(json::value json_result)> callFunction;
};


#endif //AUTOTRACE_GATEIOEXCHANGE_H
