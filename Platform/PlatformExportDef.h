#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_PLATFORM_EXPORT
#define LIGHTNING_PLATFORM_API __declspec(dllexport)
#else
#define LIGHTNING_PLATFORM_API
#endif
#else
#define LIGHTNING_PLATFORM_API
#endif
