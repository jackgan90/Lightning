#pragma once

#define DYNAMIC_CAST_PTR(subclass, smartptr) (dynamic_cast<subclass*>(smartptr.get()))
#define STATIC_CAST_PTR(subclass, smartptr) (static_cast<subclass*>(smartptr.get()))
#define SAFE_DELETE(p) do{if((p)){delete (p); (p) = nullptr;}}while(0)
#define SAFE_RELEASE(p) do{ if((p)){(p)->Release(); (p) = 0;}}while(0)
