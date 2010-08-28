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

#ifndef __ZSet_T__
#define __ZSet_T__ 1
#include "zconfig.h"

#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T declaration

template <class T>
class ZSet_T : public std::set<T>
	{
public:
	typedef std::set<T> Base_t;

	ZSet_T(Base_t* ioElems);

	ZSet_T();
	ZSet_T(const ZSet_T& iOther);
	~ZSet_T();
	ZSet_T& operator=(const ZSet_T& iOther);

	ZSet_T(const T& iElem);

	ZSet_T(const Base_t& iElems);

	template <class Iterator>
	ZSet_T(Iterator iBegin, Iterator iEnd);

	template <class Iterator>
	ZSet_T(Iterator iBegin, size_t iCount);

	ZSet_T& operator&=(const T& iElem);
	ZSet_T operator&(const T& iElem) const;

	ZSet_T& operator&=(const Base_t& iOther);
	ZSet_T operator&(const Base_t& iOther) const;

	ZSet_T& operator|=(const T& iElem);
	ZSet_T operator|(const T& iElem) const;

	ZSet_T& operator|=(const Base_t& iOther);
	ZSet_T operator|(const Base_t& iOther) const;

	ZSet_T& operator-=(const T& iElem);
	ZSet_T operator-(const T& iElem) const;

	ZSet_T& operator-=(const Base_t& iOther);
	ZSet_T operator-(const Base_t& iOther) const;

	ZSet_T& operator^=(const T& iElem);
	ZSet_T operator^(const T& iElem) const;

	ZSet_T& operator^=(const Base_t& iOther);
	ZSet_T operator^(const Base_t& iOther) const;

	bool Contains(const Base_t& iOther) const;
	bool Contains(const T& iElem) const;

	ZSet_T& Insert(const T& iElem);
	ZSet_T& Erase(const T& iElem);

	bool Empty() const;

	const Base_t& GetElems() const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T definition

template <class T>
ZSet_T<T>::ZSet_T(Base_t* ioElems)
	{ this->swap(*ioElems); }

template <class T>
ZSet_T<T>::ZSet_T()
	{}

template <class T>
ZSet_T<T>::ZSet_T(const ZSet_T& iOther)
:	Base_t(iOther)
	{}

template <class T>
ZSet_T<T>::~ZSet_T()
	{}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator=(const ZSet_T& iOther)
	{
	Base_t::operator=(iOther);
	return *this;
	}

template <class T>
ZSet_T<T>::ZSet_T(const T& iElem)
:	Base_t(&iElem, &iElem + 1)
	{}

template <class T>
ZSet_T<T>::ZSet_T(const std::set<T>& iElems)
:	Base_t(iElems)
	{}

template <class T>
template <class Iterator>
ZSet_T<T>::ZSet_T(Iterator iBegin, Iterator iEnd)
:	Base_t(iBegin, iEnd)
	{}

template <class T>
template <class Iterator>
ZSet_T<T>::ZSet_T(Iterator iBegin, size_t iCount)
:	Base_t(iBegin, iBegin + iCount)
	{}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator&=(const T& iElem)
	{
	if (ZUtil_STL::sContains(*this, iElem))
		{
		this->clear();
		this->insert(iElem);
		}
	else
		{
		this->clear();
		}
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator&(const T& iElem) const
	{
	if (ZUtil_STL::sContains(*this, iElem))
		return ZSet_T<T>(iElem);
	else
		ZSet_T<T>();
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator&=(const Base_t& iOther)
	{
	*this = *this & iOther;
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator&(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL_set::sAnd(*static_cast<const Base_t*>(this), iOther, result);
	return ZSet_T<T>(&result);
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator|=(const T& iElem)
	{
	this->insert(iElem);
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator|(const T& iElem) const
	{
	std::set<T> result = *this;
	result.insert(iElem);
	return ZSet_T<T>(&result);
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator|=(const Base_t& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator|(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL_set::sOr(*this, iOther, result);
	return ZSet_T<T>(&result);
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator-=(const T& iElem)
	{
	this->erase(iElem);
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator-(const T& iElem) const
	{
	std::set<T> result = *this;
	result.erase(iElem);
	return ZSet_T<T>(&result);
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator-=(const Base_t& iOther)
	{
	*this = *this - iOther;
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator-(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL_set::sMinus(*this, iOther, result);
	return ZSet_T<T>(&result);
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::operator^=(const T& iElem)
	{
	typename std::set<T>::iterator iter = this->find(iElem);
	if (iter == this->end() || iElem != *iter)
		this->insert(iter, iElem);
	else
		this->erase(iter);
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator^(const T& iElem) const
	{ return ZSet_T(*this) ^= iElem; }

template <class T>
ZSet_T<T>& ZSet_T<T>::operator^=(const Base_t& iOther)
	{
	*this = *this ^ iOther;
	return *this;
	}

template <class T>
ZSet_T<T> ZSet_T<T>::operator^(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL_set::sXor(*this, iOther, result);
	return ZSet_T<T>(&result);
	}

template <class T>
bool ZSet_T<T>::Contains(const Base_t& iOther) const
	{ return ZUtil_STL_set::sIncludes(*this, iOther); }

template <class T>
bool ZSet_T<T>::Contains(const T& iElem) const
	{ return ZUtil_STL::sContains(*this, iElem); }

template <class T>
ZSet_T<T>& ZSet_T<T>::Insert(const T& iElem)
	{
	this->insert(iElem);
	return *this;
	}

template <class T>
ZSet_T<T>& ZSet_T<T>::Erase(const T& iElem)
	{
	this->erase(iElem);
	return *this;
	}

template <class T>
bool ZSet_T<T>::Empty() const
	{ return this->empty(); }

template <class T>
const std::set<T>& ZSet_T<T>::GetElems() const
	{ return *this ; }

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T operators

template <class T>
inline ZSet_T<T> operator&(const T& iElem, const ZSet_T<T>& iSet)
	{ return iSet & iElem; }

template <class T>
inline ZSet_T<T> operator|(const T& iElem, const ZSet_T<T>& iSet)
	{ return iSet | iElem; }

template <class T>
inline void swap(ZSet_T<T>& a, ZSet_T<T>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZSet_T__
