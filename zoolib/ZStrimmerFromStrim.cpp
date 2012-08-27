/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStrimmerFromStrim.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimmerFromStrimR

class ZStrimmerFromStrimR::StrimmerR
:	public ZStrimmerR
,	public ZStrimR
	{
public:
	StrimmerR(const ZStrimR& r)
	:	fStrimR(&r)
		{}
	
	virtual ~StrimmerR()
		{}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR()
		{ return *this; }

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
		{
		if (fStrimR)
			{
			fStrimR->Read(oDest, iCount, oCount);
			}
		else
			{
			if (oCount)
				*oCount = 0;
			}
		}

	virtual void Imp_ReadUTF16(UTF16* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
		{
		if (fStrimR)
			{
			fStrimR->Read(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
			}
		else
			{
			if (oCountCU)
				*oCountCU = 0;
			if (oCountCP)
				*oCountCP = 0;
			}
		}

	virtual void Imp_ReadUTF8(UTF8* oDest,
		size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
		{
		if (fStrimR)
			{
			fStrimR->Read(oDest, iCountCU, oCountCU, iCountCP, oCountCP);
			}
		else
			{
			if (oCountCU)
				*oCountCU = 0;
			if (oCountCP)
				*oCountCP = 0;
			}
		}

// Our protocol
	void pDetach()
		{
		ZAssert(fStrimR);
		fStrimR = nullptr;
		}

private:
	const ZStrimR* fStrimR;
	};

ZStrimmerFromStrimR::ZStrimmerFromStrimR(const ZStrimR& r)
:	fStrimmerR(new StrimmerR(r))
	{}

ZStrimmerFromStrimR::~ZStrimmerFromStrimR()
	{ fStrimmerR->pDetach(); }

ZStrimmerFromStrimR::operator ZRef<ZStrimmerR>()
	{ return fStrimmerR; }

} // namespace ZooLib
