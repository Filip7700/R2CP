class R2CPCommandDictionary {
    constructor() {
        // R2CP built-in commands
        this.MOTORPWMENABLE  = 1;
        this.MOTORPWMDISABLE = 2;
        this.MOTORIDLE       = 3;
        this.MOTORDUTYCYCLE  = 4;
        this.IMAGESEND       = 5;
        this.IMAGERECEIVE    = 6;
        this.ROBOTSHUTDOWN   = 7;
        this.PRINTMESSAGE    = 8;
        this.INVALIDCOMMAND  = 9;

        // R2CP built-in arguments
        this.NOARGS          =  0;
        this.MOTORRIGHT      =  1;
        this.MOTORLEFT       =  2;
        this.IMAGEWIDTH      =  3;
        this.IMAGEHEIGHT     =  4;
        this.IMAGECHANNELS   =  5;
        this.IMAGEDATA       =  6;
        this.BMPIMAGE        =  7;
        this.PNGIMAGE        =  8;
        this.JPEGIMAGE       =  9;
        this.GIFIMAGE        = 10;
        this.INVALIDARGUMENT = 11;

        // R2CP command content types (command specific)
        this.NOCONTENT         = 0;
        this.DIRECTCONTENT     = 1;
        this.ARGUMENTEDCONTENT = 2;

        this.commands = new Set([
            this.MOTORPWMENABLE,
            this.MOTORPWMDISABLE,
            this.MOTORIDLE,
            this.MOTORDUTYCYCLE,
            this.IMAGESEND,
            this.IMAGERECEIVE,
            this.ROBOTSHUTDOWN,
            this.PRINTMESSAGE,
            this.INVALIDCOMMAND]);

        this.arguments = new Set([
            this.NOARGS,
            this.MOTORRIGHT,
            this.MOTORLEFT,
            this.IMAGEWIDTH,
            this.IMAGEHEIGHT,
            this.IMAGECHANNELS,
            this.IMAGEDATA,
            this.INVALIDARGUMENT]);

        this.commandargumentsdictionary = new Map([
            [this.MOTORPWMENABLE, new Set([this.NOARGS])],
            [this.MOTORPWMDISABLE, new Set([this.NOARGS])],
            [this.MOTORIDLE, new Set([this.NOARGS])],
            [this.MOTORDUTYCYCLE, new Set([this.MOTORRIGHT, this.MOTORLEFT])],
            [this.IMAGESEND, new Set([this.IMAGEWIDTH, this.IMAGEHEIGHT, this.IMAGECHANNELS, this.IMAGEDATA, this.BMPIMAGE, this.PNGIMAGE, this.JPEGIMAGE, this.GIFIMAGE])],
            [this.IMAGERECEIVE, new Set([this.IMAGEWIDTH, this.IMAGEHEIGHT, this.IMAGECHANNELS, this.IMAGEDATA, this.BMPIMAGE, this.PNGIMAGE, this.JPEGIMAGE, this.GIFIMAGE])],
            [this.ROBOTSHUTDOWN, new Set([this.NOARGS])],
            [this.PRINTMESSAGE, new Set([this.NOARGS])],
            [this.INVALIDCOMMAND, new Set([this.INVALIDARGUMENT])]
        ]);

        this.commandcontenttypedictionary = new Map([
            [this.MOTORPWMENABLE, this.NOCONTENT],
            [this.MOTORPWMDISABLE, this.NOCONTENT],
            [this.MOTORIDLE, this.NOCONTENT],
            [this.MOTORDUTYCYCLE, this.ARGUMENTEDCONTENT],
            [this.IMAGESEND, this.ARGUMENTEDCONTENT],
            [this.IMAGERECEIVE, this.ARGUMENTEDCONTENT],
            [this.ROBOTSHUTDOWN, this.NOCONTENT],
            [this.PRINTMESSAGE, this.DIRECTCONTENT],
            [this.INVALIDCOMMAND, this.NOCONTENT]
        ]);
    }



    getcommandarguments(commandcode) {
        let cc = commandcode;

        if(this.commands.has(commandcode) == false) {
            cc = this.INVALIDCOMMAND;
        }

        let commandarguments = this.commandargumentsdictionary.get(cc);

        return commandarguments;
    }



    getcommandcontenttype(commandcode) {
        let cc = commandcode;

        if(this.commands.has(commandcode) == false) {
            cc = this.INVALIDCOMMAND;
        }

        let commandcontenttype = this.commandcontenttypedictionary.get(cc);

        return commandcontenttype;
    }



    checkifvalidcommand(commandcode) {
        let isvalidcommand = this.commands.has(commandcode);
        return isvalidcommand;
    }
}







