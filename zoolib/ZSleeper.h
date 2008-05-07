/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZSleeper__
#define __ZSleeper__ 1
#include "zconfig.h"

#include "ZRefCount.h"
#include "ZThread.h"
#include "ZTime.h"

class ZSleeperRunner;

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeper

class ZSleeper
	{
	friend class ZSleeperRunner;

protected:
	ZSleeper();
	virtual ~ZSleeper();

public:
// Our protocol
	virtual bool Execute() = 0;

	virtual void RunnerAttached(ZSleeperRunner* iRunner);
	virtual void RunnerDetached(ZSleeperRunner* iRunner);

	void Wake();
	void WakeAt(ZTime iSystemTime);

private:
	ZSleeperRunner* fRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunner

class ZSleeperRunner
	{
	friend class ZSleeper;
protected:
// Called by subclasses
	void pAttachSleeper(ZSleeper* iSleeper);
	void pDetachSleeper(ZSleeper* iSleeper);

// Called by ZSleeper instances.
	virtual void WakeAt(ZSleeper* iSleeper, ZTime iSystemTime) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunnerFactory

// Do we even need this? And it should not return a SleeperRunner

class ZSleeperRunnerFactory : public ZRefCountedWithFinalization
	{
public:
	virtual void MakeRunner(ZSleeper* iSleeper) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunner_Threaded

class ZSleeperRunner_Threaded : public ZSleeperRunner
	{
public:
	ZSleeperRunner_Threaded(ZSleeper* iSleeper);
	~ZSleeperRunner_Threaded();

	void Start();

// From ZSleeperRunner
	virtual void WakeAt(ZSleeper* iSleeper, ZTime iSystemTime);

private:
	void pRun();
	static void spRun(ZSleeperRunner_Threaded* iRunner);

	ZMutex fMutex;
	ZCondition fCondition;
	ZSleeper* fSleeper;
	ZTime fNextWake;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunnerFactory_Threaded

class ZSleeperRunnerFactory_Threaded : public ZSleeperRunnerFactory
	{
public:
	virtual void MakeRunner(ZSleeper* iSleeper);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSleeperRunnerFactory_Pooled??

#endif // __ZSleeper__
