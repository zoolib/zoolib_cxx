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

#ifndef __ZooLib_QueryEngine_Result_h__
#define __ZooLib_QueryEngine_Result_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Multi.h"
#include "zoolib/Val_Any.h"

#include "zoolib/ZCounted.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Result

class Result : public ZCounted
	{
	Result(const Result& iOther);

public:
	Result(RelationalAlgebra::RelHead* ioRelHead,
		std::vector<Val_Any>* ioPackedRows);

	Result(const RelationalAlgebra::RelHead& iRelHead,
		std::vector<Val_Any>* ioPackedRows);

	Result(const ZRef<Result>& iOther, size_t iRow);

	virtual ~Result();

	const RelationalAlgebra::RelHead& GetRelHead();

	size_t Count();
	const Val_Any* GetValsAt(size_t iIndex);

	int Compare(const Result& iOther) const;

	ZRef<Result> Fresh();

public:
	RelationalAlgebra::RelHead fRelHead;
	std::vector<Val_Any> fPackedRows;
	};

// =================================================================================================
#pragma mark - ResultDeltas

class ResultDeltas : public ZCounted
	{
public:
	ResultDeltas();

	virtual ~ResultDeltas();

public:
	std::vector<size_t> fMapping;
	std::vector<Val_Any> fPackedRows;
	};

// =================================================================================================
#pragma mark - ResultDiffer

class ResultDiffer
	{
public:
	ResultDiffer(const RelationalAlgebra::RelHead& iIdentity,
		const RelationalAlgebra::RelHead& iSignificant,
		bool iEmitDummyChanges = false);

	void Apply(const ZRef<Result>& iResult,
		ZRef<Result>* oPriorResult,
		const ZRef<ResultDeltas>& iResultDeltas,
		ZRef<Result>* oCurResult,
		std::vector<size_t>* oRemoved,
		std::vector<std::pair<size_t,size_t> >* oAdded,
		std::vector<Multi3<size_t,size_t,size_t> >* oChanged);

private:
	const RelationalAlgebra::RelHead fIdentity;
	const RelationalAlgebra::RelHead fSignificant;
	const bool fEmitDummyChanges;

	ZRef<Result> fResult_Prior;
	std::vector<size_t> fSort_Prior;

	std::vector<size_t> fPermute;
	};

// =================================================================================================
#pragma mark - sBuildBindings

void sBuildBindings(ZRef<Result> iResult, std::map<string8,size_t>& oResult);

} // namespace QueryEngine

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T<QueryEngine::Result>(
	const QueryEngine::Result& iL, const QueryEngine::Result& iR);

template <>
int sCompare_T<ZRef<QueryEngine::Result> >(
	const ZRef<QueryEngine::Result>& iL, const ZRef<QueryEngine::Result>& iR);

} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Result_h__