class R2CPArgument {
    constructor(argumentcode, argumentdata) {
        this.argumentcode = argumentcode;
        this.argumentdata = argumentdata;
    }

    getargumentcode() {
        let ac = this.argumentcode;
        return ac;
    }

    getargumentdata() {
        let ad = this.argumentdata;
        return ad;
    }

    getargumentdatadeepcopy() {
        let argumentdatadeepcopy = new Uint8Array(this.argumentdata);
        return argumentdatadeepcopy;
    }

    getargumentdatasize() {
        let argumentdatasize = this.argumentdata.length;
        return argumentdatasize;
    }
}







class R2CPCommand {
    constructor(commandcode, commandarguments) {
        this.commandcode = commandcode;
        this.commandarguments = commandarguments;
    }



    getcommandcode() {
        let cc = this.commandcode;
        return cc;
    }

    getcommandarguments() {
        let ca = this.commandarguments;
        return ca;
    }



    getcommandargumentsdeepcopy() {
        let commandargumentsdeepcopy = [];
        let argumentscount = this.commandarguments.length;

        for(let i = 0; i < argumentscount; i++) {
            let commandargument = this.commandarguments[i];
            let argumentcode = commandargument.getargumentcode();
            let argumentdata = commandargument.getargumentdatadeepcopy();
            commandargumentsdeepcopy.push(new R2CPArgument(argumentcode, argumentdata));
        }

        return commandargumentsdeepcopy;
    }



    getnumberofcommandarguments() {
        let numberofcommandarguments = this.commandarguments.length;
        return numberofcommandarguments;
    }
}







class R2CPParser {
    constructor(commanddictionary) {
        this.MINR2CPPACKETSIZE = 8;
        this.R2CPMAGICNUMBER = 1379025744;
        this.RRCPMAGICNUMBER = 1381122896;

        this.parsingoffsetstack = [];
        this.commanddictionary = commanddictionary;

        console.log("R2CP parser initialized.");
    }



    parser2cppacket(base64encodedr2cppacket) {
        let commands = [];

        let isr2cppacketvalid = this.validater2cppacket(base64encodedr2cppacket);

        if(isr2cppacketvalid == true) {
            this.parsingoffsetstack.push(0);

            let r2cppacket = this.decodebase64encodedr2cppacket(base64encodedr2cppacket);
            let r2cppacketsize = r2cppacket.length;

            if(r2cppacketsize > this.MINR2CPPACKETSIZE) {
                this.skipbytesfromr2cppacket(this.MINR2CPPACKETSIZE);

                let commandscount = this.readr2cppacketas32bitint(r2cppacket);

                for(let i = 0; i < commandscount; i++) {
                    let commandcode = this.readr2cppacketas32bitint(r2cppacket);
                    let commandsize = this.readr2cppacketas32bitint(r2cppacket);
                    let commandcontent = this.readbytesfromr2cppacket(r2cppacket, commandsize);

                    try {
                        let iscommandcodevalid = this.commanddictionary.checkifvalidcommand(commandcode);

                        if(iscommandcodevalid == false) {
                            throw "Error. Invalid command code detected.";
                        }
                        else {
                            let commandarguments = this.parsecommandcontent(commandcode, commandcontent);
                            commands.push(new R2CPCommand(commandcode, commandarguments));
                        }
                    }
                    catch(err) {
                        console.log(err.message);
                        console.log(
                            "Error parsing command content of command code: " +
                            commandcode.toString() +
                            " Faulty command discarded.");
                    }
                }
            }

            this.clearparsingoffsetstack();
        }
        else {
            throw "Parsing failed. Invalid R2CP packet.";
        }

        return commands;
    }



