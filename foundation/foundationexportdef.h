#pragma once

#ifdef WIN32
#ifdef LIGHTNINGGE_FOUNDATION_EXPORT
#define LIGHTNINGGE_FOUNDATION_API __declspec(dllexport)
#else
#define LIGHTNINGGE_FOUNDATION_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_FOUNDATION_API
#endif
