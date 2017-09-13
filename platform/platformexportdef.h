#pragma once

#ifdef WIN32
#ifdef LIGHTNINGGE_PLATFORM_EXPORT
#define LIGHTNINGGE_PLATFORM_API __declspec(dllexport)
#else
#define LIGHTNINGGE_PLATFORM_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_PLATFORM_API
#endif
