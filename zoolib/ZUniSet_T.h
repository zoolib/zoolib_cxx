/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZUniSet_T__
#define __ZUniSet_T__ 1
#include "zconfig.h"

#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZUniSet_T declaration

template <class T>
class ZUniSet_T
	{
	explicit ZUniSet_T(bool iUniversal, std::set<T>* ioElems);

public:
	typedef T key_type;
	typedef T value_type;

	void swap(ZUniSet_T& iOther);

	ZUniSet_T();
	ZUniSet_T(const ZUniSet_T& iOther);
	~ZUniSet_T();
	ZUniSet_T& operator=(const ZUniSet_T& iOther);

	static ZUniSet_T sUniversal();

	ZUniSet_T(const std::set<T>& iElems);
	ZUniSet_T(bool iUniversal, const std::set<T>& iElems);

	ZUniSet_T(const T& iElem);

	ZUniSet_T(const T& iElem1, const T& iElem2);

	template <class Iterator>
	ZUniSet_T(Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	ZUniSet_T(Iterator iBegin, size_t iCount);

	template <class Iterator>
	ZUniSet_T(bool iUniversal, Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	ZUniSet_T(bool iUniversal, Iterator iBegin, size_t count);

	bool operator==(const ZUniSet_T& iOther) const;
	bool operator!=(const ZUniSet_T& iOther) const;

	ZUniSet_T& operator&=(const ZUniSet_T& iOther);
	ZUniSet_T operator&(const ZUniSet_T& iOther) const;

	ZUniSet_T& operator|=(const ZUniSet_T& iOther);
	ZUniSet_T operator|(const ZUniSet_T& iOther) const;

	ZUniSet_T& operator-=(const ZUniSet_T& iOther);
	ZUniSet_T operator-(const ZUniSet_T& iOther) const;

	ZUniSet_T& operator^=(const ZUniSet_T& iOther);
	ZUniSet_T operator^(const ZUniSet_T& iOther) const;

	bool Contains(const ZUniSet_T& iOther) const;
	bool Contains(const T& iElem) const;

	ZUniSet_T& Insert(const T& iElem);
	ZUniSet_T& Erase(const T& iElem);

	void GetElems(bool& oUniversal, std::set<T>& oElems) const;
	const std::set<T>& GetElems(bool& oUniversal) const;

private:
	bool fUniversal;
	std::set<T> fElems;
	};

template <class T>
ZUniSet_T<T> operator|(const T& iElem, const ZUniSet_T<T>& iUniSet_T);

template <class T>
ZUniSet_T<T> operator&(const T& iElem, const ZUniSet_T<T>& iUniSet_T);

// =================================================================================================
#pragma mark -
#pragma mark * ZUniSet_T definition

template <class T>
void ZUniSet_T<T>::swap(ZUniSet_T& iOther)
	{
	std::swap(fUniversal, iOther.fUniversal);
	std::swap(fElems, iOther.fElems);
	}

template <class T>
ZUniSet_T<T>::ZUniSet_T(bool iUniversal, std::set<T>* ioElems)
:	fUniversal(iUniversal)
	{
	if (ioElems)
		ioElems->swap(fElems);
	}

template <class T>
ZUniSet_T<T>::ZUniSet_T()
:	fUniversal(false)
	{}

template <class T>
ZUniSet_T<T>::ZUniSet_T(const ZUniSet_T& iOther)
:	fUniversal(iOther.fUniversal)
,	fElems(iOther.fElems)
	{}

template <class T>
ZUniSet_T<T>::~ZUniSet_T()
	{}

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::operator=(const ZUniSet_T& iOther)
	{
	fUniversal = iOther.fUniversal;
	fElems = iOther.fElems;
	return *this;
	}

template <class T>
ZUniSet_T<T> ZUniSet_T<T>::sUniversal()
	{ return ZUniSet_T<T>(true, nullptr); }

template <class T>
ZUniSet_T<T>::ZUniSet_T(const std::set<T>& iElems)
:	fUniversal(false)
,	fElems(iElems)
	{}

template <class T>
ZUniSet_T<T>::ZUniSet_T(bool iUniversal, const std::set<T>& iElems)
:	fUniversal(iUniversal)
,	fElems(iElems)
	{}

template <class T>
ZUniSet_T<T>::ZUniSet_T(const T& iElem)
:	fUniversal(false)
,	fElems(&iElem, &iElem + 1)
	{}

template <class T>
ZUniSet_T<T>::ZUniSet_T(const T& iElem1, const T& iElem2)
:	fUniversal(false)
	{
	fElems.insert(iElem1);
	fElems.insert(iElem2);
	}

template <class T>
template <class Iterator>
ZUniSet_T<T>::ZUniSet_T(Iterator iBegin, Iterator iEnd)
:	fUniversal(false)
,	fElems(iBegin, iEnd)
	{}

template <class T>
template <class Iterator>
ZUniSet_T<T>::ZUniSet_T(Iterator iBegin, size_t iCount)
:	fUniversal(false)
,	fElems(iBegin, iBegin + iCount)
	{}

template <class T>
template <class Iterator>
ZUniSet_T<T>::ZUniSet_T(bool iUniversal, Iterator iBegin, Iterator iEnd)
:	fUniversal(iUniversal)
,	fElems(iBegin, iEnd)
	{}

template <class T>
template <class Iterator>
ZUniSet_T<T>::ZUniSet_T(bool iUniversal, Iterator iBegin, size_t iCount)
:	fUniversal(iUniversal)
,	fElems(iBegin, iBegin + iCount)
	{}

template <class T>
bool ZUniSet_T<T>::operator==(const ZUniSet_T& iOther) const
	{ return fUniversal == iOther.fUniversal && fElems == iOther.fElems; }

template <class T>
bool ZUniSet_T<T>::operator!=(const ZUniSet_T& iOther) const
	{ return fUniversal != iOther.fUniversal || fElems != iOther.fElems; }

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::operator&=(const ZUniSet_T& iOther)
	{
	*this = *this & iOther;
	return *this;
	}

template <class T>
ZUniSet_T<T> ZUniSet_T<T>::operator&(const ZUniSet_T& iOther) const
	{
	std::set<T> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			ZUtil_STL::sOr(fElems, iOther.fElems, result);
			}
		else
			{
			resultUniversal = false;
			ZUtil_STL::sMinus(iOther.fElems, fElems, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			ZUtil_STL::sMinus(fElems, iOther.fElems, result);
			}
		else
			{
			resultUniversal = false;
			ZUtil_STL::sAnd(iOther.fElems, fElems, result);
			}
		}
	return ZUniSet_T(resultUniversal, &result);
	}

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::operator|=(const ZUniSet_T& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

template <class T>
ZUniSet_T<T> ZUniSet_T<T>::operator|(const ZUniSet_T& iOther) const
	{
	std::set<T> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			ZUtil_STL::sAnd(fElems, iOther.fElems, result);
			}
		else
			{
			resultUniversal = true;
			ZUtil_STL::sMinus(fElems, iOther.fElems, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = true;
			ZUtil_STL::sMinus(iOther.fElems, fElems, result);
			}
		else
			{
			resultUniversal = false;
			ZUtil_STL::sOr(fElems, iOther.fElems, result);
			}
		}
	return ZUniSet_T(resultUniversal, &result);
	}

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::operator-=(const ZUniSet_T<T>& iOther)
	{
	*this = *this - iOther;
	return *this;
	}

template <class T>
ZUniSet_T<T> ZUniSet_T<T>::operator-(const ZUniSet_T& iOther) const
	{
	std::set<T> result;
	bool resultUniversal;
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			ZUtil_STL::sMinus(iOther.fElems, fElems, result);
			}
		else
			{
			resultUniversal = true;
			ZUtil_STL::sOr(fElems, iOther.fElems, result);
			}
		}
	else
		{
		if (iOther.fUniversal)
			{
			resultUniversal = false;
			ZUtil_STL::sAnd(fElems, iOther.fElems, result);
			}
		else
			{
			resultUniversal = false;
			ZUtil_STL::sMinus(fElems, iOther.fElems, result);
			}
		}
	return ZUniSet_T(resultUniversal, &result);
	}

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::operator^=(const ZUniSet_T& iOther)
	{
	*this = *this ^ iOther;
	return *this;
	}

