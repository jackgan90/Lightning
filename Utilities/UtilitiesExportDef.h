#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_UTILITIES_EXPORT
#define LIGHTNING_UTILITIES_API __declspec(dllexport)
#else
#define LIGHTNING_UTILITIES_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_UTILITIES_API
#endif
