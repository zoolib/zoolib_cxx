// Copyright (c) 2006-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2020 Andrew Green.
// MIT License. http://www.zoolib.org/

#ifndef __ZooLib_DList_h__
#define __ZooLib_DList_h__ 1
#include "zconfig.h"

#include "zoolib/size_t.h" // For size_t

#include "zoolib/ZDebug.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
#pragma mark - DListHead

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
		{ ZAssertStop(L::kDebug, not iOther.fHeadL && not iOther.fSize); }

	~DListHead()
		{ ZAssertStop(L::kDebug, not fHeadL && not fSize); }

	explicit operator bool() const
		{ return true && fHeadL; }

	size_t Size() const { return fSize; }

	bool IsEmpty() const
		{
		ZAssertStop(L::kDebug, (not fSize && not fHeadL) || (fSize && fHeadL));
		return not fHeadL;
		}

	bool Contains(L* iL) const
		{ return iL->fNext; }

	bool QErase(L* iL)
		{
		if (not fHeadL)
			{
			ZAssertStop(L::kDebug, not iL->fPrev);
			ZAssertStop(L::kDebug, not iL->fNext);
			return false;
			}

		if (not iL->fPrev)
			{
			ZAssertStop(L::kDebug, not iL->fNext);
			return false;
			}

		ZAssertStop(L::kDebug, fHeadL);
		ZAssertStop(L::kDebug, iL->fPrev);

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
		return true;
		}

	bool QInsertBack(L* iL)
		{
		if (iL->fPrev)
			{
			ZAssertStop(L::kDebug, iL->fNext);
			return false;
			}

		ZAssertStop(L::kDebug, not iL->fNext);

		if (not fHeadL)
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
		return true;
		}

	bool QInsertFront(L* iL)
		{
		if (not this->QInsertBack(iL))
			return false;
		fHeadL = iL;
		return true;
		}

	L* GetEraseFront()
		{
		if (L* theL = fHeadL)
			{
			this->QErase(theL);
			return theL;
			}
		return nullptr;
		}

	L* GetEraseBack()
		{
		if (L* theL = fHeadL->fPrev)
			{
			this->QErase(theL);
			return theL;
			}
		return nullptr;
		}

	L* fHeadL;
	size_t fSize;
	};

// =================================================================================================
#pragma mark -

template <typename L>
bool sIsEmpty(const DListHead<L>& iDListHead)
	{ return iDListHead.IsEmpty(); }

template <typename L>
bool sNotEmpty(const DListHead<L>& iDListHead)
	{ return not sIsEmpty(iDListHead); }

template <typename L, typename P>
bool sContains(const DListHead<L>& iDListHead, P* iP)
	{ return iDListHead.Contains(iP); }

template <typename L, typename P>
bool sQErase(DListHead<L>& ioDListHead, P* iP)
	{ return ioDListHead.QErase(iP); }

//template <typename L, typename P>
//void sErase(DListHead<L>& ioDListHead, P* iP)
//	{ ioDListHead.QErase(iP); }

template <typename L, typename P>
void sEraseMust(DListHead<L>& ioDListHead, P* iP)
	{
	bool result = ioDListHead.QErase(iP);
	ZAssertStop(L::kDebug, result);
	}

template <typename L, typename P>
bool sQInsertBack(DListHead<L>& ioDListHead, P* iP)
	{ return ioDListHead.QInsertBack(iP); }

//template <typename L, typename P>
//void sInsertBack(DListHead<L>& ioDListHead, P* iP)
//	{ ioDListHead.QInsertBack(iP); }

template <typename L, typename P>
void sInsertBackMust(DListHead<L>& ioDListHead, P* iP)
	{
	bool result = ioDListHead.QInsertBack(iP);
	ZAssertStop(L::kDebug, result);
	}

template <typename L, typename P>
bool sQInsertFront(DListHead<L>& ioDListHead, P* iP)
	{ return ioDListHead.QInsertFront(iP); }

//template <typename L, typename P>
//void sInsertFront(DListHead<L>& ioDListHead, P* iP)
//	{ ioDListHead.QInsertFront(iP); }

template <typename L, typename P>
void sInsertFrontMust(DListHead<L>& ioDListHead, P* iP)
	{
	bool result = ioDListHead.QInsertFront(iP);
	ZAssertStop(L::kDebug, result);
	}

template <typename P, typename L>
P* sGetEraseFront(DListHead<L>& ioDListHead)
	{ return static_cast<P*>(ioDListHead.GetEraseFront()); }

template <typename P, typename L>
P* sGetEraseFrontMust(DListHead<L>& ioDListHead)
	{
	P* result = static_cast<P*>(ioDListHead.GetEraseFront());
	ZAssertStop(L::kDebug, result);
	return result;
	}

template <typename P, typename L>
P* sGetEraseBack(DListHead<L>& ioDListHead)
	{ return static_cast<P*>(ioDListHead.GetEraseBack()); }

template <typename P, typename L>
P* sGetEraseBackMust(DListHead<L>& ioDListHead)
	{
	P* result = static_cast<P*>(ioDListHead.GetEraseBack());
	ZAssertStop(L::kDebug, result);
	return result;
	}

// =================================================================================================
#pragma mark - DListLink

template <typename P, typename L = P, int kDebug_T = 1>
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
		{ ZAssertStop(kDebug, not iOther.fPrev && not iOther.fNext); }

	~DListLink()
		{ ZAssertStop(kDebug, not fPrev && not fNext); }

	L* fPrev;
	L* fNext;
	};

// =================================================================================================
#pragma mark - DListIterator

template <typename P, typename L = P>
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

	explicit operator bool() const
		{ return true && fCurrent; }

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
#pragma mark - DListEraser

template <typename P, typename L = P>
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
		{ ZAssertStop(L::kDebug, not fCurrent && not fNext); }

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
		if (not fCurrent)
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

	explicit operator bool() const
		{
		ZAssertStop(L::kDebug, fCurrent || not fCurrent && not fNext);
		return true && fCurrent;
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

#endif // __ZooLib_DList_h__
