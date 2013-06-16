/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTBQuery__
#define __ZTBQuery__ 1
#include "zconfig.h"

#include "zoolib/tuplebase/ZTBSpec.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery

class ZTBQueryNode;

/// Describes the tuple selection operations to be applied to a tuplebase or tuplesoup.

class ZTBQuery
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTBQuery,
		operator_bool_generator_type, operator_bool_type);
public:
	class SortSpec;

/** \name Canonical methods
*/	//@{
	/// Constructs empty query.
	ZTBQuery();

	/// Copy-construct from \a iOther.
	ZTBQuery(const ZTBQuery& iOther);

	/// Assign from \a iOther.
	ZTBQuery& operator=(const ZTBQuery& iOther);

	/// Destructor.
	~ZTBQuery();
	//@}

/** \name Conversion and assignment
*/	//@{
	/// Construct from tuple-ized representation.
	explicit ZTBQuery(const ZTuple& iTuple);

	/// Construct from serialized representation.
	explicit ZTBQuery(const ZStreamR& iStreamR);

	/// Return tuple-ized representation.
	ZTuple AsTuple() const;

	/// Write serialized representation.
	void ToStream(const ZStreamW& iStreamW) const;

	//@}

/** \name Constructors, from IDs, ZTBSpec, or using results of another query.
*/	//@{
	/// All tuples whose ID is \a iID (ie just the one).
	ZTBQuery(uint64 iID);

	/// All tuples whose IDs are in \a iIDs.
	ZTBQuery(const uint64* iIDs, size_t iCount);

	/// All tuples whose IDs are in \a iIDs.
	ZTBQuery(const std::vector<uint64>& iIDs);

	/// All tuples whose IDs are in \a iIDs.
	ZTBQuery(const std::set<uint64>& iIDs);

	/// All tuples which match iSpec.
	ZTBQuery(const ZTBSpec& iSpec);

	/** \brief All tuples whose ID matches values from \a iSourceQuery's
	property of type ID and name \a iSourcePropName. */
	ZTBQuery(const ZTBQuery& iSourceQuery, const ZTName& iSourcePropName);

	/** \brief All tuples whose property \a iPropName is of type ID and matches
	IDs of tuples from \a iSourceQuery. */
	ZTBQuery(const ZTName& iPropName, const ZTBQuery& iSourceQuery);
	//@}

/** \name Combining operators
*/	//@{
	ZTBQuery operator&(const ZTBSpec& iSpec) const;
	ZTBQuery& operator&=(const ZTBSpec& iSpec);

	ZTBQuery operator&(const ZTBQuery& iQuery) const;
	ZTBQuery& operator&=(const ZTBQuery& iQuery);

	ZTBQuery operator|(const ZTBQuery& iQuery) const;
	ZTBQuery& operator|=(const ZTBQuery& iQuery);
	//@}

/** \name Sorting and selection
*/	//@{
	/// Ascending or descending sort by \a iPropName, with comparator strength \a iStrength.
	ZTBQuery Sorted(const ZTName& iPropName, bool iAscending, int iStrength) const;

	/// Ascending or descending sort by \a iPropName, with comparator strength 0.
	ZTBQuery Sorted(const ZTName& iPropName, bool iAscending) const;

	/// Ascending sort by \a iPropName, with comparator strength 0.
	ZTBQuery Sorted(const ZTName& iPropName) const;

	/// Return only the first tuple for each unique value of \a iPropName.
	ZTBQuery First(const ZTName& iPropName) const;
	//@}

/** \name Comparison, to facilitate use as keys in associative containers.
*/	//@{
	operator operator_bool_type() const;

	int Compare(const ZTBQuery& iOther) const;

	bool operator==(const ZTBQuery& iOther) const;
	bool operator!=(const ZTBQuery& iOther) const;

	bool operator<(const ZTBQuery& iOther) const;
	bool operator<=(const ZTBQuery& iOther) const;
	bool operator>(const ZTBQuery& iOther) const;
	bool operator>=(const ZTBQuery& iOther) const;
	//@}

	ZRef<ZTBQueryNode> GetNode() const;

private:
	ZTBQuery(const ZRef<ZTBQueryNode>& iNode);
	ZTBQuery(ZTBQueryNode* iNode);

	ZRef<ZTBQueryNode> fNode;
	};

