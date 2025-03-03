#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <memory>
#include <string>

namespace PitchToMidi {

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Initialize the logger with console and file sinks
    void init(const std::string& loggerName = "PitchToMidi",
              const std::string& logFile = "logs/pitchtomidi.log",
              size_t maxFileSize = 1048576 * 5,  // 5MB
              size_t maxFiles = 3) {
        try {
            // Create the logs directory if it doesn't exist
            std::filesystem::create_directories("logs");

            // Create sinks
            auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                logFile, maxFileSize, maxFiles);

            // Create logger with multiple sinks
            logger_ = std::make_shared<spdlog::logger>(loggerName, 
                spdlog::sinks_init_list{consoleSink, fileSink});

            // Set log pattern
            logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

            // Set global logging level
            #ifdef NDEBUG
                logger_->set_level(spdlog::level::info);
            #else
                logger_->set_level(spdlog::level::debug);
            #endif

            // Register as default logger
            spdlog::register_logger(logger_);
            spdlog::set_default_logger(logger_);

            logger_->info("Logger initialized successfully");
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
            throw;
        }
    }

    // Logging methods
    template<typename... Args>
    void trace(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->trace(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->debug(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->warn(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->error(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void critical(fmt::format_string<Args...> fmt, Args&&... args) {
        logger_->critical(fmt, std::forward<Args>(args)...);
    }

private:
    Logger() = default;
    std::shared_ptr<spdlog::logger> logger_;
};

// Convenience macros for logging
#define LOG_TRACE(...) PitchToMidi::Logger::getInstance().trace(__VA_ARGS__)
#define LOG_DEBUG(...) PitchToMidi::Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) PitchToMidi::Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARN(...) PitchToMidi::Logger::getInstance().warn(__VA_ARGS__)
#define LOG_ERROR(...) PitchToMidi::Logger::getInstance().error(__VA_ARGS__)
#define LOG_CRITICAL(...) PitchToMidi::Logger::getInstance().critical(__VA_ARGS__)

} // namespace PitchToMidi 