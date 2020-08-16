// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Visitor_h__
#define __ZooLib_Visitor_h__ 1
#include "zconfig.h"

#include "zoolib/Counted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitee

class Visitor;

class Visitee
:	public virtual Counted
	{
public:
// Our protocol
	virtual void Accept(const Visitor& iVisitor) = 0;
	};

void sAccept(const ZP<Visitee>& iVisitee, const Visitor& iVisitor);

// =================================================================================================
#pragma mark - Visitor

class Visitor
	{
public:
	virtual ~Visitor();
	virtual void Visit(const ZP<Visitee>& iRep);
	};

} // namespace ZooLib

#endif // __ZooLib_Visitor_h__
