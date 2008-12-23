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

#include "zoolib/ZTQLEngine.h"

#include "zoolib/ZStrim.h"

NAMESPACE_ZOOLIB_USING

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTQLEngine::Iterator

ZTQLEngine::Iterator::Iterator()
	{}

ZTQLEngine::Iterator::~Iterator()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace ZANONYMOUS {

class Iterator_Explicit : public ZTQLEngine::Iterator
	{
public:
	Iterator_Explicit(ZRef<ZTQL::Node_Explicit> iNode);

	virtual bool HasValue();
	virtual ZTuple Current();
	virtual void Advance();

private:
	ZRef<ZTQL::Node_Explicit> fNode;

	vector<ZTuple>::const_iterator fCurrent;
	vector<ZTuple>::const_iterator fEnd;
	};

Iterator_Explicit::Iterator_Explicit(ZRef<ZTQL::Node_Explicit> iNode)
:	fNode(iNode),
	fCurrent(iNode->GetTuples().begin()),
	fEnd(iNode->GetTuples().end())
	{}

bool Iterator_Explicit::HasValue()
	{ return fCurrent != fEnd; }

ZTuple Iterator_Explicit::Current()
	{ return *fCurrent; }

void Iterator_Explicit::Advance()
	{ ++fCurrent; }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTQLEngine

ZTQLEngine::ZTQLEngine()
	{}

// oResults probably needs to be a more sophisicated structure -- a tree, each
// path from leaf to root of which constitutes a single result.

void ZTQLEngine::Execute(ZRef<ZTQL::Node> iNode, vector<ZTuple>& oResults)
	{
	this->pExecute(iNode, oResults);
	}

void ZTQLEngine::pExecute(ZRef<ZTQL::Node> iNode, vector<ZTuple>& oResults)
	{
	if (ZRef<ZTQL::Node_Explicit> theNode_Explicit = ZRefDynamicCast<ZTQL::Node_Explicit>(iNode))
		{
		oResults = theNode_Explicit->GetTuples();
		}
	}