    validater2cppacket(base64encodedr2cppacket) {
        let isr2cppacketvalid = false;

        this.parsingoffsetstack.push(0);

        let r2cppacket = this.decodebase64encodedr2cppacket(base64encodedr2cppacket);
        let r2cppacketsize = r2cppacket.length;

        if(r2cppacketsize >= this.MINR2CPPACKETSIZE) {
            let magicnumber = this.readr2cppacketas32bitint(r2cppacket);

            if(magicnumber == this.R2CPMAGICNUMBER || magicnumber == this.RRCPMAGICNUMBER) {
                let totalr2cppacketsize = this.readr2cppacketas32bitint(r2cppacket);

                if(r2cppacketsize == this.MINR2CPPACKETSIZE + totalr2cppacketsize) {
                    if(totalr2cppacketsize == 0) {
                        // R2CP packet is valid, it's just empty
                        isr2cppacketvalid = true;
                    }
                    else {
                        let r2cpcommands = this.readbytesfromr2cppacket(r2cppacket, totalr2cppacketsize);
                        isr2cppacketvalid = this.validater2cppacketcommands(r2cpcommands);
                    }
                }
                else {
                    console.log(
                        "Error in validating R2CP packet. " +
                        "R2CP packet sizes don't match. " +
                        "r2cppacket.length: " + r2cppacketsize.toString() +
                        ". total R2CP packet size + min R2CP packet size: " +
                        (totalr2cppacketsize + this.MINR2CPPACKETSIZE).toString());
                }
            }
            else {
                console.log(
                    "Error in validating R2CP packet. " +
                    "Packet size is less than minimum valid R2CP packet size (" +
                    this.MINR2CPPACKETSIZE.toString() +
                    ").");
            }
        }

        this.clearparsingoffsetstack();

        return isr2cppacketvalid;
    }



    parsecommandcontent(commandcode, commandcontent) {
        let commandarguments = [];
        let commandcontenttype = this.commanddictionary.getcommandcontenttype(commandcode);

        if(commandcontenttype == this.commanddictionary.DIRECTCONTENT) {
            commandarguments.push(new R2CPArgument(this.commanddictionary.NOARGS, commandcontent));
        }
        else if(commandcontenttype == this.commanddictionary.ARGUMENTEDCONTENT) {
            try {
                commandarguments = this.parseargumentedcontent(commandcode, commandcontent);
            }
            catch(err) {
                throw err;
            }
        }
        // commandcontenttype == this.commanddictionary.NOCONTENT

        return commandarguments;
    }



    parseargumentedcommandcontent(commandcode, commandcontent) {
        let commandarguments = [];

        this.parsingoffsetstack.push(0);

        let argumentscount = this.readr2cppacketas32bitint(commandcontent);
        let validcommandarguments = this.commanddictionary.getcommandarguments(commandcode);
        let argumentcode = this.commanddictionary.NOARGS;

        let isinvalidargumentparsed = false;

        for(let i = 0; i < argumentscount && isinvalidargumentparsed == false; i++) {
            argumentcode = this.readr2cppacketas32bitint(commandcontent);

            if(validcommandarguments.has(argumentcode)) {
                let argumentsize = this.readr2cppacketas32bitint(commandcontent);
                let argumentcontent = this.readbytesfromr2cppacket(commandcontent, argumentsize);
                commandarguments.push(new R2CPArgument(argumentcode, argumentcontent));
            }
            else {
                isinvlaidargumentparsed = true;
            }
        }

        var _ = this.parsingoffsetstack.pop();

        if(isinvalidargumentparsed == true) {
            commandarguments.splice(0, commandarguments.length);

            let errstr = (
                "Error while parsing arumented command content. "
                + "Argument code "
                + argumentcode.toString()
                + " is not defined for command code "
                + commandcode.toString() + ".");

            throw errstr;
        }

        return commandarguments;
    }



    validater2cppacketcommands(r2cppacketcommands) {
        let isr2cppacketvalid = false;

        this.parsingoffsetstack.push(0);

        try {
            let commandscount = this.readr2cppacketas32bitint(r2cppacketcommands);
            let iscommandcontentvalidationfailed = false;

            for(let i = 0; i < commandscount && iscommandcontentvalidationfailed == false; i++) {
                let commandcode = this.readr2cppacketas32bitint(r2cppacketcommands);
                let commandsize = this.readr2cppacketas32bitint(r2cppacketcommands);
                let commandcontent = this.readbytesfromr2cppacket(r2cppacketcommands, commandsize);
                let iscommandcontentvalid = this.validater2cppacketcommandcontent(commandcode, commandcontent);

                if(iscommandcontentvalid == false) {
                    iscommandcontentvalidationfailed = true;
                }
            }

            if(iscommandcontentvalidationfailed == false) {
                isr2cppacketvalid = this.checkr2cppacketendisreached(r2cppacketcommands);
            }
        }
        catch(err) {
            console.log(
                "Error in validating R2CP packet. "
                + "R2CP packet commands content invalid. "
                + err.message);

            console.log(r2cppacketcommands);
        }

        var _ = this.parsingoffsetstack.pop();

        return isr2cppacketvalid;
    }



