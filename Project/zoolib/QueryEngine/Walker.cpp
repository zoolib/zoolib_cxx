// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

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
