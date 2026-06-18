#pragma once
#include "core/platform.h"

#if defined(OS_WINDOWS)
    #ifdef WNE_BUILD_DLL
        #define WNE_API __declspec(dllexport)
    #else
        #define WNE_API __declspec(dllimport)
    #endif
#elif defined(OS_LINUX)
    #ifdef WNE_BUILD_DLL
        #define WNE_API __attribute__((visibility("default")))
    #else
        #define WNE_API
    #endif
#endif