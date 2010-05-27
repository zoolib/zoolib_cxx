/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTupleIndex__
#define __ZTupleIndex__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTBSpec.h"

#include <deque>
#include <list>
#include <set>
#include <vector>

#ifndef ZCONFIG_TupleIndex_Debug
#	define ZCONFIG_TupleIndex_Debug 3
#endif

namespace ZooLib {

class ZStrimW;

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex

class ZTupleIndex
	{
public:
	static const uint64 kMaxID = ZUINT64_C(0xFFFFFFFFFFFFFFFF);

	virtual ~ZTupleIndex();

	virtual void Insert(uint64 iID, const ZTuple* iTuple) = 0;

	virtual void Erase(uint64 iID, const ZTuple* iTuple) = 0;

	virtual void Find(const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs) = 0;

	virtual size_t CanHandle(const ZTBSpec::CriterionSect& iCriterionSect) = 0;

	void Find(const ZTBSpec::CriterionSect& iCriterionSect, const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& oUncheckedCriteria, std::vector<uint64>& oIDs);

	virtual void WriteDescription(const ZStrimW& s);

	static bool sMatchIndices(const ZTBSpec::CriterionUnion& iCriterionUnion,
		const std::vector<ZTupleIndex*>& iIndices, std::vector<ZTupleIndex*>& oIndices);

	static bool sGatherMergeConstraints(const ZTName& iPropName,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria,
		const ZTValue*& oValueEqual,
		const ZTValue*& oBestValueLess, const ZTValue*& oBestValueLessEqual,
		const ZTValue*& oBestValueGreater, const ZTValue*& oBestValueGreaterEqual);

protected:
	static ZTValue spEmptyVal;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory

class ZTupleIndexFactory : public ZCountedWithoutFinalize
	{
public:
	virtual ~ZTupleIndexFactory();
	virtual ZTupleIndex* Make() = 0;
	};

} // namespace ZooLib

#endif // __ZTupleIndex__
