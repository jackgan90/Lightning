#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_LIB_EXPORT
#define LIGHTNING_LIB_API __declspec(dllexport)
#else
#define LIGHTNING_LIB_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_LIB_API
#endif
