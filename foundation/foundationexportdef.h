#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_FOUNDATION_EXPORT
#define LIGHTNING_FOUNDATION_API __declspec(dllexport)
#else
#define LIGHTNING_FOUNDATION_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_FOUNDATION_API
#endif
