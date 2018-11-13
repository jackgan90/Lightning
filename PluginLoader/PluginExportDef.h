#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_PLUGIN_EXPORT
#define LIGHTNING_PLUGIN_API __declspec(dllexport)
#else
#define LIGHTNING_PLUGIN_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_PLUGIN_API
#endif
