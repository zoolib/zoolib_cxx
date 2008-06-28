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

#ifndef __ZTQL_LogOp__
#define __ZTQL_LogOp__ 1

#include "zconfig.h"

#include "zoolib/ZTQL_Condition.h"
#include "zoolib/ZTQL_RelHead.h"
#include "zoolib/ZTuple.h"

#include <set>

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp

namespace ZTQL {

class LogOp : public ZRefCounted
	{
protected:
	LogOp();

public:
	virtual ~LogOp();

	virtual bool Matches(const ZTuple& iTuple) = 0;
	virtual void GatherPropNames(std::set<ZTName>& ioNames);
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_True

namespace ZTQL {

class LogOp_True : public LogOp
	{
public:
	LogOp_True();
	virtual ~LogOp_True();

// From LogOp
	virtual bool Matches(const ZTuple& iTuple);
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_False

namespace ZTQL {

class LogOp_False : public LogOp
	{
public:
	LogOp_False();
	virtual ~LogOp_False();

// From LogOp
	virtual bool Matches(const ZTuple& iTuple);
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_And

namespace ZTQL {

class LogOp_And : public LogOp
	{
public:
	LogOp_And(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS);
	virtual ~LogOp_And();

// From LogOp
	virtual bool Matches(const ZTuple& iTuple);
	virtual void GatherPropNames(std::set<ZTName>& ioNames);

// Our protocol
	ZRef<LogOp> GetLHS();
	ZRef<LogOp> GetRHS();

private:
	ZRef<LogOp> fLHS;
	ZRef<LogOp> fRHS;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_Or

namespace ZTQL {

class LogOp_Or : public LogOp
	{
public:
	LogOp_Or(ZRef<LogOp> iLHS, ZRef<LogOp> iRHS);
	virtual ~LogOp_Or();

// From LogOp
	virtual bool Matches(const ZTuple& iTuple);
	virtual void GatherPropNames(std::set<ZTName>& ioNames);

// Our protocol
	ZRef<LogOp> GetLHS();
	ZRef<LogOp> GetRHS();

private:
	ZRef<LogOp> fLHS;
	ZRef<LogOp> fRHS;
	};

} // namespace ZTQL

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::LogOp_Condition

namespace ZTQL {

class LogOp_Condition : public LogOp
	{
public:
	LogOp_Condition(const Condition& iCondition);
	virtual ~LogOp_Condition();

// From LogOp
	virtual bool Matches(const ZTuple& iTuple);
	virtual void GatherPropNames(std::set<ZTName>& ioNames);

// Our protocol
	const Condition& GetCondition();

private:
	Condition fCondition;
	};

} // namespace ZTQL

#endif // __ZTQL_LogOp__
