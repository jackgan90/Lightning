#pragma once

#ifdef LIGHTNINGGE_WIN32
#ifdef LIGHTNINGGE_SCENE_EXPORT
#define LIGHTNINGGE_SCENE_API __declspec(dllexport)
#else
#define LIGHTNINGGE_SCENE_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_SCENE_API
#endif
