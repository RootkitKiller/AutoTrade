cmake_minimum_required(VERSION 3.5)
project(AutoTrace)

set(CMAKE_CXX_STANDARD 11)


IF(APPLE)
    link_directories("/usr/local/opt/openssl/lib")
    include_directories("/usr/local/opt/openssl/include")
ENDIF(APPLE)
IF(UNIX)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
ENDIF()

add_executable(AutoTrace main.cpp ExchangeFac.cpp ExchangeFac.h
        GateioExchange/GateioExchange.cpp GateioExchange/GateioExchange.h
        HuobiproExchange/HuobiproExchange.cpp HuobiproExchange/HuobiproExchange.h
        HuobiproExchange/hmac-sha256.cpp HuobiproExchange/hmac-sha256.h
        HuobiproExchange/base64.cpp HuobiproExchange/base64.h
        ZBExchange/ZBExchange.cpp ZBExchange/ZBExchange.h
        BiboxExchange/BiboxExchange.cpp BiboxExchange/BiboxExchange.h
        HttpRequest.cpp HttpRequest.h
        TradeBase.cpp TradeBase.h HedgingTrade/HedgingTrade.cpp HedgingTrade/HedgingTrade.h)
IF(APPLE)
    target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread-mt)
ELSEIF(UNIX)
    target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread)
ENDIF()
