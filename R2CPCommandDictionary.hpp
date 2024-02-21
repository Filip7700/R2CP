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

// R2CP built-in parameters
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



typedef uint32_t R2CPCommandCodeType;
typedef uint32_t R2CPParameterCodeType;
typedef uint32_t R2CPCommandContentType;



namespace R2CP {

// Singleton class
class R2CPCommandDictionary final {
private:
    std::set<R2CPCommandCodeType> commandsset;
    std::set<R2CPParameterCodeType> parametersset;
    std::map<R2CPCommandCodeType, std::set<R2CPParameterCodeType>> commandparametersmap;
    std::map<R2CPCommandCodeType, R2CPCommandContentType> commandcontenttypemap;

    // Defualt constructor
    R2CPCommandDictionary();

public:
    R2CPCommandDictionary(R2CPCommandDictionary const&) = delete;
    void operator=(R2CPCommandDictionary const&) = delete;

    // Getters
    static R2CPCommandDictionary& getinstance();
    std::set<R2CPParameterCodeType> getcommandparameters(R2CPCommandCodeType);
    R2CPCommandContentType getcommandcontenttype(R2CPCommandCodeType);
    bool checkifvalidcommand(R2CPCommandCodeType);
};

} // End of namespace: R2CP
