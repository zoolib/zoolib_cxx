/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZTaskWorker__
#define __ZTaskWorker__ 1
#include "zconfig.h"

#include "zoolib/ZTask.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker

class ZTaskWorker
:	public ZTask
,	public ZWorker
	{
public:
	ZTaskWorker(ZRef<ZTaskMaster> iTaskMaster);

// From ZWorker
	virtual void RunnerDetached();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T0

class ZTaskWorker_T0 : public ZTaskWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZTaskWorker>);

	ZTaskWorker_T0(ZRef<ZTaskMaster> iTaskMaster, Function_t iFunction)
	:	ZTaskWorker(iTaskMaster)
	,	fFunction(iFunction)
		{}

	virtual bool Work()
		{ return fFunction(this); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T1

template <class T0>
class ZTaskWorker_T1 : public ZTaskWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZTaskWorker>, const T0&);

	ZTaskWorker_T1(ZRef<ZTaskMaster> iTaskMaster, Function_t iFunction, const T0& iT0)
	:	ZTaskWorker(iTaskMaster)
	,	fFunction(iFunction)
	,	fT0(iT0)
		{}

	virtual bool Work()
		{ return fFunction(this, fT0); }

private:
	Function_t fFunction;
	T0 fT0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T2

template <class T0, class T1>
class ZTaskWorker_T2 : public ZTaskWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZTaskWorker>, const T0&, const T1&);

	ZTaskWorker_T2(ZRef<ZTaskMaster> iTaskMaster,
		Function_t iFunction, const T0& iT0, const T1& iT1)
	:	ZTaskWorker(iTaskMaster)
	,	fFunction(iFunction)
	,	fT0(iT0)
	,	fT1(iT1)
		{}

	virtual bool Work()
		{ return fFunction(this, fT0, fT1); }

private:
	Function_t fFunction;
	T0 fT0;
	T1 fT1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskWorker_T3

template <class T0, class T1, class T2>
class ZTaskWorker_T3 : public ZTaskWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZTaskWorker>, const T0& iT0, const T1&, const T2&);

	ZTaskWorker_T3(ZRef<ZTaskMaster> iTaskMaster,
		Function_t iFunction, const T0& iT0, const T1& iT1, const T2& iT2)
	:	ZTaskWorker(iTaskMaster)
	,	fFunction(iFunction)
	,	fT0(iT0)
	,	fT1(iT1)
	,	fT2(iT2)
		{}

	virtual bool Work()
		{ return fFunction(this, fT0, fT1, fT2); }

private:
	Function_t fFunction;
	T0 fT0;
	T1 fT1;
	T2 fT2;
	};

} // namespace ZooLib

#endif // __ZTaskWorker__
