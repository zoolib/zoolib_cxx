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

#ifndef __ZVal__
#define __ZVal__ 1
#include "zconfig.h"

#include "zoolib/ZStdInt.h" // For size_t
#include "zoolib/ZString.h"//##

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZMapIndex_T

template <class T>
class ZMapIndex_T
	{
public:
	ZMapIndex_T() : fVal(0) {}
	ZMapIndex_T(const ZMapIndex_T& iOther) : fVal(iOther.fVal) {}
	~ZMapIndex_T() {}
	ZMapIndex_T& operator=(const ZMapIndex_T& iOther)
		{
		fVal = iOther.fVal;
		return *this;
		}

	explicit ZMapIndex_T(size_t iVal) : fVal(iVal) {}

	ZMapIndex_T& operator++()
		{
		++fVal;
		return *this;
		}

	ZMapIndex_T operator++(int)
		{ return ZMapIndex_T(fVal++); }

	bool operator==(const ZMapIndex_T& iOther) const { return fVal == iOther.fVal; }
	bool operator!=(const ZMapIndex_T& iOther) const { return fVal != iOther.fVal; }

	size_t GetIndex() const { return fVal; }

private:
	size_t fVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * sPGeti, case-insensitive get

template <class Map_t>
typename Map_t::Val_t* sPGeti(Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t i = iMap.Begin(); i != iMap.End(); ++i)
		{
		if (ZString::sEquali(iMap.NameOf(i), iNamei))
			return iMap.PGet(i);
		}
	return nullptr;
	}

template <class Map_t>
const typename Map_t::Val_t* sPGeti(const Map_t& iMap, const std::string& iNamei)
	{
	for (typename Map_t::Index_t i = iMap.Begin(); i != iMap.End(); ++i)
		{
		if (ZString::sEquali(iMap.NameOf(i), iNamei))
			return iMap.PGet(i);
		}
	return nullptr;
	}

NAMESPACE_ZOOLIB_END

#endif // __ZVal__
