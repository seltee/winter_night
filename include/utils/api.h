#pragma once

#ifdef WNE_BUILD_DLL
#define WNE_API __declspec(dllexport)
#else
#define WNE_API __declspec(dllimport)
#endif