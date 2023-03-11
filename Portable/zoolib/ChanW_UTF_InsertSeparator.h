// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ChanW_UTF_InsertSeparator_h__
#define __ChanW_UTF_InsertSeparator_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include <map>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_UTF_InsertSeparator

class ChanW_UTF_InsertSeparator
:	public virtual ChanW_UTF_Native32
	{
public:
	typedef std::map<size_t,string8> Spacings;

	ChanW_UTF_InsertSeparator(size_t iSpacing, const string8& iSeparator, const ChanW_UTF& iChanW);

	ChanW_UTF_InsertSeparator(const Spacings& iSpacings, const ChanW_UTF& iChanW);
	
// From ChanW<UTF32> via ChanW_UTF_Native32
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

private:
	const ChanW_UTF& fChanW;
	Spacings fSpacings;	
	uint64 fCount;
	};

} // namespace ZooLib

#endif // __ChanW_UTF_InsertSeparator_h__
