/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/QueryEngine/Util_Strim_Walker.h"

#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

#include "zoolib/QueryEngine/Walker_Comment.h"
#include "zoolib/QueryEngine/Walker_Embed.h"
#include "zoolib/QueryEngine/Walker_Product.h"
#include "zoolib/QueryEngine/Walker_Union.h"

#include "zoolib/pdesc.h"
#if defined(ZMACRO_pdesc)
	#include "zoolib/StdIO.h"
#endif

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - sDumpWalkers

class DumpWalkers : public Visitor_Walker
	{
public:
	DumpWalkers(const ChanW_UTF& iW)
	:	fW(iW)
	,	fIndent(0)
		{}

	virtual void Visit_Walker(const ZRef<Walker>& iWalker)
		{
		fW << "\n";

		for (size_t xx = 0; xx < fIndent; ++xx)
			fW << "\t";

		fW << iWalker->fCalled_Rewind << "\t" << iWalker->fCalled_QReadInc;
		fW << " " << typeid(*iWalker).name();

		++fIndent;

		if (false)
			{}
		else if (ZRef<Walker_Embed> theW = iWalker.DynamicCast<Walker_Embed>())
			{
			theW->GetParent()->Accept(*this);
			theW->GetEmbedee()->Accept(*this);
			}
		else if (ZRef<Walker_Product> theW = iWalker.DynamicCast<Walker_Product>())
			{
			theW->GetLeft()->Accept(*this);
			theW->GetRight()->Accept(*this);
			}
		else if (ZRef<Walker_Union> theW = iWalker.DynamicCast<Walker_Union>())
			{
			theW->GetLeft()->Accept(*this);
			theW->GetRight()->Accept(*this);
			}
		else if (ZRef<Walker_Unary> theW = iWalker.DynamicCast<Walker_Unary>())
			{
			if (ZRef<Walker_Comment> theWalker_Comment = iWalker.DynamicCast<Walker_Comment>())
				{
				fW << " ";
				Util_Chan_JSON::sWriteString(theWalker_Comment->GetComment(), false, fW);
				}
			theW->GetChild()->Accept(*this);
			}

		--fIndent;
		}

	const ChanW_UTF& fW;
	size_t fIndent;
	};

void sDumpWalkers(ZRef<Walker> iWalker, const ChanW_UTF& w)
	{ iWalker->Accept(DumpWalkers(w)); }

} // namespace QueryEngine
} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZRef<QueryEngine::Walker>& iWalker)
	{
	ZooLib::QueryEngine::sDumpWalkers(iWalker, StdIO::sChan_UTF_Err);
	}

#endif // defined(ZMACRO_pdesc)
