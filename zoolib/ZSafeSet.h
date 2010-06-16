/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZSafeSet__
#define __ZSafeSet__ 1
#include "zconfig.h"

#include "zoolib/ZDList.h"
#include "zoolib/ZQ_T.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZWeakRef.h"

#include <list>
#include <map>

namespace ZooLib {

template <class T> class ZSafeSet;
template <class T> class ZSafeSetIter;
template <class T> class ZSafeSetIterConst;

template <class T>
class DLink_SafeSetIterConst
:	public DListLink<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSetRep

template <class T>
class ZSafeSetRep
:	public ZCounted,
	public ZWeakReferee
	{
private:
	struct Entry
		{
		Entry(const T& iT)
		:	fT(iT)
			{}

		T fT;
		DListHead<DLink_SafeSetIterConst<T> > fIters;
		};

	typedef std::list<Entry> EntryList;
	typedef std::map<T, typename EntryList::iterator> EntryMap;

	ZSafeSetRep()
		{}

	ZSafeSetRep(const ZSafeSetRep& iOther)
		{
		ZAcqMtx acq(iOther.fMtx);
		for (typename EntryList::iterator i = iOther.fList.begin();
			i != iOther.fList.end(); ++i)
			{
			const T& val = *i;
			typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(val));
			fMap.insert(typename EntryMap::value_type(val, listIter));
			}
		}

	virtual ~ZSafeSetRep()
		{}

	// From ZCounted
	virtual void Finalize()
		{
		{ // Scope for guard
		ZAcqMtx acq(fMtx);
		if (this->GetRefCount() != 1)
			{
			this->FinalizationComplete();
			return;
			}

		// Invalidate all iterators referencing entries.
		for (typename EntryList::iterator listIter = fList.begin();
			listIter != fList.end(); ++listIter)
			{
			for (DListIteratorEraseAll<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
				i = (*listIter).fIters; i; i.Advance())
				{
				ZSafeSetIterConst<T>* theIter = i.Current();
				theIter->fRep.Clear();
				}
			}

		this->FinalizationComplete();
		}
		delete this;
		}

	size_t pSize() const
		{
		ZAcqMtx acq(fMtx);
		return fList.size();
		}

	bool pEmpty() const
		{
		ZAcqMtx acq(fMtx);
		return fList.empty();
		}

	bool pInsert(const T& iT)
		{
		ZAcqMtx acq(fMtx);

		typename EntryMap::iterator mapIter = fMap.lower_bound(iT);
		if (mapIter != fMap.end() && (*mapIter).first == iT)
			return false;

		typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(iT));
		fMap.insert(mapIter, typename EntryMap::value_type(iT, listIter));
		return true;
		}

	bool pErase(const T& iT);

	void pClear()
		{
		ZAcqMtx acq(fMtx);
		// Invalidate all iterators referencing entries.
		for (typename EntryList::iterator listIter = fList.begin();
			listIter != fList.end(); ++listIter)
			{
			for (DListIteratorEraseAll<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
				i = (*listIter).fIters; i; i.Advance())
				{
				ZSafeSetIterConst<T>* theIter = i.Current();
				theIter->fRep.Clear();
				}
			}
		fMap.clear();
		fList.clear();
		}

	void pInit(ZRef<ZSafeSetRep> iSelf,
		ZSafeSetIterConst<T>& ioIter)
		{
		ZAcqMtx acq(fMtx);
		ioIter.fNextEntry = fList.begin();
		if (ioIter.fNextEntry != fList.end())
			{
			ioIter.fRep = iSelf;
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);
			}
		}

	void pInitFrom(ZRef<ZSafeSetRep> iSelf,
		ZSafeSetIterConst<T>& ioIter, const ZSafeSetIterConst<T>& iOther)
		{
		ZAcqMtx acq(fMtx);

		ioIter.fNextEntry = iOther.fNextEntry;
		if (ioIter.fNextEntry != fList.end())
			{
			ioIter.fRep = iSelf;
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);
			}
		}

	void pDestroy(ZSafeSetIterConst<T>& ioIter)
		{
		ZAcqMtx acq(fMtx);

		(*ioIter.fNextEntry).fIters.Erase(&ioIter);
		ioIter.fRep.Clear();
		}

	bool pReadInc(ZSafeSetIterConst<T>& ioIter, T& oValue);
	ZQ_T<T> pReadInc(ZSafeSetIterConst<T>& ioIter);

	bool pReadErase(ZSafeSetIter<T>& ioIter, T& oValue);
	ZQ_T<T> pReadErase(ZSafeSetIter<T>& ioIter);

	ZMtx fMtx;
	EntryList fList;
	EntryMap fMap;

	friend class ZSafeSet<T>;
	friend class ZSafeSetIter<T>;
	friend class ZSafeSetIterConst<T>;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSet

