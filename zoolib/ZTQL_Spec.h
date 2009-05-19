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

#ifndef __ZTQL_Spec__
#define __ZTQL_Spec__ 1

#include "zconfig.h"

#include "zoolib/ZTQL_Condition.h"
#include "zoolib/ZTQL_RelHead.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

class LogOp;

// =================================================================================================
#pragma mark -
#pragma mark * Comparand derivatives

class CName : public Comparand
	{
public:
	CName(const ZTName& iName);
	};

class CValue : public Comparand
	{
public:
	CValue(const ZTValue& iValue);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Spec

class Spec
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Spec, operator_bool_generator_type, operator_bool_type);
public:

	Spec();

	Spec(const Spec& iOther);

	Spec(ZRef<LogOp> iLogOp);

	Spec(bool iBool);

	Spec(const Condition& iCondition);

	Spec& operator=(const Spec& iOther);

	~Spec();

	operator operator_bool_type() const;

	bool Matches(const ZTuple& iTuple) const;

	RelHead GetRelHead() const;

	Spec operator&(const Spec& iOther) const;

	Spec& operator&=(const Spec& iOther);

	Spec operator|(const Spec& iOther) const;

	Spec& operator|=(const Spec& iOther);

	ZRef<LogOp> GetLogOp() const;

private:
	ZRef<LogOp> fLogOp;
	};

Spec operator&(const Condition& iCondition, const Spec& iSpec);

Spec operator|(const Condition& iCondition, const Spec& iSpec);

} // namespace ZTQL

NAMESPACE_ZOOLIB_END

#endif // __ZTQL_Spec__
