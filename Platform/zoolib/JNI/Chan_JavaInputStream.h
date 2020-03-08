// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_JNI_Chan_JavaInputStream__
#define __ZooLib_JNI_Chan_JavaInputStream__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/JNI/JNI.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_JavaInputStream

class ChanR_Bin_JavaInputStream
:	public ChanR_Bin
	{
public:
	ChanR_Bin_JavaInputStream(jobject iInputStream);
	~ChanR_Bin_JavaInputStream();

// From ChanAspect_Read<byte>
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

protected:
	jobject fInputStream;
	};

} // namespace ZooLib

#endif // __ZooLib_JNI_Chan_JavaInputStream__
