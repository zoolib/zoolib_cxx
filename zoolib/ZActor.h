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

#ifndef __ZActor__
#define __ZActor__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZRefWeak.h"
#include "zoolib/ZTime.h"

NAMESPACE_ZOOLIB_BEGIN

class ZActor;

// =================================================================================================
#pragma mark -
#pragma mark * ZActorRunner

class ZActorRunner
:	public ZRefCountedWithFinalize,
	public ZWeakReferee
	{
public:
	ZActorRunner();
	virtual ~ZActorRunner();

protected:
// Called by subclasses
	void pAttachActor(ZRef<ZActor> iActor);
	void pDetachActor(ZRef<ZActor> iActor);

// Called by ZActor instances.
	virtual void Actor_Wake(ZRef<ZActor> iActor) = 0;
	virtual void Actor_WakeAt(ZRef<ZActor> iActor, ZTime iSystemTime) = 0;
	virtual void Actor_WakeIn(ZRef<ZActor> iActor, double iInterval) = 0;

	friend class ZActor;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZActor

class ZActor : public ZRefCountedWithFinalize
	{
public:
	ZActor();

	virtual void RunnerAttached();
	virtual void RunnerDetached();

	virtual bool Act() = 0;

	void Wake();
	void WakeAt(ZTime iSystemTime);
	void WakeIn(double iInterval);

private:
	void pRunnerAttached();
	void pRunnerDetached();

	ZRefWeak<ZActorRunner> fRunner;
	friend class ZActorRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Utility methods

// Here for now till I find a better home.
void sStartActorRunner(ZRef<ZActor> iActor);

NAMESPACE_ZOOLIB_END

#endif // __ZActor__
