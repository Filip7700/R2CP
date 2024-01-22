#include <stdexcept>

#include "R2CPSerializer.hpp"
#include "R2CPCommandDictionary.hpp"



R2CP::R2CPSerializer::R2CPSerializer() {}



std::vector<uint8_t> R2CP::R2CPSerializer::serializecommands(std::vector<R2CP::R2CPCommand> &commands) {
    uint32_t numberofbytesinr2cppacket = this->MINR2CPPACKETSIZE;
    uint32_t r2cppacketsize = 0U;
    uint32_t commandscount = commands.size();

    this->serializingoffsetstack.push_back(0U);

    if(commandscount > 0U) {
        r2cppacketsize = this->precomputer2cppacketsize(commands);
        numberofbytesinr2cppacket += r2cppacketsize;
    }

    // Empty R2CP packet
    std::vector<uint8_t> r2cppacket(numberofbytesinr2cppacket, 0U);

    this->writeuin32tor2cppacket(r2cppacket, this->R2CPMAGICNUMBER);

    if(commandscount > 0U) {
        this->writeuin32tor2cppacket(r2cppacket, r2cppacketsize);
        this->writeuin32tor2cppacket(r2cppacket, commandscount);
    }
    else {
        this->writeuin32tor2cppacket(r2cppacket, 0U);
    }

    for(R2CP::R2CPCommand &command: commands) {
        uint32_t commandcode = command.getcommandcode();
        std::vector<R2CP::R2CPParameter> &commandparameters = command.getcommandparameters();
        uint32_t commandparameterscount = command.getnumberofcommandparameters();

        if(commandparameterscount > 0U) {
            if(commandparameterscount == 1U) {
                R2CP::R2CPParameter &commandparameter = commandparameters.at(0U);
                uint32_t parametercode = commandparameter.getparametercode();

                if(parametercode == NOPARAMETERS) {
                    // Serialize direct command
                    std::vector<uint8_t> &commanddata = commandparameter.getparameterdata();
                    uint32_t commanddatasize = commandparameter.getparameterdatasize();

                    this->writeuin32tor2cppacket(r2cppacket, commandcode);
                    this->writeuin32tor2cppacket(r2cppacket, commanddatasize);
                    this->writebytestor2cppacket(r2cppacket, commanddata);
                }
                else {
                    // Serialize parameterized command content
                    std::vector<uint8_t> commandcontent = this->serializeparameterizedcommandcontent(commandparameters);
                    uint32_t commandcontentsize = commandcontent.size();

                    this->writeuin32tor2cppacket(r2cppacket, commandcode);
                    this->writeuin32tor2cppacket(r2cppacket, commandcontentsize);
                    this->writebytestor2cppacket(r2cppacket, commandcontent);
                }
            }
            else {
                // Serialize parameterized command content
                std::vector<uint8_t> commandcontent = this->serializeparameterizedcommandcontent(commandparameters);
                uint32_t commandcontentsize = commandcontent.size();

                this->writeuin32tor2cppacket(r2cppacket, commandcode);
                this->writeuin32tor2cppacket(r2cppacket, commandcontentsize);
                this->writebytestor2cppacket(r2cppacket, commandcontent);
            }
        }
        else {
            this->writeuin32tor2cppacket(r2cppacket, commandcode);
            this->writeuin32tor2cppacket(r2cppacket, 0U);
        }
    }

    this->serializingoffsetstack.clear();

    return r2cppacket;
}



std::vector<uint8_t> R2CP::R2CPSerializer::serializeparameterizedcommandcontent(std::vector<R2CP::R2CPParameter> &commandparameters) {
    uint32_t commandcontentsize = this->precomputecommandcontentsize(commandparameters);
    std::vector<uint8_t> commandcontent(commandcontentsize, 0U);

    this->serializingoffsetstack.push_back(0U);

    for(R2CP::R2CPParameter &commandparameter: commandparameters) {
        uint32_t parametercode = commandparameter.getparametercode();
        std::vector<uint8_t> &parameterdata = commandparameter.getparameterdata();
        uint32_t parameterdatasize = parameterdata.size();

        this->writeuin32tor2cppacket(commandcontent, parametercode);
        this->writeuin32tor2cppacket(commandcontent, parameterdatasize);
        this->writebytestor2cppacket(commandcontent, parameterdata);
    }

    this->serializingoffsetstack.pop_back();

    return commandcontent;
}



