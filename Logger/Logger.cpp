#include "Logger.hpp"



Logger::Logger() {
    this->stdoutlogseverity = this->DEFAULTLOGSEVERITY;
}



Logger::~Logger() {
    this->clearfileloggers();
}



std::string Logger::gettimestring() {
    time_t t1 = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto t2 = std::chrono::system_clock::now();

    const unsigned int PREDEFINEDTIMEFORMATSIZE = 30U;

    std::string timeformatstring(PREDEFINEDTIMEFORMATSIZE, '\0');
    std::strftime(&timeformatstring[0], PREDEFINEDTIMEFORMATSIZE, "%Y-%m-%d %H:%M:%S", std::localtime(&t1));

    auto duration = t2.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration) - std::chrono::duration_cast<std::chrono::seconds>(duration);

    unsigned int millisecondscount = (unsigned int)milliseconds.count();

    /* Extra string variable is needed since TimeFormatStr can have terminating characters ('\0') leftovers.
    It's because TimeFormatStr has size set to PredefinedTimeFormatSize.
    Assigning to new string variable removes terminating character leftovers. */
    std::string timestring = timeformatstring.c_str();

    timestring += ".";

    if(millisecondscount < 10U) {
        timestring += "00";
    }
    else if(millisecondscount < 100U) {
        timestring += "0";
    }

    timestring += std::to_string(millisecondscount);

    return timestring;
}



std::string Logger::getlogseveritystring(const LogSeverityType severity) {
    // None severity by default
    std::string logseveritystring = "[ NONE  ]";

    switch(severity) {
    case Logger::LOGSEVERITYDEBUG:
        logseveritystring = "[ DEBUG ]";
        break;
    case Logger::LOGSEVERITYINFO:
        logseveritystring = "[ INFO  ]";
        break;
    case Logger::LOGSEVERITYWARNING:
        logseveritystring = "[WARNING]";
        break;
    case Logger::LOGSEVERITYERROR:
        logseveritystring = "[ ERROR ]";
        break;
    case Logger::LOGSEVERITYFATAL:
        logseveritystring = "[ FATAL ]";
        break;
    default:
        break;
    }

    return logseveritystring;
}



void Logger::logstream(const std::string &message) {
    // Method for when using Logger with operator <<

    static std::string messagetolog;

    if(message == "\n") {
        this->log(messagetolog, this->stdoutlogseverity);
        messagetolog = "";
    }
    else {
        messagetolog += message;
    }
}



void Logger::log(const std::string &message, const LogSeverityType severity) {
    // Main log method

    std::string timestring = this->gettimestring();
    std::string severitystring = this->getlogseveritystring(severity);

    std::string entrystring = timestring;
    entrystring += " ";
    entrystring += severitystring;
    entrystring += " ";
    entrystring += message;

    // Lock the rest of method code, so only one thread can execute these lines
    std::scoped_lock loggerlock(this->loggermutex);

    // Output log entry to standard output
    if(severity > Logger::LOGSEVERITYNONE && this->stdoutlogseverity > Logger::LOGSEVERITYNONE && severity >= this->stdoutlogseverity) {
        std::cout << entrystring << std::endl;
    }

    for(std::vector<Logger::FileLogger*>::iterator it = this->fileloggers.begin(); it != this->fileloggers.end(); it++) {
        (*it)->log(entrystring, severity);
    }
}



void Logger::addfilelogger(const std::string &filename, const LogSeverityType severity) {
    std::scoped_lock loggerlock(loggermutex);
    this->fileloggers.push_back(new FileLogger(filename, severity));
}



void Logger::removefileloggerat(const unsigned int index) {
    unsigned int fileloggerscount = (unsigned int)this->fileloggers.size();

    if(fileloggerscount > 0U) {
        if(index >= 0U && index < fileloggerscount) {
            delete this->fileloggers.at(index);
            this->fileloggers.erase(this->fileloggers.begin() + index);
        }

        else {
            Logger::logwarning("File logger not removed, specified index (%u) out of range [0, %u>.", index, fileloggerscount);
        }
    }
    else {
        Logger::logwarning("Nothing to remove, no file loggers have been created...");
    }
}



void Logger::clearfileloggers() {
    for(std::vector<Logger::FileLogger*>::iterator it = this->fileloggers.begin(); it != this->fileloggers.end(); it++) {
        delete (*it);
    }

    this->fileloggers.clear();
}



Logger& Logger::getinstance() {
    // Singleton creation
    static Logger instance;
    return instance;
}



void Logger::logdebug(const std::string &message) {
    // Convinient Log Method
    Logger &lref = Logger::getinstance();
    lref.log(message, Logger::LOGSEVERITYDEBUG);
}



void Logger::loginfo(const std::string &message) {
    // Convinient Log Method
    Logger &lref = Logger::getinstance();
    lref.log(message, Logger::LOGSEVERITYINFO);
}



void Logger::logwarning(const std::string &message) {
    // Convinient Log Method
    Logger &lref = Logger::getinstance();
    lref.log(message, Logger::LOGSEVERITYWARNING);
}



void Logger::logerror(const std::string &message) {
    // Convinient Log Method
    Logger &lref = Logger::getinstance();
    lref.log(message, Logger::LOGSEVERITYERROR);
}



void Logger::logfatal(const std::string &message) {
    // Convinient Log Method
    Logger &lref = Logger::getinstance();
    lref.log(message, Logger::LOGSEVERITYFATAL);
}



void Logger::setstdoutlogseverity(const LogSeverityType severity) {
    this->stdoutlogseverity = severity;
}



Logger::FileLogger::FileLogger(const std::string &filename, const LogSeverityType severity) {
    this->filelogname = filename;
    this->filelogseverity = severity;
    this->filelog.open(filename, std::ios::out);
}



Logger::FileLogger::~FileLogger() {
    this->filelog.close();
}



void Logger::FileLogger::log(const std::string &message, const LogSeverityType severity) {
    if(severity > Logger::LOGSEVERITYNONE && this->filelogseverity > Logger::LOGSEVERITYNONE && severity >= this->filelogseverity) {
        this->filelog << message << std::endl;
    }
}
