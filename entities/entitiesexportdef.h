#pragma once

#ifdef LIGHTNINGGE_WIN32
#ifdef LIGHTNINGGE_ENTITIES_EXPORT
#define LIGHTNINGGE_ENTITIES_API __declspec(dllexport)
#else
#define LIGHTNINGGE_ENTITIES_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_ENTITIES_API
#endif
