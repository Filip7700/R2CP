#pragma once

#include <cstdint>

#include "R2CPWebSocketServer.hpp"
#include "R2CPPacketProcessor.hpp"



namespace R2CP {

class R2CPRuntimeBroker {
public:
    R2CPRuntimeBroker();
    R2CPRuntimeBroker(const uint32_t, const uint32_t);
    ~R2CPRuntimeBroker();

    void run();

    R2CPWebSocketServer& getrobotserver();
    R2CPPacketProcessor& getpacketprocessor();

private:
    R2CPWebSocketServer robotserver;
    R2CPPacketProcessor packetprocessor;
};

} // End of namespace: R2CP
