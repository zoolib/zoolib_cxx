#ifndef __ZTask__
#define __ZTask__ 1
#include "zconfig.h"

#include "ZDList.h"
#include "ZThread.h"

class ZTask;
class ZTaskOwner;

// =================================================================================================
#pragma mark -
#pragma mark * ZTask

class ZTaskLink_Live :
	public ZooLib::DListLink<ZTask, ZTaskLink_Live>
	{};

class ZTaskLink_Processed :
	public ZooLib::DListLink<ZTask, ZTaskLink_Processed>
	{};

class ZTask
:	public ZTaskLink_Live,
	public ZTaskLink_Processed
	{
protected:
	ZTask();
	virtual ~ZTask();

public:
// Control
	virtual void Stop() = 0;

// Notification
	virtual void OwnerDetached();
	virtual void Message(const string& iString, void* iRefcon);

protected:
// Called by subclasses
	void pStarted(ZTaskOwner* iOwner);
	void pStopped();

private:
	friend class ZTaskOwner;
	ZTaskOwner* fOwner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTaskOwner

class ZTaskOwner
	{
public:
	ZTaskOwner();
	~ZTaskOwner();

	void Tell(const string& iString, void* iRefcon);
	void StopAll();

	void Wake();

	bool WaitLess(size_t iCount);
	bool WaitGreater(size_t iCount);
	bool WaitEmpty();

// Notifications
	virtual void TaskAttached(ZTask* iTask);
	virtual void TaskDetached(ZTask* iTask);
	virtual void CountChanged(size_t iCount);

private:
	friend class ZTask;
	void pTaskStarted(ZTask* iTask);
	void pTaskStopped(ZTask* iTask);

	ZMutex fMutex;
	ZCondition fCondition;

	ZooLib::DListHead<ZTaskLink_Live> fTasks_Live;
	ZooLib::DListHead<ZTaskLink_Processed> fTasks_Processed;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTask_Threaded

class ZTask_Threaded : public ZTask
	{
public:
	ZTask_Threaded();
	virtual ~ZTask_Threaded();

// Our protocol
	void TaskStart(ZTaskOwner* iOwner, bool iWait);

	virtual void TaskRun() = 0;

private:
	static void sTaskRun(ZTask_Threaded* iTask);
	ZThread* fThread;
	};

#endif // __ZTask__
