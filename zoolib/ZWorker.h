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

#ifndef __ZWorker__
#define __ZWorker__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZWeakRef.h"

namespace ZooLib {

class ZWorkerRunner;

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker

class ZWorker
:	public ZCounted
	{
public:
	virtual void RunnerAttached();
	virtual void RunnerDetached();

	virtual bool Work() = 0;

	void Wake();
	void WakeAt(ZTime iSystemTime);
	void WakeIn(double iInterval);

	bool IsAwake();
	bool IsAttached();

private:
	ZWeakRef<ZWorkerRunner> fRunner;
	friend class ZWorkerRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorkerRunner

class ZWorkerRunner
:	public ZCounted
,	public ZWeakReferee
	{
protected:
// Called by subclasses
	bool pAttachWorker(ZRef<ZWorker> iWorker);
	void pDetachWorker(ZRef<ZWorker> iWorker);
	bool pInvokeWork(ZRef<ZWorker> iWorker);

// Called by ZWorker instances.
	virtual void Wake(ZRef<ZWorker> iWorker) = 0;
	virtual void WakeAt(ZRef<ZWorker> iWorker, ZTime iSystemTime) = 0;
	virtual void WakeIn(ZRef<ZWorker> iWorker, double iInterval) = 0;
	virtual bool IsAwake(ZRef<ZWorker> iWorker) = 0;

	friend class ZWorker;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

void sStartWorkerRunner(ZRef<ZWorker> iWorker);

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_T0

class ZWorker_T0 : public ZWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZWorker> iWorker);

	ZWorker_T0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	virtual bool Work()
		{ return fFunction(this); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_T1

template <class T0>
class ZWorker_T1 : public ZWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZWorker>, const T0&);

	ZWorker_T1(Function_t iFunction, const T0& iT0)
	:	fFunction(iFunction)
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
#pragma mark * ZWorker_T2

template <class T0, class T1>
class ZWorker_T2 : public ZWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZWorker>, const T0&, const T1&);

	ZWorker_T2(Function_t iFunction, const T0& iT0, const T1& iT1)
	:	fFunction(iFunction)
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
#pragma mark * ZWorker_T3

template <class T0, class T1, class T2>
class ZWorker_T3 : public ZWorker
	{
public:
	typedef bool (*Function_t)(ZRef<ZWorker>, const T0&, const T1&, const T2&);

	ZWorker_T3(Function_t iFunction, const T0& iT0, const T1& iT1, const T2& iT2)
	:	fFunction(iFunction)
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

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Void_T0

class ZWorker_Void_T0 : public ZWorker
	{
public:
	typedef void (*Function_t)();

	ZWorker_Void_T0(Function_t iFunction)
	:	fFunction(iFunction)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fFunction(); }

private:
	Function_t fFunction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Void_T1

template <class T0>
class ZWorker_Void_T1 : public ZWorker
	{
public:
	typedef void (*Function_t)(const T0&);

	ZWorker_Void_T1(Function_t iFunction, const T0& iT0)
	:	fFunction(iFunction)
	,	fT0(iT0)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fFunction(fT0); }

private:
	Function_t fFunction;
	T0 fT0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Void_T2

template <class T0, class T1>
class ZWorker_Void_T2 : public ZWorker
	{
public:
	typedef void (*Function_t)(const T0&, const T1&);

	ZWorker_Void_T2(Function_t iFunction, const T0& iT0, const T1& iT1)
	:	fFunction(iFunction)
	,	fT0(iT0)
	,	fT1(iT1)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fFunction(fT0, fT1); }

private:
	Function_t fFunction;
	T0 fT0;
	T1 fT1;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZWorker_Void_T3

template <class T0, class T1, class T2>
class ZWorker_Void_T3 : public ZWorker
	{
public:
	typedef void (*Function_t)(const T0&, const T1&, const T2&);

	ZWorker_Void_T3(Function_t iFunction, const T0& iT0, const T1& iT1, const T2& iT2)
	:	fFunction(iFunction)
	,	fT0(iT0)
	,	fT1(iT1)
	,	fT2(iT2)
		{}

	virtual bool Work()
		{ return false; }

	virtual void RunnerDetached()
		{ fFunction(fT0, fT1, fT2); }

private:
	Function_t fFunction;
	T0 fT0;
	T1 fT1;
	T2 fT2;
	};
} // namespace ZooLib

#endif // __ZWorker__
