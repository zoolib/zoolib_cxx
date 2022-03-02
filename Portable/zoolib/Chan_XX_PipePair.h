// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_PipePair_h__
#define __ZooLib_Chan_XX_PipePair_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/Counted.h"
#include "zoolib/Time.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ImpPipePair

template <class EE>
class ImpPipePair
:	public Counted
	{
public:
	ImpPipePair()
		{
		fClosed = false;

		fSource = nullptr;
		fSourceEnd = nullptr;

		fDest = nullptr;
		fDestCount = 0;
		}

	~ImpPipePair()
		{
		ZAcqMtx acq(fMutex);
		ZAssert(fSource == nullptr && fDest == nullptr);
		}

// For ChanAspect_Abort
	void Abort()
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fClosed = true;
			fCondition_Read.Broadcast();
			fCondition_Write.Broadcast();
			}
		}

// For ChanAspect_DisconnectRead
	bool DisconnectRead(double iTimeout)
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

// For ChanAspect_DisconnectWrite
	void DisconnectWrite()
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fClosed = true;
			fCondition_Read.Broadcast();
			}
		}

// For ChanAspect_Read
	size_t Read(EE* oDest, size_t iCount)
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
			else if (fDest)
				{
				// Someone else has registered as wanting data.
				fCondition_Read.Wait(fMutex);
				}
			else
				{
				// Register ourselves as waiting for data.
				if (fClosed)
					break;
				fDest = localDest;
				fDestCount = localEnd - localDest;
				fCondition_Write.Broadcast();
				fCondition_Read.Wait(fMutex);
				fDestCount = 0;
				bool readAny = localDest != fDest;
				localDest = fDest;
				fDest = nullptr;
				if (readAny)
					break;
				}
			}

		return localDest - static_cast<EE*>(oDest);
		}

	size_t Readable()
		{
		ZAcqMtx acq(fMutex);
		if (fSource)
			return fSourceEnd - fSource;
		return 0;
		}

// For ChanAspect_WaitReadable
	bool WaitReadable(double iTimeout)
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

// For ChanAspect_Write
	size_t Write(const EE* iSource, size_t iCount)
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
				std::copy(localSource, localSource + countToCopy, fDest);
				fDest += countToCopy;
				localSource += countToCopy;
				fDestCount -= countToCopy;
				fCondition_Read.Broadcast();
				break;
				}
			else if (fSource)
				{
				// Someone else has registered as having data to provide.
				fCondition_Write.Wait(fMutex);
				}
			else
				{
				// Register ourselves as having data to provide.
				fSource = localSource;
				fSourceEnd = localEnd;
				fCondition_Read.Broadcast();
				fCondition_Write.Wait(fMutex);
				localSource = fSource;
				fSource = nullptr;
				fSourceEnd = nullptr;
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

// ----------

template <class EE>
class ChanR_XX_PipePair
:	public virtual ChanR<EE>
	{
public:
	ChanR_XX_PipePair(const ZP<ImpPipePair<EE>>& iPipePair)
	:	fPipePair(iPipePair)
		{}

	virtual ~ChanR_XX_PipePair()
		{
		while (not fPipePair->DisconnectRead(1 * Time::kDay))
			{}
		}

// From ChanAspect_Read
	virtual size_t Read(EE* oDest, size_t iCount)
		{ return fPipePair->Read(oDest, iCount); }

	virtual size_t Readable()
		{ return fPipePair->Readable(); }

private:
	ZP<ImpPipePair<EE>> fPipePair;
	};

// ----------

template <class EE>
class ChanWCon_XX_PipePair
:	public virtual ChanWCon<EE>
	{
public:
	ChanWCon_XX_PipePair(const ZP<ImpPipePair<EE>>& iPipePair)
	:	fPipePair(iPipePair)
		{}

	virtual ~ChanWCon_XX_PipePair()
		{ fPipePair->Abort(); }

// From ChanAspect_Abort
	virtual void Abort()
		{ fPipePair->Abort(); }

// From ChanAspect_DisconnectWrite
	virtual void DisconnectWrite()
		{ return fPipePair->DisconnectWrite(); }

// From ChanAspect_Write
	virtual size_t Write(const EE* iSource, size_t iCount)
		{ return fPipePair->Write(iSource, iCount); }

private:
	ZP<ImpPipePair<EE>> fPipePair;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_PipePair_h__
