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

#include "zoolib/ZTask.h"

namespace ZooLib {

/**
\defgroup Task

*/

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskMaster

/**
\class ZTaskMaster
\ingroup Task
\sa task
*/

void ZTaskMaster::Task_Finished(ZRef<ZTask> iTask)
	{}

void ZTaskMaster::pDetachTask(ZRef<ZTask> iTask)
	{
	iTask->TaskMaster_Detached(this);
	iTask->fTaskMaster.Clear();
	}

void ZTaskMaster::pTask_Finished(ZRef<ZTask> iTask)
	{
	this->Task_Finished(iTask);
	iTask->fTaskMaster.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTask

/**
\class ZTask
\ingroup Task
\sa task
*/

ZTask::ZTask(ZRef<ZTaskMaster> iTaskMaster)
:	fTaskMaster(iTaskMaster)
	{}

ZRef<ZTaskMaster> ZTask::GetTaskMaster()
	{ return fTaskMaster; }

void ZTask::TaskMaster_Detached(ZRef<ZTaskMaster> iTaskMaster)
	{}

void ZTask::Kill()
	{}

void ZTask::pFinished()
	{
	if (ZRef<ZTaskMaster> theTaskMaster = fTaskMaster)
		theTaskMaster->pTask_Finished(this);
	}

} // namespace ZooLib
