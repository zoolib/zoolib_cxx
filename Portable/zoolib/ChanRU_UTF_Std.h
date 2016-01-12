/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_ChanRU_UTF_Std_h__
#define __ZooLib_ChanRU_UTF_Std_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF_CRLF.h"
#include "zoolib/ChanU_UTF.h"

#include "zoolib/Channer_UTF.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanRU_UTF_Std

class ChanRU_UTF_Std
:	public ChanR_UTF
,	public ChanU_UTF
	{
public:
	ChanRU_UTF_Std(const ChanR_UTF& iChanR);

// From ChanR_UTF
	virtual size_t QRead(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);
	virtual size_t UnreadableLimit();

// Our protocol
	int GetPos();
	int GetLine();
	int GetColumn();

private:
	ChanR_UTF_CRLFRemove fChanR_CRLFRemove;
	std::vector<UTF32> fStack;
	size_t fPos;
	size_t fLine;
	size_t fColumn;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRU_UTF_Std

class ChannerRU_UTF_Std
:	public ChannerRU_UTF
	{
public:
	ChannerRU_UTF_Std(const ZRef<ChannerR_UTF>& iChannerR_UTF)
	:	fChannerR_UTF(iChannerR_UTF)
	,	fChanRU_UTF_Std(sGetChan(fChannerR_UTF))
		{}

	virtual ~ChannerRU_UTF_Std()
		{}

// From ChannerR_UTF via ChannerRU_UTF
	virtual void GetChan(const ChanR_UTF*& oChanPtr)
		{ oChanPtr = &fChanRU_UTF_Std; }

// From ChannerU_UTF via ChannerRU_UTF
	virtual void GetChan(const ChanU_UTF*& oChanPtr)
		{ oChanPtr = &fChanRU_UTF_Std; }

// Our protocol
	ChanRU_UTF_Std& GetChanActual()
		{ return fChanRU_UTF_Std; }

private:
	const ZRef<ChannerR_UTF> fChannerR_UTF;
	ChanRU_UTF_Std fChanRU_UTF_Std;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRU_UTF_Std_h__
