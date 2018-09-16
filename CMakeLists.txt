cmake_minimum_required(VERSION 3.10)
project(AutoTrace)

set(CMAKE_CXX_STANDARD 11)

link_directories("/usr/local/opt/openssl/lib")
include_directories("/usr/local/opt/openssl/include")


add_executable(AutoTrace main.cpp Trace.cpp Trace.h ExchangeFac.cpp ExchangeFac.h GateioExchange.cpp GateioExchange.h
    HuobiproExchange.cpp HuobiproExchange.h hmac-sha256.cpp hmac-sha256.h base64.cpp base64.h)
target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread-mt)