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



typedef uint8_t LogSeverity;



class Logger final {
    /* Logger is a singleton class.
    It's object can be used everywhere in code. */

public:
    // Log severities
    static const LogSeverity NONE    = 0U;
    static const LogSeverity DEBUG   = 1U;
    static const LogSeverity INFO    = 2U;
    static const LogSeverity WARNING = 3U;
    static const LogSeverity ERROR   = 4U;
    static const LogSeverity FATAL   = 5U;

private:
    Logger();
    ~Logger();

    /* Private Class (Internal)
       ====================== */

    class FileLogger {
    private:
        std::string filelogname;
        std::fstream filelog;
        LogSeverity filelogseverity;

    public:
        FileLogger(const std::string&, const LogSeverity);
        ~FileLogger();
        void log(const std::string&, const LogSeverity);
    };

    /* Private Attributes
       ================== */

    const LogSeverity DEFAULTLOGSEVERITY = DEBUG;

    /* Severity used for standard output.
    Also it's used when logging using iostream. */
    LogSeverity stdoutlogseverity;

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
        lref.log(message, DEBUG);
    }

    template<class... Args>
    static void loginfo(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, INFO);
    }

    template<class... Args>
    static void logwarning(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, WARNING);
    }

    template<class... Args>
    static void logerror(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, ERROR);
    }

    template<class... Args>
    static void logfatal(const char *format, Args... arguments) {
        // Advanced Convinient Method (Formatted Output Support)
        std::string message = convertformattostring(format, arguments...);
        Logger &lref = Logger::getinstance();
        lref.log(message, FATAL);
    }

    template<class... Args>
    static std::string convertformattostring(const char *format, Args... arguments) {
        unsigned int formatsize = (unsigned int)strlen(format);
        unsigned int buffersize = formatsize + ((unsigned int)sizeof(arguments) + ... + 0U);

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
