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

#ifndef __ZStrimR_Boundary_h__
#define __ZStrimR_Boundary_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrimmer.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStrimR_Boundary

/// A read filter strim that reads from another strim until a boundary sequence is encountered.

class ZStrimR_Boundary : public ZStrimR
	{
public:
	ZStrimR_Boundary(const string8& iBoundary, const ZStrimR& iStrimSource);
	ZStrimR_Boundary(const UTF8* iBoundary, size_t iBoundarySize, const ZStrimR& iStrimSource);
	ZStrimR_Boundary(const string32& iBoundary, const ZStrimR& iStrimSource);
	ZStrimR_Boundary(const UTF32* iBoundary, size_t iBoundarySize, const ZStrimR& iStrimSource);
	~ZStrimR_Boundary();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

// Our protocol
	bool HitBoundary() const;
	void Reset();

protected:
	void pInit();

	const ZStrimR& fStrimSource;
	string32 fBoundary;
	size_t fSkip[256];
	UTF32* fBuffer;
	size_t fStart;
	size_t fEnd;
	bool fHitBoundary;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimerR_Boundary

/// A read filter strimmer encapsulating a ZStrimR_Boundary.

class ZStrimmerR_Boundary : public ZStrimmerR
	{
public:
	ZStrimmerR_Boundary(const string8& iBoundary, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(
		const UTF8* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(const string32& iBoundary, ZRef<ZStrimmerR> iStrimmerSource);

	ZStrimmerR_Boundary(
		const UTF32* iBoundary, size_t iBoundarySize, ZRef<ZStrimmerR> iStrimmerSource);

	virtual ~ZStrimmerR_Boundary();

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR();

protected:
	ZRef<ZStrimmerR> fStrimmerSource;
	ZStrimR_Boundary fStrim;
	};

// typedef ZStrimmerRFT<ZStrimR_Boundary> ZStrimmerR_Boundary;

// =================================================================================================

} // namespace ZooLib

#endif // __ZStrimR_Boundary_h__
