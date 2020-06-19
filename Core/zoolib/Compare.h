// Copyright (c) 2005-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2020 Andrew Green.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Compare_h__
#define __ZooLib_Compare_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"

#include <typeinfo>

namespace ZooLib {

// =================================================================================================
#pragma mark - Comparer

class Comparer
	{
protected:
	Comparer(const char* iTypeName);
	virtual ~Comparer();

	virtual int Compare(const void* iL, const void* iR) = 0;

public:
	static int sCompare(const char* iTypeName, const void* iL, const void* iR);
	const char* fTypeName;
	};

// =================================================================================================
#pragma mark - ComparerRegistration_T

template <class T, int (*CompareProc)(const T&, const T&)>
class ComparerRegistration_T : public Comparer
	{
public:
	ComparerRegistration_T() : Comparer(typeid(T).name()) {}

// From Comparer
	virtual int Compare(const void* iL, const void* iR)
		{ return CompareProc(*static_cast<const T*>(iL), *static_cast<const T*>(iR)); }
	};

#define ZMACRO_CompareRegistration_T_Real(t, CLASS, INST) \
	namespace { class CLASS : public ComparerRegistration_T<t, sCompare_T<t>> {} INST; }

// =================================================================================================
#pragma mark - Macros

#define ZMACRO_CompareRegistration_T(t) \
	ZMACRO_CompareRegistration_T_Real(t, \
		ZMACRO_Concat(Comparer_,__LINE__), \
		ZMACRO_Concat(sComparer_,__LINE__))

} // namespace ZooLib

#endif // __ZooLib_Compare_h__
