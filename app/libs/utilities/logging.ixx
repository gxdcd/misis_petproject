module;

#pragma region Header
#include <Windows.h>
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
export module logging;
#pragma endregion

#define use_spdlog_chars(log_function)               \
    template<size_t N>                               \
    constexpr void log_function(const char(&msg)[N]) \
    {                                                \
        spdlog::log_function(msg);                   \
    }

#define use_spdlog_short(log_function)     \
    template<typename T>                   \
    inline void log_function(const T& msg) \
    {                                      \
        spdlog::log_function(msg);         \
    }

#define use_spdlog_long(log_function)                                                        \
    template<typename... Args>                                                               \
    inline void log_function(                                                                \
        const spdlog::format_string_t<Args...>&& fmt, Args&&... args) noexcept               \
    {                                                                                        \
        spdlog::log_function(std::forward<decltype(fmt)>(fmt), std::forward<Args>(args)...); \
    }

#define use_spdlog(log_function)    \
    use_spdlog_chars(log_function); \
    use_spdlog_short(log_function); \
    use_spdlog_long(log_function);

export namespace logging {
    use_spdlog(trace);
    use_spdlog(debug);
    use_spdlog(info);
    use_spdlog(warn);
    use_spdlog(error);
    use_spdlog(critical);
}

export struct logging_scope {
    [[nodiscard]] logging_scope(bool asynchronous);
    ~logging_scope();
    logging_scope(const logging_scope&) = delete;
    logging_scope(logging_scope&&) = delete;
    logging_scope& operator=(const logging_scope&) = delete;
    logging_scope& operator=(logging_scope&&) = delete;
};

module :private;

namespace details {
    void init_logger(bool async)
    {
        // https://github.com/ocornut/imgui/issues/4866#issuecomment-1006539446
        std::setlocale(LC_CTYPE, ".UTF8");
        SetConsoleOutputCP(CP_UTF8);

        if (async) {
            // Asynchronous logger with multi sinks
            // https://github.com/gabime/spdlog?tab=readme-ov-file#asynchronous-logger-with-multi-sinks
            spdlog::init_thread_pool(8192, 1);
            auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            // #include "spdlog/sinks/rotating_file_sink.h"
            // auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("mylog.txt", 1024 * 1024 * 10, 3);
            std::vector<spdlog::sink_ptr> sinks{stdout_sink /*, rotating_sink*/};
            auto logger = std::make_shared<spdlog::async_logger>("",
                sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);

            // Replace the default logger
            // https://github.com/gabime/spdlog?tab=readme-ov-file#replace-the-default-logger
            spdlog::set_default_logger(logger);
        }

        spdlog::set_level(spdlog::level::debug);

        // https://spdlog.docsforge.com/v1.x/api/spdlog/set_pattern/
        // https://spdlog.docsforge.com/v1.x/3.custom-formatting/#pattern-flags
        // spdlog::set_pattern("%H:%M:%S.%e [%l] [thread %t]: %^%v%$ %@");
        // spdlog::set_pattern("%H:%M:%S.%e %L: %^%v%$ %s:%#");
        // spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
        // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%s:%#] %v");
        // spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l]  %v");

        // eat error messages
        spdlog::set_error_handler([](const std::string& s) {
            spdlog::warn("Internal spdlog error: {}", s);
        });
    }

    void shutdown_logger()
    {
        spdlog::default_logger()->flush();
        spdlog::shutdown(); // sometimes causes a crash in std::atomic
    }

    void disable_logger()
    {
        spdlog::set_level(spdlog::level::off);
    }
}

logging_scope::logging_scope(bool asynchronous)
{
    details::init_logger(asynchronous);
}

logging_scope::~logging_scope()
{
    details::shutdown_logger();
}
