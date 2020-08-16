// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZStreamRPos_StreamR_h__
#define __ZStreamRPos_StreamR_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamRPos_StreamR

/** Puts a ZStreamRPos interface over a ZStreamR. */

class ZStreamRPos_StreamR : public ZStreamRPos
	{
public:
	ZStreamRPos_StreamR(const ZStreamR& iSource, const ZStreamRWPos& iBuffer);
	~ZStreamRPos_StreamR();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual size_t Imp_CountReadable();
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);
	virtual uint64 Imp_GetSize();

// Our protocol
	const ZStreamR& Committed();

protected:
	const ZStreamR& fSource;
	const ZStreamRWPos& fBuffer;
	bool fCommitted;
	};

} // namespace ZooLib

#endif // __ZStreamRPos_StreamR_h__
