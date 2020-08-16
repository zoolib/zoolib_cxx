// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Walker_h__
#define __ZooLib_QueryEngine_Walker_h__ 1
#include "zconfig.h"

#include "zoolib/UnicodeString.h"
#include "zoolib/Val_DB.h"
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

	virtual bool QReadInc(Val_DB* ioResults) = 0;

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