    validater2cppacketcommandcontent(commandcode, r2cpcommandcontent) {
        let isr2cppacketvalid = false;

        let commandcontenttype = this.commanddictionary.getcommandcontenttype(commandcode);

        if(commandcontenttype == this.commanddictionary.ARGUMENTEDCONTENT) {
            isr2cppacketvalid = this.validater2cppacketargumentedcommandcontent(r2cpcommandcontent);
        }
        else if(commandcontenttype == this.commanddictionary.NOCONTENT) {
            let r2cpcommandcontentsize = r2cpcommandcontent.length;

            if(r2cpcommandcontentsize == 0) {
                isr2cppacketvalid = true;
            }
        }
        else {
            // commandcontenttype == DIRECTCONTENT
            isr2cppacketvalid = true;
        }

        return isr2cppacketvalid;
    }



    validater2cppacketargumentedcommandcontent(r2cpcommandcontent) {
        let isr2cppacketvalid = false;

        this.parsingoffsetstack.push(0);

        try {
            let argumentscount = this.readr2cppacketas32bitint(r2cpcommandcontent);

            for(let i = 0; i < argumentscount; i++) {
                /* Skip argument code bytes, there is no use of argument code while validating.
                Only what matters is that those 4 bytes exist. */
                this.skipbytesfromr2cppacket(4);

                let argumentsize = this.readr2cppacketas32bitint(r2cpcommandcontent);

                /* Skip argument content bytes, there is no use of it while validating.
                Only what matters is that number of those bytes exist, which is read from previous 4 bytes. */
                this.skipbytesfromr2cppacket(argumentsize);
            }

            isr2cppacketvalid = this.checkr2cppacketendisreached(r2cpcommandcontent);
        }
        catch(err) {
            console.log(
                "Error in validating R2CP packet. "
                + "R2CP packet argumented command content is invalid. "
                + err.message);
        }

        var _ = this.parsingoffsetstack.pop();
    }



    checkr2cppacketendisreached(r2cppacket) {
        let isendreached = false;
        let r2cppacketsize = r2cppacket.length;

        let offset = this.parsingoffsetstack.pop();

        if(offset == r2cppacketsize) {
            isendreached = true;
        }

        this.parsingoffsetstack.push(offset);

        return isendreached;
    }



    readr2cppacketas32bitint(r2cppacket) {
        let intvalue = 0;
        let r2cppacketsize = r2cppacket.length;
        let isoutofmemory = true;

        let offset = this.parsingoffsetstack.pop();

        if(offset + 4 <= r2cppacketsize) {
            intvalue = (r2cppacket[offset] << 24) | (r2cppacket[offset + 1] << 16) | (r2cppacket[offset + 2] << 8) | r2cppacket[offset + 3];
            offset += 4;
            isoutofmemory = false;
        }

        this.parsingoffsetstack.push(offset);

        if(isoutofmemory == true) {
            throw "Error! R2CP packet has no more bytes left to read.";
        }

        return intvalue;
    }



    readbytesfromr2cppacket(r2cppacket, numberofbytestoread) {
        let readr2cppacketbytes = new Uint8Array(numberofbytestoread);
        readr2cppacketbytes.fill(0);

        let r2cppacketsize = r2cppacket.length;
        let isoutofmemory = true;

        let offset = this.parsingoffsetstack.pop();

        if(offset + numberofbytestoread <= r2cppacketsize) {
            for(let i = 0; i < numberofbytestoread; i++) {
                readr2cppacketbytes[i] = r2cppacket[offset + i];
            }

            offset += numberofbytestoread;
            isoutofmemory = false;
        }

        this.parsingoffsetstack.push(offset);

        if(isoutofmemory == true) {
            throw "Error! R2CP packet has no more bytes left to read.";
        }

        return readr2cppacketbytes;
    }



