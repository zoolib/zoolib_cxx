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

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskOwner

void ZTaskOwner::Task_Finished(ZRef<ZTask> iTask)
	{}

void ZTaskOwner::pDetachTask(ZRef<ZTask> iTask)
	{
	iTask->TaskOwner_Detached(this);
	iTask->fTaskOwner.Clear();
	}

void ZTaskOwner::pTask_Finished(ZRef<ZTask> iTask)
	{
	this->Task_Finished(iTask);
	iTask->fTaskOwner.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTask

ZTask::ZTask(ZRef<ZTaskOwner> iTaskOwner)
:	fTaskOwner(iTaskOwner)
	{}

ZRef<ZTaskOwner> ZTask::GetOwner()
	{ return fTaskOwner; }

void ZTask::TaskOwner_Detached(ZRef<ZTaskOwner> iTaskOwner)
	{}

void ZTask::Kill()
	{}

void ZTask::pFinished()
	{
	if (ZRef<ZTaskOwner> theTaskOwner = fTaskOwner)
		theTaskOwner->pTask_Finished(this);
	}

NAMESPACE_ZOOLIB_END
