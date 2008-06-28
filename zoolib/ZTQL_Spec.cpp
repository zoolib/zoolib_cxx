/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "ZTQL_Spec.h"

#include "ZTQL_LogOp.h"

using std::set;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Comparand derivatives

ZTQL::CName::CName(const ZTName& iName)
:	Comparand(new ComparandRep_Name(iName))
	{}

ZTQL::CValue::CValue(const ZTValue& iValue)
:	Comparand(new ComparandRep_Value(iValue))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Spec

ZTQL::Spec::Spec()
	{}

ZTQL::Spec::Spec(const Spec& iOther)
:	fLogOp(iOther.fLogOp)
	{}

ZTQL::Spec::Spec(ZRef<LogOp> iLogOp)
:	fLogOp(iLogOp)
	{}

ZTQL::Spec::Spec(const Condition& iCondition)
:	fLogOp(new LogOp_Condition(iCondition))
	{}

ZTQL::Spec& ZTQL::Spec::operator=(const Spec& iOther)
	{
	fLogOp = iOther.fLogOp;
	return *this;
	}

ZTQL::Spec::~Spec()
	{}

bool ZTQL::Spec::Matches(const ZTuple& iTuple) const
	{
	if (fLogOp)
		return fLogOp->Matches(iTuple);
	return false;
	}

ZTQL::RelHead ZTQL::Spec::GetRelHead() const
	{
	set<ZTName> theNames;
	if (fLogOp)
		fLogOp->GatherPropNames(theNames);
	return theNames;
	}

ZTQL::Spec ZTQL::Spec::operator&(const Spec& iOther) const
	{
	if (fLogOp && iOther.fLogOp)
		return Spec(new LogOp_And(fLogOp, iOther.fLogOp));

	return Spec();
	}

ZTQL::Spec& ZTQL::Spec::operator&=(const Spec& iOther)
	{
	if (fLogOp && iOther.fLogOp)
		fLogOp = new LogOp_And(fLogOp, iOther.fLogOp);
	else
		fLogOp.Clear();

	return *this;
	}

ZTQL::Spec ZTQL::Spec::operator|(const Spec& iOther) const
	{
	if (fLogOp)
		{
		if (iOther.fLogOp)
			return Spec(new LogOp_Or(fLogOp, iOther.fLogOp));
		else
			return fLogOp;
		}
	else
		{
		return iOther.fLogOp;
		}
	}

ZTQL::Spec& ZTQL::Spec::operator|=(const Spec& iOther)
	{
	if (fLogOp)
		{
		if (iOther.fLogOp)
			fLogOp = new LogOp_Or(fLogOp, iOther.fLogOp);
		}
	else
		{
		fLogOp = iOther.fLogOp;
		}

	return *this;
	}

ZRef<ZTQL::LogOp> ZTQL::Spec::GetLogOp() const
	{ return fLogOp; }

ZTQL::Spec ZTQL::operator&(const Condition& iCondition, const Spec& iSpec)
	{ return Spec(iCondition) & iSpec; }

ZTQL::Spec ZTQL::operator|(const Condition& iCondition, const Spec& iSpec)
	{ return Spec(iCondition) | iSpec; }
