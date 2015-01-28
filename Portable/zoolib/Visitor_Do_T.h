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

#ifndef __ZooLib_Visitor_Do_T_h__
#define __ZooLib_Visitor_Do_T_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // For SaveSetRestore
#include "zoolib/Visitor.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Visitor_Do_T

template <class Result_t>
class Visitor_Do_T
:	public virtual Visitor
	{
public:
	Visitor_Do_T()
		{}

// Our protocol
	virtual ZQ<Result_t> QDo(const ZRef<Visitee>& iRep)
		{
		if (iRep)
			{
			SaveSetRestore<ZQ<Result_t> > ssr(fResultQ, null);
			iRep->Accept(*this);
			return fResultQ;
			}
		return null;
		}

	Result_t DDo(const Result_t& iDefault, const ZRef<Visitee>& iRep)
		{ return sDGet(iDefault, this->QDo(iRep)); }

	Result_t Do(const ZRef<Visitee>& iRep)
		{ return sGet(this->QDo(iRep)); }

protected:
	const ZQ<Result_t>& QGetResult()
		{ return fResultQ; }

	void pSetResult(const Result_t& iResult)
		{
		ZAssert(not fResultQ);
		fResultQ = iResult;
		}

private:
	ZQ<Result_t> fResultQ;
	};

} // namespace ZooLib

#endif // __ZooLib_Visitor_Do_T_h__
