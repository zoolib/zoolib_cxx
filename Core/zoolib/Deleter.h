// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Deleter_h__
#define __ZooLib_Deleter_h__ 1
#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Deleter

template <class T>
class Deleter
	{
public:
	Deleter(T*& iPtr) : fPtr(iPtr) {}
	~Deleter() { delete fPtr; }

private:
	T*& fPtr;
	};

// =================================================================================================
#pragma mark - Deleter specialized for arrays.

template <class T>
class Deleter<T[]>
	{
public:
	Deleter(T*& iPtr) : fPtr(iPtr) {}
	~Deleter() { delete[] fPtr; }

private:
	T*& fPtr;
	};

} // namespace ZooLib

#endif // __ZooLib_Deleter_h__
