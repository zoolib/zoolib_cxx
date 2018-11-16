// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org/

#include "zoolib/Compare.h"
#include "zoolib/Compat_string.h"
#include "zoolib/Singleton.h"
#include "zoolib/Util_STL_unordered_map.h"
//#include "zoolib/Util_STL_map.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Functor_CompareCharStars (anonymous)

namespace { // anonymous

class Functor_CompareCharStars
	{
public:
	bool operator()(const char* iL, const char* iR) const
		{ return strcmp(iL, iR) < 0; }
	};

typedef std::unordered_map<const char*, Comparer*> ComparerMap;

//typedef std::map<const char*, Comparer*, Functor_CompareCharStars> ComparerMap;

} // anonymous namespace

// =================================================================================================
#pragma mark - Comparer

Comparer::Comparer(const char* iTypeName)
:	fTypeName(iTypeName)
	{ Util_STL::sInsertMust(sSingleton<ComparerMap>(), iTypeName, this); }

Comparer::~Comparer()
	{}

int Comparer::Compare(const void* iL, const void* iR)
	{
	ZDebugStopf(0, "Compare::Compare not overridden type '%s'", fTypeName);
	return 0;
	}

int Comparer::sCompare(const char* iTypeName, const void* iL, const void* iR)
	{
	if (ZQ<Comparer*> theQ = Util_STL::sQGet(sSingleton<ComparerMap>(), iTypeName))
		return (*theQ)->Compare(iL, iR);

	ZDebugStopf(0, "Compare::sCompare called on unsupported type '%s'", iTypeName);
	return iL < iR ? -1 : iL > iR ? 1 : 0;
	}

// =================================================================================================
#pragma mark - CompareRegistration_Void (anonymous)

namespace { // anonymous

class ComparerRegistration_Void : public Comparer
	{
public:
	ComparerRegistration_Void() : Comparer(typeid(void).name()) {}
	virtual int Compare(const void* iL, const void* iR)
		{ return 0; }
	} ZMACRO_Concat(sComparer_,__LINE__);

} // anonymous namespace

} // namespace ZooLib
