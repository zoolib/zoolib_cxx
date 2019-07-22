// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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

	virtual void Visit_Walker(const ZP<Walker>& iWalker)
		{
		fW << "\n";

		for (size_t xx = 0; xx < fIndent; ++xx)
			fW << "\t";

		fW << iWalker->fCalled_Rewind << "\t" << iWalker->fCalled_QReadInc;
		Walker* asPointer = iWalker.Get();
		fW << " " << typeid(*asPointer).name();

		++fIndent;

		if (false)
			{}
		else if (ZP<Walker_Embed> theW = iWalker.DynamicCast<Walker_Embed>())
			{
			theW->GetParent()->Accept(*this);
			theW->GetEmbedee()->Accept(*this);
			}
		else if (ZP<Walker_Product> theW = iWalker.DynamicCast<Walker_Product>())
			{
			theW->GetLeft()->Accept(*this);
			theW->GetRight()->Accept(*this);
			}
		else if (ZP<Walker_Union> theW = iWalker.DynamicCast<Walker_Union>())
			{
			theW->GetLeft()->Accept(*this);
			theW->GetRight()->Accept(*this);
			}
		else if (ZP<Walker_Unary> theW = iWalker.DynamicCast<Walker_Unary>())
			{
			if (ZP<Walker_Comment> theWalker_Comment = iWalker.DynamicCast<Walker_Comment>())
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

void sDumpWalkers(ZP<Walker> iWalker, const ChanW_UTF& w)
	{ iWalker->Accept(DumpWalkers(w)); }

} // namespace QueryEngine
} // namespace ZooLib

// =================================================================================================
#pragma mark - pdesc

#if defined(ZMACRO_pdesc)

using namespace ZooLib;

ZMACRO_pdesc(const ZP<QueryEngine::Walker>& iWalker)
	{
	ZooLib::QueryEngine::sDumpWalkers(iWalker, StdIO::sChan_UTF_Err);
	}

#endif // defined(ZMACRO_pdesc)
