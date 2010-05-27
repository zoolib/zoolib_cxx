/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTupleIndex_General__
#define __ZTupleIndex_General__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTupleIndex.h"

#include <set>
#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_General

class ZTupleIndex_General : public ZTupleIndex
	{
public:
	static const int kMaxProperties = 4;

	ZTupleIndex_General(const std::string* iPropNames, size_t iPropNameCount);

	virtual void Insert(uint64 iID, const ZTuple* iTuple);

	virtual void Erase(uint64 iID, const ZTuple* iTuple);

	virtual void Find(const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs);

	virtual size_t CanHandle(const ZTBSpec::CriterionSect& iCriterionSect);

	virtual void WriteDescription(const ZStrimW& s);

private:
	class Key
		{
	public:
		bool operator<(const Key& iOther) const;

		uint64 fID;
		const ZTValue* fValues[kMaxProperties];
		size_t fPropCount;
		};

	bool pSetupIterators(std::vector<const ZTBSpec::Criterion*>& oUncheckedCriteria,
		std::set<Key>::const_iterator& oLowerBound, std::set<Key>::const_iterator& oUpperBound);

	size_t pCanHandle(const ZTBSpec::CriterionSect& iCriterionSect, bool& oConsistent);

	bool pKeyFromTuple(uint64 iID, const ZTuple* iTuple, Key& oKey);

	ZTName fPropNames[kMaxProperties];
	size_t fPropNameCount;
	std::set<Key> fSet;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_General

class ZTupleIndexFactory_General : public ZTupleIndexFactory
	{
public:
	ZTupleIndexFactory_General(const std::vector<std::string>& iPropNames);
	virtual ZTupleIndex* Make();

private:
	std::string fPropNames[ZTupleIndex_General::kMaxProperties];
	size_t fPropNameCount;
	};

} // namespace ZooLib

#endif // __ZTupleIndex_General__
