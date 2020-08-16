// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZStreamR_Random_h__
#define __ZStreamR_Random_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ZStreamR_Random

/// A read stream providing an endless sequence of pseudo-random bytes.

class ZStreamR_Random : public ZStreamR
	{
public:
	ZStreamR_Random(int32 iSeed);
	~ZStreamR_Random();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// Our protocol
	void SetSeed(int32 iSeed);
	int32 GetSeed() const;

protected:
	int32 fState;
	};

//typedef ZStreamerR_T<ZStreamR_Random> ZStreamerR_Random;

} // namespace ZooLib

#endif // __ZStreamR_Random_h__
