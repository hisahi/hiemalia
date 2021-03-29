/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logger.cc: implementation of logger

#include "logger.hh"

#include <iomanip>

#include "defs.hh"
#include "secure.hh"

namespace hiemalia {
Logger logger{};
bool logger_ok = true;

constexpr const char iso8601[] = "%FT%T%z";

void Logger::log_(LogLevel level, const char *file, size_t line,
                  const std::string &s) {
    std::time_t t_now = std::time(nullptr);
    std::tm tm_now = *s_localtime(&t_now);
    for (auto &handler : handlers_)
        handler->handle(level, tm_now, file, line, s);
}

static std::string levelName(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::TRACE:
            return "trac";
        case LogLevel::DEBUG:
            return "dbg ";
        case LogLevel::INFO:
            return "info";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERR ";
        case LogLevel::FAIL:
            return "!!! ";
    }
    never("invalid log level");
}

void StdLogHandler::handle(LogLevel level, const std::tm &tm, const char *file,
                           size_t line, const std::string &msg) {
    if (level >= minimumLevel_)
        std::cerr << "[" << levelName(level) << " : " << file << ":" << line
                  << " @ " << std::put_time(&tm, iso8601) << "] " << msg
                  << std::endl;
}

void FileLogHandler::handle(LogLevel level, const std::tm &tm, const char *file,
                            size_t line, const std::string &msg) {
    if (level >= minimumLevel_)
        *stream_ << "[" << levelName(level) << " : " << file << ":" << line
                 << " @ " << std::put_time(&tm, iso8601) << "] " << msg
                 << std::endl;
}

}  // namespace hiemalia
