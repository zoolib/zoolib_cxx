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

#ifndef __ZooLib_MIME_h__
#define __ZooLib_MIME_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"

namespace ZooLib {
namespace MIME {

// Character classification.
bool sIs_LWS(char iChar);

// These methods expect a ChanR_Bin that encompasses exactly a single header line. Most likely
// it will be the top of a stack of streams that handle line unfolding, encoded-word decoding etc.

bool sReadFieldName(const ChanR_Bin& iChanR, std::string* oName, std::string* oNameExact);

//bool sReadFieldBody(const ChanR_Bin& iChanR, std::string& oFieldBody);

// =================================================================================================
#pragma mark -
#pragma mark MIME::ChanR_Bin_Header

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
	virtual size_t QRead(Elmt_t* oDest, size_t iCount);

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
#pragma mark -
#pragma mark ZMIME::ChanR_Bin_Line

/// Returns bytes from the real stream until an end of line is detected.

class ChanR_Bin_Line
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Line(const ChanR_Bin& iChanR);

// From ChanR_Bin
	virtual size_t QRead(Elmt_t* oDest, size_t iCount);

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

#endif // __ZooLib_MIME_h__
