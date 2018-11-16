/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#ifndef __ZooLib_Dataspace_Daton_h__
#define __ZooLib_Dataspace_Daton_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Data_Any.h"
#include "zoolib/Util_Relops.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Daton

//! A trivial implementation till I get the signing stuff figured out

class Daton
	{
public:
	Daton();
	Daton(const Daton& iOther);
	~Daton();
	Daton& operator=(const Daton& iOther);

	Daton(Data_Any iData);

	bool operator==(const Daton& iOther) const;
	bool operator<(const Daton& iOther) const;

	Data_Any GetData() const;

private:
	Data_Any fData;
	};

typedef Callable<void(const Daton* iAsserted, size_t iAssertedCount,
	const Daton* iRetracted, size_t iRetractedCount)> Callable_DatonUpdate;

} // namespace Dataspace

template <> struct RelopsTraits_HasEQ<Dataspace::Daton> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Dataspace::Daton> : public RelopsTraits_Has {};

} // namespace ZooLib

#endif // __ZooLib_Dataspace_Daton_h__
