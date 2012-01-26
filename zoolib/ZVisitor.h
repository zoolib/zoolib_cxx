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

#ifndef __ZVisitor_h__
#define __ZVisitor_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZVisitee

class ZVisitor;

class ZVisitee : public virtual ZCounted
	{
public:
// Our protocol
	virtual void Accept(const ZVisitor& iVisitor) = 0;
	};

// =================================================================================================
// MARK: - ZVisitor

class ZVisitor
	{
public:
	virtual ~ZVisitor();
	virtual void Visit(const ZRef<ZVisitee>& iRep);
	};

} // namespace ZooLib

#endif // __ZVisitor_h__
