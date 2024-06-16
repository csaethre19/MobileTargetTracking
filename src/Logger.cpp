#include "Logger.h"


Logger::Logger(const std::string& loggerName, const std::string& logFile) {
    try {
        logger = spdlog::basic_logger_mt(loggerName, "logs/" + logFile);
        logger->set_level(spdlog::level::debug);
        // Set logger to automatically flush on all log levels
        logger->flush_on(spdlog::level::debug);
        logger->info("Debug logs set.");
        spdlog::get(loggerName)->info("Logging initialized!");

        // Ensure logger remains in scope
        spdlog::get(loggerName)->flush();
    }
    catch (const spdlog::spdlog_ex &ex) {
        std::cerr << "Log initialization failed: " << ex.what();
    }
    }

std::shared_ptr<spdlog::logger> Logger::getLogger() {
    return logger;
}


