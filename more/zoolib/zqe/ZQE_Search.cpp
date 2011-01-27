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

#include "zoolib/ZLog.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"
#include "zoolib/zqe/ZQE_Search.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;
using ZRA::RelHead;

// =================================================================================================
#pragma mark -
#pragma mark * sSearch

ZRef<Result> sSearch(ZRef<Walker> iWalker)
	{
	map<string8,size_t> offsets;
	size_t baseOffset = 0;
	iWalker = iWalker->Prime(map<string8,size_t>(), offsets, baseOffset);

	ZLOGF(s, eDebug);

	s << "\n";
	for (map<string8,size_t>::iterator i = offsets.begin(); i != offsets.end(); ++i)
		s << i->first << ": " << i->second << ", ";

	vector<ZVal_Any> thePackedRows;
	vector<vector<ZRef<ZCounted> > > theAnnotationsVector;
	vector<ZVal_Any> theRow(baseOffset, ZVal_Any());
	for (;;)
		{
		set<ZRef<ZCounted> > theAnnotations;
		if (!iWalker->ReadInc(&theRow[0], &theAnnotations))
			break;
		
		theAnnotationsVector.push_back(
			vector<ZRef<ZCounted> >(theAnnotations.begin(), theAnnotations.end()));
		
		for (map<string8,size_t>::iterator i = offsets.begin(); i != offsets.end(); ++i)
			thePackedRows.push_back(theRow[i->second]);

		s << "\n";
		for (map<string8,size_t>::iterator i = offsets.begin(); i != offsets.end(); ++i)
			{
			ZYad_ZooLibStrim::sToStrim(sMakeYadR(theRow[i->second]), s);
			s << ", ";
			}

		if (int theAnnoCount = theAnnotations.size())
			s << "annotations: " << theAnnoCount;
		}

	s << "\n";
	s.Emit();

	RelHead theRelHead;
	for (map<string8,size_t>::iterator i = offsets.begin(); i != offsets.end(); ++i)
		theRelHead.insert(i->first);

	return new ZQE::Result(theRelHead, &thePackedRows, &theAnnotationsVector);
	}

} // namespace ZQE
} // namespace ZooLib
