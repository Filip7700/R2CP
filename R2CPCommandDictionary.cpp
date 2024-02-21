#include "R2CPCommandDictionary.hpp"



R2CP::R2CPCommandDictionary::R2CPCommandDictionary() {
    this->commandsset.insert(MOTORPWMENABLE);
    this->commandsset.insert(MOTORPWMDISABLE);
    this->commandsset.insert(MOTORIDLE);
    this->commandsset.insert(MOTORDUTYCYCLE);
    this->commandsset.insert(IMAGESEND);
    this->commandsset.insert(IMAGERECEIVE);
    this->commandsset.insert(ROBOTSHUTDOWN);
    this->commandsset.insert(PRINTMESSAGE);
    this->commandsset.insert(INVALIDCOMMAND);

    this->parametersset.insert(NOPARAMETERS);
    this->parametersset.insert(MOTORRIGHT);
    this->parametersset.insert(MOTORLEFT);
    this->parametersset.insert(IMAGEWIDTH);
    this->parametersset.insert(IMAGEHEIGHT);
    this->parametersset.insert(IMAGECHANNELS);
    this->parametersset.insert(IMAGEDATA);
    this->parametersset.insert(INVALIDPARAMETER);

    // Helper variables for command parameters map
    std::set<R2CPParameterCodeType> noparametersset;
    noparametersset.insert(NOPARAMETERS);

    std::set<R2CPParameterCodeType> invalidparametersset;
    invalidparametersset.insert(INVALIDPARAMETER);

    std::set<R2CPParameterCodeType> motordutycycleparametersset;
    motordutycycleparametersset.insert(MOTORRIGHT);
    motordutycycleparametersset.insert(MOTORLEFT);

    std::set<R2CPParameterCodeType> imageparametersset;
    motordutycycleparametersset.insert(IMAGEWIDTH);
    motordutycycleparametersset.insert(IMAGECHANNELS);
    motordutycycleparametersset.insert(IMAGEDATA);
    motordutycycleparametersset.insert(IMAGEBMP);
    motordutycycleparametersset.insert(IMAGEPNG);
    motordutycycleparametersset.insert(IMAGEJPEG);
    motordutycycleparametersset.insert(IMAGEGIF);
    // End of helper variables for commandparameters map

    this->commandparametersmap[MOTORPWMENABLE]  = noparametersset;
    this->commandparametersmap[MOTORPWMDISABLE] = noparametersset;
    this->commandparametersmap[MOTORIDLE]       = noparametersset;
    this->commandparametersmap[MOTORDUTYCYCLE]  = motordutycycleparametersset;
    this->commandparametersmap[IMAGESEND]       = motordutycycleparametersset;
    this->commandparametersmap[IMAGERECEIVE]    = motordutycycleparametersset;
    this->commandparametersmap[ROBOTSHUTDOWN]   = noparametersset;
    this->commandparametersmap[PRINTMESSAGE]    = noparametersset;
    this->commandparametersmap[INVALIDCOMMAND]  = invalidparametersset;

    this->commandcontenttypemap[MOTORPWMENABLE]  = NOCONTENT;
    this->commandcontenttypemap[MOTORPWMDISABLE] = NOCONTENT;
    this->commandcontenttypemap[MOTORIDLE]       = NOCONTENT;
    this->commandcontenttypemap[MOTORDUTYCYCLE]  = PARAMETERIZEDCONTENT;
    this->commandcontenttypemap[IMAGESEND]       = PARAMETERIZEDCONTENT;
    this->commandcontenttypemap[IMAGERECEIVE]    = PARAMETERIZEDCONTENT;
    this->commandcontenttypemap[ROBOTSHUTDOWN]   = NOCONTENT;
    this->commandcontenttypemap[PRINTMESSAGE]    = DIRECTCONTENT;
    this->commandcontenttypemap[INVALIDCOMMAND]  = NOCONTENT;
}



R2CP::R2CPCommandDictionary& R2CP::R2CPCommandDictionary::getinstance() {
    static R2CP::R2CPCommandDictionary instance;
    return instance;
}


std::set<R2CPParameterCodeType> R2CP::R2CPCommandDictionary::getcommandparameters(R2CPCommandCodeType commandcode) {
    std::set<R2CPParameterCodeType> commandparameters;

    std::set<R2CPCommandCodeType>::iterator it = this->commandsset.find(commandcode);

    if(it != this->commandsset.end()) {
        commandparameters = this->commandparametersmap[commandcode];
    }
    else {
        commandparameters.insert(INVALIDPARAMETER);
    }

    return commandparameters;
}



R2CPCommandContentType R2CP::R2CPCommandDictionary::getcommandcontenttype(R2CPCommandCodeType commandcode) {
    R2CPCommandContentType commandcontenttype = NOCONTENT;

    std::set<R2CPCommandCodeType>::iterator it = this->commandsset.find(commandcode);

    if(it != this->commandsset.end()) {
        commandcontenttype = this->commandcontenttypemap[commandcode];
    }

    return commandcontenttype;
}



bool R2CP::R2CPCommandDictionary::checkifvalidcommand(R2CPCommandCodeType commandcode) {
    bool iscommandvalid = false;

    std::set<R2CPCommandCodeType>::iterator it = this->commandsset.find(commandcode);

    if(commandcode != INVALIDCOMMAND && it != this->commandsset.end()) {
        iscommandvalid = true;
    }

    return iscommandvalid;
}
