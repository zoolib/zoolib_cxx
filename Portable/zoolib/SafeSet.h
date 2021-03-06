// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_SafeSet_h__
#define __ZooLib_SafeSet_h__ 1
#include "zconfig.h"

#include "zoolib/Counted.h"
#include "zoolib/DList.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZThread.h"

#include <list>
#include <map>

namespace ZooLib {

template <class T> class SafeSet;
template <class T> class SafeSetIter;
template <class T> class SafeSetIterConst;

template <class T>
class DLink_SafeSetIterConst
:	public DListLink<SafeSetIterConst<T>, DLink_SafeSetIterConst<T>>
	{};

// =================================================================================================
#pragma mark - SafeSetRep

template <class T>
class SafeSetRep
:	public Counted
	{
private:
	struct Entry
		{
		Entry(const T& iT)
		:	fT(iT)
			{}

		T fT;
		DListHead<DLink_SafeSetIterConst<T>> fIters;
		};

	typedef std::list<Entry> EntryList;
	typedef std::map<T, typename EntryList::iterator> EntryMap;

	SafeSetRep()
		{}

	SafeSetRep(const SafeSetRep& iOther)
		{
		ZAcqMtx acq(iOther.fMtx);
		for (typename EntryList::const_iterator ii = iOther.fList.begin();
			ii != iOther.fList.end(); ++ii)
			{
			const T& val = ii->fT;
			typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(val));
			fMap.insert(typename EntryMap::value_type(val, listIter));
			}
		}

	virtual ~SafeSetRep()
		{}

// From Counted
	virtual void Finalize()
		{
		{ // Scope for guard
		ZAcqMtx acq(fMtx);

		if (not this->FinishFinalize())
			return;

		// Invalidate all iterators referencing entries.
		for (typename EntryList::iterator listIter = fList.begin();
			listIter != fList.end(); ++listIter)
			{
			for (DListEraser<SafeSetIterConst<T>, DLink_SafeSetIterConst<T>>
				ii = listIter->fIters; ii; ii.Advance())
				{
				SafeSetIterConst<T>* theIter = ii.Current();
				theIter->fRep.Clear();
				}
			}
		}
		delete this;
		}

	bool pWaitFor(double iTimeout, size_t iCount)
		{
		ZAcqMtx acq(fMtx);
		if (fList.size() == iCount)
			fCnd.WaitFor(fMtx, iTimeout);
		return fList.size() != iCount;
		}

	bool pWaitUntil(double iDeadline, size_t iCount)
		{
		ZAcqMtx acq(fMtx);
		if (fList.size() == iCount)
			fCnd.WaitUntil(fMtx, iDeadline);
		return fList.size() != iCount;
		}

	size_t pSize() const
		{
		ZAcqMtx acq(fMtx);
		return fList.size();
		}

	bool pIsEmpty() const
		{
		ZAcqMtx acq(fMtx);
		return fList.empty();
		}

	bool pInsert(const T& iT)
		{
		ZAcqMtx acq(fMtx);

		typename EntryMap::iterator mapIter = fMap.lower_bound(iT);
		if (mapIter != fMap.end() && mapIter->first == iT)
			return false;

		typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(iT));
		fMap.insert(mapIter, typename EntryMap::value_type(iT, listIter));

