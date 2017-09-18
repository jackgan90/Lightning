#pragma once

#define SMART_POINTER_DYNAMIC_CAST(subclass, smartptr) (dynamic_cast<subclass*>(smartptr.get()))
