// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRU_UTF_Std_h__
#define __ZooLib_ChanRU_UTF_Std_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF_CRLF.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_UTF_Std

class ChanRU_UTF_Std
:	public virtual ChanRU<UTF32>
	{
public:
	ChanRU_UTF_Std(const ChanR_UTF& iChanR);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

// From ChanU_UTF
	virtual size_t Unread(const UTF32* iSource, size_t iCount);

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

} // namespace ZooLib

#endif // __ZooLib_ChanRU_UTF_Std_h__
