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

#ifndef __ZValPredCompound__
#define __ZValPredCompound__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic_ValPred.h"
#include "zoolib/ZValPred.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZValPredCompound

class ZValPredCompound
	{
public:
	typedef std::vector<ZValPred> Sect;
	typedef std::vector<Sect> SectUnion;

	static ZValPredCompound sTrue();
	static ZValPredCompound sFalse();

	ZValPredCompound();
	ZValPredCompound(const ZValPredCompound& iOther);
	~ZValPredCompound();
	ZValPredCompound& operator=(const ZValPredCompound& iOther);

	ZValPredCompound(SectUnion* ioSectUnion);

	ZValPredCompound(const ZValPred& iValPred);
	ZValPredCompound(const Sect& iSect);
	ZValPredCompound(const SectUnion& iSectUnion);

	bool IsAny() const;
	bool IsNone() const;

	ZValPredCompound operator&(const ZValPredCompound& iOther) const;
	ZValPredCompound& operator&=(const ZValPredCompound& iOther);

	ZValPredCompound operator|(const ZValPredCompound& iOther) const;
	ZValPredCompound& operator|=(const ZValPredCompound& iOther);

	bool Matches(ZValContext& iContext, const ZVal_Any& iVal) const;

public:
	SectUnion fSectUnion;
	};

ZValPredCompound sAsValPredCompound(ZRef<ZExpr_Logic> iExpr);
ZRef<ZExpr_Logic> sAsExpr_Logic(const ZValPredCompound& iVCF);

} // namespace ZooLib

#endif // __ZValPredCompound__
