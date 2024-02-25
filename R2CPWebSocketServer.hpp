#pragma once

#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_THREAD_

#include <memory>
#include <thread>

#include <cstdint>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"



namespace R2CP {

class R2CPWebSocketServer {
public:
    R2CPWebSocketServer();
    R2CPWebSocketServer(const uint32_t);
    ~R2CPWebSocketServer();

    void run();
    void halt();
    void waituntilfinished();

private:
    static const uint32_t DEFAULTPOLLTIMEOUTINMILLISECONDS = 10U;

    bool isrunning;
    uint32_t polltimeoutinmilliseconds;
    std::thread *receptionthread;
    std::thread *transmissionthread;

    bool hasconnection;
    websocketpp::connection_hdl operatorconnectionhandle;
    websocketpp::server<websocketpp::config::asio> wsserver;

    void receiver2cppackettask();
    void transmitr2cppackettask();

    void waituntilfinished(std::thread *const);

    void closewsserverconnections();

    // WebSocket events
    void onmessage(
        websocketpp::server<websocketpp::config::asio>*,
        websocketpp::connection_hdl,
        websocketpp::server<websocketpp::config::asio>::message_ptr);
    void onopen(
        websocketpp::server<websocketpp::config::asio>*,
        websocketpp::connection_hdl);
    void onclose(
        websocketpp::server<websocketpp::config::asio>*,
        websocketpp::connection_hdl);
};

} // End of namespace: R2CP
