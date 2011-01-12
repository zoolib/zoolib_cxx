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

#include "zoolib/ZUtil_STL_set.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T declaration

template <class T, class Comparator = std::less<T> >
class ZSet_T : public std::set<T,Comparator>
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

	size_t IndexOf(const T& iElem) const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T definition

template <class T, class Comparator>
ZSet_T<T,Comparator>::ZSet_T(Base_t* ioElems)
	{ this->swap(*ioElems); }

template <class T, class Comparator>
ZSet_T<T,Comparator>::ZSet_T()
	{}

template <class T, class Comparator>
ZSet_T<T,Comparator>::ZSet_T(const ZSet_T& iOther)
:	Base_t(iOther)
	{}

template <class T, class Comparator>
ZSet_T<T,Comparator>::~ZSet_T()
	{}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator=(const ZSet_T& iOther)
	{
	Base_t::operator=(iOther);
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>::ZSet_T(const T& iElem)
:	Base_t(&iElem, &iElem + 1)
	{}

template <class T, class Comparator>
ZSet_T<T,Comparator>::ZSet_T(const std::set<T>& iElems)
:	Base_t(iElems)
	{}

template <class T, class Comparator>
template <class Iterator>
ZSet_T<T,Comparator>::ZSet_T(Iterator iBegin, Iterator iEnd)
:	Base_t(iBegin, iEnd)
	{}

template <class T, class Comparator>
template <class Iterator>
ZSet_T<T,Comparator>::ZSet_T(Iterator iBegin, size_t iCount)
:	Base_t(iBegin, iBegin + iCount)
	{}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator&=(const T& iElem)
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

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator&(const T& iElem) const
	{
	if (ZUtil_STL::sContains(*this, iElem))
		return ZSet_T<T,Comparator>(iElem);
	else
		ZSet_T<T,Comparator>();
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator&=(const Base_t& iOther)
	{
	*this = *this & iOther;
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator&(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL::sAnd(*static_cast<const Base_t*>(this), iOther, result);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator|=(const T& iElem)
	{
	this->insert(iElem);
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator|(const T& iElem) const
	{
	std::set<T> result = *this;
	result.insert(iElem);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator|=(const Base_t& iOther)
	{
	*this = *this | iOther;
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator|(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL::sOr(*this, iOther, result);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator-=(const T& iElem)
	{
	this->erase(iElem);
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator-(const T& iElem) const
	{
	std::set<T> result = *this;
	result.erase(iElem);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator-=(const Base_t& iOther)
	{
	*this = *this - iOther;
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator-(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL::sMinus(*this, iOther, result);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator^=(const T& iElem)
	{
	typename std::set<T>::iterator iter = this->find(iElem);
	if (iter == this->end() || iElem != *iter)
		this->insert(iter, iElem);
	else
		this->erase(iter);
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator^(const T& iElem) const
	{ return ZSet_T(*this) ^= iElem; }

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::operator^=(const Base_t& iOther)
	{
	*this = *this ^ iOther;
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator> ZSet_T<T,Comparator>::operator^(const Base_t& iOther) const
	{
	std::set<T> result;
	ZUtil_STL::sXor(*this, iOther, result);
	return ZSet_T<T,Comparator>(&result);
	}

template <class T, class Comparator>
bool ZSet_T<T,Comparator>::Contains(const Base_t& iOther) const
	{ return ZUtil_STL::sIncludes(*this, iOther); }

template <class T, class Comparator>
bool ZSet_T<T,Comparator>::Contains(const T& iElem) const
	{ return ZUtil_STL::sContains(*this, iElem); }

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::Insert(const T& iElem)
	{
	this->insert(iElem);
	return *this;
	}

template <class T, class Comparator>
ZSet_T<T,Comparator>& ZSet_T<T,Comparator>::Erase(const T& iElem)
	{
	this->erase(iElem);
	return *this;
	}

template <class T, class Comparator>
bool ZSet_T<T,Comparator>::Empty() const
	{ return this->empty(); }

template <class T, class Comparator>
const std::set<T>& ZSet_T<T,Comparator>::GetElems() const
	{ return *this ; }

template <class T, class Comparator>
size_t ZSet_T<T,Comparator>::IndexOf(const T& iElem) const
	{ return std::distance(this->begin(), this->find(iElem)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZSet_T operators

template <class T, class Comparator>
inline ZSet_T<T,Comparator> operator&(const T& iElem, const ZSet_T<T,Comparator>& iSet)
	{ return iSet & iElem; }

template <class T, class Comparator>
inline ZSet_T<T,Comparator> operator|(const T& iElem, const ZSet_T<T,Comparator>& iSet)
	{ return iSet | iElem; }

template <class T, class Comparator>
inline void swap(ZSet_T<T,Comparator>& a, ZSet_T<T,Comparator>& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // __ZSet_T__
