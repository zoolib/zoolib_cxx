// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_CRLF_h__
#define __ZooLib_Chan_UTF_CRLF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_CRLFRemove

/// A read filter strim that removes CRLF sequences and replaces them with another code point.

class ChanR_UTF_CRLFRemove : public virtual ChanR_UTF
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
//class ChanW_CRLFRemove : public virtual ChanW_NativeUTF32
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
