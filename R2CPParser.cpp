#include <stdexcept>
#include <sstream>

#include "Logger.hpp"

#include "R2CPParser.hpp"



R2CP::R2CPParser::R2CPParser() {}



std::vector<R2CP::R2CPCommand> R2CP::R2CPParser::parser2cppacket(const std::vector<uint8_t> &r2cppacket) {
    R2CP::R2CPCommandDictionary &rcdref = R2CP::R2CPCommandDictionary::getinstance();

    std::vector<R2CP::R2CPCommand> commands;

    bool isr2cppacketvalid = this->validater2cppacket(r2cppacket);

    if(isr2cppacketvalid) {
        this->parsingoffsetstack.push_back(0U);

        this->skipbytesfromr2cppacket(R2CP::R2CPParser::MINR2CPPACKETSIZE);
        uint32_t commandscount = this->readr2cppacketasuint32(r2cppacket);

        for(uint32_t i = 0U; i < commandscount; i++) {
            uint32_t commandcode = this->readr2cppacketasuint32(r2cppacket);
            uint32_t commandsize = this->readr2cppacketasuint32(r2cppacket);

            bool iscommandcodevalid = rcdref.checkifvalidcommand(commandcode);

            if(iscommandcodevalid == false) {
                this->skipbytesfromr2cppacket(commandsize);
                Logger::logwarning("Invalid command code detected: %u. Faulty command parsing has been skipped.", commandcode);
            }
            else {
                std::vector<uint8_t> commandcontent = this->readbytesfromr2cppacket(r2cppacket, commandsize);

                try {
                    std::vector<R2CP::R2CPParameter> parameters = this->parsecommandcontent(commandcode, commandcontent);
                    R2CP::R2CPCommand parsedcommand{commandcode, parameters};
                    commands.push_back(parsedcommand);
                }
                catch(std::invalid_argument &e) {
                    Logger::logerror(
                        "%s. "
                        "Error parsing command content of command code: %u. "
                        "Faulty command discarded.",
                        e.what(), commandcode);
                }
            }
        }

        this->parsingoffsetstack.clear();
    }
    else {
        throw std::invalid_argument("Parsing failed. Invalid R2CP packet.");
    }

    return commands;
}



bool R2CP::R2CPParser::validater2cppacket(const std::vector<uint8_t> &r2cppacket) {
    bool isr2cppacketvalid = false;

    this->parsingoffsetstack.push_back(0U);

    uint32_t numberofbytesinr2cppacket = r2cppacket.size();

    if(numberofbytesinr2cppacket >= R2CP::R2CPParser::MINR2CPPACKETSIZE) {
        uint32_t magicnumber = this->readr2cppacketasuint32(r2cppacket);

        if(magicnumber == R2CP::R2CPParser::R2CPMAGICNUMBER || magicnumber == R2CP::R2CPParser::RRCPMAGICNUMBER) {
            uint32_t r2cppacketsize = this->readr2cppacketasuint32(r2cppacket);

            if(numberofbytesinr2cppacket == R2CP::R2CPParser::MINR2CPPACKETSIZE + r2cppacketsize) {
                if(r2cppacketsize == 0) {
                    isr2cppacketvalid = true;
                }
                else {
                    std::vector<uint8_t> r2cppacketcommands = this->readbytesfromr2cppacket(r2cppacket, r2cppacketsize);
                    isr2cppacketvalid = this->validatecommandsfromr2cppacket(r2cppacketcommands);
                }
            }
            else {
                Logger::logerror(
                    "Error in validating R2CP packet. "
                    "R2CP packet size (%u) + 8 (bytes containing magic number and R2CP packet isz itself) "
                    "don't match total number of bytes in R2CP packet. (%u)",
                    r2cppacketsize,
                    numberofbytesinr2cppacket);
            }
        }
        else {
            Logger::logerror(
                "Error in validating R2CP packet. "
                "Invalid magic number (%u). "
                "Magic number should be either %u or %u.",
                magicnumber,
                R2CP::R2CPParser::R2CPMAGICNUMBER,
                R2CP::R2CPParser::RRCPMAGICNUMBER);
        }
    }
    else {
        Logger::logerror(
            "Error in validating R2CP packet. "
            "Packet size is less than minimum valid R2CP packet size (%u)",
            R2CP::R2CPParser::MINR2CPPACKETSIZE);
    }

    this->parsingoffsetstack.clear();

    return isr2cppacketvalid;
}



