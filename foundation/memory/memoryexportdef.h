#pragma once

#ifdef MEMORY_PLATFORM_WIN32
#ifdef MEMORY_EXPORT
#define MEMORY_API __declspec(dllexport)
#else
#define MEMORY_API __declspec(dllimport)
#endif
#else
#define MEMORY_API
#endif