template <class T>
class ZSafeSet
	{
public:
	ZSafeSet()
	:	fRep(new ZSafeSetRep<T>)
		{}

	ZSafeSet(const ZSafeSet& iOther)
	:	fRep(new ZSafeSetRep<T>(*iOther.GetRep().GetObject()))
		{}

	~ZSafeSet()
		{}

	ZSafeSet& operator=(const ZSafeSet& iOther)
		{
		if (this != &iOther)
			fRep = new ZSafeSetRep<T>(*iOther.fRep.GetObject());

		return *this;
		}

	size_t Size() const
		{ return fRep->pSize(); }

	bool Empty() const
		{ return fRep->pEmpty(); }

	bool Insert(const T& iT)
		{ return fRep->pInsert(iT); }

	bool Erase(const T& iT)
		{ return fRep->pErase(iT); }

	void Clear()
		{ fRep->pClear(); }

	ZRef<ZSafeSetRep<T> > GetRep() const
		{ return fRep; }

private:
	ZRef<ZSafeSetRep<T> > fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSetIterConst

template <class T>
class ZSafeSetIterConst : public DLink_SafeSetIterConst<T>
	{
public:
	ZSafeSetIterConst()
		{}

	ZSafeSetIterConst(const ZSafeSet<T>& iSafeSet)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = iSafeSet.GetRep())
			theRep->pInit(theRep, *this);
		}

	ZSafeSetIterConst(const ZSafeSetIterConst& iOther)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = iOther.fRep)
			theRep->pInitFrom(theRep, *this, iOther);
		}

	~ZSafeSetIterConst()
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep)
			theRep->pDestroy(*this);
		}

	ZSafeSetIterConst& operator=(const ZSafeSetIterConst& iOther)
		{
		if (this != &iOther)
			{
			if (ZRef<ZSafeSetRep<T> > theRep = fRep)
				theRep->pDestroy(*this);

			if (ZRef<ZSafeSetRep<T> > theRep = iOther.fRep)
				theRep->pInitFrom(theRep, *this, iOther);
			}
		return *this;
		}

	ZSafeSetIterConst& operator=(const ZSafeSet<T>& iSafeSet)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep)
			theRep->pDestroy(*this);

		if (ZRef<ZSafeSetRep<T> > theRep = iSafeSet.GetRep())
			theRep->pInit(theRep, *this);
		}

	bool QReadInc(T& oValue)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep)
			return theRep->pReadInc(*this, oValue);
		return false;
		}

	ZQ_T<T> QReadInc()
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep)
			return theRep->pReadInc(*this);
		return null;
		}

	T ReadInc()
		{
		T temp;
		if (this->QReadInc(temp))
			return temp;
		return T();
		}

protected:
	ZWeakRef<ZSafeSetRep<T> > fRep;

private:
	typename ZSafeSetRep<T>::EntryList::iterator fNextEntry;
	friend class ZSafeSetRep<T>;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSetIter

