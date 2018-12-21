#pragma once

#ifdef LIGHTNING_WIN32
#define INTERFACECALL __stdcall
#else
#define INTERFACECALL
#endif
