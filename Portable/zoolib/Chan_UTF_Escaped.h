// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_Escaped_h__
#define __ZooLib_Chan_UTF_Escaped_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_Escaped

class ChanR_UTF_Escaped
:	public ChanR_UTF
	{
public:
	ChanR_UTF_Escaped(UTF32 iDelimiter, const ChanRU_UTF& iChan);
	~ChanR_UTF_Escaped();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCountCU);

protected:
	const UTF32 fDelimiter;
	const ChanRU_UTF& fChanRU;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_Escaped

/// A write filter strim that inserts C-style escape sequences.

class ChanW_UTF_Escaped
:	public ChanW_UTF_Native32
	{
public:
	struct Options
		{
		Options();

		string8 fEOL;
		bool fQuoteQuotes;
		bool fEscapeHighUnicode;
		};

	ChanW_UTF_Escaped(const Options& iOptions, const ChanW_UTF& iChanW);
	ChanW_UTF_Escaped(const ChanW_UTF& iChanW);
	~ChanW_UTF_Escaped();

// From ChanW_UTF_Native32
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

private:
	const ChanW_UTF& fChanW;
	string8 fEOL;
	bool fQuoteQuotes;
	bool fEscapeHighUnicode;
	bool fLastWasCR;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_Escaped_h__
