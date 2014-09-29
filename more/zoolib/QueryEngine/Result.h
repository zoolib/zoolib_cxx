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

#ifndef  __ZooLib_QueryEngine_Result_h__
#define  __ZooLib_QueryEngine_Result_h__ 1
#include "zconfig.h"

#include "zoolib/Multi.h"

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZVal_Any.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
// MARK: - Result

class Result : public ZCounted
	{
public:
	Result(const RelationalAlgebra::RelHead& iRelHead,
		std::vector<ZVal_Any>* ioPackedRows);

	Result(const ZRef<Result>& iOther, size_t iRow);

	virtual ~Result();

	const RelationalAlgebra::RelHead& GetRelHead();

	size_t Count();
	const ZVal_Any* GetValsAt(size_t iIndex);

	int Compare(const Result& iOther) const;

private:
	RelationalAlgebra::RelHead fRelHead;
	std::vector<ZVal_Any> fPackedRows;
	};

// =================================================================================================
// MARK: - ResultDiffer

class ResultDiffer
	{
public:
	ResultDiffer(const RelationalAlgebra::RelHead& iIdentity,
		const RelationalAlgebra::RelHead& iSignificant);

	void Apply(const ZRef<Result>& iResult,
		ZRef<Result>* oPrior,
		std::vector<size_t>* oRemoved,
		std::vector<std::pair<size_t,size_t> >* oAdded,
		std::vector<Multi3<size_t,size_t,size_t> >* oChanged);

private:
	const RelationalAlgebra::RelHead fIdentity;
	const RelationalAlgebra::RelHead fSignificant;

	ZRef<Result> fResult_Prior;
	std::vector<size_t> fSort_Prior;

	std::vector<size_t> fPermute;
	};

} // namespace QueryEngine

template <>
int sCompare_T<QueryEngine::Result>(
	const QueryEngine::Result& iL, const QueryEngine::Result& iR);

template <>
int sCompare_T<ZRef<QueryEngine::Result> >(
	const ZRef<QueryEngine::Result>& iL, const ZRef<QueryEngine::Result>& iR);

} // namespace ZooLib

#endif //  __ZooLib_QueryEngine_Result_h__
