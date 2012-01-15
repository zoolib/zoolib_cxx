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

#ifndef __ZQE_Result_h__
#define __ZQE_Result_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZVal_Any.h"

#include "zoolib/zra/ZRA_RelHead.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace ZQE {

// =================================================================================================
// MARK: - Result

class Result : public ZCounted
	{
public:
	Result(const ZRA::RelHead& iRelHead,
		std::vector<ZVal_Any>* ioPackedRows,
		std::vector<std::vector<ZRef<ZCounted> > >* ioAnnotations);

	Result(const ZRef<ZQE::Result>& iOther, size_t iRow);

	virtual ~Result();

	const ZRA::RelHead& GetRelHead();

	size_t Count();
	const ZVal_Any* GetValsAt(size_t iIndex);
	void GetAnnotationsAt(size_t iIndex, std::set<ZRef<ZCounted> >& oAnnotations);

	int Compare(const Result& iOther) const;

private:
	ZRA::RelHead fRelHead;
	std::vector<ZVal_Any> fPackedRows;
	std::vector<std::vector<ZRef<ZCounted> > > fAnnotations;
	};

} // namespace ZQE

template<>
int sCompare_T(const ZQE::Result& iL, const ZQE::Result& iR);

template <>
int sCompare_T<ZRef<ZQE::Result> >(const ZRef<ZQE::Result>& iL, const ZRef<ZQE::Result>& iR);

} // namespace ZooLib

#endif // __ZQE_Result_h__
