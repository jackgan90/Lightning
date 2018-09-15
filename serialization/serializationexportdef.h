#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_SERIALIZATION_EXPORT
#define LIGHTNING_SERIALIZATION_API __declspec(dllexport)
#else
#define LIGHTNING_SERIALIZATION_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_SERIALIZATION_API 
#endif
