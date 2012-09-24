/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZSafePtrStack_h__
#define __ZSafePtrStack_h__
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
// MARK: - ZSafePtrStack

template <class L>
class ZSafePtrStack
	{
public:
	ZSafePtrStack()
	:	fHead(nullptr)
		{}

	~ZSafePtrStack()
		{
		ZAssert(not fHead);
		}

	bool IsEmpty() const
		{ return not fHead; }

	void Push(L* iL)
		{
		ZAssertStop(L::kDebug, iL);
		ZAssertStop(L::kDebug, not iL->fNext);

		ZAcqMtx acq(fMtx);
		iL->fNext = fHead;
		fHead = iL;
		}

	template <class P>
	P* Pop()
		{
		ZAcqMtx acq(fMtx);
		ZAssertStop(L::kDebug, fHead);
		L* result = fHead;
		fHead = fHead->fNext;
		result->fNext = nullptr;
		return static_cast<P*>(result);
		}

	template <class P>
	P* PopIfNotEmpty()
		{
		ZAcqMtx acq(fMtx);
		if (L* result = fHead)
			{
			fHead = fHead->fNext;
			result->fNext = nullptr;
			return static_cast<P*>(result);
			}
		return nullptr;
		}

	ZMtx fMtx;
	L* fHead;
	};

// =================================================================================================
// MARK: - ZSafePtrStack_WithDestroyer

template <typename P, typename L = P>
class ZSafePtrStack_WithDestroyer
:	public ZSafePtrStack<L>
	{
public:
	~ZSafePtrStack_WithDestroyer()
		{
		while (P* p = ZSafePtrStack<L>::template PopIfNotEmpty<P>())
			delete p;
		}
	};

// =================================================================================================
// MARK: - ZSafePtrStackLink

template <typename P, typename L = P, int kDebug_T = 1>
class ZSafePtrStackLink
	{
private:
	ZSafePtrStackLink& operator=(const ZSafePtrStackLink&);

public:
	static const int kDebug = kDebug_T;

	ZSafePtrStackLink()
	:	fNext(nullptr) {}

	ZSafePtrStackLink(const ZSafePtrStackLink& iOther)
	:	fNext(nullptr)
		{ ZAssertStop(kDebug, not iOther.fNext); }

	~ZSafePtrStackLink()
		{ ZAssertStop(kDebug, not fNext); }

	L* fNext;
	};

} // namespace ZooLib

#endif // __ZSafePtrStack_h__
