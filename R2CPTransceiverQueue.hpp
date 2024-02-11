#pragma once

#include <cstdint>

#include <vector>
#include <mutex>



class R2CPTransceiverQueue final {
    /* R2CPTransceiverQueue is a singleton class.
    It's object can be used everywhere in code. */

private:
    static const uint32_t DEFAULTMAXQUEUESIZE = 16777216U;

    std::mutex receptionqueuemutex;
    std::mutex transmissionqueuemutex;

    std::vector<std::vector<uint8_t>> receptionqueue;
    std::vector<std::vector<uint8_t>> transmissionqueue;

    uint32_t maxreceptionqueuesize;
    uint32_t maxtransmissionqueuesize;
    uint32_t receptionqueuesize;
    uint32_t transmissionqueuesize;

    R2CPTransceiverQueue();
    ~R2CPTransceiverQueue();

public:
    static R2CPTransceiverQueue& getinstance();

    void pushtoreceptionqueue(const std::vector<uint8_t>&);
    void pushtotransmissionqueue(const std::vector<uint8_t>&);
    std::vector<uint8_t> popfromreceptionqueue();
    std::vector<uint8_t> popfromtransmissionqueue();

    void setmaxqueuesize(const uint32_t, const uint32_t);
    void setmaxreceptionqueuesize(const uint32_t);
    void setmaxtransmissionqueuesize(const uint32_t);

    uint32_t getreceptionqueuesize();
    uint32_t gettransmissionqueuesize();
    uint32_t getreceptionqueuer2cppacketscount();
    uint32_t gettransmissionqueuer2cppacketscount();
};
