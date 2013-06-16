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

#ifndef __ZNetscape_Object_h__
#define __ZNetscape_Object_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"
#include "zoolib/netscape/ZNetscape.h"

#include <string>
#include <vector>

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPObject_T

template <class Variant_t>
class NPObject_T
:	public NPObject
	{
protected:
	NPObject_T();

public:
	typedef NPObject_T Base_t;
	typedef typename Variant_t::Object_t Self_t;

	Variant_t Invoke(const std::string& iName, const Variant_t* iArgs, size_t iCount);

	Variant_t Invoke(const std::string& iName);

	Variant_t Invoke(const std::string& iName,
		const Variant_t& i0);

	Variant_t Invoke(const std::string& iName,
		const Variant_t& i0,
		const Variant_t& i1);

	Variant_t Invoke(const std::string& iName,
		const Variant_t& i0,
		const Variant_t& i1,
		const Variant_t& i2);

	Variant_t InvokeDefault(const Variant_t* iArgs, size_t iCount);

	Variant_t InvokeDefault();

	Variant_t InvokeDefault(
		const Variant_t& i0);

	Variant_t InvokeDefault(
		const Variant_t& i0,
		const Variant_t& i1);

	Variant_t InvokeDefault(
		const Variant_t& i0,
		const Variant_t& i1,
		const Variant_t& i2);

	bool Enumerate(std::vector<NPIdentifier>& oIdentifiers);

// ZMap protocol (ish)
	const ZQ<Variant_t> QGet(const std::string& iName);
	const ZQ<Variant_t> QGet(size_t iIndex);

	const Variant_t DGet(const Variant_t& iDefault, const std::string& iName);
	const Variant_t DGet(const Variant_t& iDefault, size_t iIndex);

	const Variant_t Get(const std::string& iName);
	const Variant_t Get(size_t iIndex);

	bool Set(const std::string& iName, const Variant_t& iValue);
	bool Set(size_t iIndex, const Variant_t& iValue);

	bool Erase(const std::string& iName);
	bool Erase(size_t iIndex);
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_Object_h__
