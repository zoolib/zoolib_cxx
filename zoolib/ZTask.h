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

namespace ZooLib {

class ZTask;

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskMaster

class ZTaskMaster
:	public ZCounted,
	public ZWeakReferee
	{
public:
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

class ZTask : public ZCounted
	{
public:
	ZTask(ZRef<ZTaskMaster> iTaskMaster);

	ZRef<ZTaskMaster> GetTaskMaster();

	virtual void TaskMaster_Detached(ZRef<ZTaskMaster> iTaskMaster);

	virtual void Kill();

protected:
	void pFinished();

private:
	ZWeakRef<ZTaskMaster> fTaskMaster;
	friend class ZTaskMaster;
	};

} // namespace ZooLib

#endif // __ZTask__
