#pragma once

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>

#include <vector>

// Time Functions
#include <chrono>

// Thread Safety
#include <mutex>

// C Libs
#include <cstdio>
#include <cstdint>
#include <cstring>



typedef uint8_t LogSeverityType;



class Logger final {
    /* Logger is a singleton class.
    It's object can be used everywhere in code. */

public:
    // Log severities
    static const LogSeverityType LOGSEVERITYNONE    = 0U;
    static const LogSeverityType LOGSEVERITYDEBUG   = 1U;
    static const LogSeverityType LOGSEVERITYINFO    = 2U;
    static const LogSeverityType LOGSEVERITYWARNING = 3U;
    static const LogSeverityType LOGSEVERITYERROR   = 4U;
    static const LogSeverityType LOGSEVERITYFATAL   = 5U;

private:
    Logger();
    ~Logger();

    /* Private Class (Internal)
       ====================== */

    class FileLogger {
    private:
        std::string filelogname;
        std::fstream filelog;
        LogSeverityType filelogseverity;

    public:
        FileLogger(const std::string&, const LogSeverityType);
        ~FileLogger();
        void log(const std::string&, const LogSeverityType);
    };

    /* Private Attributes
       ================== */

    const LogSeverityType DEFAULTLOGSEVERITY = LOGSEVERITYDEBUG;

    /* Severity used for standard output.
    Also it's used when logging using iostream. */
    LogSeverityType stdoutlogseverity;

    std::vector<FileLogger*> fileloggers;
    std::mutex loggermutex;

    /* Private Methods
       =============== */

    // Misc
    std::string gettimestring();
    std::string getlogseveritystring(const uint8_t);

    // Method for when using Logger with operator <<
    void logstream(const std::string&);

public:
    /* Public Methods
       ============== */

    // Main log method
    void log(const std::string&, const uint8_t);

    // File Logger Management
    void addfilelogger(const std::string&, const uint8_t);
    void removefileloggerat(const unsigned int);
    void clearfileloggers();

    // Public Class Methods
    // ====================

    // Singleton creation
    static Logger& getinstance();

    // Convinient Methods
    static void logdebug(const std::string&);
    static void loginfo(const std::string&);
    static void logwarning(const std::string&);
    static void logerror(const std::string&);
    static void logfatal(const std::string&);

    // Advanced Convinient Methods (Formatted Output Support)
    template<class... Args>
    static void LogDebug(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, LOGSEVERITYDEBUG);
    }

    template<class... Args>
    static void loginfo(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, LOGSEVERITYINFO);
    }

    template<class... Args>
    static void logwarning(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, LOGSEVERITYWARNING);
    }

    template<class... Args>
    static void logerror(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, LOGSEVERITYERROR);
    }

    template<class... Args>
    static void logfatal(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, LOGSEVERITYFATAL);
    }

    template<class... Args>
    static std::string convertformattostring(const char *format, Args... arguments) {
        unsigned int formatsize = (unsigned int)strlen(format);
        unsigned int buffersize = formatsize + ((unsigned int)sizeof(arguments) + ... + 0U);
        buffersize *= 2U;

        char *buffer = new char[buffersize];
        for(unsigned int i = 0U; i < buffersize; i++) {
            buffer[i] = '\0';
        }

        sprintf(buffer, format, arguments...);
        std::string formatedstring(buffer);

        // Free memory
        delete[] buffer;

        return formatedstring;
    }

    /* Setters
       ======= */

    void setstdoutlogseverity(const uint8_t);

    /* Operator Overloading
       ==================== */

    Logger& operator<<(std::ostream& (*pf)(std::ostream&)) {
        std::ostringstream messagestringstream;
        messagestringstream << pf;
        this->logstream(messagestringstream.str());

        return *this;
    }

    template<class T>
    Logger& operator<<(const T &argument) {
        std::stringstream messagestringstream;
        messagestringstream << argument;
        this->logstream(messagestringstream.str());

        return *this;
    }

    /* Removals
       ======== */

    Logger(Logger&) = delete;
    void operator=(const Logger&) = delete;
};
