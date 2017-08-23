/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/Yad_Any.h"

#include "zoolib/SQLite/YadSeqR_Iter.h"

namespace ZooLib {
namespace SQLite {

// =================================================================================================
#pragma mark -
#pragma mark Anonymous YadMapR

namespace { // anonymous

class YadMapR : public YadMapR_Std
	{
public:
	YadMapR(ZRef<Iter> iIter);

// From YadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR);

private:
	ZRef<Iter> fIter;
	size_t fIndex;
	};

YadMapR::YadMapR(ZRef<Iter> iIter)
:	fIter(iIter)
,	fIndex(0)
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
	{
	if (fIndex < fIter->Count())
		{
		oName = fIter->NameOf(fIndex);
		oYadR = sYadR(fIter->Get(fIndex));
		++fIndex;
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark SQLite::ChanR_RefYad_Iter

ChanR_RefYad_Iter::ChanR_RefYad_Iter(ZRef<Iter> iIter)
:	fIter(iIter)
	{}

ChanR_RefYad_Iter::~ChanR_RefYad_Iter()
	{}

void ChanR_RefYad_Iter::Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
	{
	if (not iIsFirst)
		fIter->Advance();

	if (fIter->HasValue())
		oYadR = new YadMapR(fIter);
	}

} // namespace SQLite
} // namespace ZooLib
