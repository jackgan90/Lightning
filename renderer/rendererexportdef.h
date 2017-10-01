#pragma once

#ifdef LIGHTNINGGE_WIN32
#ifdef LIGHTNINGGE_RENDERER_EXPORT
#define LIGHTNINGGE_RENDERER_API __declspec(dllexport)
#else
#define LIGHTNINGGE_RENDERER_API __declspec(dllimport)
#endif
#else
#define LIGHTNINGGE_RENDERER_API
#endif
