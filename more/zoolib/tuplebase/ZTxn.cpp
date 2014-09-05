/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZThreadSafe.h"
#include "zoolib/ZTxn.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - ZTxn

ZThreadSafe_t sNextID(1);

/** \class ZTxn
Some more words about transactions.
*/

ZTxn::ZTxn()
	{
	fID = ZThreadSafe_IncReturnOld(sNextID);
	}

ZTxn::~ZTxn()
	{
	ZAcqMtxR locker(fMutex);
	if (!fTargets.empty())
		{
		try
			{
			this->pAbort();
			}
		catch (...)
			{
			ZDebugStopf(0, ("Exception thrown when calling pAbort"));
			}
		}
	}

int32 ZTxn::GetID() const
	{
	return fID;
	}

bool ZTxn::Commit()
	{
	ZAcqMtxR locker(fMutex);
	return this->pCommit();
	}

void ZTxn::Abort()
	{
	ZAcqMtxR locker(fMutex);
	this->pAbort();
	}

struct Waiter_Validate
	{
	ZMtxR* fMutex;
	ZCnd* fCondition;
	bool fCompleted;
	bool fOkay;
	};

struct Waiter_Commit
	{
	ZMtxR* fMutex;
	ZCnd* fCondition;
	bool fCompleted;
	};

bool ZTxn::pCommit()
	{
	ZCnd theCondition;

	vector<Waiter_Validate> waiterValidates(fTargets.size());
	for (size_t x = 0; x < fTargets.size(); ++x)
		{
		waiterValidates[x].fMutex = &fMutex;
		waiterValidates[x].fCondition = &theCondition;
		waiterValidates[x].fCompleted = false;
		waiterValidates[x].fOkay = false;
		fTargets[x]->TxnValidate(fID, sValidateCallback, &waiterValidates[x]);
		}

	bool anyFailures = false;
	for (;;)
		{
		bool allDone = true;
		for (size_t x = 0; x < waiterValidates.size(); ++x)
			{
			if (!waiterValidates[x].fCompleted)
				allDone = false;
			else if (!waiterValidates[x].fOkay)
				anyFailures = true;
			}
		if (allDone || anyFailures)
			break;
		theCondition.Wait(fMutex);
		}

	if (anyFailures)
		{
		for (;;)
			{
			// It would be nice to be able to early-abort targets, that is whilst they're in
			// the middle of executing their validate. The only current ZTxnTarget (ZTBRep)
			// doesn't support it, and it might be that it is just too difficult to handle
			// generally. I need to think on this some more. Therefore, at least for the moment,
			// we wait for all validate calls to complete before issuing post-validate aborts.
			bool allDone = true;
			for (size_t x = 0; x < waiterValidates.size() && allDone; ++x)
				{
				if (!waiterValidates[x].fCompleted)
					allDone = false;
				}
			if (allDone)
				break;
			theCondition.Wait(fMutex);
			}

		// Abort all the targets, telling them whether they had voted yes or no. An abort
		// after a yes vote is usually more costly than after a no, which so far involves
		// no substantial work.
		for (size_t x = 0; x < fTargets.size(); ++x)
			fTargets[x]->TxnAbortPostValidate(fID, waiterValidates[x].fOkay);

		fTargets.clear();
		return false;
		}
	else
		{
		vector<Waiter_Commit> waiterCommits(fTargets.size());
		for (size_t x = 0; x < fTargets.size(); ++x)
			{
			waiterCommits[x].fMutex = &fMutex;
			waiterCommits[x].fCondition = &theCondition;
			waiterCommits[x].fCompleted = false;
			fTargets[x]->TxnCommit(fID, sCommitCallback, &waiterCommits[x]);
			}

		for (;;)
			{
			bool allDone = true;
			for (size_t x = 0; x < waiterCommits.size() && allDone; ++x)
				{
				if (!waiterCommits[x].fCompleted)
					allDone = false;
				}
			if (allDone)
				break;
			theCondition.Wait(fMutex);
			}

		fTargets.clear();
		return true;
		}
	}

void ZTxn::pAbort()
	{
//	ZAssertStop(0, fMutex.IsLocked());

	// Abort all targets.
	for (size_t x = 0; x < fTargets.size(); ++x)
		fTargets[x]->TxnAbortPreValidate(fID);

	fTargets.clear();
	}

void ZTxn::pRegisterTarget(ZTxnTarget* iTarget) const
	{
	ZAcqMtxR locker(fMutex);
	ZAssertStop(0, !ZUtil_STL::sContains(fTargets, iTarget));
	fTargets.push_back(iTarget);
	}

void ZTxn::sValidateCallback(bool iOkay, void* iRefcon)
	{
	Waiter_Validate* theWaiter = static_cast<Waiter_Validate*>(iRefcon);

	theWaiter->fMutex->Acquire();
	theWaiter->fOkay = iOkay;
	theWaiter->fCompleted = true;
	theWaiter->fCondition->Broadcast();
	theWaiter->fMutex->Release();
	}

void ZTxn::sCommitCallback(void* iRefcon)
	{
	Waiter_Commit* theWaiter = static_cast<Waiter_Commit*>(iRefcon);

	theWaiter->fMutex->Acquire();
	theWaiter->fCompleted = true;
	theWaiter->fCondition->Broadcast();
	theWaiter->fMutex->Release();
	}

// =================================================================================================
// MARK: - ZTxnTarget

void ZTxnTarget::RegisterWithTxn(const ZTxn& iTxn)
	{
	iTxn.pRegisterTarget(this);
	}

} // namespace ZooLib
