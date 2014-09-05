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

#ifndef __ZTxn_h__
#define __ZTxn_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTypes.h"

#include <map>
#include <vector>

namespace ZooLib {

class ZTxnTarget;

// =================================================================================================
// MARK: - ZTxn

/// Represents a transaction.
class ZTxn : NonCopyable
	{
public:
	ZTxn();
	~ZTxn();

	bool Commit();
	void Abort();

	int32 GetID() const;

private:
	bool pCommit();
	void pAbort();
	void pRegisterTarget(ZTxnTarget* iTarget) const;

	static void sValidateCallback(bool iOkay, void* iRefcon);
	static void sCommitCallback(void* iRefcon);

	int32 fID;

	ZMtxR fMutex;
	mutable std::vector<ZTxnTarget*> fTargets;

	friend class ZTxnTarget;
	};

// =================================================================================================
// MARK: - ZTxnTarget

class ZTxnTarget
	{
protected:
	void RegisterWithTxn(const ZTxn& iTxn);

	typedef void (*ValidateCallbackProc)(bool iOkay, void* iRefcon);
	typedef void (*CommitCallbackProc)(void* iRefcon);

	virtual void TxnAbortPreValidate(int32 iTxnID) = 0;
	virtual void TxnValidate(int32 iTxnID, ValidateCallbackProc iCallback, void* iRefcon) = 0;
	virtual void TxnAbortPostValidate(int32 iTxnID, bool iValidationSucceeded) = 0;
	virtual void TxnCommit(int32 iTxnID, CommitCallbackProc iCallback, void* iRefcon) = 0;

	friend class ZTxn;
	};

} // namespace ZooLib

#endif // __ZTxn_h__
