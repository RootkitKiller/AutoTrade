//
// Created by mooninwater on 2018/9/15.
//

#ifndef AUTOTRACE_HUOBIPROEXCHANGE_H
#define AUTOTRACE_HUOBIPROEXCHANGE_H

#include "ExchangeFac.h"
#include <vector>

class HuobiproExchange : public ExchangeFac {
public:
    HuobiproExchange(const std::string ApiKey,const std::string SecKey){
        rest_addr="https://api.huobi.pro";
        exchange_name="Huobi.pro";
        AccessKeyId=ApiKey;
        Secret_Key=SecKey;
        p_client=std::make_shared<http_client>(U("https://api.huobi.pro"));
        current_pair_rate=0;
        p_pair_list=std::make_shared<std::vector<std::string>>();
    }

    virtual std::shared_ptr<std::vector<std::string>> print_market_list();          //输出交易对列表

    virtual double print_pair_rate(const std::string pair_str);                     //输出某个交易对的最新成交价(示例: BTC_ETH)
    virtual std::pair<std::shared_ptr<std::vector<Depth>>,std::shared_ptr<std::vector<Depth>>> \
                   print_pair_depth(const std::string pair_str);                    //输出某个交易对的深度

    virtual void send_to_market(const Trade & trade_data);                                //发送交易接口

private:
    inline std::string get_utctime(){
        std::time_t curr_time=time(NULL);
        char utc_time[80];
        auto sttime = gmtime(&curr_time);
        //2017-05-11T15%3A19%3A30
        strftime(utc_time,80,"%Y-%m-%dT%H%%3A%M%%3A%S",sttime);
        return utc_time;
    };
    char dec2hexChar(short int n) {
        if (0 <= n && n <= 9) {
            return char(short('0') + n);
        }
        else if (10 <= n && n <= 15) {
            return char(short('A') + n - 10);
        }
        else {
            return char(0);
        }
    }
    short int hexChar2dec(char c) {
        if ('0' <= c && c <= '9') {
            return short(c - '0');
        }
        else if ('a' <= c && c <= 'f') {
            return (short(c - 'a') + 10);
        }
        else if ('A' <= c && c <= 'F') {
            return (short(c - 'A') + 10);
        }
        else {
            return -1;
        }
    }
    std::string escapeURL(const std::string &URL){
        std::string result = "";
        for (unsigned int i = 0; i < URL.size(); i++) {
            char c = URL[i];
            if (
                    ('0' <= c && c <= '9') ||
                    ('a' <= c && c <= 'z') ||
                    ('A' <= c && c <= 'Z') ||
                    c == '/' || c == '.'
                    ) {
                result += c;
            }
            else {
                int j = (short int)c;
                if (j < 0) {
                    j += 256;
                }
                int i1, i0;
                i1 = j / 16;
                i0 = j - i1 * 16;
                result += '%';
                result += dec2hexChar(i1);
                result += dec2hexChar(i0);
            }
        }
        return result;
    }
    std::string deescapeURL(const std::string &URL) {
        std::string result = "";
        for (unsigned int i = 0; i < URL.size(); i++) {
            char c = URL[i];
            if (c != '%') {
                result += c;
            }
            else {
                char c1 = URL[++i];
                char c0 = URL[++i];
                int num = 0;
                num += hexChar2dec(c1) * 16 + hexChar2dec(c0);
                result += char(num);
            }
        }
        return result;
    }

    std::string rest_addr;                                                          //Rest API 地址
    std::string exchange_name;                                                      //交易所名称
    std::shared_ptr<http_client> p_client;
    std::string AccessKeyId;
    std::string Secret_Key;
    double current_pair_rate;                                                       //当前交易对的价格
    std::shared_ptr<std::vector<std::string>> p_pair_list;                          //交易所支持的交易对
};


#endif //AUTOTRACE_HUOBIPROEXCHANGE_H