inline ZTBQuery operator&(const ZTBSpec& iSpec, const ZTBQuery& iQuery)
	{ return iQuery & iSpec; }

inline ZTBQuery operator|(const ZTBSpec& iSpec, const ZTBQuery& iQuery)
	{ return iQuery | iSpec; }


template <> inline int sCompare_T(const ZTBQuery& iL, const ZTBQuery& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery::SortSpec

/// Describes how a single property should be sorted.

class ZTBQuery::SortSpec
	{
private:
	SortSpec();

public:
	explicit SortSpec(const ZStreamR& iStreamR);

	/// Ascending sort by \a iPropName.
	SortSpec(const ZTName& iPropName)
	: fPropName(iPropName), fAscending(true), fStrength(0) {}

	/// Ascending or descending sort by \a iPropName.
	SortSpec(const ZTName& iPropName, bool iAscending)
	: fPropName(iPropName), fAscending(iAscending), fStrength(0) {}

	/// Ascending or descending sort by \a iPropName, with comparator strength \a iStrength.
	SortSpec(const ZTName& iPropName, bool iAscending, int iStrength)
	: fPropName(iPropName), fAscending(iAscending), fStrength(iStrength) {}

/** \name Comparison, to facilitate use as keys in associative containers.
*/	//@{
	int Compare(const SortSpec& iOther) const;
	bool operator<(const SortSpec& iOther) const;
	//@}

	void ToStream(const ZStreamW& iStreamW) const;

	ZTName fPropName;
	bool fAscending;
	int fStrength;
	};

template <>
inline int sCompare_T(const ZTBQuery::SortSpec& iL, const ZTBQuery::SortSpec& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode

class ZTBQueryNode_All;
class ZTBQueryNode_Combo;
class ZTBQueryNode_Difference;
class ZTBQueryNode_First;
class ZTBQueryNode_ID_Constant;
class ZTBQueryNode_ID_FromSource;
class ZTBQueryNode_Property;

class ZTBQueryNode : public ZCountedWithoutFinalize
	{
protected:
	ZTBQueryNode();

public:
	virtual ~ZTBQueryNode();

	int Compare(const ZRef<ZTBQueryNode>& iOther);

	virtual ZTuple AsTuple() = 0;
	virtual void ToStream(const ZStreamW& iStreamW) = 0;

	// These are not protected, to allow ZTBQueryNode subclasses
	// to invoke them. But they remain an implementation detail
	// and should not be used by external code.
	virtual int pRevCompare(ZTBQueryNode* iOther) = 0;
	virtual int pCompare_All(ZTBQueryNode_All* iAll) = 0;
	virtual int pCompare_Combo(ZTBQueryNode_Combo* iCombo) = 0;
	virtual int pCompare_Difference(ZTBQueryNode_Difference* iDifference) = 0;
	virtual int pCompare_First(ZTBQueryNode_First* iFirst) = 0;
	virtual int pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant) = 0;
	virtual int pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource) = 0;
	virtual int pCompare_Property(ZTBQueryNode_Property* iProperty) = 0;
	};

template <>
inline int sCompare_T(const ZRef<ZTBQueryNode>& iL, const ZRef<ZTBQueryNode>& iR)
	{ return iL->Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT

class ZTBQueryNode_GT : public ZTBQueryNode
	{
public:
	virtual int pCompare_All(ZTBQueryNode_All* iAll);
	virtual int pCompare_Combo(ZTBQueryNode_Combo* iCombo);
	virtual int pCompare_Difference(ZTBQueryNode_Difference* iDifference);
	virtual int pCompare_First(ZTBQueryNode_First* iFirst);
	virtual int pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant);
	virtual int pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource);
	virtual int pCompare_Property(ZTBQueryNode_Property* iProperty);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_All

