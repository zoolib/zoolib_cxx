/* -------------------------------------------------------------------------------------------------
Copyright (c) 2021 Andrew Green
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

#ifndef __ZSafeStack_h__
#define __ZSafeStack_h__
#include "zconfig.h"

#include "zoolib/ZAtomic.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
// MARK: - ZSafeStack

template <class L>
class ZSafeStack
	{
public:
	ZSafeStack()
		{
		fHead = &fDummy;
		fHead->fNext = fHead;
		}

	~ZSafeStack()
		{
		ZAssert(fHead == &fDummy);
		fDummy.fNext = nullptr;
		}

	bool IsEmpty() const
		{ return fHead == &fDummy; }

	void Push(L* iL)
		{
		ZAssertStop(L::kDebug, not iL->fNext);

		for (;;)
			{
			L* theHead = fHead;
			iL->fNext = theHead;
			if (sAtomic_CASPtr(&fHead, theHead, iL))
				break;
			}
		}

	template <class P>
	P* Pop()
		{
		for (;;)
			{
			L* theHead = fHead;

			if (sAtomic_CASPtr(&fHead, theHead, theHead->fNext))
				{
				ZAssertStop(L::kDebug, theHead != &fDummy);
				theHead->fNext = nullptr;
				return static_cast<P*>(theHead);
				}
			}
		}

	template <class P>
	P* PopIfNotEmpty()
		{
		for (;;)
			{
			L* theHead = fHead;

			if (sAtomic_CASPtr(&fHead, theHead, theHead->fNext))
				{
				if (theHead == &fDummy)
					return nullptr;
				theHead->fNext = nullptr;
				return static_cast<P*>(theHead);
				}
			}
		}

	L* fHead;
	L fDummy;
	};

// =================================================================================================
// MARK: - ZSafeStack_WithDestroyer

template <typename P, typename L = P>
class ZSafeStack_WithDestroyer
:	public ZSafeStack<L>
	{
public:
	~ZSafeStack_WithDestroyer()
		{
		while (P* p = ZSafeStack<L>::template PopIfNotEmpty<P>())
			delete p;
		}
	};

// =================================================================================================
// MARK: - ZSafeStackLink

template <typename P, typename L = P, int kDebug_T = 1>
class ZSafeStackLink
	{
private:
	ZSafeStackLink& operator=(const ZSafeStackLink&);

public:
	static const int kDebug = kDebug_T;

	ZSafeStackLink()
	:	fNext(nullptr) {}

	ZSafeStackLink(const ZSafeStackLink& iOther)
	:	fNext(nullptr)
		{ ZAssertStop(kDebug, not iOther.fNext); }

	~ZSafeStackLink()
		{ ZAssertStop(kDebug, not fNext); }

	L* fNext;
	};

} // namespace ZooLib

#endif // __ZSafeStack_h__
