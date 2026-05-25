#pragma once
#include "core/core.h"
#include "core/api.h"
#include <memory>
#include <fstream>
#include <iostream>

namespace wne
{
    inline constexpr const char *endl = "\n";

    class WNE_API LoggerLog
    {
    public:
        template <typename T>
        LoggerLog &operator<<(const T &value)
        {
            std::cout << value;
            if (loggingState && file.is_open())
                file << value;
            return *this;
        }

        void setLoggingState(bool state);

        inline bool isLogging()
        {
            return loggingState;
        }

    protected:
        bool loggingState = false;
        std::ofstream file;
    };

    class WNE_API Logger
    {
    public:
        static LoggerLog log;

        static inline void setLoggingState(bool state)
        {
            log.setLoggingState(state);
        }

        static inline bool isLogging()
        {
            return log.isLogging();
        }
    };
};
