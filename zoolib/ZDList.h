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

#ifndef __ZDList__
#define __ZDList__
#include "zconfig.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h"

namespace ZooLib {

// In these templates, P is Pointer and L is Link.

// =================================================================================================
#pragma mark -
#pragma mark * DListHead

template <typename L>
struct DListHead
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(
    	DListHead, operator_bool_generator_type, operator_bool_type);

	DListHead() : fHeadL(nil), fSize(0) {}

	operator operator_bool_type() const { return operator_bool_generator_type::translate(fHeadL); }

	size_t Size() const { return fSize; }

	bool Empty() const
		{
		ZAssertStop(L::kDebug, (!fSize && !fHeadL) || (fSize && fHeadL));
		return !fHeadL;
		}

	bool Contains(L* iL) const
		{
		return iL->fNext;
		}

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
		if (!iL->fNext)
			this->Insert(iL);
		}

	void Remove(L* iL)
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
			fHeadL = nil;
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
		iL->fNext = nil;
		iL->fPrev = nil;
		}

	void RemoveIfContains(L* iL)
		{
		if (iL->fNext)
			this->Remove(iL);
		}

	void PushFront(L* iL)
		{
		this->Insert(iL);
		fHeadL = iL;
		}

	void PushBack(L* iL)
		{
		this->Insert(iL);
		}

	template <typename P>
	P* PopFront()
		{
		ZAssertStop(L::kDebug, fHeadL);
		L* theL = fHeadL;
		this->Remove(theL);
		return static_cast<P*>(theL);
		}

	template <typename P>
	P* PopBack()
		{
		ZAssertStop(L::kDebug, fHeadL);
		L* theL = fHeadL->fPrev;
		this->Remove(theL);
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
public:
//	DListLink(const DListLink&); // not implemented/implementable
//	DListLink& operator=(const DListLink&); // not implemented/implementable
public:
	enum { kDebug = kDebug_T };

	DListLink() : fPrev(nil), fNext(nil) {}
	~DListLink() { ZAssertStop(kDebug, !fPrev && !fNext); }
	void Clear() { fPrev = nil; fNext = nil; }

	L* fPrev;
	L* fNext;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DListIterator

template <typename P, typename L>
class DListIterator
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(
    	DListIterator, operator_bool_generator_type, operator_bool_type);

public:
	enum { kDebug = L::kDebug };

	DListIterator(const ZooLib::DListHead<L>& iDListHead)
	:	fDListHead(iDListHead),
		fCurrent(iDListHead.fHeadL)
		{}

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fCurrent); }

	P* Current() const
		{ return static_cast<P*>(fCurrent); }

	void Advance()
		{
		ZAssertStop(kDebug, fCurrent);
		ZAssertStop(kDebug, fCurrent->fNext);
		fCurrent = fCurrent->fNext;
		if (fCurrent == fDListHead.fHeadL)
			fCurrent = nil;
		}

private:
	const ZooLib::DListHead<L>& fDListHead;
	L* fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DListIteratorEraseAll

template <typename P, typename L>
class DListIteratorEraseAll
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(
    	DListIteratorEraseAll, operator_bool_generator_type, operator_bool_type);

public:
	enum { kDebug = L::kDebug };

	DListIteratorEraseAll(ZooLib::DListHead<L>& ioDListHead)
	:	fDListHead(ioDListHead),
		fCurrent(ioDListHead.fHeadL)
		{
		if (fCurrent)
			{
			ZAssertStop(kDebug, fCurrent->fNext);
			ZAssertStop(kDebug, fCurrent->fPrev);
			fNext = fCurrent->fNext;
			fCurrent->fNext = nil;
			fCurrent->fPrev = nil;
			}
		else
			{
			fNext = nil;
			}
		}

	~DListIteratorEraseAll()
		{
		fDListHead.fHeadL = nil;
		fDListHead.fSize = 0;
		}

	operator operator_bool_type() const
		{
		ZAssertStop(kDebug, fCurrent && fNext || !fCurrent && !fNext);
		return operator_bool_generator_type::translate(fCurrent);
		}

	P* Current() const
		{ return static_cast<P*>(fCurrent); }

	void Advance()
		{
		ZAssertStop(kDebug, fCurrent);
		ZAssertStop(kDebug, fNext);
		if (fNext == fDListHead.fHeadL)
			{
			fCurrent = nil;
			fNext = nil;
			}
		else
			{
			ZAssertStop(kDebug, fNext->fNext);
			fCurrent = fNext;
			fNext = fNext->fNext;
			fCurrent->fNext = nil;
			fCurrent->fPrev = nil;
			}
		}

private:
	ZooLib::DListHead<L>& fDListHead;
	L* fCurrent;
	L* fNext;
	};

} // namespace ZooLib

#endif // __ZDList__
