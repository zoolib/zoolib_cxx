/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_CRLF_h__
#define __ZStream_CRLF_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStreamR_CRLFRemove

class ZStreamR_CRLFRemove : public ZStreamR
	{
public:
	ZStreamR_CRLFRemove(const ZStreamR& iStreamSource);
	ZStreamR_CRLFRemove(char iReplaceChar, const ZStreamR& iStreamSource);
	~ZStreamR_CRLFRemove();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

protected:
	const ZStreamR& fStreamSource;
	char fReplaceChar;
	bool fLastWasCR;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamR_CRLFInsert

class ZStreamR_CRLFInsert : public ZStreamR
	{
public:
	ZStreamR_CRLFInsert(const ZStreamR& iStreamSource);
	~ZStreamR_CRLFInsert();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

protected:
	const ZStreamR& fStreamSource;
	bool fLastWasCR;
	bool fHasBuffChar;
	char fBuffChar;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamW_CRLFRemove

class ZStreamW_CRLFRemove : public ZStreamW
	{
public:
	ZStreamW_CRLFRemove(const ZStreamW& iStreamSink);
	ZStreamW_CRLFRemove(char iReplaceChar, const ZStreamW& iStreamSink);
	~ZStreamW_CRLFRemove();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;
	char fReplaceChar;
	bool fLastWasCR;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamW_CRLFInsert

class ZStreamW_CRLFInsert : public ZStreamW
	{
public:
	ZStreamW_CRLFInsert(const ZStreamW& iStreamSink);
	~ZStreamW_CRLFInsert();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;
	bool fLastWasCR;
	};

} // namespace ZooLib

#endif // __ZStream_CRLF_h__
