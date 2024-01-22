#pragma once

#include <set>
#include <map>
#include <cstdint>



// R2CP built-in commands
#define MOTORPWMENABLE  1U
#define MOTORPWMDISABLE 2U
#define MOTORIDLE       3U
#define MOTORDUTYCYCLE  4U
#define IMAGESEND       5U
#define IMAGERECEIVE    6U
#define ROBOTSHUTDOWN   7U
#define PRINTMESSAGE    8U
#define INVALIDCOMMAND  9U

// R2CP built-in arguments
#define NOPARAMETERS      0U
#define MOTORRIGHT        1U
#define MOTORLEFT         2U
#define IMAGEWIDTH        3U
#define IMAGEHEIGHT       4U
#define IMAGECHANNELS     5U
#define IMAGEDATA         6U
#define IMAGEBMP          7U
#define IMAGEPNG          8U
#define IMAGEJPEG         9U
#define IMAGEGIF         10U
#define INVALIDPARAMETER 11U

// R2CP command content types (command specific)
#define NOCONTENT            0U
#define DIRECTCONTENT        1U
#define PARAMETERIZEDCONTENT 2U



namespace R2CP {

// Singleton class
class R2CPCommandDictionary final {
private:
    std::set<uint32_t> commandsset;
    std::set<uint32_t> parametersset;
    std::map<uint32_t, std::set<uint32_t>> commandparametersmap;
    std::map<uint32_t, uint32_t> commandcontenttypemap;

    // Defualt constructor
    R2CPCommandDictionary();

public:
    R2CPCommandDictionary(R2CPCommandDictionary const&) = delete;
    void operator=(R2CPCommandDictionary const&) = delete;

    // Getters
    static R2CPCommandDictionary& getinstance();
    std::set<uint32_t> getcommandparameters(uint32_t);
    uint32_t getcommandcontenttype(uint32_t);
    bool checkifvalidcommand(uint32_t);
};

} // End of namespace: R2CP
