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

#include "zoolib/QueryEngine/Walker.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Walker

Walker::Walker()
	{
	fCalled_Rewind = 0;
	fCalled_QReadInc = 0;
	}

Walker::~Walker()
	{}

void Walker::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Walker* theVisitor = sDynNonConst<Visitor_Walker>(&iVisitor))
		this->Accept_Walker(*theVisitor);
	else
		Visitee::Accept(iVisitor);
	}

void Walker::Accept_Walker(Visitor_Walker& iVisitor)
	{ iVisitor.Visit_Walker(this); }

void Walker::Called_Rewind()
	{
	++fCalled_Rewind;
	}

void Walker::Called_QReadInc()
	{
	++fCalled_QReadInc;
	}

// =================================================================================================
#pragma mark - Visitor_Walker

void Visitor_Walker::Visit_Walker(const ZP<Walker>& iWalker)
	{ this->Visit(iWalker); }

// =================================================================================================
#pragma mark - Walker_Unary

Walker_Unary::Walker_Unary(const ZP<Walker>& iWalker)
:	fWalker(iWalker)
	{}

Walker_Unary::~Walker_Unary()
	{}

void Walker_Unary::Rewind()
	{
	this->Called_Rewind();
	fWalker->Rewind();
	}

} // namespace QueryEngine
} // namespace ZooLib
