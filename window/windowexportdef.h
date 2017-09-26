#pragma once

#ifdef WIN32
#ifdef LIGHTNINGGE_WINDOW_EXPORT
#define LIGHTNINGGE_WINDOW_API __declspec(dllexport)
#else
#define LIGHTNINGGE_WINDOW_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_WINDOW_API
#endif
