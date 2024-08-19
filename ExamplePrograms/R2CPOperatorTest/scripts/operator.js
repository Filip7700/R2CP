var r2cpsocket = null;
var robotaddress = "";
const MINR2CPPACKETSIZE = 8;

var r2cpcommanddictionaryobj = new R2CPCommandDictionary();
var r2cpparserobj = new R2CPParser(r2cpcommanddictionaryobj);
var r2cpserializerobj = new R2CPSerializer(r2cpcommanddictionaryobj);

var pendingcommands = []

var joyposx = document.getElementById("joyposxinput");
var joyposy = document.getElementById("joyposyinput");
var joyangle = document.getElementById("joyangleinput");
var joymag = document.getElementById("joymaginput");
var motorrpwm = document.getElementById("motorrpwminput");
var motorlpwm = document.getElementById("motorlpwminput");

var robotsendinginterval = 0;



function sendpendingcommands() {
    makemotorcommandfromjoystickdata();

    let pendingcommandscount = pendingcommands.length

    if(pendingcommandscount > 0 && r2cpsocket != null) {
        let base64encodedr2cppacket = r2cpserializerobj.serializecommands(pendingcommands);
        clearpendingcommands();

        r2cpsocket.send(base64encodedr2cppacket);
        // console.log(base64encodedr2cppacket);
    }
}



function clearpendingcommands() {
    while(pendingcommands.length > 0) {
        var _ = pendingcommands.pop();
    }
}



function makemotorcommandfromjoystickdata() {
    let motorpwmright = motorrpwm.value;
    let motorpwmleft = motorlpwm.value;

    let motorpwmrightbytes = new Uint8Array(new Float32Array([motorpwmright]).buffer);
    let motorpwmleftbytes = new Uint8Array(new Float32Array([motorpwmleft]).buffer);

    let motorpwmrightargument = new R2CPArgument(r2cpcommanddictionaryobj.MOTORRIGHT, motorpwmrightbytes);
    let motorpwmleftargument = new R2CPArgument(r2cpcommanddictionaryobj.MOTORLEFT, motorpwmleftbytes);

    let motorpwmcommandarguments = [
        motorpwmrightargument,
        motorpwmleftargument];

    let motorpwmcommand = new R2CPCommand(r2cpcommanddictionaryobj.MOTORDUTYCYCLE, motorpwmcommandarguments);

    pendingcommands.push(motorpwmcommand);
}



// Create JoyStick object into the DIV 'joy1Div'
var shroom = new JoyStick("joydiv", {}, function(stickdata) {
    const sqrtof2 = 1.414213562373095;
    const deadzone = 0.15;

    let x = stickdata.x / 100.0;
    let y = stickdata.y / 100.0;

    let joyanglerads = Math.atan2(y, x);
    let joyangledegs = joyanglerads * 180.0 / Math.PI;
    let joymagval = clamp(Math.sqrt(x * x + y * y), 0.0, 1.0);

    if(joyangledegs < 0.0) {
        joyangledegs += 360.0;
    }

    joyposx.value = x;
    joyposy.value = y;
    joyangle.value = joyangledegs;
    joymag.value = joymagval;

    if(joymagval < deadzone) {
        motorrpwm.value = 0.0;
        motorlpwm.value = 0.0;
    }
    else {
        motorrpwm.value = 100 * joymagval * clamp(sqrtof2 * Math.sin(joyanglerads - Math.PI / 4.0), -1.0, 1.0);
        motorlpwm.value = 100 * joymagval * clamp(sqrtof2 * Math.cos(joyanglerads - Math.PI / 4.0), -1.0, 1.0);
    }
});



function clamp(n, min, max) {
    let cn = n;

    if(n < min) {
        cn = min;
    }
    else if(n > max) {
        cn = max;
    }

    return cn;
}



document.forms.r2cpconnectform.onsubmit = function() {
    if(r2cpsocket == null) {
        let ip = this.robotip.value;
        let port = this.robotport.value;

        robotaddress = "ws://" + ip + ":" + port;

        console.log(robotaddress);

        r2cpsocket = new WebSocket(robotaddress);

        r2cpsocket.onopen = establishr2cpconnection;
        r2cpsocket.onerror = handlefailedr2cpconnection;

        robotsendinginterval = setInterval(sendpendingcommands, 500);
    }
    else {
        r2cpsocket.close();
        r2cpsocket = null;
        clearInterval(robotsendinginterval);
        this.connectformbutton.value = "Connect";
        printmessage("Connection closed...");
    }

    return false;
}



// Send message from the form
document.forms.operatorsendform.onsubmit = function() {
    if(r2cpsocket == null) {
        printmessage("Can't send message. Not connected.");
    }
    else {
        let msg = this.message.value;
        let printmsgcmd = makeprintcommandfromstring(msg);

        pendingcommands.push(printmsgcmd);
    }

    this.message.value = "";

    return false;
}



function makeprintcommandfromstring(str) {
    let argumentdata = new TextEncoder("utf-8").encode(str);
    let argumentdatasize = argumentdata.length;

    let printcommandcode = r2cpcommanddictionaryobj.PRINTMESSAGE;
    let noargscode = r2cpcommanddictionaryobj.NOARGS;

    let printcommandarguments = [];
    let arg = new R2CPArgument(noargscode, argumentdata);

    printcommandarguments.push(arg);

    let cmd = new R2CPCommand(printcommandcode, printcommandarguments);

    return cmd;
}



function printmessage(msg) {
    let msgelement = document.createElement("p");
    msgelement.textContent = msg;
    document.getElementById("messagesarea").prepend(msgelement);
}



function printreceivedmessage(e) {
    try {
        let robotresponsecommands = r2cpparserobj.parser2cppacket(e.data);
        let robotresponsecommandscount = robotresponsecommands.length;

        for(let i = 0; i < robotresponsecommandscount; i++) {
            let cmd = robotresponsecommands[i];
            let commandcode = cmd.getcommandcode();

            if(commandcode == r2cpcommanddictionaryobj.PRINTMESSAGE) {
                let args = cmd.getcommandarguments();
                let arg = args[0];
                let msgbytes = arg.getargumentdatadeepcopy();

                // let msgstr = String.fromCharCode.apply(null, msgbytes);
                let msgstr = new TextDecoder("utf-8").decode(msgbytes);

                printmessage(msgstr);
            }
        }
    }
    catch(err) {
        console.log(err);
        printmessage(err);
    }
}



function establishr2cpconnection(event) {
   // Message received show the messages in message area
    r2cpsocket.onmessage = printreceivedmessage;

    document.forms.r2cpconnectform.connectformbutton.value = "Disconnect";
    printmessage("Connected to address: " + robotaddress);
}



function handlefailedr2cpconnection(event) {
    r2cpsocket.close();
    r2cpsocket = null;
    printmessage("Could not connect to address: " + robotaddress);
}
