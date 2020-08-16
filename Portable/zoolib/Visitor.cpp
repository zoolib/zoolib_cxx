// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Visitor.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitee

void Visitee::Accept(const Visitor& iVisitor)
	{ const_cast<Visitor&>(iVisitor).Visit(this); }

void sAccept(const ZP<Visitee>& iVisitee, const Visitor& iVisitor)
	{ iVisitee->Accept(iVisitor); }

// =================================================================================================
#pragma mark - Visitor

Visitor::~Visitor()
	{}

void Visitor::Visit(const ZP<Visitee>& iRep)
	{}

} // namespace ZooLib