    skipbytesfromr2cppacket(numberofbytestoskip) {
        let offset = this.parsingoffsetstack.pop();
        offset += numberofbytestoskip;
        this.parsingoffsetstack.push(offset);
    }



    decodebase64encodedr2cppacket(base64encodedr2cppacket) {
        let r2cppacketstr = atob(base64encodedr2cppacket);
        let r2cppacketsize = r2cppacketstr.length;
        let r2cppacket = new Uint8Array(r2cppacketsize);

        for(let i = 0; i < r2cppacketsize; i++) {
            r2cppacket[i] = r2cppacketstr.charCodeAt(i);
        }

        return r2cppacket;
    }



    clearparsingoffsetstack() {
        while(this.parsingoffsetstack.length > 0) {
            var _ = this.parsingoffsetstack.pop();
        }
    }
}







class R2CPSerializer {
    constructor(commanddictionary) {
        this.MINR2CPPACKETSIZE = 8;
        this.R2CPMAGICNUMBER = 1379025744;
        this.RRCPMAGICNUMBER = 1381122896;

        this.commanddictionary = commanddictionary;
        this.serializingoffsetstack = [];

        console.log("R2CP serializer initialized.");
    }



    serializecommands(commands) {
        let r2cppacketsize = this.MINR2CPPACKETSIZE;
        let totalr2cppacketsize = 0;
        let commandscount = commands.length;

        this.serializingoffsetstack.push(0);

        if(commandscount > 0) {
            totalr2cppacketsize = this.precomputetotalr2cppacketsize(commands);
            r2cppacketsize += totalr2cppacketsize;
        }

        // Empty R2CP packet
        let r2cppacket = new Uint8Array(r2cppacketsize);
        this.write32bitinttor2cppacket(r2cppacket, this.R2CPMAGICNUMBER);

        if(commandscount > 0) {
            this.write32bitinttor2cppacket(r2cppacket, totalr2cppacketsize);
            this.write32bitinttor2cppacket(r2cppacket, commandscount);
        }
        else {
            this.write32bitinttor2cppacket(r2cppacket, 0);
        }

        for(let i = 0; i < commandscount; i++) {
            let command = commands[i];
            let commandcode = command.getcommandcode();
            let commandarguments = command.getcommandarguments();

            let commandargumentscount = commandarguments.length;

            if(commandargumentscount > 0) {
                if(commandargumentscount == 1) {
                    let commandargument = commandarguments[0];

                    if(commandargument.getargumentcode() == this.commanddictionary.NOARGS) {
                        // Serialize direct command content
                        let commanddata = commandargument.getargumentdata();
                        let commanddatasize = commanddata.length;

                        this.write32bitinttor2cppacket(r2cppacket, commandcode);
                        this.write32bitinttor2cppacket(r2cppacket, commanddatasize);
                        this.writebytestor2cppacket(r2cppacket, commanddata);
                    }
                    else {
                        // Serialize argumented command content
                        let commandcontent = this.serializeargumentedcommandcontent(commandarguments);
                        let commandcontentsize = commandcontent.length;

                        this.write32bitinttor2cppacket(r2cppacket, commandcode);
                        this.write32bitinttor2cppacket(r2cppacket, commandcontentsize);
                        this.writebytestor2cppacket(r2cppacket, commandcontent);
                    }
                }
                else {
                    // Serialize argumented command content
                    let commandcontent = this.serializeargumentedcommandcontent(commandarguments);
                    let commandcontentsize = commandcontent.length;

                    this.write32bitinttor2cppacket(r2cppacket, commandcode);
                    this.write32bitinttor2cppacket(r2cppacket, commandcontentsize);
                    this.writebytestor2cppacket(r2cppacket, commandcontent);
                }
            }
            else {
                this.write32bitinttor2cppacket(r2cppacket, commandcode);
                this.write32bitinttor2cppacket(r2cppacket, 0);
            }
        }

        this.clearserializingoffsetstack();

        let r2cppacketstr = String.fromCharCode.apply(null, r2cppacket);
        let base64encodedr2cppacket = btoa(r2cppacketstr);

        return base64encodedr2cppacket;
    }



