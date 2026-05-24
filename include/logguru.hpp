#ifndef LOGGURU_HPP
#define LOGGURU_HPP

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>

namespace logguru {

enum Verbosity {
    Verbosity_ERROR = 0,
    Verbosity_WARNING = 1,
    Verbosity_INFO = 2,
    Verbosity_DEBUG = 3
};

inline int& min_verbosity() {
    static int level = Verbosity_INFO;
    return level;
}

inline std::mutex& log_mutex() {
    static std::mutex mutex;
    return mutex;
}

inline const char* level_name(int verbosity) {
    switch (verbosity) {
        case Verbosity_ERROR:
            return "ERROR";
        case Verbosity_WARNING:
            return "WARNING";
        case Verbosity_INFO:
            return "INFO";
        case Verbosity_DEBUG:
            return "DEBUG";
        default:
            return "LOG";
    }
}

inline void set_verbosity(int verbosity) {
    min_verbosity() = verbosity;
}

inline void init(int&, char**) {
    if (const char* env = std::getenv("LOGGURU_LEVEL")) {
        if (std::strcmp(env, "DEBUG") == 0) {
            set_verbosity(Verbosity_DEBUG);
        } else if (std::strcmp(env, "INFO") == 0) {
            set_verbosity(Verbosity_INFO);
        } else if (std::strcmp(env, "WARNING") == 0) {
            set_verbosity(Verbosity_WARNING);
        } else if (std::strcmp(env, "ERROR") == 0) {
            set_verbosity(Verbosity_ERROR);
        }
    }
}

inline std::string vformat(const char* format, va_list args) {
    va_list copy;
    va_copy(copy, args);
    const int size = std::vsnprintf(nullptr, 0, format, copy);
    va_end(copy);

    if (size <= 0) {
        return {};
    }

    std::string message(static_cast<std::size_t>(size), '\0');
    std::vsnprintf(message.data(), message.size() + 1, format, args);
    return message;
}

inline void log_message(int verbosity, const char* file, int line, const char* format, ...) {
    if (verbosity > min_verbosity()) {
        return;
    }

    va_list args;
    va_start(args, format);
    const std::string message = vformat(format, args);
    va_end(args);

    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time {};
    localtime_r(&time, &local_time);

    char time_buffer[20];
    std::strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", &local_time);

    std::lock_guard<std::mutex> lock(log_mutex());
    std::fprintf(stderr, "%s %-7s %s:%d %s\n", time_buffer, level_name(verbosity), file, line, message.c_str());
    std::fflush(stderr);
}

inline void log_errno_message(int verbosity, const char* file, int line, const char* format, ...) {
    if (verbosity > min_verbosity()) {
        return;
    }

    const int saved_errno = errno;
    va_list args;
    va_start(args, format);
    std::string message = vformat(format, args);
    va_end(args);

    if (!message.empty()) {
        message += ": ";
    }
    message += std::strerror(saved_errno);

    log_message(verbosity, file, line, "%s", message.c_str());
}

}  // namespace logguru

#define ERROR ::logguru::Verbosity_ERROR
#define WARNING ::logguru::Verbosity_WARNING
#define INFO ::logguru::Verbosity_INFO
#define DEBUG ::logguru::Verbosity_DEBUG

#define LOG_F(level, format, ...) \
    ::logguru::log_message(level, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define PLOG_F(level, format, ...) \
    ::logguru::log_errno_message(level, __FILE__, __LINE__, format, ##__VA_ARGS__)

#endif
