// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UniSet_h__
#define __ZooLib_UniSet_h__ 1
#include "zconfig.h"

#include "zoolib/Util_STL_set.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - UniSet declaration

template <class T>
class UniSet
	{
	explicit UniSet(bool iUniversal, std::set<T>* ioElems);

public:
	typedef T key_type;
	typedef T value_type;

	void swap(UniSet& iOther);

	UniSet();
	UniSet(const UniSet& iOther);
	~UniSet();
	UniSet& operator=(const UniSet& iOther);

	static UniSet sUniversal();

	UniSet(const std::set<T>& iElems);
	UniSet(bool iUniversal, const std::set<T>& iElems);

	UniSet(const T& iElem);

	UniSet(const T& iElem1, const T& iElem2);

	template <class Iterator>
	UniSet(Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	UniSet(Iterator iBegin, size_t iCount);

	template <class Iterator>
	UniSet(bool iUniversal, Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	UniSet(bool iUniversal, Iterator iBegin, size_t count);

	bool operator==(const UniSet& iOther) const;
	bool operator!=(const UniSet& iOther) const;

	UniSet& operator&=(const UniSet& iOther);
	UniSet operator&(const UniSet& iOther) const;

	UniSet& operator|=(const UniSet& iOther);
	UniSet operator|(const UniSet& iOther) const;

	UniSet& operator-=(const UniSet& iOther);
	UniSet operator-(const UniSet& iOther) const;

	UniSet& operator^=(const UniSet& iOther);
	UniSet operator^(const UniSet& iOther) const;

	bool Contains(const UniSet& iOther) const;
	bool Contains(const T& iElem) const;

	UniSet& Insert(const T& iElem);
	bool QInsert(const T& iElem);

	UniSet& Erase(const T& iElem);
	bool QErase(const T& iElem);

	void GetElems(bool& oUniversal, std::set<T>& oElems) const;
	const std::set<T>& GetElems(bool& oUniversal) const;

private:
	bool fUniversal;
	std::set<T> fElems;
	};

template <class T>
UniSet<T> operator|(const T& iElem, const UniSet<T>& iUniSet_T);

template <class T>
UniSet<T> operator&(const T& iElem, const UniSet<T>& iUniSet_T);

// =================================================================================================
#pragma mark - UniSet definition

template <class T>
void UniSet<T>::swap(UniSet& iOther)
	{
	using std::swap;
	swap(fUniversal, iOther.fUniversal);
	swap(fElems, iOther.fElems);
	}

template <class T>
UniSet<T>::UniSet(bool iUniversal, std::set<T>* ioElems)
:	fUniversal(iUniversal)
	{
	if (ioElems)
		ioElems->swap(fElems);
	}

template <class T>
UniSet<T>::UniSet()
:	fUniversal(false)
	{}

template <class T>
UniSet<T>::UniSet(const UniSet& iOther)
:	fUniversal(iOther.fUniversal)
,	fElems(iOther.fElems)
	{}

template <class T>
UniSet<T>::~UniSet()
	{}

template <class T>
UniSet<T>& UniSet<T>::operator=(const UniSet& iOther)
	{
	fUniversal = iOther.fUniversal;
	fElems = iOther.fElems;
	return *this;
	}

template <class T>
UniSet<T> UniSet<T>::sUniversal()
	{ return UniSet<T>(true, nullptr); }

template <class T>
UniSet<T>::UniSet(const std::set<T>& iElems)
:	fUniversal(false)
,	fElems(iElems)
	{}

template <class T>
UniSet<T>::UniSet(bool iUniversal, const std::set<T>& iElems)
:	fUniversal(iUniversal)
,	fElems(iElems)
	{}

template <class T>
UniSet<T>::UniSet(const T& iElem)
:	fUniversal(false)
,	fElems(&iElem, &iElem + 1)
	{}

template <class T>
UniSet<T>::UniSet(const T& iElem1, const T& iElem2)
:	fUniversal(false)
	{
	fElems.insert(iElem1);
	fElems.insert(iElem2);
	}

template <class T>
template <class Iterator>
UniSet<T>::UniSet(Iterator iBegin, Iterator iEnd)
:	fUniversal(false)
,	fElems(iBegin, iEnd)
	{}

template <class T>
template <class Iterator>
UniSet<T>::UniSet(Iterator iBegin, size_t iCount)
:	fUniversal(false)
,	fElems(iBegin, iBegin + iCount)
	{}

template <class T>
template <class Iterator>
UniSet<T>::UniSet(bool iUniversal, Iterator iBegin, Iterator iEnd)
:	fUniversal(iUniversal)
,	fElems(iBegin, iEnd)
	{}

template <class T>
template <class Iterator>
UniSet<T>::UniSet(bool iUniversal, Iterator iBegin, size_t iCount)
:	fUniversal(iUniversal)
,	fElems(iBegin, iBegin + iCount)
	{}

template <class T>
bool UniSet<T>::operator==(const UniSet& iOther) const
	{ return fUniversal == iOther.fUniversal && fElems == iOther.fElems; }

template <class T>
bool UniSet<T>::operator!=(const UniSet& iOther) const
	{ return fUniversal != iOther.fUniversal || fElems != iOther.fElems; }

template <class T>
UniSet<T>& UniSet<T>::operator&=(const UniSet& iOther)
	{
	*this = *this & iOther;
	return *this;
	}

template <class T>
UniSet<T> UniSet<T>::operator&(const UniSet& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			return UniSet(true, fElems | iOther.fElems);
		else
			return UniSet(false, iOther.fElems - fElems);
		}
	else
		{
		if (iOther.fUniversal)
			return UniSet(false, fElems - iOther.fElems);
		else
			return UniSet(false, iOther.fElems & fElems);
		}
	}

template <class T>
UniSet<T>& UniSet<T>::operator|=(const UniSet& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

template <class T>
UniSet<T> UniSet<T>::operator|(const UniSet& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			return UniSet(true, fElems & iOther.fElems);
		else
			return UniSet(true, fElems - iOther.fElems);
		}
	else
		{
		if (iOther.fUniversal)
			return UniSet(true, iOther.fElems - fElems);
		else
			return UniSet(false, fElems | iOther.fElems);
		}
	}

template <class T>
UniSet<T>& UniSet<T>::operator-=(const UniSet<T>& iOther)
	{
	*this = *this - iOther;
	return *this;
	}

template <class T>
UniSet<T> UniSet<T>::operator-(const UniSet& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			return UniSet(false, iOther.fElems - fElems);
		else
			return UniSet(true, fElems | iOther.fElems);
		}
	else
		{
		if (iOther.fUniversal)
			return UniSet(false, fElems & iOther.fElems);
		else
			return UniSet(false, fElems - iOther.fElems);
		}
	}

