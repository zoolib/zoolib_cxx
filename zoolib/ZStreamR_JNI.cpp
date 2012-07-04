/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZCompat_algorithm.h" // For sMin
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStreamR_JNI.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_JNI

ZStreamR_JNI::ZStreamR_JNI(JNIEnv* iEnv, jobject iJavaStream)
:	fJavaStream(iJavaStream)
	{
	ZAssert(fJavaStream);
	}

ZStreamR_JNI::~ZStreamR_JNI()
	{}

void ZStreamR_JNI::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	JNIEnv* env = ZJNI::Env::sGet();

	uint8* localDest = reinterpret_cast<uint8*>(oDest);

	if (size_t countRemaining = iCount)
		{
		if (jclass class_Stream = env->GetObjectClass(fJavaStream))
			{
			if (jmethodID mid_read = env->GetMethodID(class_Stream, "read", "([BII)I"))
				{
				const size_t theBufferSize = sMin(4096, iCount);
				if (jbyteArray theBufferArray = env->NewByteArray(theBufferSize))
					{
					while (countRemaining > 0)
						{
						const size_t countToRead = sMin(countRemaining, theBufferSize);
						jint countRead = env->CallIntMethod(fJavaStream, mid_read,
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
		
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t ZStreamR_JNI::Imp_CountReadable()
	{
	JNIEnv* env = ZJNI::Env::sGet();

	if (jclass class_Stream = env->GetObjectClass(fJavaStream))
		{
		if (jmethodID mid_available = env->GetMethodID(class_Stream, "available", "()I"))
			return env->CallIntMethod(fJavaStream, mid_available);
		}
	return 0;
	}

} // namespace ZooLib