std::vector<R2CP::R2CPParameter> R2CP::R2CPParser::parsecommandcontent(const uint32_t commandcode, const std::vector<uint8_t> &commandcontent) {
    R2CP::R2CPCommandDictionary &rcdref = R2CP::R2CPCommandDictionary::getinstance();
    std::vector<R2CP::R2CPParameter> parameters;

    uint32_t commandcontenttype = rcdref.getcommandcontenttype(commandcode);

    if(commandcontenttype == DIRECTCONTENT) {
        R2CP::R2CPParameter p{NOPARAMETERS, commandcontent};
        parameters.push_back(p);
    }
    else if(commandcontenttype == PARAMETERIZEDCONTENT) {
        try {
            parameters = this->parseparameterizedcommandcontent(commandcode, commandcontent);
        }
        catch(std::invalid_argument &e) {
            std::string errstr = e.what();
            throw std::invalid_argument(errstr);
        }
    }
    // else commandcontenttype == NOCONTENT

    return parameters;
}



std::vector<R2CP::R2CPParameter> R2CP::R2CPParser::parseparameterizedcommandcontent(const uint32_t commandcode, const std::vector<uint8_t> &commandcontent) {
    R2CP::R2CPCommandDictionary &rcdref = R2CP::R2CPCommandDictionary::getinstance();
    std::vector<R2CP::R2CPParameter> parameters;

    this->parsingoffsetstack.push_back(0U);

    uint32_t parameterscount = this->readr2cppacketasuint32(commandcontent);

    std::set<uint32_t> commandcodeparametercodes = rcdref.getcommandparameters(commandcode);

    bool isinvalidparameterparsed = false;
    uint32_t parametercode;

    for(uint32_t i = 0; i < parameterscount && !isinvalidparameterparsed; i++) {
        parametercode = this->readr2cppacketasuint32(commandcontent);

        std::set<uint32_t>::iterator it = commandcodeparametercodes.find(parametercode);
        if(it != commandcodeparametercodes.end()) {
            uint32_t parametersize = this->readr2cppacketasuint32(commandcontent);
            std::vector<uint8_t> parameterdata = readbytesfromr2cppacket(commandcontent, parametersize);

            R2CP::R2CPParameter p{parametercode, parameterdata};
            parameters.push_back(p);
        }
        else {
            isinvalidparameterparsed = true;
        }
    }

    this->parsingoffsetstack.pop_back();

    if(isinvalidparameterparsed) {
        parameters.clear();

        std::ostringstream errstrstream;
        errstrstream << "Error while parsing parameterized command content. ";
        errstrstream << "Parameter code " << parametercode;
        errstrstream << " is not defined for command code " << commandcode;
        errstrstream << ".";

        std::string errstr = errstrstream.str();

        throw std::invalid_argument(errstr);
    }

    return parameters;
}



bool R2CP::R2CPParser::validatecommandsfromr2cppacket(const std::vector<uint8_t> &r2cppacketcommands) {
    bool isr2cppacketvalid = false;
    this->parsingoffsetstack.push_back(0U);

    try {
        uint32_t commandscount = this->readr2cppacketasuint32(r2cppacketcommands);
        bool iscommandcontentvalidationfailed = false;

        for(uint32_t i = 0U; i < commandscount && !iscommandcontentvalidationfailed; i++) {
            uint32_t commandcode = this->readr2cppacketasuint32(r2cppacketcommands);
            uint32_t commandsize = this->readr2cppacketasuint32(r2cppacketcommands);
            std::vector<uint8_t> commandcontent = this->readbytesfromr2cppacket(r2cppacketcommands, commandsize);

            bool iscommandcontentvalid = this->validatecommandcontent(commandcode, commandcontent);

            if(!iscommandcontentvalid) {
                iscommandcontentvalidationfailed = true;
            }
        }

        if(!iscommandcontentvalidationfailed) {
            isr2cppacketvalid = this->checkifr2cppacketendisreached(r2cppacketcommands);
        }
    }
    catch(std::out_of_range &e) {
        Logger::logerror("Error in validating R2CP packet. Command sizes inside R2CP packet go beyond the R2CP packet size. %s", e.what());
    }

    this->parsingoffsetstack.pop_back();

    return isr2cppacketvalid;
}



bool R2CP::R2CPParser::validatecommandcontent(const uint32_t commandcode, const std::vector<uint8_t> &commandcontent) {
    R2CP::R2CPCommandDictionary &rcdref = R2CP::R2CPCommandDictionary::getinstance();
    bool isr2cppacketvalid = false;

    uint32_t commandcontenttype = rcdref.getcommandcontenttype(commandcode);

    if(commandcontenttype == PARAMETERIZEDCONTENT) {
        isr2cppacketvalid = this->validateparameterizedcommandcontent(commandcontent);
    }
    else if(commandcontenttype == NOCONTENT) {
        uint32_t commandcontentsize = commandcontent.size();

        if(commandcontentsize == 0U) {
            isr2cppacketvalid = true;
        }
    }
    else {
        // commandcontenttype == DIRECTCONTENT
        isr2cppacketvalid = true;
    }

    return isr2cppacketvalid;
}



