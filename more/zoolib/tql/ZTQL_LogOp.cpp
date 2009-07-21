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

#include "zoolib/tql/ZTQL_LogOp.h"

using std::set;

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

// =================================================================================================
#pragma mark -
#pragma mark * LogOp

LogOp::LogOp()
	{}

LogOp::~LogOp()
	{}

void LogOp::GatherPropNames(set<ZTName>& ioNames)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * LogOp_True

LogOp_True::LogOp_True()
	{}

LogOp_True::~LogOp_True()
	{}

bool LogOp_True::Accept(LogOpVisitor& iVisitor)
	{ return iVisitor.Visit_True(this); }

bool LogOp_True::Matches(const ZTuple& iTuple)
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * LogOp_False

LogOp_False::LogOp_False()
	{}

LogOp_False::~LogOp_False()
	{}

bool LogOp_False::Accept(LogOpVisitor& iVisitor)
	{ return iVisitor.Visit_False(this); }

bool LogOp_False::Matches(const ZTuple& iTuple)
	{ return false; }

// =================================================================================================
#pragma mark -
#pragma mark * LogOp_And

LogOp_And::LogOp_And(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS)
:	fLHS(iLHS),
	fRHS(iRHS)
	{}

LogOp_And::~LogOp_And()
	{}

bool LogOp_And::Accept(LogOpVisitor& iVisitor)
	{ return iVisitor.Visit_And(this); }

bool LogOp_And::Matches(const ZTuple& iTuple)
	{
	if (fLHS)
		{
		if (fRHS)
			return fLHS->Matches(iTuple) && fRHS->Matches(iTuple);
		}
	return false;
	}

void LogOp_And::GatherPropNames(set<ZTName>& ioNames)
	{
	if (fLHS)
		fLHS->GatherPropNames(ioNames);
	if (fRHS)
		fRHS->GatherPropNames(ioNames);
	}

ZRef<LogOp> LogOp_And::GetLHS()
	{ return fLHS; }

ZRef<LogOp> LogOp_And::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * LogOp_Or

LogOp_Or::LogOp_Or(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS)
:	fLHS(iLHS),
	fRHS(iRHS)
	{}

LogOp_Or::~LogOp_Or()
	{}

bool LogOp_Or::Accept(LogOpVisitor& iVisitor)
	{ return iVisitor.Visit_Or(this); }

bool LogOp_Or::Matches(const ZTuple& iTuple)
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

void LogOp_Or::GatherPropNames(set<ZTName>& ioNames)
	{
	if (fLHS)
		fLHS->GatherPropNames(ioNames);
	if (fRHS)
		fRHS->GatherPropNames(ioNames);
	}

ZRef<LogOp> LogOp_Or::GetLHS()
	{ return fLHS; }

ZRef<LogOp> LogOp_Or::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * LogOp_Condition

LogOp_Condition::LogOp_Condition(const Condition& iCondition)
:	fCondition(iCondition)
	{}

LogOp_Condition::~LogOp_Condition()
	{}

bool LogOp_Condition::Accept(LogOpVisitor& iVisitor)
	{ return iVisitor.Visit_Condition(this); }

bool LogOp_Condition::Matches(const ZTuple& iTuple)
	{ return fCondition.Matches(iTuple); }

void LogOp_Condition::GatherPropNames(set<ZTName>& ioNames)
	{ fCondition.GatherPropNames(ioNames); }

const Condition& LogOp_Condition::GetCondition()
	{ return fCondition; }

// =================================================================================================
#pragma mark -
#pragma mark * LogOpVisitor

LogOpVisitor::LogOpVisitor()
	{}

LogOpVisitor::~LogOpVisitor()
	{}

bool LogOpVisitor::Visit_True(ZRef<LogOp_True> iLogOp)
	{ return true; }

bool LogOpVisitor::Visit_False(ZRef<LogOp_False> iLogOp)
	{ return true; }

bool LogOpVisitor::Visit_And(ZRef<LogOp_And> iLogOp)
	{ return true; }

bool LogOpVisitor::Visit_Or(ZRef<LogOp_Or> iLogOp)
	{ return true; }

bool LogOpVisitor::Visit_Condition(ZRef<LogOp_Condition> iLogOp)
	{ return true; }

} // namespace ZTQL

NAMESPACE_ZOOLIB_END
