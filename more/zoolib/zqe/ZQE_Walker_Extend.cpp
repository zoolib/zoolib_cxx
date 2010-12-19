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

#include "zoolib/zqe/ZQE_Result.h"
#include "zoolib/zqe/ZQE_Walker_Extend.h"

namespace ZooLib {
namespace ZQE {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend

Walker_Extend::Walker_Extend(ZRef<Walker> iWalker, const string8& iRelName)
:	Walker_Unary(iWalker)
,	fRelName(iRelName)
	{}

Walker_Extend::~Walker_Extend()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Calculate

Walker_Extend_Calculate::Walker_Extend_Calculate(ZRef<Walker> iWalker, const string8& iRelName,
	ZRef<ZCallable<ZVal_Any(ZMap_Any)> > iCallable)
:	Walker_Extend(iWalker, iRelName)
,	fCallable(iCallable)
	{
	//## Need better API on the callable, so it can look up data in bindings (and output?)
	}

Walker_Extend_Calculate::~Walker_Extend_Calculate()
	{}

void Walker_Extend_Calculate::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	fBindingOffsets = iBindingOffsets;
	fWalker->Prime(iBindingOffsets, fChildOffsets, ioBaseOffset);
	oOffsets.insert(fChildOffsets.begin(), fChildOffsets.end());
	}

bool Walker_Extend_Calculate::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (!fWalker->ReadInc(iBindings, oResults, oAnnotations))
		return false;

	ZMap_Any theMap;
	for (map<string8,size_t>::iterator i = fBindingOffsets.begin(); i != fBindingOffsets.end(); ++i)
		theMap.Set(i->first, iBindings[i->second]);
	for (map<string8,size_t>::iterator i = fChildOffsets.begin(); i != fChildOffsets.end(); ++i)
		theMap.Set(i->first, oResults[i->second]);

	oResults[fOutputOffset] = fCallable->Call(theMap);
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Rel

Walker_Extend_Rel::Walker_Extend_Rel(ZRef<Walker> iWalker, const string8& iRelName,
	ZRef<Walker> iWalker_Ext)
:	Walker_Extend(iWalker, iRelName)
,	fWalker_Ext(iWalker_Ext)
,	fExtWidth(0)
	{}

Walker_Extend_Rel::~Walker_Extend_Rel()
	{}

void Walker_Extend_Rel::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	map<string8,size_t> childOffsets;
	fWalker->Prime(iBindingOffsets, childOffsets, ioBaseOffset);
	oOffsets.insert(childOffsets.begin(), childOffsets.end());

	map<string8,size_t> extOffsets;
	fWalker_Ext->Prime(childOffsets, extOffsets, fExtWidth); 

	fExtOffsets.reserve(extOffsets.size());
	for (map<string8,size_t>::iterator i = extOffsets.begin(); i != extOffsets.end(); ++i)
		{
		fExtRelHead |= i->first;
		fExtOffsets.push_back(i->second);
		}
	}

bool Walker_Extend_Rel::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (!fWalker->ReadInc(iBindings, oResults, oAnnotations))
		return false;
	
	fWalker_Ext->Rewind();

	vector<ZVal_Any> thePackedRows;
	vector<ZVal_Any> extStorage(fExtWidth, ZVal_Any());
	for (;;)
		{
		if (!fWalker_Ext->ReadInc(oResults, &extStorage[0], nullptr))
			break;

		for (vector<size_t>::iterator i = fExtOffsets.begin(); i != fExtOffsets.end(); ++i)
			thePackedRows.push_back(extStorage[*i]);
		}

	ZRef<ZQE::Result> theResult = new ZQE::Result(fExtRelHead, &thePackedRows, nullptr);
	oResults[fOutputOffset] = theResult;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Val

Walker_Extend_Val::Walker_Extend_Val(ZRef<Walker> iWalker, const string8& iRelName,
	const ZVal_Any& iVal)
:	Walker_Extend(iWalker, iRelName)
,	fVal(iVal)
	{}

Walker_Extend_Val::~Walker_Extend_Val()
	{}

void Walker_Extend_Val::Prime(const std::map<string8,size_t>& iBindingOffsets, 
	std::map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fOutputOffset = ioBaseOffset++;
	oOffsets[fRelName] = fOutputOffset;

	fWalker->Prime(iBindingOffsets, oOffsets, ioBaseOffset);
	}

bool Walker_Extend_Val::ReadInc(const ZVal_Any* iBindings,
	ZVal_Any* oResults,
	set<ZRef<ZCounted> >* oAnnotations)
	{
	if (!fWalker->ReadInc(iBindings, oResults, oAnnotations))
		return false;
	oResults[fOutputOffset] = fVal;
	return true;
	}

} // namespace ZQE
} // namespace ZooLib