		fCnd.Broadcast();

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
			for (DListEraser<SafeSetIterConst<T>, DLink_SafeSetIterConst<T>>
				ii = listIter->fIters; ii; ii.Advance())
				{
				SafeSetIterConst<T>* theIter = ii.Current();
				theIter->fRep.Clear();
				}
			}
		fMap.clear();
		fList.clear();
		fCnd.Broadcast();
		}

	void pInit(ZP<SafeSetRep> iSelf,
		SafeSetIterConst<T>& ioIter)
		{
		ZAcqMtx acq(fMtx);
		ioIter.fNextEntry = fList.begin();
		if (ioIter.fNextEntry != fList.end())
			{
			ioIter.fRep = iSelf;
			sInsertBackMust(ioIter.fNextEntry->fIters, &ioIter);
			}
		}

	void pInitFrom(ZP<SafeSetRep> iSelf,
		SafeSetIterConst<T>& ioIter, const SafeSetIterConst<T>& iOther)
		{
		ZAcqMtx acq(fMtx);

		ioIter.fNextEntry = iOther.fNextEntry;
		if (ioIter.fNextEntry != fList.end())
			{
			ioIter.fRep = iSelf;
			sInsertBackMust(ioIter.fNextEntry->fIters, &ioIter);
			}
		}

	void pDestroy(SafeSetIterConst<T>& ioIter)
		{
		ZAcqMtx acq(fMtx);

		sEraseMust(ioIter.fNextEntry->fIters, &ioIter);
		ioIter.fRep.Clear();
		}

	bool pReadInc(SafeSetIterConst<T>& ioIter, T& oValue);
	ZQ<T> pReadInc(SafeSetIterConst<T>& ioIter);

	bool pReadErase(SafeSetIter<T>& ioIter, T& oValue);
	ZQ<T> pReadErase(SafeSetIter<T>& ioIter);

	ZMtx fMtx;
	ZCnd fCnd;
	EntryList fList;
	EntryMap fMap;

	friend class SafeSet<T>;
	friend class SafeSetIter<T>;
	friend class SafeSetIterConst<T>;
	};

// =================================================================================================
#pragma mark - SafeSet

template <class T>
class SafeSet
	{
public:
	SafeSet()
	:	fRep(new SafeSetRep<T>)
		{}

	SafeSet(const SafeSet& iOther)
	:	fRep(new SafeSetRep<T>(*iOther.GetRep().Get()))
		{}

	~SafeSet()
		{}

	SafeSet& operator=(const SafeSet& iOther)
		{
		if (this != &iOther)
			fRep = new SafeSetRep<T>(*iOther.fRep.Get());

		return *this;
		}

	size_t Size() const
		{ return fRep->pSize(); }

	bool IsEmpty() const
		{ return fRep->pIsEmpty(); }

	bool Insert(const T& iT)
		{ return fRep->pInsert(iT); }

	bool Erase(const T& iT)
		{ return fRep->pErase(iT); }

	void Clear()
		{ fRep->pClear(); }

	bool WaitFor(double iTimeout, size_t iCount)
		{ return fRep->pWaitFor(iTimeout, iCount); }

	bool WaitUntil(double iDeadline, size_t iCount)
		{ return fRep->pWaitUntil(iDeadline, iCount); }

	ZP<SafeSetRep<T>> GetRep() const
		{ return fRep; }

private:
	ZP<SafeSetRep<T>> fRep;
	};

// =================================================================================================
#pragma mark - SafeSetIterConst

