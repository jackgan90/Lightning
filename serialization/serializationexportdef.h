#pragma once

#ifdef LIGHTNINGGE_WIN32
#ifdef LIGHTNINGGE_SERIALIZATION_EXPORT
#define LIGHTNINGGE_SERIALIZATION_API __declspec(dllexport)
#else
#define LIGHTNINGGE_SERIALIZATION_API __declspec(dllimport)
#endif
#else
#defineLIGHTNINGGE_SERIALIZATION_API 
#endif
