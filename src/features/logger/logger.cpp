#include "features/logger/logger.h"
#include <filesystem>

using namespace wne;

LoggerLog Logger::log;

void LoggerLog::setLoggingState(bool state)
{
    loggingState = state;

    if (loggingState)
    {
        try
        {
            std::filesystem::create_directories("logs");
        }
        catch (const std::filesystem::filesystem_error &e)
        {
        }

        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H;%M;%S", timeinfo);

        std::string logPath = std::string("./logs/") + std::string(buffer) + std::string(".txt");

        file = std::ofstream(logPath);
        if (file.is_open())
            std::cout << "Writing logs to " << logPath << std::endl;
        else
            std::cout << "Unable to write logs to " << logPath << std::endl;
    }
}
