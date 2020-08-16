// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Name.h"

#include "zoolib/CtorDtor.h"

namespace ZooLib {

// These are in a separate file so they're part of a separate compilation unit from
// the main Name code. Otherwise when optimizing the compiler would likely inline these, and
// unnecessarily taint their callers with Unwind_SjLj_Register/Unwind_SjLj_Unregister calls,
// which are a noticeable cost on iOS.

#if ! ZMACRO_NameUsesString

void Name::spRetain(const CountedString* iCounted)
	{
	try { const_cast<CountedString*>(iCounted)->Retain(); }
	catch (...) {}
	}

void Name::spRelease(const CountedString* iCounted)
	{
	try { const_cast<CountedString*>(iCounted)->Release(); }
	catch (...) {}
	}

#endif // ZMACRO_NameUsesString

} // namespace ZooLib
