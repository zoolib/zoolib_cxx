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

#ifndef __ZooLib_Chan_UTF_CRLF_h__
#define __ZooLib_Chan_UTF_CRLF_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_CRLFRemove

/// A read filter strim that removes CRLF sequences and replaces them with another code point.

class ChanR_UTF_CRLFRemove : public ChanR_UTF
	{
public:
	ChanR_UTF_CRLFRemove(const ChanR_UTF& iChanR);
	ChanR_UTF_CRLFRemove(UTF32 iReplacementCP, const ChanR_UTF& iChanR);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	const ChanR_UTF& fChanR;
	const UTF32 fReplacementCP;
	bool fLastWasCR;
	};

// 
// // =================================================================================================
// #pragma mark -
// #pragma mark ZStreamR_CRLFInsert
// 
// class ZStreamR_CRLFInsert : public ZStreamR
// 	{
// public:
// 	ZStreamR_CRLFInsert(const ZStreamR& iStreamSource);
// 	~ZStreamR_CRLFInsert();
// 
// // From ZStreamR
// 	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
// 
// protected:
// 	const ZStreamR& fStreamSource;
// 	bool fLastWasCR;
// 	bool fHasBuffChar;
// 	char fBuffChar;
// 	};
// 
//// =================================================================================================
//#pragma mark -
//#pragma mark ZStrimW_CRLFRemove
//
///// A write filter strim that removes CRLF sequences and replaces them with another code point.
//
//class ChanW_CRLFRemove : public ChanW_NativeUTF32
//	{
//public:
//	ZStrimW_CRLFRemove(const ZStrimW& iStrimSink);
//	ZStrimW_CRLFRemove(UTF32 iReplaceCP, const ZStrimW& iStrimSink);
//
//	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
//
//private:
//	const ZStrimW& fStrimSink;
//	UTF32 fReplaceCP;
//	bool fLastWasCR;
//	};
//
//// =================================================================================================
//#pragma mark -
//#pragma mark ZStrimW_CRLFInsert
//
///// A write filter strim that replaces standalone CR or LF with CRLF.
//
//class ZStrimW_CRLFInsert : public ZStrimW_NativeUTF32
//	{
//public:
//	ZStrimW_CRLFInsert(const ZStrimW& iStrimSink);
//
//	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
//
//private:
//	const ZStrimW& fStrimSink;
//	bool fLastWasCR;
//	};

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_CRLF_h__