uint32_t R2CP::R2CPSerializer::precomputer2cppacketsize(std::vector<R2CP::R2CPCommand> &commands) {
    uint32_t commandscount = commands.size();

    // Total R2CP packet size theorem
    uint32_t r2cppacketsize = 4U + commandscount * 8U;

    for(R2CP::R2CPCommand command: commands) {
        std::vector<R2CP::R2CPParameter> &commandparameters = command.getcommandparameters();
        uint32_t commandparameterscount = command.getnumberofcommandparameters();

        if(commandparameterscount > 0U) {
            if(commandparameterscount == 1U) {
                R2CP::R2CPParameter &commandparameter = commandparameters.at(0U);
                uint32_t parametercode = commandparameter.getparametercode();

                if(parametercode == NOPARAMETERS) {
                    // Direct command
                    uint32_t commanddatasize = commandparameter.getparameterdatasize();
                    r2cppacketsize += commanddatasize;
                }
                else {
                    // Argumented command
                    uint32_t commandcontentsize = this->precomputecommandcontentsize(commandparameters);
                    r2cppacketsize += commandcontentsize;
                }
            }
            else {
                // Argumented command
                uint32_t commandcontentsize = this->precomputecommandcontentsize(commandparameters);
                r2cppacketsize += commandcontentsize;
            }
        }
    }

    return r2cppacketsize;
}



uint32_t R2CP::R2CPSerializer::precomputecommandcontentsize(std::vector<R2CP::R2CPParameter> &commandparameters) {
    uint32_t commandparameterscount = commandparameters.size();

    // Size of parameterized command theorem
    uint32_t commandcontentsize = 4U + commandparameterscount * 8U;

    for(R2CP::R2CPParameter commandparameter: commandparameters) {
        uint32_t parameterdatasize = commandparameter.getparameterdatasize();
        commandcontentsize += parameterdatasize;
    }

    return commandcontentsize;
}



void R2CP::R2CPSerializer::writeuin32tor2cppacket(std::vector<uint8_t> &r2cppacket, const uint32_t uint32value) {
    bool isoutofmemory = true;
    uint32_t numberofbytesinr2cppacket = r2cppacket.size();

    uint32_t offset = this->serializingoffsetstack.back();
    this->serializingoffsetstack.pop_back();

    if(offset + 4U <= numberofbytesinr2cppacket) {
        uint32_t offset1 = offset + 1U;
        uint32_t offset2 = offset + 2U;
        uint32_t offset3 = offset + 3U;

        r2cppacket[offset] = (uint32value & 4278190080U) >> 24U;
        r2cppacket[offset1] = (uint32value & 16711680U) >> 16U;
        r2cppacket[offset2] = (uint32value & 65280U) >> 8U;
        r2cppacket[offset3] = uint32value & 255U;

        offset += 4U;

        isoutofmemory = false;
    }

    this->serializingoffsetstack.push_back(offset);

    if(isoutofmemory) {
        throw std::out_of_range("R2CP packet has no more bytes left for writing.");
    }
}



void R2CP::R2CPSerializer::writebytestor2cppacket(std::vector<uint8_t> &r2cppacket, const std::vector<uint8_t> &writingbytes) {
    bool isoutofmemory = true;
    uint32_t numberofbytesinr2cppacket = r2cppacket.size();
    uint32_t numberofwritingbytes = writingbytes.size();

    uint32_t offset = this->serializingoffsetstack.back();
    this->serializingoffsetstack.pop_back();

    if(offset + numberofwritingbytes <= numberofbytesinr2cppacket) {
        for(uint32_t i = 0U; i < numberofwritingbytes; i++) {
            uint32_t offseti = offset + i;
            r2cppacket[offseti] = writingbytes.at(i);
        }

        offset += numberofwritingbytes;

        isoutofmemory = false;
    }

    this->serializingoffsetstack.push_back(offset);

    if(isoutofmemory) {
        throw std::out_of_range("R2CP packet has no more bytes left for writing.");
    }
}
