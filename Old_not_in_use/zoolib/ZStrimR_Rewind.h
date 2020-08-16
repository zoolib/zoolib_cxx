// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZStrimR_Rewind_h__
#define __ZStrimR_Rewind_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStrimR_Rewind

class ZStreamRWPos;

class ZStrimR_Rewind : public ZStrimR
	{
public:
	ZStrimR_Rewind(const ZStrimR& iStrimSource, const ZStreamRWPos& iBuffer);
	~ZStrimR_Rewind();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

// Our protocol
	void Rewind();

	uint64 GetMark();
	void SetMark(uint64 iMark);

protected:
	const ZStrimR& fStrimSource;
	const ZStreamRWPos& fBuffer;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZStrimR_Rewind_h__
