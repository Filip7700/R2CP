#include "R2CPTransceiverQueue.hpp"

#include <exception>



R2CPTransceiverQueue::R2CPTransceiverQueue() {
    this->maxreceptionqueuesize = R2CPTransceiverQueue::DEFAULTMAXQUEUESIZE;
    this->maxtransmissionqueuesize = R2CPTransceiverQueue::DEFAULTMAXQUEUESIZE;

    this->receptionqueuesize = 0U;
    this->transmissionqueuesize = 0U;
}



R2CPTransceiverQueue::~R2CPTransceiverQueue() {}



R2CPTransceiverQueue& R2CPTransceiverQueue::getinstance() {
    static R2CPTransceiverQueue instance;
    return instance;
}



void R2CPTransceiverQueue::pushtoreceptionqueue(const std::vector<uint8_t> &r2cppacket) {
    uint32_t r2cppacketsize = r2cppacket.size();

    uint32_t newreceptionqueuesize = this->getreceptionqueuesize() + r2cppacketsize;

    if(newreceptionqueuesize <= this->maxreceptionqueuesize) {
        std::scoped_lock receptionqueuelock(this->receptionqueuemutex);
        this->receptionqueue.push_back(r2cppacket);
        this->receptionqueuesize += r2cppacketsize;
    }
    else {
        throw std::overflow_error("Out of memory in reception queue.");
    }
}



void R2CPTransceiverQueue::pushtotransmissionqueue(const std::vector<uint8_t> &r2cppacket) {
    uint32_t r2cppacketsize = r2cppacket.size();

    uint32_t newtransmissionqueuesize = this->gettransmissionqueuesize() + r2cppacketsize;

    if(newtransmissionqueuesize <= this->maxtransmissionqueuesize) {
        std::scoped_lock transmissionqueuelock(this->transmissionqueuemutex);
        this->transmissionqueue.push_back(r2cppacket);
        this->transmissionqueuesize += r2cppacketsize;
    }
    else {
        throw std::overflow_error("Out of memory in transmission queue.");
    }
}



std::vector<uint8_t> R2CPTransceiverQueue::popfromreceptionqueue() {
    const uint32_t QUEUEPOPPOSITION = 0U;

    std::vector<uint8_t> r2cppacket;
    uint32_t receptionqueuer2cppacketscount = this->getreceptionqueuer2cppacketscount();

    if(receptionqueuer2cppacketscount > 0U) {
        std::scoped_lock receptionqueuelock(this->receptionqueuemutex);

        r2cppacket = this->receptionqueue.at(QUEUEPOPPOSITION);

        std::vector<std::vector<uint8_t>>::iterator it = this->receptionqueue.begin();
        this->receptionqueue.erase(it);

        uint32_t r2cppacketsize = r2cppacket.size();
        this->receptionqueuesize -= r2cppacketsize;
    }
    else {
        throw std::underflow_error("Reception queue has no R2CP packets to pop.");
    }

    return r2cppacket;
}



std::vector<uint8_t> R2CPTransceiverQueue::popfromtransmissionqueue() {
    const uint32_t QUEUEPOPPOSITION = 0U;

    std::vector<uint8_t> r2cppacket;
    uint32_t transmissionqueuer2cppacketscount = this->gettransmissionqueuer2cppacketscount();

    if(transmissionqueuer2cppacketscount > 0U) {
        std::scoped_lock transmissionqueuelock(this->transmissionqueuemutex);

        r2cppacket = this->transmissionqueue.at(QUEUEPOPPOSITION);

        std::vector<std::vector<uint8_t>>::iterator it = this->transmissionqueue.begin();
        this->transmissionqueue.erase(it);

        uint32_t r2cppacketsize = r2cppacket.size();
        this->transmissionqueuesize -= r2cppacketsize;
    }
    else {
        throw std::underflow_error("Transmission queue has no R2CP packets to pop.");
    }

    return r2cppacket;
}



void R2CPTransceiverQueue::setmaxqueuesize(const uint32_t newmaxreceptionqueuesize, const uint32_t newmaxtransmissionqueuesize) {
    this->maxreceptionqueuesize = newmaxreceptionqueuesize;
    this->maxtransmissionqueuesize = newmaxtransmissionqueuesize;
}



void R2CPTransceiverQueue::setmaxreceptionqueuesize(const uint32_t newmaxreceptionqueuesize) {
    this->maxreceptionqueuesize = newmaxreceptionqueuesize;
}



void R2CPTransceiverQueue::setmaxtransmissionqueuesize(const uint32_t newmaxtransmissionqueuesize) {
    this->maxtransmissionqueuesize = newmaxtransmissionqueuesize;
}



uint32_t R2CPTransceiverQueue::getreceptionqueuesize() {
    std::scoped_lock receptionqueuelock(this->receptionqueuemutex);
    uint32_t receptionqueuesizecopy = this->receptionqueuesize;

    return receptionqueuesizecopy;
}



uint32_t R2CPTransceiverQueue::gettransmissionqueuesize() {
    std::scoped_lock transmissionqueuelock(this->transmissionqueuemutex);
    uint32_t transmissionqueuesizecopy = this->transmissionqueuesize;

    return transmissionqueuesizecopy;
}



uint32_t R2CPTransceiverQueue::getreceptionqueuer2cppacketscount() {
    std::scoped_lock receptionqueuelock(this->receptionqueuemutex);
    uint32_t receptionqueuer2cppacketscount = this->receptionqueue.size();

    return receptionqueuer2cppacketscount;
}



uint32_t R2CPTransceiverQueue::gettransmissionqueuer2cppacketscount() {
    std::scoped_lock transmissionqueuelock(this->transmissionqueuemutex);
    uint32_t transmissionqueuer2cppacketscount = this->transmissionqueue.size();

    return transmissionqueuer2cppacketscount;
}
