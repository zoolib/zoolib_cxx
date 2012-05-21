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

#ifndef __ZVisitor_Do_T_h__
#define __ZVisitor_Do_T_h__
#include "zconfig.h"

#include "zoolib/ZCompat_algorithm.h" // For ZSetRestore_T
#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZVisitor.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZVisitor_Do_T

template <class Result_t>
class ZVisitor_Do_T
:	public virtual ZVisitor
	{
public:
	ZVisitor_Do_T()
		{}

// Our protocol
	virtual ZQ<Result_t> QDo(const ZRef<ZVisitee>& iRep)
		{
		if (iRep)
			{
			ZSetRestore_T<ZQ<Result_t> > sr(fResultQ);
			iRep->Accept(*this);
			return fResultQ;
			}
		return null;
		}

	Result_t DDo(const Result_t& iDefault, const ZRef<ZVisitee>& iRep)
		{
		if (ZQ<Result_t> theQ = this->QDo(iRep))
			return *theQ;
		return iDefault;
		}

	Result_t Do(const ZRef<ZVisitee>& iRep)
		{
		if (ZQ<Result_t> theQ = this->QDo(iRep))
			return *theQ;
		return Result_t();
		}

protected:
	const ZQ<Result_t>& QGetResult()
		{ return fResultQ; }

	void pSetResult(const Result_t& iResult)
		{
		ZAssert(!fResultQ);
		fResultQ = iResult;
		}

private:
	ZQ<Result_t> fResultQ;
	};

} // namespace ZooLib

#endif // __ZVisitor_Do_T_h__