bool R2CP::R2CPParser::validateparameterizedcommandcontent(const std::vector<uint8_t> &commandcontent) {
    bool isr2cppacketvalid = true;
    uint32_t parameterscount;

    this->parsingoffsetstack.push_back(0U);

    try {
        parameterscount = this->readr2cppacketasuint32(commandcontent);
    }
    catch(std::out_of_range &e) {
        isr2cppacketvalid = false;
        Logger::logerror("Error in validating R2CP packet. Content of parameterized command is invalid. %s", e.what());
    }

    for(uint32_t i = 0U; i < parameterscount && isr2cppacketvalid; i++) {
        try {
            /* Skip 4 bytes of parameter code, there is no use of parameter code while validating.
            Only what matters is that those 4 bytes exist. */
            this->skipbytesfromr2cppacket(4U);

            uint32_t parametersize = this->readr2cppacketasuint32(commandcontent);

            /* Skip parameter data bytes, there is no use of it while validating.
            Only what matters is that number of those bytes exist, which is read from previous 4 bytes (parameter size). */
            this->skipbytesfromr2cppacket(parametersize);
        }
        catch(std::out_of_range &e) {
            isr2cppacketvalid = false;
            Logger::logerror("Error in validating R2CP packet. Content of parameterized command is invalid. %s", e.what());
        }
    }

    if(isr2cppacketvalid) {
        isr2cppacketvalid = this->checkifr2cppacketendisreached(commandcontent);
    }

    this->parsingoffsetstack.pop_back();

    return isr2cppacketvalid;
}



bool R2CP::R2CPParser::checkifr2cppacketendisreached(const std::vector<uint8_t> &r2cppacket) {
    bool isendreached = false;
    uint32_t numberofbytesinr2cppacket = r2cppacket.size();

    uint32_t offset = this->parsingoffsetstack.back();

    if(offset == numberofbytesinr2cppacket) {
        isendreached = true;
    }

    return isendreached;
}



uint32_t R2CP::R2CPParser::readr2cppacketasuint32(const std::vector<uint8_t> &r2cppacket) {
    uint32_t uint32value = 0U;
    uint32_t numberofbytesinr2cppacket = r2cppacket.size();
    bool isoutofmemory = true;

    uint32_t offset = this->parsingoffsetstack.back();
    this->parsingoffsetstack.pop_back();

    if(offset + 4U <= numberofbytesinr2cppacket) {
        uint32value = r2cppacket.at(offset) << 24U | r2cppacket.at(offset + 1U) << 16U | r2cppacket.at(offset + 2U) << 8U | r2cppacket.at(offset + 3U);
        offset += 4U;
        isoutofmemory = false;
    }

    this->parsingoffsetstack.push_back(offset);

    if(isoutofmemory) {
        throw std::out_of_range("R2CP packet has no more bytes left to read.");
    }

    return uint32value;
}



std::vector<uint8_t> R2CP::R2CPParser::readbytesfromr2cppacket(const std::vector<uint8_t> &r2cppacket, const uint32_t numberofbytestoread) {
    std::vector<uint8_t> readr2cpbytes;
    uint32_t numberofbytesinr2cppacket = r2cppacket.size();
    bool isoutofmemory = true;

    uint32_t offset = this->parsingoffsetstack.back();
    this->parsingoffsetstack.pop_back();

    if(offset + numberofbytestoread <= numberofbytesinr2cppacket) {
        for(uint32_t i = 0U; i < numberofbytestoread; i++) {
            readr2cpbytes.push_back(r2cppacket.at(offset + i));
        }

        offset += numberofbytestoread;
        isoutofmemory = false;
    }

    this->parsingoffsetstack.push_back(offset);

    if(isoutofmemory) {
        throw std::out_of_range("R2CP packet has no more bytes left to read.");
    }

    return readr2cpbytes;
}



void R2CP::R2CPParser::skipbytesfromr2cppacket(const uint32_t numberofbytestoskip) {
    uint32_t offset = this->parsingoffsetstack.back();
    this->parsingoffsetstack.pop_back();

    offset += numberofbytestoskip;

    this->parsingoffsetstack.push_back(offset);
}
