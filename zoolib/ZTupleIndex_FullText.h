/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTupleIndex_FullText__
#define __ZTupleIndex_FullText__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

// FullText is dependent on ICU features.
#if ZCONFIG_SPI_Enabled(ICU)

#include "zoolib/ZTupleIndex.h"

#include <map>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndex_FullText

class ZTupleIndex_FullText : public ZTupleIndex
	{
public:
	ZTupleIndex_FullText(const ZTName& iPropName);

	virtual void Add(uint64 iID, const ZTuple* iTuple);

	virtual void Remove(uint64 iID, const ZTuple* iTuple);

	virtual void Find(const std::set<uint64>& iSkipIDs,
		std::vector<const ZTBSpec::Criterion*>& ioCriteria, std::vector<uint64>& oIDs);

	virtual size_t CanHandle(const ZTBSpec::CriterionSect& iCriterionSect);

	virtual void WriteDescription(const ZStrimW& s);

private:
	struct CPChunk
		{
		CPChunk(UTF32 iCPs[3]);
		CPChunk(UTF32 iCP0, UTF32 iCP1, UTF32 iCP2);
		
		bool operator<(const CPChunk& iOther) const;
		bool operator==(const CPChunk& iOther) const;
		bool operator!=(const CPChunk& iOther) const;

		const UTF32 fCP0;
		const UTF32 fCP1;
		const UTF32 fCP2;
		};

	static void sGetCPChunks(const string& iPattern, set<CPChunk>& oCPChunks);

	size_t fCountTuples;
	map<CPChunk, set<uint64> > fMap;

	ZTName fPropName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleIndexFactory_FullText

class ZTupleIndexFactory_FullText : public ZTupleIndexFactory
	{
public:
	ZTupleIndexFactory_FullText(const std::string& iPropName);
	virtual ZTupleIndex* Make();

private:
	std::string fPropName;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(ICU)

#endif // __ZTupleIndex_FullText__