template <class T>
ZUniSet_T<T> ZUniSet_T<T>::operator^(const ZUniSet_T& iOther) const
	{
	std::set<T> result;
	bool resultUniversal = fUniversal ^ iOther.fUniversal;
	ZUtil_STL::sXor(fElems, iOther.fElems, result);
	return ZUniSet_T(resultUniversal, &result);
	}

template <class T>
bool ZUniSet_T<T>::Contains(const ZUniSet_T& iOther) const
	{
	if (fUniversal)
		{
		if (iOther.fUniversal)
			{
			return ZUtil_STL::sIncludes(iOther.fElems, fElems);
			}
		else
			{
			std::set<T> sect;
			ZUtil_STL::sAnd(fElems, iOther.fElems, sect);
			return sect.empty();
			}
		}
	else
		{
		if (iOther.fUniversal)
			return false;
		else
			return ZUtil_STL::sIncludes(fElems, iOther.fElems);
		}
	}

template <class T>
bool ZUniSet_T<T>::Contains(const T& iElem) const
	{ return fUniversal != (fElems.end() != fElems.find(iElem)); }

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::Insert(const T& iElem)
	{
	if (fUniversal)
		fElems.erase(iElem);
	else
		fElems.insert(iElem);
	return *this;
	}

template <class T>
ZUniSet_T<T>& ZUniSet_T<T>::Erase(const T& iElem)
	{
	if (fUniversal)
		fElems.insert(iElem);
	else
		fElems.erase(iElem);
	return *this;
	}

template <class T>
void ZUniSet_T<T>::GetElems(bool& oUniversal, std::set<T>& oElems) const
	{
	oUniversal = fUniversal;
	oElems = fElems;
	}

template <class T>
const std::set<T>& ZUniSet_T<T>::GetElems(bool& oUniversal) const
	{
	oUniversal = fUniversal;
	return fElems;
	}

template <class T>
ZUniSet_T<T> operator|(const T& iElem, const ZUniSet_T<T>& iUniSet_T)
	{ return iUniSet_T | iElem; }

template <class T>
ZUniSet_T<T> operator&(const T& iElem, const ZUniSet_T<T>& iUniSet_T)
	{ return iUniSet_T & iElem; }

template <class T>
inline void swap(ZUniSet_T<T>& a, ZUniSet_T<T>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZUniSet_T__
