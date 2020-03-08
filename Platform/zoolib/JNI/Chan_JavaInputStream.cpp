// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/JNI/Chan_JavaInputStream.h"

#include "zoolib/Compat_algorithm.h" // For sMin
#include "zoolib/Memory.h" // For sMemCopy

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_Bin_JavaInputStream

ChanR_Bin_JavaInputStream::ChanR_Bin_JavaInputStream(jobject iInputStream)
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	ZAssert(iInputStream);

	fInputStream = env->NewGlobalRef(iInputStream);
	}

ChanR_Bin_JavaInputStream::~ChanR_Bin_JavaInputStream()
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	env->DeleteGlobalRef(fInputStream);
	}

size_t ChanR_Bin_JavaInputStream::Read(byte* oDest, size_t iCount)
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	uint8* localDest = reinterpret_cast<uint8*>(oDest);

	if (size_t countRemaining = iCount)
		{
		if (jclass class_Stream = env->GetObjectClass(fInputStream))
			{
			if (jmethodID mid_read = env->GetMethodID(class_Stream, "read", "([BII)I"))
				{
				const size_t theBufferSize = sMin<size_t>(4096, countRemaining);
				if (jbyteArray theBufferArray = env->NewByteArray(theBufferSize))
					{
					while (countRemaining > 0)
						{
						const size_t countToRead = sMin(countRemaining, theBufferSize);

						jint countRead = env->CallIntMethod(fInputStream, mid_read,
							theBufferArray, 0, countToRead);

						if (countRead <= 0)
							break;

						jbyte* elems = static_cast<jbyte*>
							(env->GetPrimitiveArrayCritical(theBufferArray, nullptr));

						sMemCopy(localDest, elems, countRead);

						env->ReleasePrimitiveArrayCritical(theBufferArray, elems, JNI_ABORT);

						localDest += countRead;
						countRemaining -= countRead;
						}
					env->DeleteLocalRef(theBufferArray);
					}
				}
			}
		}
	return localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t ChanR_Bin_JavaInputStream::Readable()
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	if (jclass class_Stream = env->GetObjectClass(fInputStream))
		{
		if (jmethodID mid_available = env->GetMethodID(class_Stream, "available", "()I"))
			return env->CallIntMethod(fInputStream, mid_available);
		}

	return 0;
	}

} // namespace ZooLib
