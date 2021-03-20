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

#include "str.hh"

namespace hiemalia {
enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FAIL };

class LogHandler {
   public:
    LogHandler(const LogHandler& copy) = default;
    LogHandler& operator=(const LogHandler& copy) = default;
    LogHandler(LogHandler&& move) = default;
    LogHandler& operator=(LogHandler&& move) = default;
    virtual void handle(LogLevel level, const std::tm& tm, const char* file,
                        size_t line, const std::string& msg) = 0;
    virtual ~LogHandler() {}

   protected:
    LogHandler() {}
};

class StdLogHandler : public LogHandler {
   public:
    StdLogHandler(LogLevel minimumLevel) : minimumLevel_(minimumLevel) {}
    void handle(LogLevel level, const std::tm& tm, const char* file,
                size_t line, const std::string& msg) override;
    ~StdLogHandler() {}

   private:
    LogLevel minimumLevel_;
};

class FileLogHandler : public LogHandler {
   public:
    FileLogHandler(std::unique_ptr<std::ostream> stream, LogLevel minimumLevel)
        : stream_(std::move(stream)), minimumLevel_(minimumLevel) {}
    void handle(LogLevel level, const std::tm& tm, const char* file,
                size_t line, const std::string& msg) override;
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

    template <typename... Ts>
    void log(const char* file, size_t line, LogLevel level,
             const std::string& fmt, Ts&&... args) {
        if constexpr (sizeof...(Ts) > 0)
            log_(level, file, line,
                 stringFormat(fmt, std::forward<Ts>(args)...));
        else
            log_(level, file, line, fmt);
    }
    template <typename... Ts>
    void trace(const char* file, size_t line, const std::string& fmt,
               Ts&&... args) {
        log(file, line, LogLevel::TRACE, fmt, std::forward<Ts>(args)...);
    }
    template <typename... Ts>
    void debug(const char* file, size_t line, const std::string& fmt,
               Ts&&... args) {
        log(file, line, LogLevel::DEBUG, fmt, std::forward<Ts>(args)...);
    }
    template <typename... Ts>
    void info(const char* file, size_t line, const std::string& fmt,
              Ts&&... args) {
        log(file, line, LogLevel::INFO, fmt, std::forward<Ts>(args)...);
    }
    template <typename... Ts>
    void warn(const char* file, size_t line, const std::string& fmt,
              Ts&&... args) {
        log(file, line, LogLevel::WARN, fmt, std::forward<Ts>(args)...);
    }
    template <typename... Ts>
    void error(const char* file, size_t line, const std::string& fmt,
               Ts&&... args) {
        log(file, line, LogLevel::ERROR, fmt, std::forward<Ts>(args)...);
    }
    template <typename... Ts>
    void fail(const char* file, size_t line, const std::string& fmt,
              Ts&&... args) {
        log(file, line, LogLevel::FAIL, fmt, std::forward<Ts>(args)...);
    }

    template <typename T, typename... Ts>
    void addHandler(Ts&&... args) {
        handlers_.emplace_back(std::make_unique<T>(std::forward<Ts>(args)...));
        debug(__FILE__, __LINE__,
              std::string("Added new logger ") + typeid(T).name());
    }

   private:
    void log_(LogLevel level, const char* file, size_t line,
              const std::string& s);
    LogHandlerContainer<LogHandlerPtr> handlers_;
};

extern Logger logger;
extern bool logger_ok;

#define LOG_ADD_HANDLER(T, ...) logger.addHandler<T>(__VA_ARGS__)
#define LOG_TRACE(...) \
    (logger_ok ? logger.trace(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
#define LOG_DEBUG(...) \
    (logger_ok ? logger.debug(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
#define LOG_INFO(...) \
    (logger_ok ? logger.info(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
#define LOG_WARN(...) \
    (logger_ok ? logger.warn(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
#define LOG_ERROR(...) \
    (logger_ok ? logger.error(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
#define LOG_FAIL(...) \
    (logger_ok ? logger.fail(__FILE__, __LINE__, __VA_ARGS__) : (void)0)
};  // namespace hiemalia

#endif  // M_LOGGER_HH
