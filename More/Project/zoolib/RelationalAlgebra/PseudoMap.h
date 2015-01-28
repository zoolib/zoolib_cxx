/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_RelationalAlgebra_PseudoMap_h__
#define __ZooLib_RelationalAlgebra_PseudoMap_h__ 1
#include "zconfig.h"

#include "zoolib/Val_Any.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark -
#pragma mark PseudoMap

class PseudoMap
	{
public:
	PseudoMap(const std::map<string8,size_t>* iBindings, const Val_Any* iVals);

	const Val_Any* PGet(const string8& iName) const;
	const ZQ<Val_Any> QGet(const string8& iName) const;
	const Val_Any& DGet(const Val_Any& iDefault, const string8& iName) const;
	const Val_Any& Get(const string8& iName) const;

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const Val_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{
		if (const Val_Any* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get(const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return S();
		}

	Map_Any AsMap() const;

protected:
	const std::map<string8,size_t>* fBindings;
	const Val_Any* fVals;
	};

// =================================================================================================
#pragma mark -
#pragma mark PseudoMap_RelHead

class PseudoMap_RelHead
:	public PseudoMap
	{
public:
	PseudoMap_RelHead(const RelHead& iRH, const Val_Any* iVals);

private:
	std::map<string8,size_t> fBindings_Storage;
	};

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_PseudoMap_h__
