/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// logger.cc: implementation of logger

#include "logger.hh"

#include <ctime>
#include <iomanip>

namespace hiemalia {
Logger logger{};
bool logger_ok = true;

void Logger::log(LogLevel level, const std::string &s) {
    std::time_t t_now = std::time(nullptr);
    std::tm tm_now = *std::localtime(&t_now);
    for (auto &handler : handlers_) handler->handle(level, tm_now, s);
}

static std::string levelName(LogLevel level) noexcept {
    switch (level) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO ";
        case LogLevel::WARN:
            return "WARN ";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FAIL:
            return "FAIL ";
    }
    return "???";
}

void StdLogHandler::handle(LogLevel level, const std::tm &tm,
                           const std::string &msg) {
    if (level >= minimumLevel_)
        std::cerr << "[" << std::put_time(&tm, "%c") << " | "
                  << levelName(level) << "] " << msg << std::endl;
}

void FileLogHandler::handle(LogLevel level, const std::tm &tm,
                            const std::string &msg) {
    if (level >= minimumLevel_)
        *stream_ << "[" << std::put_time(&tm, "%c") << " | " << levelName(level)
                 << "] " << msg << std::endl;
}

}  // namespace hiemalia
