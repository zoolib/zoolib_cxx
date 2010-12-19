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

#ifndef __ZQE_Walker_Extend__
#define __ZQE_Walker_Extend__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/zqe/ZQE_Walker.h"
#include "zoolib/zra/ZRA_RelHead.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend

class Walker_Extend : public Walker_Unary
	{
public:
	Walker_Extend(ZRef<Walker> iWalker, const string8& iRelName);
	virtual ~Walker_Extend();

protected:
	const string8 fRelName;
	size_t fOutputOffset;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Calculate

class Walker_Extend_Calculate : public Walker_Extend
	{
public:
	Walker_Extend_Calculate(ZRef<Walker> iWalker, const string8& iRelName,
		ZRef<ZCallable<ZVal_Any(ZMap_Any)> > iCallable);
	virtual ~Walker_Extend_Calculate();

// From ZQE::Walker
	virtual void Prime(const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	const ZRef<ZCallable<ZVal_Any(ZMap_Any)> > fCallable;
	std::map<string8,size_t> fBindingOffsets;
	std::map<string8,size_t> fChildOffsets;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Rel

class Walker_Extend_Rel : public Walker_Extend
	{
public:
	Walker_Extend_Rel(ZRef<Walker> iWalker, const string8& iRelName,
		ZRef<Walker> iWalker_Ext);
	virtual ~Walker_Extend_Rel();

// From ZQE::Walker
	virtual void Prime(const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	const ZRef<Walker> fWalker_Ext;
	ZRA::RelHead fExtRelHead;
	std::vector<size_t> fExtOffsets;
	size_t fExtWidth;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Walker_Extend_Val

class Walker_Extend_Val : public Walker_Extend
	{
public:
	Walker_Extend_Val(ZRef<Walker> iWalker, const string8& iRelName,
		const ZVal_Any& iVal);
	virtual ~Walker_Extend_Val();

// From ZQE::Walker
	virtual void Prime(const std::map<string8,size_t>& iBindingOffsets, 
		std::map<string8,size_t>& oOffsets,
		size_t& ioBaseOffset);

	virtual bool ReadInc(const ZVal_Any* iBindings,
		ZVal_Any* oResults,
		std::set<ZRef<ZCounted> >* oAnnotations);

private:
	const ZVal_Any fVal;
	};

} // namespace ZQE
} // namespace ZooLib

#endif // __ZQE_Walker_Extend__
