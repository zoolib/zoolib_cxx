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

#ifndef __ZooLib_QueryEngine_Walker_h__
#define __ZooLib_QueryEngine_Walker_h__ 1
#include "zconfig.h"

#include "zoolib/UnicodeString.h"
#include "zoolib/Val_Any.h"
#include "zoolib/Visitor.h"

#include <set>

namespace ZooLib {
namespace QueryEngine {

class Visitor_Walker;

// =================================================================================================
#pragma mark - Walker

class Walker : public Visitee
	{
protected:
	Walker();

public:
	virtual ~Walker();

// From Visitee
	virtual void Accept(const Visitor& iVisitor);

// Our protocol
	virtual void Accept_Walker(Visitor_Walker& iVisitor);

	virtual void Rewind() = 0;

	virtual ZP<Walker> Prime(
		const std::map<string8,size_t>& iOffsets,
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset) = 0;

	virtual bool QReadInc(Val_Any* ioResults) = 0;

protected:
	void Called_Rewind();
	void Called_QReadInc();

public:
	size_t fCalled_Rewind;
	size_t fCalled_QReadInc;
	};

// =================================================================================================
#pragma mark - Visitor_Walker

class Visitor_Walker
:	public virtual Visitor
	{
public:
// Our protocol
	virtual void Visit_Walker(const ZP<Walker>& iWalker);
	};

// =================================================================================================
#pragma mark - Walker_Unary

class Walker_Unary : public Walker
	{
protected:
	Walker_Unary(const ZP<Walker>& iWalker);

public:
	virtual ~Walker_Unary();

// From QueryEngine::Walker
	virtual void Rewind();

// Our protocol
	ZP<Walker> GetChild()
		{ return fWalker; }

protected:
	ZP<Walker> fWalker;
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Walker_h__
