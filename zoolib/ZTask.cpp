#include "zoolib/ZTask.h"

#include "zoolib/ZThreadSimple.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZTask

ZTask::ZTask()
:	fOwner(nil)
	{}

ZTask::~ZTask()
	{
	ZAssert(!fOwner);
	}

void ZTask::OwnerDetached()
	{}

void ZTask::Message(const string& iString, void* iRefcon)
	{}

void ZTask::pStarted(ZTaskOwner* iOwner)
	{
	ZAssert(!fOwner);
	iOwner->pTaskStarted(this);
	}

void ZTask::pStopped()
	{
	if (ZTaskOwner* theOwner = fOwner)
		theOwner->pTaskStopped(this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskOwner

ZTaskOwner::ZTaskOwner()
	{}

ZTaskOwner::~ZTaskOwner()
	{
	ZMutexLocker locker(fMutex);
	for (ZooLib::DListIteratorEraseAll<ZTask, ZTaskLink_Live>
		iter = fTasks_Live;iter; iter.Advance())
		{
		ZTask* theTask = iter.Current();
		ZAssert(theTask->fOwner == this);
		theTask->fOwner = nil;
		theTask->OwnerDetached();
		}
	}

void ZTaskOwner::Tell(const string& iString, void* iRefcon)
	{
	ZMutexLocker locker(fMutex);
	for (ZooLib::DListIteratorEraseAll<ZTask, ZTaskLink_Live>
		iter = fTasks_Live;iter; iter.Advance())
		{
		ZTask* theTask = iter.Current();
		fTasks_Processed.Insert(theTask);
		theTask->Message(iString, iRefcon);
		}

	for (ZooLib::DListIteratorEraseAll<ZTask, ZTaskLink_Processed>
		iter = fTasks_Processed;iter; iter.Advance())
		{
		fTasks_Live.Insert(iter.Current());
		}
	}

void ZTaskOwner::StopAll()
	{
	ZMutexLocker locker(fMutex);
	for (ZooLib::DListIteratorEraseAll<ZTask, ZTaskLink_Live>
		iter = fTasks_Live;iter; iter.Advance())
		{
		ZTask* theTask = iter.Current();
		fTasks_Processed.Insert(theTask);
		theTask->Stop();
		}

	for (ZooLib::DListIteratorEraseAll<ZTask, ZTaskLink_Processed>
		iter = fTasks_Processed;iter; iter.Advance())
		{
		fTasks_Live.Insert(iter.Current());
		}
	}

void ZTaskOwner::Wake()	
	{
	ZMutexLocker locker(fMutex);
	fCondition.Broadcast();
	}

bool ZTaskOwner::WaitLess(size_t iCount)	
	{
	ZMutexLocker locker(fMutex);
	if (fTasks_Live.Size() < iCount)
		return true;
	fCondition.Wait(fMutex);
	return fTasks_Live.Size() < iCount;
	}

bool ZTaskOwner::WaitGreater(size_t iCount)
	{
	ZMutexLocker locker(fMutex);
	if (fTasks_Live.Size() > iCount)
		return true;
	fCondition.Wait(fMutex);
	return fTasks_Live.Size() > iCount;
	}

bool ZTaskOwner::WaitEmpty()
	{ return this->WaitLess(1); }

void ZTaskOwner::TaskAttached(ZTask* iTask)
	{}

void ZTaskOwner::TaskDetached(ZTask* iTask)
	{}

void ZTaskOwner::CountChanged(size_t iCount)
	{}

void ZTaskOwner::pTaskStarted(ZTask* iTask)
	{
	ZMutexLocker locker(fMutex);
	ZAssert(!iTask->fOwner);
	iTask->fOwner = this;
	fTasks_Live.Insert(iTask);

	this->TaskAttached(iTask);
	this->CountChanged(fTasks_Live.Size());

	fCondition.Broadcast();
	}

void ZTaskOwner::pTaskStopped(ZTask* iTask)
	{
	ZMutexLocker locker(fMutex);
	ZAssert(iTask->fOwner == this);
	iTask->fOwner = nil;
	fTasks_Live.Remove(iTask);

	this->TaskDetached(iTask);
	this->CountChanged(fTasks_Live.Size());

	fCondition.Broadcast();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTask_Threaded

ZTask_Threaded::ZTask_Threaded()
:	fThread(nil)
	{}

ZTask_Threaded::~ZTask_Threaded()
	{}

void ZTask_Threaded::TaskStart(ZTaskOwner* iOwner, bool iWait)
	{
	ZAssert(!fThread);

	this->pStarted(iOwner);

	fThread = new ZThreadSimple<ZTask_Threaded*>(sTaskRun, this, "ZTask_Threaded::TaskStartWait");
	fThread->Start(); // May need to do a wait	
	}

void ZTask_Threaded::sTaskRun(ZTask_Threaded* iTask)
	{
	iTask->TaskRun();
	iTask->pStopped();
	delete iTask;
	}
