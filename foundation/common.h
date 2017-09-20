#pragma once

#define SMART_POINTER_DYNAMIC_CAST(subclass, smartptr) (dynamic_cast<subclass*>(smartptr.get()))
#define SAFE_DELETE(p) do{if((p)){delete (p); (p) = nullptr;}}while(0);