template <class T>
class SafeSetIterConst : public DLink_SafeSetIterConst<T>
	{
public:
	SafeSetIterConst()
		{}

	SafeSetIterConst(const SafeSet<T>& iSafeSet)
		{
		if (ZP<SafeSetRep<T>> theRep = iSafeSet.GetRep())
			theRep->pInit(theRep, *this);
		}

	SafeSetIterConst(const SafeSetIterConst& iOther)
		{
		if (ZP<SafeSetRep<T>> theRep = iOther.fRep)
			theRep->pInitFrom(theRep, *this, iOther);
		}

	~SafeSetIterConst()
		{
		if (ZP<SafeSetRep<T>> theRep = fRep)
			theRep->pDestroy(*this);
		}

	SafeSetIterConst& operator=(const SafeSetIterConst& iOther)
		{
		if (this != &iOther)
			{
			if (ZP<SafeSetRep<T>> theRep = fRep)
				theRep->pDestroy(*this);

			if (ZP<SafeSetRep<T>> theRep = iOther.fRep)
				theRep->pInitFrom(theRep, *this, iOther);
			}
		return *this;
		}

	SafeSetIterConst& operator=(const SafeSet<T>& iSafeSet)
		{
		if (ZP<SafeSetRep<T>> theRep = fRep)
			theRep->pDestroy(*this);

		if (ZP<SafeSetRep<T>> theRep = iSafeSet.GetRep())
			theRep->pInit(theRep, *this);
		}

	bool QReadInc(T& oValue)
		{
		if (ZP<SafeSetRep<T>> theRep = fRep)
			return theRep->pReadInc(*this, oValue);
		return false;
		}

	ZQ<T> QReadInc()
		{
		if (ZP<SafeSetRep<T>> theRep = fRep)
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
	WP<SafeSetRep<T>> fRep;

private:
	typename SafeSetRep<T>::EntryList::iterator fNextEntry;
	friend class SafeSetRep<T>;
	};

// =================================================================================================
#pragma mark - SafeSetIter

template <class T>
class SafeSetIter : public SafeSetIterConst<T>
	{
public:
	SafeSetIter()
		{}

	SafeSetIter(SafeSet<T>& iSafeSet)
	:	SafeSetIterConst<T>(iSafeSet)
		{}

	SafeSetIter(const SafeSetIter& iOther)
	:	SafeSetIterConst<T>(iOther)
		{}

	~SafeSetIter()
		{}

	SafeSetIter& operator=(const SafeSetIter& iOther)
		{ return SafeSetIterConst<T>::operator=(iOther); }

	bool QReadErase(T& oValue)
		{
		if (ZP<SafeSetRep<T>> theRep = SafeSetIterConst<T>::fRep)
			return theRep->pReadErase(*this, oValue);
		return false;
		}

	ZQ<T> QReadErase()
		{
		if (ZP<SafeSetRep<T>> theRep = SafeSetIterConst<T>::fRep)
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
#pragma mark - SafeSetRep methods that reference details of iterators

template <class T>
bool SafeSetRep<T>::pReadInc(SafeSetIterConst<T>& ioIter, T& oValue)
	{
	if (ZQ<T> result = this->pReadInc(ioIter))
		{
		oValue = result.Get();
		return true;
		}
	return false;
	}

template <class T>
ZQ<T> SafeSetRep<T>::pReadInc(SafeSetIterConst<T>& ioIter)
	{
	ZAcqMtx acq(fMtx);
	ZQ<T> result;
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		}
	else
		{
		sEraseMust(ioIter.fNextEntry->fIters, &ioIter);

		result = ioIter.fNextEntry->fT;

		if (++ioIter.fNextEntry == fList.end())
			ioIter.fRep.Clear();
		else
			sInsertBackMust(ioIter.fNextEntry->fIters, &ioIter);

		}
	return result;
	}

template <class T>
bool SafeSetRep<T>::pReadErase(SafeSetIter<T>& ioIter, T& oValue)
	{
	if (ZQ<T> result = this->pReadErase(ioIter))
		{
		oValue = result.Get();
		return true;
		}
	return false;
	}

template <class T>
ZQ<T> SafeSetRep<T>::pReadErase(SafeSetIter<T>& ioIter)
	{
	ZAcqMtx acq(fMtx);
	fCnd.Broadcast();
	ZQ<T> result;
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		}
	else
		{
		ioIter.fNextEntry->fIters.Erase(&ioIter);

		result = ioIter.fNextEntry->fT;

		for (DListEraser<SafeSetIterConst<T>, DLink_SafeSetIterConst<T>>
			ii = ioIter.fNextEntry->fIters; ii; ii.Advance())
			{
			SafeSetIterConst<T>* theIter = ii.Current();

			if (++theIter->fNextEntry == fList.end())
				theIter->fRep.Clear();
			else
				theIter->fNextEntry->fIters.PushBack(theIter);
			}

		ioIter.fNextEntry = fList.erase(ioIter.fNextEntry);
		if (ioIter.fNextEntry == fList.end())
			ioIter.fRep.Clear();
		else
			ioIter.fNextEntry->fIters.PushBack(&ioIter);

		fMap.erase(result.Get());
		}
	return result;
	}

template <class T>
bool SafeSetRep<T>::pErase(const T& iT)
	{
	ZAcqMtx acq(fMtx);
	fCnd.Broadcast();

	typename EntryMap::iterator mapIter = fMap.lower_bound(iT);
	if (mapIter == fMap.end() || mapIter->first != iT)
		return false;

	typename EntryList::iterator& listIter = mapIter->second;
	for (DListEraser<SafeSetIterConst<T>, DLink_SafeSetIterConst<T>>
		ii = listIter->fIters; ii; ii.Advance())
		{
		SafeSetIterConst<T>* theIter = ii.Current();

		if (++theIter->fNextEntry == fList.end())
			theIter->fRep.Clear();
		else
			sInsertBackMust(theIter->fNextEntry->fIters, theIter);
		}
	fList.erase(listIter);
	fMap.erase(mapIter);

	return true;
	}

} // namespace ZooLib

#endif // __ZooLib_SafeSet_h__
