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

#include "zoolib/zqe/ZQE_ResultSet.h"
#include "zoolib/zqe/ZQE_Walker_Extend.h"

namespace ZooLib {
namespace ZQE {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend

Walker_Extend::Walker_Extend(ZRef<Walker> iWalker, const string8& iRelName)
:	fWalker(iWalker)
,	fRelName(iRelName)
	{}
Walker_Extend::~Walker_Extend()
	{}

size_t Walker_Extend::NameCount()
	{ return fWalker->NameCount() + 1; }

string8 Walker_Extend::NameAt(size_t iIndex)
	{
	if (iIndex == 0)
		return fRelName;
	return fWalker->NameAt(iIndex - 1);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Calculate

Walker_Extend_Calculate::Walker_Extend_Calculate(ZRef<Walker> iWalker, const string8& iRelName,
	ZRef<ZCallable<ZVal_Any(ZMap_Any)> > iCallable)
:	Walker_Extend(iWalker, iRelName)
,	fCallable(iCallable)
	{}

Walker_Extend_Calculate::~Walker_Extend_Calculate()
	{}

ZRef<Walker> Walker_Extend_Calculate::Clone()
	{ return new Walker_Extend_Calculate(fWalker->Clone(), fRelName, fCallable); }

ZRef<Row> Walker_Extend_Calculate::ReadInc(ZMap_Any iBindings)
	{
	if (ZRef<Row, false> theRow = fWalker->ReadInc(iBindings))
		{ return null; }
	else
		{
		ZMap_Any theBindings = iBindings;
		for (size_t x = 0, count = fWalker->NameCount(); x < count; ++x)
			theBindings.Set(fWalker->NameAt(x), theRow->Get(x));
		return new Row_Pair(new Row_Val(fCallable->Call(theBindings)), theRow);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Rel

Walker_Extend_Rel::Walker_Extend_Rel(ZRef<Walker> iWalker, const string8& iRelName,
	ZRef<Walker> iWalker_Child_Model)
:	Walker_Extend(iWalker, iRelName)
,	fWalker_Child_Model(iWalker_Child_Model)
	{}

Walker_Extend_Rel::~Walker_Extend_Rel()
	{}

ZRef<Walker> Walker_Extend_Rel::Clone()
	{ return new Walker_Extend_Rel(fWalker->Clone(), fRelName, fWalker_Child_Model); }

ZRef<Row> Walker_Extend_Rel::ReadInc(ZMap_Any iBindings)
	{
	if (ZRef<Row, false> theRow = fWalker->ReadInc(iBindings))
		{ return null; }
	else
		{
		ZMap_Any theBindings = iBindings;
		for (size_t x = 0, count = fWalker->NameCount(); x < count; ++x)
			theBindings.Set(fWalker->NameAt(x), theRow->Get(x));

		ZRef<Walker> theWalker = fWalker_Child_Model->Clone();

		vector<string8> theRowHead;
		for (size_t x = 0, count = theWalker->NameCount(); x < count; ++x)
			theRowHead.push_back(theWalker->NameAt(x));

		vector<ZRef<Row> > theRows;
		for (ZRef<ZQE::Row> aRow; aRow = theWalker->ReadInc(theBindings); /*no inc*/)
			theRows.push_back(aRow);

		ZRef<ResultSet> theResultSet = new ResultSet(&theRowHead, new RowVector(&theRows));
		
		return new Row_Pair(new Row_Val(theResultSet), theRow);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Val

Walker_Extend_Val::Walker_Extend_Val(ZRef<Walker> iWalker, const string8& iRelName,
	ZRef<Row> iRow)
:	Walker_Extend(iWalker, iRelName)
,	fRow(iRow)
	{}
	
Walker_Extend_Val::Walker_Extend_Val(ZRef<Walker> iWalker, const string8& iRelName,
	const ZVal_Any& iVal)
:	Walker_Extend(iWalker, iRelName)
,	fRow(new Row_Val(iVal))
	{}

Walker_Extend_Val::~Walker_Extend_Val()
	{}

ZRef<Walker> Walker_Extend_Val::Clone()
	{ return new Walker_Extend_Val(fWalker, fRelName, fRow); }

ZRef<Row> Walker_Extend_Val::ReadInc(ZMap_Any iBindings)
	{
	if (ZRef<Row> theRow = fWalker->ReadInc(iBindings))
		return new Row_Pair(fRow, theRow);
	return null;
	}

} // namespace ZQE
} // namespace ZooLib
