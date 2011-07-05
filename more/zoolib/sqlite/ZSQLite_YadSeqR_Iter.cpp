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

#include "zoolib/ZYad_Any.h"
#include "zoolib/sqlite/ZSQLite_YadSeqR_Iter.h"

namespace ZooLib {
namespace ZSQLite {

// =================================================================================================
#pragma mark -
#pragma mark * Anonymous YadMapR

namespace { // anonymous

class YadMapR : public ZYadMapR_Std
	{
public:
	YadMapR(ZRef<Iter> iIter);

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR);

private:
	ZRef<Iter> fIter;
	size_t fIndex;
	};

YadMapR::YadMapR(ZRef<Iter> iIter)
:	fIter(iIter)
,	fIndex(0)
	{}

void YadMapR::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
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
#pragma mark * ZSQLite::YadSeqR_Iter

YadSeqR_Iter::YadSeqR_Iter(ZRef<Iter> iIter)
:	fIter(iIter)
	{}

YadSeqR_Iter::~YadSeqR_Iter()
	{}

void YadSeqR_Iter::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	if (not iIsFirst)
		fIter->Advance();

	if (fIter->HasValue())
		oYadR = new YadMapR(fIter);
	}

} // namespace ZSQLite
} // namespace ZooLib
