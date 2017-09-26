#pragma once

#define DYNAMIC_CAST_PTR(subclass, smartptr) (dynamic_cast<subclass*>(smartptr.get()))
#define SAFE_DELETE(p) do{if((p)){delete (p); (p) = nullptr;}}while(0)