template <class T>
UniSet<T>& UniSet<T>::operator^=(const UniSet& iOther)
	{
	*this = *this ^ iOther;
	return *this;
	}

template <class T>
UniSet<T> UniSet<T>::operator^(const UniSet& iOther) const
	{ return UniSet(fUniversal ^ iOther.fUniversal, fElems ^ iOther.fElems); }

template <class T>
bool UniSet<T>::Contains(const UniSet& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			return Util_STL::sIncludes(iOther.fElems, fElems);
		else
			return (fElems & iOther.fElems).empty();
		}
	else
		{
		if (iOther.fUniversal)
			return false;
		else
			return Util_STL::sIncludes(fElems, iOther.fElems);
		}
	}

template <class T>
bool UniSet<T>::Contains(const T& iElem) const
	{ return fUniversal != (fElems.end() != fElems.find(iElem)); }

template <class T>
UniSet<T>& UniSet<T>::Insert(const T& iElem)
	{
	if (fUniversal)
		fElems.erase(iElem);
	else
		fElems.insert(iElem);
	return *this;
	}

template <class T>
bool UniSet<T>::QInsert(const T& iElem)
	{
	if (fUniversal)
		return fElems.erase(iElem);
	else
		return fElems.insert(iElem).second;
	}

template <class T>
UniSet<T>& UniSet<T>::Erase(const T& iElem)
	{
	if (fUniversal)
		fElems.insert(iElem);
	else
		fElems.erase(iElem);
	return *this;
	}

template <class T>
bool UniSet<T>::QErase(const T& iElem)
	{
	if (fUniversal)
		return fElems.insert(iElem).second;
	else
		return fElems.erase(iElem);
	}

template <class T>
void UniSet<T>::GetElems(bool& oUniversal, std::set<T>& oElems) const
	{
	oUniversal = fUniversal;
	oElems = fElems;
	}

template <class T>
const std::set<T>& UniSet<T>::GetElems(bool& oUniversal) const
	{
	oUniversal = fUniversal;
	return fElems;
	}

template <class T>
UniSet<T> operator|(const T& iElem, const UniSet<T>& iUniSet_T)
	{ return iUniSet_T | iElem; }

template <class T>
UniSet<T> operator&(const T& iElem, const UniSet<T>& iUniSet_T)
	{ return iUniSet_T & iElem; }

template <class T>
inline void swap(UniSet<T>& a, UniSet<T>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZooLib_UniSet_h__
