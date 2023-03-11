// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_SafePtrStack_h__
#define __ZooLib_SafePtrStack_h__
#include "zconfig.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
#pragma mark - SafePtrStack

template <class L>
class SafePtrStack
	{
public:
	SafePtrStack()
	:	fHead(nullptr)
		{}

	~SafePtrStack()
		{
		ZAssert(not fHead);
		}

	bool IsEmpty() const
		{ return not fHead; }

	void Push(L* iL)
		{
		ZAcqMtx acq(fMtx);
		ZAssertStop(L::kDebug, iL);
		ZAssertStop(L::kDebug, not iL->fNext);
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
#pragma mark - SafePtrStack_WithDestroyer

template <typename P, typename L = P>
class SafePtrStack_WithDestroyer
:	public SafePtrStack<L>
	{
public:
	~SafePtrStack_WithDestroyer()
		{
		while (P* p = SafePtrStack<L>::template PopIfNotEmpty<P>())
			delete p;
		}
	};

// =================================================================================================
#pragma mark - SafePtrStackLink

template <typename P, typename L = P, int kDebug_T = 1>
class SafePtrStackLink
	{
private:
	SafePtrStackLink& operator=(const SafePtrStackLink&);

public:
	static const int kDebug = kDebug_T;

	SafePtrStackLink()
	:	fNext(nullptr) {}

	SafePtrStackLink(const SafePtrStackLink& iOther)
	:	fNext(nullptr)
		{ ZAssertStop(kDebug, not iOther.fNext); }

	~SafePtrStackLink()
		{ ZAssertStop(kDebug, not fNext); }

	L* fNext;
	};

} // namespace ZooLib

#endif // __ZooLib_SafePtrStack_h__
