#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_LOADER_EXPORT
#define LIGHTNING_LOADER_API __declspec(dllexport)
#else
#define LIGHTNING_LOADER_API
#endif
#else
#define LIGHTNING_LOADER_API 
#endif
