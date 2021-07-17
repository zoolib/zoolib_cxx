// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Starter_h__
#define __ZooLib_Starter_h__ 1
#include "zconfig.h"

#include "zoolib/Startable.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter

class Starter
:	public Counted
	{
public:
// Our protocol
	virtual bool QStart(const ZP<Startable>& iStartable) = 0;
	};

// =================================================================================================
#pragma mark - Starter_Trivial

class Starter_Trivial
:	public Starter
	{
public:
// From Starter
	virtual bool QStart(const ZP<Startable>& iStartable)
		{ return sQCall(iStartable); }
	};

// =================================================================================================
#pragma mark - sQStart

inline bool sQStart(ZP<Starter> iStarter, const ZP<Startable>& iStartable)
	{
	if (iStarter && iStartable)
		return iStarter->QStart(iStartable);
	return false;
	}

} // namespace ZooLib

#endif // __ZooLib_Starter_h__
