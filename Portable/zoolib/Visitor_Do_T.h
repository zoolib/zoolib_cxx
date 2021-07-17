// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Visitor_Do_T_h__
#define __ZooLib_Visitor_Do_T_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_algorithm.h" // For SaveSetRestore
#include "zoolib/Visitor.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZQ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Do_T

template <class Result_t>
class Visitor_Do_T
:	public virtual Visitor
	{
public:
	Visitor_Do_T()
		{}

// Our protocol
	virtual ZQ<Result_t> QDo(const ZP<Visitee>& iRep)
		{
		if (iRep)
			{
			SaveSetRestore<ZQ<Result_t>> ssr(fResultQ, null);
			iRep->Accept(*this);
			return fResultQ;
			}
		return null;
		}

//	Result_t DDo(const Result_t& iDefault, const ZP<Visitee>& iRep)
//		{ return sDGet(iDefault, this->QDo(iRep)); }

	Result_t Do(const ZP<Visitee>& iRep)
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
