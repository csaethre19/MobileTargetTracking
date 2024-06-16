#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>

using namespace std;

class Logger {
public:
    Logger(const std::string& loggerName, const std::string& logFile);

    std::shared_ptr<spdlog::logger> getLogger();

private:
    std::shared_ptr<spdlog::logger> logger;
};

#endif // LOGGER_H
