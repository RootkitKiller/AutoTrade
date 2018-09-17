cmake_minimum_required(VERSION 3.5)
project(AutoTrace)

set(CMAKE_CXX_STANDARD 11)


IF(APPLE)
    link_directories("/usr/local/opt/openssl/lib")
    include_directories("/usr/local/opt/openssl/include")
ENDIF(APPLE)



add_executable(AutoTrace main.cpp ExchangeFac.cpp ExchangeFac.h GateioExchange.cpp GateioExchange.h
    HuobiproExchange.cpp HuobiproExchange.h hmac-sha256.cpp hmac-sha256.h base64.cpp base64.h ZBExchange.cpp ZBExchange.h BiboxExchange.cpp BiboxExchange.h)
IF(APPLE)
    target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread-mt)
ELSEIF(UNIX)
    target_link_libraries(AutoTrace cpprest ssl crypto boost_system boost_chrono boost_thread)
ENDIF()
