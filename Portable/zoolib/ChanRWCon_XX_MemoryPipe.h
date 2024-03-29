// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanRWCon_XX_MemoryPipe_h__
#define __ZooLib_ChanRWCon_XX_MemoryPipe_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/Time.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRWCon_XX_MemoryPipe

/** A connected RW stream that transfers data directly from a writer's source
buffer to a reader's destination buffer. Note that read and write must therefore
be called from different threads. */

template <class EE>
class ChanRWCon_XX_MemoryPipe
:	public virtual ChanRWCon<EE>
	{
public:
	ChanRWCon_XX_MemoryPipe()
		{
		fClosed = false;

		fSource = nullptr;
		fSourceEnd = nullptr;

		fDest = nullptr;
		fDestCount = 0;
		}

	~ChanRWCon_XX_MemoryPipe()
		{
		ZAcqMtx acq(fMutex);
		ZAssertStop(2, fSource == nullptr && fDest == nullptr);
		}

// From ChanAspect_Abort
	virtual void Abort()
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fClosed = true;
			fCondition_Read.Broadcast();
			fCondition_Write.Broadcast();
			}
		}

// From ChanAspect_DisconnectRead
	virtual bool DisconnectRead(double iTimeout)
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fSource = fSourceEnd;
			fClosed = true;
			fCondition_Write.Broadcast();
			}
		return true;
		}

// From ChanAspect_DisconnectWrite
	virtual void DisconnectWrite()
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fClosed = true;
			fCondition_Read.Broadcast();
			}
		}

// From ChanAspect_Read
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = static_cast<EE*>(oDest);
		EE* localEnd = localDest + iCount;

		ZAcqMtx acq(fMutex);
		while (localDest < localEnd)
			{
			if (fSource && fSource < fSourceEnd)
				{
				// We've got a source waiting to give us data.
				size_t countToCopy = std::min(localEnd - localDest, fSourceEnd - fSource);
				std::copy(fSource, fSource + countToCopy, localDest);
				localDest += countToCopy;
				fSource += countToCopy;
				fCondition_Write.Broadcast();
				break;
				}
			else
				{
				// Register ourselves as waiting for data.
				if (fClosed)
					break;
				ZAssertStop(2, fDest == nullptr && fDestCount == 0);
				fDest = localDest;
				fDestCount = localEnd - localDest;
				fCondition_Write.Broadcast();
				fCondition_Read.Wait(fMutex);
				localDest = fDest;
				fDest = nullptr;
				fDestCount = 0;
				}

			if (localDest != static_cast<EE*>(oDest))
				{
				// We were able to get *some* data. Let's give up for now
				// and possibly allow a writer to build up a head of steam, as it were.
				break;
				}
			}

		return localDest - static_cast<EE*>(oDest);
		}

	virtual size_t Readable()
		{
		ZAcqMtx acq(fMutex);
		if (fSource)
			return fSourceEnd - fSource;
		return 0;
		}

// From ChanAspect_WaitReadable
	virtual bool WaitReadable(double iTimeout)
		{
		const double deadline = Time::sSystem() + iTimeout;
		ZAcqMtx acq(fMutex);
		for (;;)
			{
			if ((fSource && fSource < fSourceEnd) || fClosed)
				return true;

			if (not fCondition_Read.WaitUntil(fMutex, deadline))
				return false;
			}
		}

// From ChanAspect_Write
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		const EE* localSource = static_cast<const EE*>(iSource);
		const EE* localEnd = localSource + iCount;

		ZAcqMtx acq(fMutex);
		while (localSource < localEnd && not fClosed)
			{
			if (fDestCount)
				{
				// A reader is waiting for data, so copy straight
				// from our source into the reader's dest.
				size_t countToCopy = std::min(fDestCount, size_t(localEnd - localSource));
				if (fDest)
					{
					std::copy(localSource, localSource + countToCopy, fDest);
					fDest += countToCopy;
					}
				localSource += countToCopy;
				fDestCount -= countToCopy;
				fCondition_Read.Broadcast();
				}
			else
				{
				// Register ourselves as having data to provide.
				ZAssertStop(2, fSource == nullptr && fSourceEnd == nullptr);
				fSource = localSource;
				fSourceEnd = localEnd;
				fCondition_Read.Broadcast();
				fCondition_Write.Wait(fMutex);
				localSource = fSource;
				fSource = nullptr;
				fSourceEnd = nullptr;
				}

			if (localSource != static_cast<const EE*>(iSource))
				{
				// We were able to write *some* data. Let's give up for now
				// and possibly allow a reader to come in.
				break;
				}
			}

		return localSource - static_cast<const EE*>(iSource);
		}

private:
	ZMtx fMutex;
	ZCnd fCondition_Read;
	ZCnd fCondition_Write;

	bool fClosed;

	const EE* fSource;
	const EE* fSourceEnd;

	EE* fDest;
	size_t fDestCount;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRWCon_XX_MemoryPipe_h__
