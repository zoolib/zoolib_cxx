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
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZRefWeak.h"
#include "zoolib/ZThreadImp.h"

#include <list>
#include <map>

NAMESPACE_ZOOLIB_BEGIN

using std::list;
using std::map;

template <class T> class ZSafeSet;
template <class T> class ZSafeSetIter;
template <class T> class ZSafeSetIterConst;

template <class T>
class DLink_SafeSetIterConst
:	public DListLink<ZSafeSetIterConst<T>, DLink_SafeSetIterConst<T> >
	{};

// =================================================================================================
#pragma mark -
#pragma mark * ZSafeSet

template <class T>
class ZSafeSetRep
:	public ZRefCountedWithFinalize,
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

	typedef list<Entry> EntryList;
	typedef map<T, typename EntryList::iterator> EntryMap;

	ZSafeSetRep()
		{}

	ZSafeSetRep(const ZSafeSetRep& iOther)
		{
		ZGuardMtx guard(iOther.fMtx);
		for (typename list<Entry>::iterator i = iOther.fList.begin(); i != iOther.fList.end(); ++i)
			{
			const T& val = *i;
			typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(val));
			fMap.insert(typename EntryMap::value_type(val, listIter));
			}
		}

	virtual ~ZSafeSetRep()
		{}

	// From ZRefCountedWithFinalize
	virtual void Finalize()
		{
		ZGuardMtx guard(fMtx);
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
		delete this;
		}

	size_t pSize() const
		{
		ZGuardMtx guard(fMtx);
		return fList.size();
		}

	bool pEmpty() const
		{
		ZGuardMtx guard(fMtx);
		return fList.empty();
		}

	bool pAdd(const T& iT)
		{
		ZGuardMtx guard(fMtx);

		typename EntryMap::iterator mapIter = fMap.lower_bound(iT);
		if (mapIter != fMap.end() && (*mapIter).first == iT)
			return false;

		typename EntryList::iterator listIter = fList.insert(fList.end(), Entry(iT));
		fMap.insert(mapIter, typename EntryMap::value_type(iT, listIter));
		return true;
		}

	bool pErase(const T& iT);

	void pInit(ZRef<ZSafeSetRep> iSelf,
		ZSafeSetIterConst<T>& ioIter)
		{
		ZGuardMtx guard(fMtx);
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
		ZGuardMtx guard(fMtx);

		ioIter.fNextEntry = iOther.fNextEntry;
		if (ioIter.fNextEntry != fList.end())
			{
			ioIter.fRep = iSelf;
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);
			}
		}

	void pDestroy(ZSafeSetIterConst<T>& ioIter)
		{
		ZGuardMtx guard(fMtx);

		(*ioIter.fNextEntry).fIters.Remove(&ioIter);
		ioIter.fRep.Clear();
		}

	bool pReadInc(ZSafeSetIterConst<T>& ioIter, T& oValue);
	bool pReadErase(ZSafeSetIter<T>& ioIter, T& oValue);

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

	bool Add(const T& iT)
		{ return fRep->pAdd(iT); }

	bool Erase(const T& iT)
		{ return fRep->pErase(iT); }

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
		if (ZRef<ZSafeSetRep<T> > theRep = iOther.fRep.Use())
			theRep->pInitFrom(theRep, *this, iOther);
		}

	~ZSafeSetIterConst()
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep.Use())
			theRep->pDestroy(*this);
		}

	ZSafeSetIterConst& operator=(const ZSafeSetIterConst& iOther)
		{
		if (this != &iOther)
			{
			if (ZRef<ZSafeSetRep<T> > theRep = fRep.Use())
				theRep->pDestroy(*this);

			if (ZRef<ZSafeSetRep<T> > theRep = iOther.fRep.Use())
				theRep->pInitFrom(theRep, *this, iOther);
			}
		return *this;
		}
	
	ZSafeSetIterConst& operator=(const ZSafeSet<T>& iSafeSet)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep.Use())
			theRep->pDestroy(*this);

		if (ZRef<ZSafeSetRep<T> > theRep = iSafeSet.GetRep())
			theRep->pInit(theRep, *this);
		}

	bool ReadInc(T& oValue)
		{
		if (ZRef<ZSafeSetRep<T> > theRep = fRep.Use())
			return theRep->pReadInc(*this, oValue);
		return false;
		}

	T ReadInc()
		{
		T temp;
		if (this->ReadInc(temp))
			return temp;
		return T();
		}

protected:
	ZRefWeak<ZSafeSetRep<T> > fRep;

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
	
	bool ReadErase(T& oValue)
		{
		// Hmm, don't know why I need to qualify the reference to fRep.
		if (ZRef<ZSafeSetRep<T> > theRep = ZSafeSetIterConst<T>::fRep.Use())
			return theRep->pReadErase(*this, oValue);
		return false;
		}

	T ReadErase()
		{
		T temp;
		if (this->ReadErase(temp))
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
	ZGuardMtx guard(fMtx);
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		return false;
		}
	else
		{
		(*ioIter.fNextEntry).fIters.Remove(&ioIter);
		oValue = (*ioIter.fNextEntry).fT;

		if (++ioIter.fNextEntry == fList.end())
			ioIter.fRep.Clear();
		else
			(*ioIter.fNextEntry).fIters.PushBack(&ioIter);
		return true;
		}
	}

template <class T>
bool ZSafeSetRep<T>::pReadErase(ZSafeSetIter<T>& ioIter, T& oValue)
	{
	ZGuardMtx guard(fMtx);
	if (ioIter.fNextEntry == fList.end())
		{
		ioIter.fRep.Clear();
		return false;
		}
	else
		{
		(*ioIter.fNextEntry).fIters.Remove(&ioIter);
		oValue = (*ioIter.fNextEntry).fT;

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

		fMap.erase(oValue);
		return true;
		}
	}

template <class T>
bool ZSafeSetRep<T>::pErase(const T& iT)
	{
	ZGuardMtx guard(fMtx);

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

NAMESPACE_ZOOLIB_END

#endif // __ZSafeSet__
