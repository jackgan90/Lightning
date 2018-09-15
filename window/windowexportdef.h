#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_WINDOW_EXPORT
#define LIGHTNING_WINDOW_API __declspec(dllexport)
#else
#define LIGHTNING_WINDOW_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_WINDOW_API
#endif
