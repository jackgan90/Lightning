#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_SCENE_EXPORT
#define LIGHTNING_SCENE_API __declspec(dllexport)
#else
#define LIGHTNING_SCENE_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_SCENE_API
#endif
