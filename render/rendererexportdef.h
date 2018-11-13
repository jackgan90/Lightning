#pragma once

#ifdef LIGHTNING_WIN32
#ifdef LIGHTNING_RENDERER_EXPORT
#define LIGHTNING_RENDER_API __declspec(dllexport)
#else
#define LIGHTNING_RENDER_API __declspec(dllimport)
#endif
#else
#define LIGHTNING_RENDER_API
#endif
