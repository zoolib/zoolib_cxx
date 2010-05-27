/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTupleIndex_String__
#define __ZTupleIndex_String__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTupleIndex.h"
#include "zoolib/ZTextCollator.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_String

class ZTupleIndex_String : public ZTupleIndex
	{
public:
	ZTupleIndex_String(const ZTName& iPropName);

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
		bool operator==(const Key& iOther) const;
		bool operator!=(const Key& iOther) const
			{ return !(*this == iOther); }

		string16 fValue;
		uint64 fID;
		};

	class Compare
		{
	public:
		typedef Key value_type;

		Compare();

		bool operator()(const value_type& iLeft, const value_type& iRight) const;

		ZTextCollator fTextCollator;
		};

	bool pKeyFromTuple(uint64 iID, const ZTuple* iTuple, Key& oKey);

	ZTName fPropName;
	std::set<Key, Compare> fSet;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_String

class ZTupleIndexFactory_String : public ZTupleIndexFactory
	{
public:
	ZTupleIndexFactory_String(const std::string& iPropName);
	virtual ZTupleIndex* Make();

private:
	std::string fPropName;
	};

} // namespace ZooLib

#endif // __ZTupleIndex_String__
