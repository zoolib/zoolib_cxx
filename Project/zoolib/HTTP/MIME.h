// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_HTTP_MIME_h__
#define __ZooLib_HTTP_MIME_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"

namespace ZooLib {
namespace MIME {

// Character classification.
bool sIs_LWS(char iChar);

// =================================================================================================
#pragma mark - MIME::ChanR_Bin_Header

/** 
Takes a source stream and reads until it sees an LFLF sequence, at which
point it appears to be empty.
RFC822 mandates the use of CRLF as the line terminating sequence and allows
bare LF and CR sequences within lines. However I'm unclear on how useful or important this is.
So this class completely strips CRs from the source stream. It also unfolds LF LWS sequences,
replacing them with LWS */

class ChanR_Bin_Header
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Header(const ChanR_Bin& iChanR);

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);

// Our protocol
	void Reset();

private:
	const ChanR_Bin& fChanR;
	enum EState
		{
		eInitial,
		eReturn_LF_X,
		eReturn_X,
		eNormal,
		eSeen_LF,
		eSeen_LF_LF
		};
	EState fState;
	byte fX;
	};

// =================================================================================================
#pragma mark - ZMIME::ChanR_Bin_Line

/// Returns bytes from the real stream until an end of line is detected.

class ChanR_Bin_Line
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Line(const ChanR_Bin& iChanR);

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);

// Our protocol
	void Reset();
	bool HitLF() const;
	bool WasEmpty() const;

private:
	const ChanR_Bin& fChanR;
	enum EState
		{
		eReturn_X,
		eNormal,
		eSeen_LF
		};
	bool fEmpty;
	EState fState;
	byte fX;
	};

} // namespace ZMIME
} // namespace ZooLib

#endif // __ZooLib_HTTP_MIME_h__
