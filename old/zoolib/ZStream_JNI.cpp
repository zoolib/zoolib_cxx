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

#include "zoolib/ZStream_JNI.h"

#if ZCONFIG_SPI_Enabled(JNI)

#include "zoolib/ZMemory.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_JNI

ZStreamR_JNI::ZStreamR_JNI(JNIEnv* iEnv, jobject iJavaStream)
	{
	fEnv = iEnv;
	fJavaStream = iJavaStream;
	}

ZStreamR_JNI::~ZStreamR_JNI()
	{}

void ZStreamR_JNI::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	if (!fEnv || !fJavaStream)
		return;

	jclass class_Stream = fEnv->GetObjectClass(fJavaStream);
	if (!class_Stream)
		return;

	jmethodID mid_read = fEnv->GetMethodID(class_Stream, "read", "([BII)I");
	if (!mid_read)
		return;

	size_t theBufferSize = min(size_t(4096), iCount);
	jbyteArray theBufferArray = fEnv->NewByteArray(theBufferSize);

	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	size_t countRemaining = iCount;
	while (countRemaining > 0)
		{
		size_t countToRead = min(countRemaining, theBufferSize);
		jint countRead = fEnv->CallIntMethod(fJavaStream, mid_read, theBufferArray, 0, countToRead);
		if (countRead <= 0)
			break;
		jboolean isCopy;
		jbyte* elems = static_cast<jbyte*>(fEnv->GetPrimitiveArrayCritical(theBufferArray, &isCopy));
		ZMemCopy(localDest, elems, countRead);
		fEnv->ReleasePrimitiveArrayCritical(theBufferArray, elems, JNI_ABORT);
		localDest += countRead;
		countRemaining -= countRead;
		if (oCountRead)
			*oCountRead += countRead;
		}
	fEnv->DeleteLocalRef(theBufferArray);
	}

size_t ZStreamR_JNI::Imp_CountReadable()
	{
	if (jclass class_Stream = fEnv->GetObjectClass(fJavaStream))
		{
		if (jmethodID mid_available = fEnv->GetMethodID(class_Stream, "available", "()I"))
			return fEnv->CallIntMethod(fJavaStream, mid_available);
		}
	return 0;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(JNI)
