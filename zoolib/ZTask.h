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

#ifndef __ZTask__
#define __ZTask__ 1
#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZWeakRef.h"

NAMESPACE_ZOOLIB_BEGIN

class ZTask;

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskOwner

class ZTaskOwner
:	public ZRefCountedWithFinalize,
	public ZWeakReferee
	{
public:
	ZTaskOwner();
	virtual ~ZTaskOwner();

	virtual void Task_Finished(ZRef<ZTask> iTask);

protected:
	void pDetachTask(ZRef<ZTask> iTask);

private:
	void pTask_Finished(ZRef<ZTask> iTask);
	friend class ZTask;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTask

class ZTask : public ZRefCountedWithFinalize
	{
public:
	ZTask(ZRef<ZTaskOwner> iTaskOwner);
	virtual ~ZTask();

	ZRef<ZTaskOwner> GetOwner();

	virtual void TaskOwner_Detached(ZRef<ZTaskOwner> iTaskOwner);

	virtual void Kill();

protected:
	void pFinished();

private:
	ZWeakRef<ZTaskOwner> fTaskOwner;
	friend class ZTaskOwner;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZTask__