template <class T>
class ZSafeSetIter : public ZSafeSetIterConst<T>
	{
public:
	ZSafeSetIter()
		{}

	ZSafeSetIter(ZSafeSet<T>& iSafeSet)
	:	ZSafeSetIterConst<T>(iSafeSet)
		{}

	ZSafeSetIter(const ZSafeSetIter& iOther)
	:	ZSafeSetIterConst<T>(iOther)
		{}

	~ZSafeSetIter()
		{}

	ZSafeSetIter& operator=(const ZSafeSetIter& iOther)
		{ return ZSafeSetIterConst<T>::operator=(iOther); }

	bool QReadErase(T& oValue)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = ZSafeSetIterConst<T>::fRep)
			return theRep->pReadErase(*this, oValue);
		return false;
		}

	ZQ_T<T> QReadInc()
		{
		if (ZRef<ZSafeSetRep<T> > theRep = ZSafeSetIterConst<T>::fRep)
			return theRep->pReadErase(*this);
		return null;
		}

	T ReadErase()
		{
		T temp;
		if (this->QReadErase(temp))
			return temp;
		return T();
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSetRep methods that reference details of iterators

template <class T>
bool ZSafeSetRep<T>::pReadInc(ZSafeSetIterConst<T>& ioIter, T& oValue)
	{
	if (ZQ_T<T> result = this->pReadInc(ioIter))
		{
		oValue = result.Get();
		return true;
		}
	return false;
	}

template <class T>
ZQ_T<T> ZSafeSetRep<T>::pReadInc(ZSafeSetIterConst<T>& ioIter)
	{
	ZAcqMtx acq(fMtx);
	ZQ_T<T> result;
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		}
	else
		{
		(*ioIter.fNextEntry).fIters.Erase(&ioIter);

		result = (*ioIter.fNextEntry).fT;

		if (++ioIter.fNextEntry == fList.end())
			ioIter.fRep.Clear();
		else
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);

		}
	return result;
	}

template <class T>
bool ZSafeSetRep<T>::pReadErase(ZSafeSetIter<T>& ioIter, T& oValue)
	{
	if (ZQ_T<T> result = this->pReadErase(ioIter))
		{
		oValue = result.Get();
		return true;
		}
	return false;
	}

template <class T>
ZQ_T<T> ZSafeSetRep<T>::pReadErase(ZSafeSetIter<T>& ioIter)
	{
	ZAcqMtx acq(fMtx);
	ZQ_T<T> result;
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		}
	else
		{
		(*ioIter.fNextEntry).fIters.Erase(&ioIter);

		result = (*ioIter.fNextEntry).fT;

		for (DListIteratorEraseAll<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
			i = (*ioIter.fNextEntry).fIters; i; i.Advance())
			{
			ZSafeSetIterConst<T>* theIter = i.Current();

			if (++theIter->fNextEntry == fList.end())
				theIter->fRep.Clear();
			else
				(*theIter->fNextEntry).fIters.PushBack(theIter);
			}

		ioIter.fNextEntry = fList.erase(ioIter.fNextEntry);
		if (ioIter.fNextEntry == fList.end())
			ioIter.fRep.Clear();
		else
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);

		fMap.erase(result.Get());
		}
	return result;
	}

template <class T>
bool ZSafeSetRep<T>::pErase(const T& iT)
	{
	ZAcqMtx acq(fMtx);

	typename EntryMap::iterator mapIter = fMap.lower_bound(iT);
	if (mapIter == fMap.end() || (*mapIter).first != iT)
		return false;

	typename EntryList::iterator& listIter = (*mapIter).second;
	for (DListIteratorEraseAll<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
		i = (*listIter).fIters; i; i.Advance())
		{
		ZSafeSetIterConst<T>* theIter = i.Current();

		if (++theIter->fNextEntry == fList.end())
			theIter->fRep.Clear();
		else
			(*theIter->fNextEntry).fIters.PushBack(theIter);
		}
	fList.erase(listIter);
	fMap.erase(mapIter);

	return true;
	}

} // namespace ZooLib

#endif // __ZSafeSet__
