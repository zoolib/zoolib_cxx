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

#ifndef __ZTupleQuisitioner__
#define __ZTupleQuisitioner__
#include "zconfig.h"

#include "zoolib/tuplebase/ZTBQuery.h"

#include "zoolib/ZTextCollator.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {

class ZTupleQuisitioner
	{
public:
	virtual void Search(const ZTBSpec& iSpec, std::set<uint64>& ioIDs) = 0;
	virtual void Search(const ZTBSpec& iSpec, std::vector<uint64>& oIDs) = 0;

	virtual void FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples) = 0;
	virtual ZTuple FetchTuple(uint64 iID) = 0;


	void Query(const ZRef<ZTBQueryNode>& iNode, const ZTBSpec* iFilter, std::vector<uint64>& ioIDs);

	void Query_Unordered(const ZRef<ZTBQueryNode>& iNode,
		const ZTBSpec* iFilter, std::set<uint64>& ioIDs);

	void Query_Combo_Unordered(const std::vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
		const ZTBSpec* iFilter, std::set<uint64>& ioIDs);

	void Query_Combo(const std::vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
		const ZTBSpec* iFilter, std::vector<uint64>& ioIDs);

	void Query_Combo_Sorted(const std::vector<ZTBQuery::SortSpec>& iSort,
		const std::vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
		const ZTBSpec* iFilter, std::vector<uint64>& ioIDs);

	void Query_Combo_First(const ZTName& iFirst, const std::vector<ZTBQuery::SortSpec>& iSort,
		const std::vector<ZTBQueryNode_Combo::Intersection>& iIntersections,
		const ZTBSpec* iFilter, std::vector<uint64>& ioIDs);

	void Query_Intersection_Unordered(const ZTBQueryNode_Combo::Intersection& iIntersection,
		const ZTBSpec* iFilter, std::set<uint64>& ioIDs);

	void Query_Intersection(const ZTBQueryNode_Combo::Intersection& iIntersection,
		const ZTBSpec* iFilter, std::vector<uint64>& oIDs);

private:
	ZTextCollator fTextCollators[4];
	};

} // namespace ZooLib

#endif // __ZTupleQuisitioner__
