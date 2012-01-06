/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZDList_h__
#define __ZDList_h__
#include "zconfig.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZStdInt.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
#pragma mark -
#pragma mark * DListHead

template <typename L>
struct DListHead
	{
private:
	DListHead& operator=(const DListHead&);

public:
	DListHead()
	:	fHeadL(nullptr)
	,	fSize(0)
		{}

	DListHead(const DListHead& iOther)
	:	fHeadL(nullptr)
	,	fSize(0)
		{ ZAssertStop(L::kDebug, !iOther.fHeadL && !iOther.fSize); }

	~DListHead()
		{ ZAssertStop(L::kDebug, !fHeadL && !fSize); }

	ZMACRO_operator_bool_T(DListHead, operator_bool) const
		{ return operator_bool_gen::translate(fHeadL); }

	size_t Size() const { return fSize; }

	bool Empty() const
		{
		ZAssertStop(L::kDebug, (!fSize && !fHeadL) || (fSize && fHeadL));
		return !fHeadL;
		}

	bool Contains(L* iL) const
		{ return iL->fNext; }

	void Insert(L* iL)
		{
		ZAssertStop(L::kDebug, !iL->fPrev);
		ZAssertStop(L::kDebug, !iL->fNext);

		if (!fHeadL)
			{
			ZAssertStop(L::kDebug, fSize == 0);
			fSize = 1;
			fHeadL = iL;
			iL->fPrev = iL;
			iL->fNext = iL;
			}
		else
			{
			ZAssertStop(L::kDebug, fSize != 0);
			++fSize;
			iL->fNext = fHeadL;
			iL->fPrev = fHeadL->fPrev;
			fHeadL->fPrev->fNext = iL;
			fHeadL->fPrev = iL;
			}
		}

	void InsertIfNotContains(L* iL)
		{
		if (not iL->fNext)
			this->Insert(iL);
		}

	void Erase(L* iL)
		{
		ZAssertStop(L::kDebug, fHeadL);
		ZAssertStop(L::kDebug, iL->fPrev);
		ZAssertStop(L::kDebug, iL->fNext);

		if (iL->fPrev == iL)
			{
			// We have a list with a single element
			ZAssertStop(L::kDebug, iL->fNext == iL);
			ZAssertStop(L::kDebug, fHeadL == iL);
			ZAssertStop(L::kDebug, fSize == 1);
			fHeadL = nullptr;
			fSize = 0;
			}
		else
			{
			ZAssertStop(L::kDebug, fSize > 1);
			--fSize;
			if (fHeadL == iL)
				fHeadL = iL->fNext;
			iL->fNext->fPrev = iL->fPrev;
			iL->fPrev->fNext = iL->fNext;
			}
		iL->fNext = nullptr;
		iL->fPrev = nullptr;
		}

	void EraseIfContains(L* iL)
		{
		if (iL->fNext)
			this->Erase(iL);
		}

	void PushFront(L* iL)
		{
		this->Insert(iL);
		fHeadL = iL;
		}

	void PushBack(L* iL)
		{ this->Insert(iL); }

	template <typename P>
	P* PopFront()
		{
		ZAssertStop(L::kDebug, fHeadL);
		L* theL = fHeadL;
		this->Erase(theL);
		return static_cast<P*>(theL);
		}

	template <typename P>
	P* PopBack()
		{
		ZAssertStop(L::kDebug, fHeadL);
		L* theL = fHeadL->fPrev;
		this->Erase(theL);
		return static_cast<P*>(theL);
		}

	L* fHeadL;
	size_t fSize;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DListLink

template <typename P, typename L, int kDebug_T = 1>
class DListLink
	{
private:
	DListLink& operator=(const DListLink&);

public:
	static const int kDebug = kDebug_T;

	DListLink()
	:	fPrev(nullptr)
	,	fNext(nullptr) {}

	DListLink(const DListLink& iOther)
	:	fPrev(nullptr)
	,	fNext(nullptr)
		{ ZAssertStop(kDebug, !iOther.fPrev && !iOther.fNext); }

	~DListLink()
		{ ZAssertStop(kDebug, !fPrev && !fNext); }

	L* fPrev;
	L* fNext;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DListIterator

template <typename P, typename L>
class DListIterator
	{
public:
	DListIterator()
	:	fDListHead(nullptr)
	,	fCurrent(nullptr)
		{}

	DListIterator(const DListIterator& iOther)
	:	fDListHead(iOther.fDListHead)
	,	fCurrent(iOther.fCurrent)
		{}

	~DListIterator()
		{}

	DListIterator& operator=(const DListIterator& iOther)
		{
		fDListHead = iOther.fDListHead;
		fCurrent = iOther.fCurrent;
		return *this;
		}

	DListIterator(const DListHead<L>& iDListHead)
	:	fDListHead(&iDListHead)
	,	fCurrent(iDListHead.fHeadL)
		{}

	ZMACRO_operator_bool_T(DListIterator, operator_bool) const
		{ return operator_bool_gen::translate(fCurrent); }

	P* Current() const
		{ return static_cast<P*>(fCurrent); }

	void Advance()
		{
		ZAssertStop(L::kDebug, fCurrent);
		ZAssertStop(L::kDebug, fCurrent->fNext);
		fCurrent = fCurrent->fNext;
		if (fCurrent == fDListHead->fHeadL)
			fCurrent = nullptr;
		}

private:
	const DListHead<L>* fDListHead;
	L* fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DListEraser

template <typename P, typename L>
class DListEraser
	{
public:
	DListEraser()
	:	fCurrent(nullptr)
	,	fNext(nullptr)
		{}

	DListEraser(const DListEraser& iOther)
	:	fCurrent(iOther.fCurrent)
	,	fNext(iOther.fNext)
		{
		iOther.fCurrent = nullptr;
		iOther.fNext = nullptr;
		}

	~DListEraser()
		{ ZAssertStop(L::kDebug, !fCurrent && !fNext); }

	DListEraser& operator=(const DListEraser& iOther)
		{
		fCurrent = iOther.fCurrent;
		fNext = iOther.fNext;
		iOther.fCurrent = nullptr;
		iOther.fNext = nullptr;
		return *this;
		}

	DListEraser(DListHead<L>& ioDListHead)
	:	fCurrent(ioDListHead.fHeadL)
		{
		ioDListHead.fHeadL = nullptr;
		ioDListHead.fSize = 0;
		if (!fCurrent)
			{
			fNext = nullptr;
			}
		else
			{
			ZAssertStop(L::kDebug, fCurrent->fNext);
			ZAssertStop(L::kDebug, fCurrent->fPrev);
			fCurrent->fPrev->fNext = nullptr;
			fNext = fCurrent->fNext;
			fCurrent->fNext = nullptr;
			fCurrent->fPrev = nullptr;
			}
		}

	ZMACRO_operator_bool_T(DListEraser, operator_bool) const
		{
		ZAssertStop(L::kDebug, fCurrent || !fCurrent && !fNext);
		return operator_bool_gen::translate(fCurrent);
		}

	P* Current() const
		{ return static_cast<P*>(fCurrent); }

	void Advance()
		{
		ZAssertStop(L::kDebug, fCurrent);
		fCurrent = fNext;
		if (fCurrent)
			{
			fNext = fCurrent->fNext;
			fCurrent->fNext = nullptr;
			fCurrent->fPrev = nullptr;
			}
		}

private:
	mutable L* fCurrent;
	mutable L* fNext;
	};

} // namespace ZooLib

#endif // __ZDList_h__
