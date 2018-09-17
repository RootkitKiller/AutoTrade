cmake_minimum_required(VERSION 3.10)
project(AutoTrace)

set(CMAKE_CXX_STANDARD 11)


IF(APPLE)
    link_directories("/usr/local/opt/openssl/lib")
    include_directories("/usr/local/opt/openssl/include")
ELSEIF(UNIX)
    MESSAGE("UNIX SYSTERM")
ENDIF(APPLE)



add_executable(AutoTrace main.cpp ExchangeFac.cpp ExchangeFac.h GateioExchange.cpp GateioExchange.h
    HuobiproExchange.cpp HuobiproExchange.h hmac-sha256.cpp hmac-sha256.h base64.cpp base64.h ZBExchange.cpp ZBExchange.h BiboxExchange.cpp BiboxExchange.h)

target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread-mt)
