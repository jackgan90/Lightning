#pragma once

#ifdef LIGHTNINGGE_WIN32
#ifdef LIGHTNINGGE_UTILITIES_EXPORT
#define LIGHTNINGGE_UTILITIES_API __declspec(dllexport)
#else
#define LIGHTNINGGE_UTILITIES_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_UTILITIES_API
#endif
