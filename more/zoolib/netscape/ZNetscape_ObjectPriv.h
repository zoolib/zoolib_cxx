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

#ifndef __ZNetscape_ObjectPriv_h__
#define __ZNetscape_ObjectPriv_h__ 1
#include "zconfig.h"

#include "zoolib/netscape/ZNetscape_Object.h"

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPObject_T

template <class Variant_t>
NPObject_T<Variant_t>::NPObject_T()
	{}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::Invoke(
	const std::string& iName, const Variant_t* iArgs, size_t iCount)
	{
	Variant_t result;
	static_cast<Self_t*>(this)->Invoke(iName, iArgs, iCount, result);
	return result;
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::Invoke(const std::string& iName)
	{
	Variant_t result;
	static_cast<Self_t*>(this)->Invoke(iName, nullptr, 0, result);
	return result;
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::Invoke(const std::string& iName,
	const Variant_t& i0)
	{ return static_cast<Self_t*>(this)->Invoke(iName, &i0, 1); }

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::Invoke(const std::string& iName,
	const Variant_t& i0,
	const Variant_t& i1)
	{
	Variant_t arr[] = { i0, i1};
	return static_cast<Self_t*>(this)->Invoke(iName, arr, countof(arr));
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::Invoke(const std::string& iName,
	const Variant_t& i0,
	const Variant_t& i1,
	const Variant_t& i2)
	{
	Variant_t arr[] = { i0, i1, i2 };
	return static_cast<Self_t*>(this)->Invoke(iName, arr, countof(arr));
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::InvokeDefault(const Variant_t* iArgs, size_t iCount)
	{
	Variant_t result;
	static_cast<Self_t*>(this)->InvokeDefault(iArgs, iCount, result);
	return result;
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::InvokeDefault()
	{ return static_cast<Self_t*>(this)->InvokeDefault(nullptr, 0); }

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::InvokeDefault(
	const Variant_t& i0)
	{ return static_cast<Self_t*>(this)->InvokeDefault(&i0, 1); }

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::InvokeDefault(
	const Variant_t& i0,
	const Variant_t& i1)
	{
	Variant_t arr[] = { i0, i1 };
	return static_cast<Self_t*>(this)->InvokeDefault(arr, countof(arr));
	}

template <class Variant_t>
Variant_t NPObject_T<Variant_t>::InvokeDefault(
	const Variant_t& i0,
	const Variant_t& i1,
	const Variant_t& i2)
	{
	Variant_t arr[] = { i0, i1, i2 };
	return static_cast<Self_t*>(this)->InvokeDefault(arr, countof(arr));
	}

template <class Variant_t>
bool NPObject_T<Variant_t>::Enumerate(std::vector<NPIdentifier>& oIdentifiers)
	{
	oIdentifiers.clear();
	NPIdentifier* theIDs = nullptr;
	uint32 theCount;
	if (!static_cast<Self_t*>(this)->Enumerate(theIDs, theCount))
		return false;

	oIdentifiers.insert(oIdentifiers.end(), theIDs, theIDs + theCount);

	sFree_T<Variant_t>(theIDs);

	return true;
	}

template <class Variant_t>
const ZQ<Variant_t> NPObject_T<Variant_t>::QGet(const std::string& iName)
	{
	Variant_t theVar;
	if (static_cast<Self_t*>(this)->GetProperty(iName, theVar))
		return theVar;
	return null;
	}

template <class Variant_t>
const ZQ<Variant_t> NPObject_T<Variant_t>::QGet(size_t iIndex)
	{
	Variant_t theVar;
	if (static_cast<Self_t*>(this)->GetProperty(iIndex, theVar))
		return theVar;
	return null;
	}

template <class Variant_t>
const Variant_t NPObject_T<Variant_t>::DGet(const Variant_t& iDefault, const std::string& iName)
	{
	Variant_t result;
	if (static_cast<Self_t*>(this)->GetProperty(iName, result))
		return result;
	return iDefault;
	}

template <class Variant_t>
const Variant_t NPObject_T<Variant_t>::DGet(const Variant_t& iDefault, size_t iIndex)
	{
	Variant_t result;
	if (static_cast<Self_t*>(this)->GetProperty(iIndex, result))
		return result;
	return iDefault;
	}

template <class Variant_t>
const Variant_t NPObject_T<Variant_t>::Get(const std::string& iName)
	{ return static_cast<Self_t*>(this)->DGet(Variant_t(), iName); }

template <class Variant_t>
const Variant_t NPObject_T<Variant_t>::Get(size_t iIndex)
	{ return static_cast<Self_t*>(this)->DGet(Variant_t(), iIndex); }

template <class Variant_t>
bool NPObject_T<Variant_t>::Set(const std::string& iName, const Variant_t& iValue)
	{ return static_cast<Self_t*>(this)->SetProperty(iName, iValue); }

template <class Variant_t>
bool NPObject_T<Variant_t>::Set(size_t iIndex, const Variant_t& iValue)
	{ return static_cast<Self_t*>(this)->SetProperty(iIndex, iValue); }

template <class Variant_t>
bool NPObject_T<Variant_t>::Erase(const std::string& iName)
	{ return static_cast<Self_t*>(this)->RemoveProperty(iName); }

template <class Variant_t>
bool NPObject_T<Variant_t>::Erase(size_t iIndex)
	{ return static_cast<Self_t*>(this)->RemoveProperty(iIndex); }

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_ObjectPriv_h__
