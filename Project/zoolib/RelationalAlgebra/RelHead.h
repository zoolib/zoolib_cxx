// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_RelHead_h__
#define __ZooLib_RelationalAlgebra_RelHead_h__ 1
#include "zconfig.h"

#include "zoolib/Compare.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/RelationalAlgebra/ColName.h"

#include <map>
#include <set>

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Rename

typedef std::map<ColName,ColName> Rename;

Rename sInverted(const Rename& iRename);

// =================================================================================================
#pragma mark - RelHead

typedef std::set<ColName> RelHead;

inline RelHead sRelHead()
	{ return RelHead(); }

inline RelHead sRelHead(const ColName* iElem, size_t iCount)
	{ return RelHead(iElem, iElem + iCount); }

inline RelHead sRelHead(const ColName& iElem)
	{ return sRelHead(&iElem, 1); }

inline RelHead sRelHead(const char* iElem)
	{ return sRelHead(ColName(iElem)); }

inline size_t sIndexOf(const RelHead& iRelHead, const ColName& iElem)
	{ return std::distance(iRelHead.begin(), iRelHead.find(iElem)); }

RelHead sRelHead(const ColName& i0, const ColName& i1);

RelHead sRelHead(const ColName& i0, const ColName& i1, const ColName& i2);

RelHead sRelHead(const ColName& i0, const ColName& i1, const ColName& i2, const ColName& i3);

RelHead sRelHead(const ColName& i0, const ColName& i1, const ColName& i2, const ColName& i3,
	const ColName& i4);

RelHead sRelHead(
	const ColName& i0, const ColName& i1, const ColName& i2, const ColName& i3,
	const ColName& i4, const ColName& i5);

RelHead sRelHead(
	const ColName& i0, const ColName& i1, const ColName& i2, const ColName& i3,
	const ColName& i4, const ColName& i5, const ColName& i6);

RelHead sRelHead(
	const ColName& i0, const ColName& i1, const ColName& i2, const ColName& i3,
	const ColName& i4, const ColName& i5, const ColName& i6, const ColName& i7);

// =================================================================================================
#pragma mark - RelHead operators

// Incorporate set operators from Util_STL

using Util_STL::operator&;
using Util_STL::operator&=;

using Util_STL::operator|;
using Util_STL::operator|=;

using Util_STL::operator^;
using Util_STL::operator^=;

using Util_STL::operator-;
using Util_STL::operator-=;

inline RelHead operator&(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead & RelHead::key_type(iElem); }

inline RelHead& operator&=(RelHead& ioRelHead, const char* iElem)
	{ return ioRelHead &= RelHead::key_type(iElem); }

inline RelHead operator&(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead & RelHead::key_type(iElem); }

inline RelHead operator|(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead | RelHead::key_type(iElem); }

inline RelHead& operator|=(RelHead& ioRelHead, const char* iElem)
	{ return ioRelHead |= RelHead::key_type(iElem); }

inline RelHead operator|(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead | RelHead::key_type(iElem); }

inline RelHead operator^(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead ^ RelHead::key_type(iElem); }

inline RelHead& operator^=(RelHead& ioRelHead, const char* iElem)
	{ return ioRelHead ^= RelHead::key_type(iElem); }

inline RelHead operator^(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead ^ RelHead::key_type(iElem); }

inline RelHead operator-(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead - RelHead::key_type(iElem); }

inline RelHead& operator-=(RelHead& ioRelHead, const char* iElem)
	{ return ioRelHead -= RelHead::key_type(iElem); }

// =================================================================================================
#pragma mark - 

bool sHasPrefix(const ColName& iPrefix, const ColName& iColName);
ColName sPrefixInserted(const ColName& iPrefix, const ColName& iColName);
ColName sPrefixErased(const ColName& iPrefix, const ColName& iColName);
ZQ<ColName> sQPrefixErased(const ColName& iPrefix, const ColName& iColName);
ColName sRenamed(const Rename& iRename, const ColName& iColName);

bool sHasPrefix(const ColName& iPrefix, const RelHead& iRelHead);
RelHead sPrefixInserted(const ColName& iPrefix, const RelHead& iRelHead);
RelHead sPrefixErased(const ColName& iPrefix, const RelHead& iRelHead);
ZQ<RelHead> sQPrefixErased(const ColName& iPrefix, const RelHead& iRelHead);
RelHead sRenamed(const Rename& iRename, const RelHead& iRelHead);

RelHead sNamesFrom(const Rename& iRename);
RelHead sNamesTo(const Rename& iRename);

// =================================================================================================
#pragma mark - ConcreteHead

typedef std::map<ColName,bool> ConcreteHead;

ConcreteHead sConcreteHead(const RelHead& iRequired);
ConcreteHead sConcreteHead(const RelHead& iRequired, const RelHead& iOptional);

ConcreteHead sAugmentedOptional(const ConcreteHead& iConcreteHead, const RelHead& iOptional);
ConcreteHead sAugmentedRequired(const ConcreteHead& iConcreteHead, const RelHead& iRequired);

RelHead sRelHead_Required(const ConcreteHead& iConcreteHead);
RelHead sRelHead_Optional(const ConcreteHead& iConcreteHead);
RelHead sRelHead(const ConcreteHead& iConcreteHead);

void sRelHeads(const ConcreteHead& iConcreteHead, RelHead& oRequired, RelHead& oOptional);

} // namespace RelationalAlgebra

// =================================================================================================
#pragma mark - sCompare_T declarations.

template <>
int sCompare_T(const RelationalAlgebra::RelHead& iL, const RelationalAlgebra::RelHead& iR);

template <>
int sCompare_T(const RelationalAlgebra::Rename& iL, const RelationalAlgebra::Rename& iR);

template <>
int sCompare_T(const RelationalAlgebra::ConcreteHead& iL, const RelationalAlgebra::ConcreteHead& iR);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_RelHead_h__
