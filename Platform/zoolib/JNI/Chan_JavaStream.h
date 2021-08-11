// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_JNI_Chan_JavaStream__
#define __ZooLib_JNI_Chan_JavaStream__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
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
	jbyteArray fBuffer;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_JavaOutputStream

class ChanW_Bin_JavaOutputStream
:	public ChanW_Bin
	{
public:
	ChanW_Bin_JavaOutputStream(jobject iOutputStream);
	~ChanW_Bin_JavaOutputStream();

// From ChanAspect_Write<byte>
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	jobject fOutputStream;
	jbyteArray fBuffer;
	};

} // namespace ZooLib

#endif // __ZooLib_JNI_Chan_JavaStream__
