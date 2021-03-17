/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logger.hh: header file for logger.cc

#ifndef M_LOGGER_HH
#define M_LOGGER_HH

#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace hiemalia {
enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FAIL };

class LogHandler {
   public:
    LogHandler(const LogHandler& copy) = default;
    LogHandler& operator=(const LogHandler& copy) = default;
    LogHandler(LogHandler&& move) = default;
    LogHandler& operator=(LogHandler&& move) = default;
    virtual void handle(LogLevel level, const std::tm& tm,
                        const std::string& msg) = 0;
    virtual ~LogHandler() {}

   protected:
    LogHandler() {}
};

class StdLogHandler : public LogHandler {
   public:
    StdLogHandler(LogLevel minimumLevel) : minimumLevel_(minimumLevel) {}
    void handle(LogLevel level, const std::tm& tm,
                const std::string& msg) override;
    ~StdLogHandler() {}

   private:
    LogLevel minimumLevel_;
};

class FileLogHandler : public LogHandler {
   public:
    FileLogHandler(std::unique_ptr<std::ostream> stream, LogLevel minimumLevel)
        : stream_(std::move(stream)), minimumLevel_(minimumLevel) {}
    void handle(LogLevel level, const std::tm& t,
                const std::string& msg) override;
    ~FileLogHandler() {}

   private:
    std::unique_ptr<std::ostream> stream_;
    LogLevel minimumLevel_;
};

class Logger {
   public:
    using LogHandlerPtr = std::unique_ptr<LogHandler>;
    template <typename T>
    using LogHandlerContainer = std::vector<T>;

    void log(LogLevel level, const std::string& s);
    void trace(const std::string& s) { log(LogLevel::TRACE, s); }
    void debug(const std::string& s) { log(LogLevel::DEBUG, s); }
    void info(const std::string& s) { log(LogLevel::INFO, s); }
    void warn(const std::string& s) { log(LogLevel::WARN, s); }
    void error(const std::string& s) { log(LogLevel::ERROR, s); }
    void fail(const std::string& s) { log(LogLevel::FAIL, s); }

    template <typename... Ts>
    void log_fmt(LogLevel level, const std::string& fmt, Ts&&... args) {
        log(level, string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void trace_fmt(const std::string& fmt, Ts&&... args) {
        trace(string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void debug_fmt(const std::string& fmt, Ts&&... args) {
        debug(string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void info_fmt(const std::string& fmt, Ts&&... args) {
        info(string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void warn_fmt(const std::string& fmt, Ts&&... args) {
        warn(string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void error_fmt(const std::string& fmt, Ts&&... args) {
        error(string_format(fmt, std::forward<Ts>(args)...));
    }
    template <typename... Ts>
    void fail_fmt(const std::string& fmt, Ts&&... args) {
        fail(string_format(fmt, std::forward<Ts>(args)...));
    }

    template <typename T, typename... Ts>
    void addHandler(Ts&&... args) {
        handlers_.emplace_back(std::make_unique<T>(std::forward<Ts>(args)...));
        debug(std::string("Added new logger ") + typeid(T).name());
    }

   private:
    LogHandlerContainer<LogHandlerPtr> handlers_;
};

extern Logger logger;
extern bool logger_ok;
};  // namespace hiemalia

#endif  // M_LOGGER_HH
