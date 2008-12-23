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

#include "zoolib/ZTQL_LogOp.h"

using std::set;

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp

ZTQL::LogOp::LogOp()
	{}

ZTQL::LogOp::~LogOp()
	{}

void ZTQL::LogOp::GatherPropNames(set<ZTName>& ioNames)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_True

ZTQL::LogOp_True::LogOp_True()
	{}

ZTQL::LogOp_True::~LogOp_True()
	{}

bool ZTQL::LogOp_True::Matches(const ZTuple& iTuple)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_False

ZTQL::LogOp_False::LogOp_False()
	{}

ZTQL::LogOp_False::~LogOp_False()
	{}

bool ZTQL::LogOp_False::Matches(const ZTuple& iTuple)
	{ return false; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_And

ZTQL::LogOp_And::LogOp_And(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS)
:	fLHS(iLHS),
	fRHS(iRHS)
	{}

ZTQL::LogOp_And::~LogOp_And()
	{}

bool ZTQL::LogOp_And::Matches(const ZTuple& iTuple)
	{
	if (fLHS)
		{
		if (fRHS)
			return fLHS->Matches(iTuple) && fRHS->Matches(iTuple);
		}
	return false;
	}

void ZTQL::LogOp_And::GatherPropNames(set<ZTName>& ioNames)
	{
	if (fLHS)
		fLHS->GatherPropNames(ioNames);
	if (fRHS)
		fRHS->GatherPropNames(ioNames);
	}

ZRef<ZTQL::LogOp> ZTQL::LogOp_And::GetLHS()
	{ return fLHS; }

ZRef<ZTQL::LogOp> ZTQL::LogOp_And::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_Or

ZTQL::LogOp_Or::LogOp_Or(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS)
:	fLHS(iLHS),
	fRHS(iRHS)
	{}

ZTQL::LogOp_Or::~LogOp_Or()
	{}

bool ZTQL::LogOp_Or::Matches(const ZTuple& iTuple)
	{
	if (fLHS)
		{
		if (fRHS)
			return fLHS->Matches(iTuple) || fRHS->Matches(iTuple);
		else
			return fLHS->Matches(iTuple);
		}
	else if (fRHS)
		{
		return fRHS->Matches(iTuple);
		}
	return false;
	}

void ZTQL::LogOp_Or::GatherPropNames(set<ZTName>& ioNames)
	{
	if (fLHS)
		fLHS->GatherPropNames(ioNames);
	if (fRHS)
		fRHS->GatherPropNames(ioNames);
	}

ZRef<ZTQL::LogOp> ZTQL::LogOp_Or::GetLHS()
	{ return fLHS; }

ZRef<ZTQL::LogOp> ZTQL::LogOp_Or::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_Condition

ZTQL::LogOp_Condition::LogOp_Condition(const Condition& iCondition)
:	fCondition(iCondition)
	{}

ZTQL::LogOp_Condition::~LogOp_Condition()
	{}

bool ZTQL::LogOp_Condition::Matches(const ZTuple& iTuple)
	{ return fCondition.Matches(iTuple); }

void ZTQL::LogOp_Condition::GatherPropNames(set<ZTName>& ioNames)
	{ fCondition.GatherPropNames(ioNames); }

const ZTQL::Condition& ZTQL::LogOp_Condition::GetCondition()
	{ return fCondition; }