class ZTBQueryNode_GT_All : public ZTBQueryNode_GT
	{
public:
	virtual int pCompare_All(ZTBQueryNode_All* iAll);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Combo

class ZTBQueryNode_GT_Combo : public ZTBQueryNode_GT_All
	{
public:
	virtual int pCompare_Combo(ZTBQueryNode_Combo* iCombo);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Difference

class ZTBQueryNode_GT_Difference : public ZTBQueryNode_GT_Combo
	{
public:
	virtual int pCompare_Difference(ZTBQueryNode_Difference* iDifference);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_First

class ZTBQueryNode_GT_First : public ZTBQueryNode_GT_Difference
	{
public:
	virtual int pCompare_First(ZTBQueryNode_First* iFirst);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_ID_Constant

class ZTBQueryNode_GT_ID_Constant : public ZTBQueryNode_GT_First
	{
public:
	virtual int pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_ID_FromSource

class ZTBQueryNode_GT_ID_FromSource : public ZTBQueryNode_GT_ID_Constant
	{
public:
	virtual int pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_GT_Property

class ZTBQueryNode_GT_Property : public ZTBQueryNode_GT_ID_FromSource
	{
public:
	virtual int pCompare_Property(ZTBQueryNode_Property* iProperty);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_All

class ZTBQueryNode_All : public ZTBQueryNode_GT
	{
public:
	ZTBQueryNode_All();
	explicit ZTBQueryNode_All(const ZStreamR& iStreamR);
	virtual ~ZTBQueryNode_All();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_All(ZTBQueryNode_All* iAll);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Combo

class ZTBQueryNode_Combo : public ZTBQueryNode_GT_All
	{
public:
	class Intersection;

	explicit ZTBQueryNode_Combo(const ZStreamR& iStreamR);

	ZTBQueryNode_Combo(
		std::vector<ZTBQuery::SortSpec>& ioSort, std::vector<Intersection>& ioIntersections);

	virtual ~ZTBQueryNode_Combo();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const std::vector<ZTBQuery::SortSpec>& GetSort() { return fSort; }
	const std::vector<Intersection>& GetIntersections() { return fIntersections; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_Combo(ZTBQueryNode_Combo* iCombo);

private:
	std::vector<ZTBQuery::SortSpec> fSort;
	std::vector<Intersection> fIntersections;
	};

class ZTBQueryNode_Combo::Intersection
	{
public:
	explicit Intersection(const ZStreamR& iStreamR);

	Intersection() {}
	Intersection(const ZTuple& iTuple);
	Intersection(const ZRef<ZTBQueryNode>& iNode)
	:	fFilter(ZTBSpec::sAny()), fNodes(1, iNode) {}

	Intersection(const ZTBSpec& iFilter, const ZRef<ZTBQueryNode>& iNode)
	:	fFilter(iFilter), fNodes(1, iNode) {}

	Intersection(const ZTBSpec& iFilter, const std::vector<ZRef<ZTBQueryNode> >& iNodes)
	:	fFilter(iFilter), fNodes(iNodes) {}

	ZTuple AsTuple() const;
	void ToStream(const ZStreamW& iStreamW) const;

	int Compare(const Intersection& iOther) const;

	ZTBSpec fFilter;
	std::vector<ZRef<ZTBQueryNode> > fNodes;
	};

template <>
inline int sCompare_T(
	const ZTBQueryNode_Combo::Intersection& iL, const ZTBQueryNode_Combo::Intersection& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Difference

class ZTBQueryNode_Difference : public ZTBQueryNode_GT_Combo
	{
public:
	explicit ZTBQueryNode_Difference(const ZStreamR& iStreamR);

	ZTBQueryNode_Difference(const ZRef<ZTBQueryNode>& iLeftNode, const ZRef<ZTBQueryNode>& iRightNode);
	virtual ~ZTBQueryNode_Difference();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const ZRef<ZTBQueryNode>& GetLeftNode() { return fLeftNode; }
	const ZRef<ZTBQueryNode>& GetRightNode() { return fRightNode; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_Difference(ZTBQueryNode_Difference* iDifference);

private:
	ZRef<ZTBQueryNode> fLeftNode;
	ZRef<ZTBQueryNode> fRightNode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_First

class ZTBQueryNode_First : public ZTBQueryNode_GT_Difference
	{
public:
	explicit ZTBQueryNode_First(const ZStreamR& iStreamR);

	ZTBQueryNode_First(const ZTName& iPropName, const ZRef<ZTBQueryNode>& iSourceNode);
	virtual ~ZTBQueryNode_First();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const ZTName& GetPropName() { return fPropName; }
	const ZRef<ZTBQueryNode>& GetSourceNode() { return fSourceNode; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_First(ZTBQueryNode_First* iFirst);

private:
	ZTName fPropName;
	ZRef<ZTBQueryNode> fSourceNode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_ID_Constant

class ZTBQueryNode_ID_Constant : public ZTBQueryNode_GT_First
	{
public:
	explicit ZTBQueryNode_ID_Constant(const ZStreamR& iStreamR);

	ZTBQueryNode_ID_Constant(uint64 iID);
	ZTBQueryNode_ID_Constant(const std::set<uint64>& iIDs);
	ZTBQueryNode_ID_Constant(const std::vector<uint64>& iIDs);
	ZTBQueryNode_ID_Constant(std::vector<uint64>& ioIDs, bool iKnowWhatImDoing);
	ZTBQueryNode_ID_Constant(const uint64* iIDs, size_t iCount);

	virtual ~ZTBQueryNode_ID_Constant();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const std::vector<uint64>& GetIDs() { return fIDs; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_ID_Constant(ZTBQueryNode_ID_Constant* iID_Constant);

private:
	std::vector<uint64> fIDs;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_ID_FromSource

class ZTBQueryNode_ID_FromSource : public ZTBQueryNode_GT_ID_Constant
	{
public:
	explicit ZTBQueryNode_ID_FromSource(const ZStreamR& iStreamR);

	ZTBQueryNode_ID_FromSource(
		const ZRef<ZTBQueryNode>& iSourceNode, const ZTName& iSourcePropName);

	virtual ~ZTBQueryNode_ID_FromSource();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const ZRef<ZTBQueryNode>& GetSourceNode() { return fSourceNode; }
	const ZTName& GetSourcePropName() { return fSourcePropName; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_ID_FromSource(ZTBQueryNode_ID_FromSource* iID_FromSource);

private:
	// Field order is significant.
	ZTName fSourcePropName;
	ZRef<ZTBQueryNode> fSourceNode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQueryNode_Property

class ZTBQueryNode_Property : public ZTBQueryNode_GT_ID_FromSource
	{
public:
	explicit ZTBQueryNode_Property(const ZStreamR& iStreamR);

	ZTBQueryNode_Property(const ZTName& iPropName, const ZRef<ZTBQueryNode>& iSourceNode);
	virtual ~ZTBQueryNode_Property();

// From ZTBQueryNode
	virtual ZTuple AsTuple();
	virtual void ToStream(const ZStreamW& iStreamW);

// Our protocol
	const ZTName& GetPropName() { return fPropName; }
	const ZRef<ZTBQueryNode>& GetSourceNode() { return fSourceNode; }

	virtual int pRevCompare(ZTBQueryNode* iOther);
	virtual int pCompare_Property(ZTBQueryNode_Property* iProperty);

private:
	// Field order is significant.
	ZTName fPropName;
	ZRef<ZTBQueryNode> fSourceNode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBQuery inlines

inline ZTBQuery::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fNode); }

inline int ZTBQuery::Compare(const ZTBQuery& iOther) const
	{
	ZTBQueryNode* otherNode = iOther.fNode.Get();

	if (ZTBQueryNode* myNode = fNode.Get())
		{
		if (otherNode)
			return - myNode->pRevCompare(otherNode);
		return 1;
		}
	else if (otherNode)
		{
		return -1;
		}

	return 0;
	}

inline bool ZTBQuery::operator==(const ZTBQuery& iOther) const
	{
	ZTBQueryNode* otherNode = iOther.fNode.Get();

	if (ZTBQueryNode* myNode = fNode.Get())
		{
		if (otherNode)
			return 0 == myNode->pRevCompare(otherNode);
		return false;
		}
	else if (otherNode)
		{
		return false;
		}

	return true;
	}

inline bool ZTBQuery::operator!=(const ZTBQuery& iOther) const
	{ return !(*this == iOther); }

inline bool ZTBQuery::operator<(const ZTBQuery& iOther) const
	{
	ZTBQueryNode* otherNode = iOther.fNode.Get();

	if (ZTBQueryNode* myNode = fNode.Get())
		{
		if (otherNode)
			return 0 < myNode->pRevCompare(otherNode);
		return false;
		}
	else if (otherNode)
		{
		return true;
		}

	return false;
	}

inline bool ZTBQuery::operator<=(const ZTBQuery& iOther) const
	{ return !(iOther < *this); }

inline bool ZTBQuery::operator>(const ZTBQuery& iOther) const
	{ return iOther < *this; }

inline bool ZTBQuery::operator>=(const ZTBQuery& iOther) const
	{ return !(*this < iOther); }

} // namespace ZooLib

#endif // __ZTBQuery__