    serializeargumentedcommandcontent(commandarguments) {
        let commandargumentscount = commandarguments.length;
        let commandcontentsize = this.precomputecommandcontentsize(commandarguments);
        let commandcontent = new Uint8Array(commandcontentsize);

        this.serializingoffsetstack.push(0);

        this.write32bitinttor2cppacket(commandcontent, commandargumentscount);

        for(let i = 0; i < commandargumentscount; i++) {
            let commandargument = commandarguments[i];

            let argumentcode = commandargument.getargumentcode();
            let argumentdata = commandargument.getargumentdata();
            let argumentdatasize = argumentdata.length;

            this.write32bitinttor2cppacket(commandcontent, argumentcode);
            this.write32bitinttor2cppacket(commandcontent, argumentdatasize);
            this.writebytestor2cppacket(commandcontent, argumentdata);
        }

        var _ = this.serializingoffsetstack.pop();

        return commandcontent;
    }



    precomputetotalr2cppacketsize(commands) {
        let commandscount = commands.length;

        // Total R2CP packet size theorem
        let totalr2cppacketsize = 4 + commandscount * 8;

        for(let i = 0; i < commandscount; i++) {
            let command = commands[i];
            let commandarguments = command.getcommandarguments();
            let commandargumentscount = commandarguments.length;

            if(commandargumentscount > 0) {
                if(commandargumentscount == 1) {
                    let commandargument = commandarguments[0];

                    if(commandargument.getargumentcode() == this.commanddictionary.NOARGS) {
                        // Direct command
                        let commanddatasize = commandargument.getargumentdatasize();
                        totalr2cppacketsize += commanddatasize;
                    }
                    else {
                        // Argumented command
                        let commandcontentsize = this.precomputecommandcontentsize(commandarguments);
                        totalr2cppacketsize += commancontentsize;
                    }
                }
                else {
                    // Argumented command
                    let commandcontentsize = this.precomputecommandcontentsize(commandarguments);
                    totalr2cppacketsize += commandcontentsize;
                }
            }
        }

        return totalr2cppacketsize;
    }



    precomputecommandcontentsize(commandarguments) {
        let commandargumentscount = commandarguments.length;

        // Size of argumented command theorem
        let commandcontentsize = 4 + commandargumentscount * 8;

        for(let i = 0; i < commandargumentscount; i++) {
            let commandargument = commandarguments[i];
            let argumentdatasize = commandargument.getargumentdatasize();

            commandcontentsize += argumentdatasize;
        }

        return commandcontentsize;
    }



    write32bitinttor2cppacket(r2cppacket, intval) {
        let offset = this.serializingoffsetstack.pop();
        let r2cppacketsize = r2cppacket.length;
        let isoutofmemory = true;

        if(offset + 4 <= r2cppacketsize) {
            r2cppacket[offset] = (intval & 4278190080) >> 24;
            r2cppacket[offset + 1] = (intval & 16711680) >> 16;
            r2cppacket[offset + 2] = (intval & 65280) >> 8;
            r2cppacket[offset + 3] = intval & 255;

            offset += 4;

            isoutofmemory = false;
        }

        this.serializingoffsetstack.push(offset);

        if(isoutofmemory == true) {
            let errstr = (
                "Error! R2CP packet has no more bytes left to write to. "
                + "offset: " + (offset + 4).toString() + " "
                + "R2CP packet size: " + r2cppacketsize.toString());

            throw errstr;
        }
    }



    writebytestor2cppacket(r2cppacket, arr) {
        let offset = this.serializingoffsetstack.pop();
        let r2cppacketsize = r2cppacket.length;
        let arrsize = arr.length;
        let isoutofmemory = true;

        if(offset + arrsize <= r2cppacketsize) {
            for(let i = 0; i < arrsize; i++) {
                r2cppacket[offset + i] = arr[i] & 255;
            }

            offset += arrsize;

            isoutofmemory = false;
        }

        this.serializingoffsetstack.push(offset);

        if(isoutofmemory == true) {
            let errstr = (
                "Error! R2CP packet has no more bytes left to write to. "
                + "offset: " + (offset + 4).toString() + " "
                + "R2CP packet size: " + r2cppacket.length.toString());

            throw errstr;
        }
    }



    clearserializingoffsetstack() {
        while(this.serializingoffsetstack.length > 0) {
            var _ = this.serializingoffsetstack.pop();
        }
    }
}
