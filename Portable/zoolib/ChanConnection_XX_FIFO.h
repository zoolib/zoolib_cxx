/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZooLib_ChanConnection_XX_FIFO_h__
#define __ZooLib_ChanConnection_XX_FIFO_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/ZThread.h"

#include <deque>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanConnection_XX_FIFO

/** A RW stream that queues written data and returns it in FIFO order when read. */

template <class EE>
class ChanConnection_XX_FIFO
:	public ChanConnection<EE>
	{
public:
	ChanConnection_XX_FIFO()
	:	fClosed(false)
	,	fMaxSize(64)
	,	fUserCount(0)
		{}

	ChanConnection_XX_FIFO(size_t iMaxSize)
	:	fClosed(false)
	,	fMaxSize(iMaxSize)
	,	fUserCount(0)
		{}

	~ChanConnection_XX_FIFO()
		{
		ZAcqMtx acq(fMutex);

		fClosed = true;
		fCondition_Read.Broadcast();
		fCondition_Write.Broadcast();

		while (fUserCount)
			fCondition_UserCount.Wait(fMutex);
		}

// From Aspect_Abort
	virtual void Abort()
		{
		ZAcqMtx acq(fMutex);
		fClosed = true;
		fBuffer.clear();
		fCondition_Read.Broadcast();
		fCondition_Write.Broadcast();
		}

// From Aspect_DisconnectRead
	virtual bool DisconnectRead(double iTimeout)
		{
		double deadline = Time::sSystem() + iTimeout;
		ZAcqMtx acq(fMutex);
		for (;;)
			{
			if (fBuffer.size())
				{
				fBuffer.clear();
				fCondition_Write.Broadcast();
				}

			if (fClosed)
				return true;

			if (not fCondition_Read.WaitUntil(fMutex, deadline))
				return false;
			}
		}

// From Aspect_DisconnectWrite
	virtual void DisconnectWrite()
		{
		ZAcqMtx acq(fMutex);
		if (not fClosed)
			{
			fClosed = true;
			fCondition_Read.Broadcast();
			fCondition_Write.Broadcast();
			}
		}

// From Aspect_Read<EE>
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		ZAcqMtx acq(fMutex);
		EE* localDest = oDest;
		++fUserCount;
		if (iCount)
			{
			for (;;)
				{
				const size_t countToCopy = std::min(iCount, fBuffer.size());
				if (countToCopy == 0)
					{
					if (fClosed)
						break;
					fCondition_Read.Wait(fMutex);
					}
				else
					{
					std::copy_n(fBuffer.begin(), countToCopy, localDest);
					fBuffer.erase(fBuffer.begin(), fBuffer.begin() + countToCopy);
					localDest += countToCopy;
					fCondition_Write.Broadcast();
					break;
					}
				}
			}

		--fUserCount;
		fCondition_UserCount.Broadcast();
		return localDest - oDest;
		}

	virtual size_t Readable()
		{
		ZAcqMtx acq(fMutex);
		return fBuffer.size();
		}

// From Aspect_WaitReadable
	virtual bool WaitReadable(double iTimeout)
		{
		const double deadline = Time::sSystem() + iTimeout;
		ZAcqMtx acq(fMutex);
		for (;;)
			{
			if (fBuffer.size())
				return true;
			if (!fCondition_Read.WaitUntil(fMutex, deadline))
				return false;
			}
		}

// From Aspect_Write<EE>
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		ZAcqMtx acq(fMutex);
		++fUserCount;
		const EE* localSource = iSource;
		while (iCount && not fClosed)
			{
			size_t countToInsert = iCount;
			if (fMaxSize)
				{
				if (fMaxSize >= fBuffer.size())
					countToInsert = std::min(iCount, fMaxSize - fBuffer.size());
				else
					countToInsert = 0;
				}

			if (countToInsert == 0)
				{
				fCondition_Write.Wait(fMutex);
				}
			else
				{
				fBuffer.insert(fBuffer.end(), localSource, localSource + countToInsert);
				localSource += countToInsert;
				iCount -= countToInsert;
				fCondition_Read.Broadcast();
				}
			}
		--fUserCount;
		fCondition_UserCount.Broadcast();
		return localSource - iSource;
		}

// Our protocol
	void Close();
	bool IsClosed();
	void Reset();

private:
	ZMtx fMutex;
	ZCnd fCondition_UserCount;
	ZCnd fCondition_Read;
	ZCnd fCondition_Write;
	bool fClosed;
	size_t fMaxSize;
	std::deque<EE> fBuffer;
	size_t fUserCount;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanConnection_XX_FIFO_h__
