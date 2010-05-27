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

#ifndef __ZRA_SQL__
#define __ZRA_SQL__
#include "zconfig.h"

#include "zoolib/ZUnicodeString.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace ZRA {
namespace SQL {

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::Expr_Rel_SFW

class Expr_Rel_SFW : public Expr_Rel_Concrete
	{
public:
	Expr_Rel_SFW(const Rename_t& iRename,
		const RelHead& iRelHead,
		ZRef<ZExpr_Logic> iCondition,
		const std::vector<ZRef<Expr_Rel_Concrete> >& iRels);

// From Expr_Rel_Concrete
	virtual RelHead GetRelHead();

// Our protocol
	const Rename_t& GetRename();
	ZRef<ZExpr_Logic> GetCondition();
	const std::vector<ZRef<Expr_Rel_Concrete> >& GetRels();

public:
	const Rename_t fRename;
	const RelHead fRelHead;
	const ZRef<ZExpr_Logic> fCondition;
	const std::vector<ZRef<Expr_Rel_Concrete> > fRels;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZRA::SQL::sConvert

ZRef<Expr_Rel_SFW> sConvert(ZRef<Expr_Rel> iExpr);

void sAsSQL(ZRef<Expr_Rel_SFW> iSFW, const ZStrimW& s);
string8 sAsSQL(ZRef<Expr_Rel_SFW> iSFW);

} // namespace SQL
} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_SQL__
