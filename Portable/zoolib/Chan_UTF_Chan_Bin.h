// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_Chan_Bin_h__
#define __ZooLib_Chan_UTF_Chan_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_Chan_Bin_UTF16

/// A read strim that sources text by reading UTF-16 code units from a ChanR_Bin.

class ChanR_UTF_Chan_Bin_UTF16
:	public virtual ChanR_UTF
	{
public:
	ChanR_UTF_Chan_Bin_UTF16(const ChanR_Bin& iChanR_Bin);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	const ChanR_Bin& fChanR_Bin;
	};

// =================================================================================================
#pragma mark - ChanR_UTF_Chan_Bin_UTF8

/// A read strim that sources text by reading UTF-8 code units from a ChanR_Bin.

class ChanR_UTF_Chan_Bin_UTF8
:	public virtual ChanR_UTF
	{
public:
	ChanR_UTF_Chan_Bin_UTF8(const ChanR_Bin& iChanR_Bin);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	const ChanR_Bin& fChanR_Bin;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_Chan_Bin_UTF8

/// A write strim that writes text to a ChanW_Bin in UTF-8.

class ChanW_UTF_Chan_Bin_UTF8
:	public virtual ChanW_UTF_Native8
	{
public:
	ChanW_UTF_Chan_Bin_UTF8(const ChanW_Bin& iChanW_Bin);

// From ChanW_UTF
	virtual void Flush();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

private:
	const ChanW_Bin& fChanW_Bin;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_Chan_Bin_h__
