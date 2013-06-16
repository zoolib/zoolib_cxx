/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTBSpec__
#define __ZTBSpec__ 1
#include "zconfig.h"

#include "zoolib/ZTextCollator.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZRegex.h"

#include <set>
#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec

/// Represents criteria to be matched against tuples.

class ZTBSpec
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTBSpec,
		operator_bool_generator_type, operator_bool_type);
public:
	class Comparator;
	class Criterion;
	class Rep;

	/// List of Criterion, all of which must match a tuple.
	typedef std::vector<Criterion> CriterionSect;

	/// List of CriterionSect, one or more of which must match a tuple.
	typedef std::vector<CriterionSect> CriterionUnion;

	/// Enum for designation of a relationship.
	enum ERel
		{
		eRel_Invalid,
		eRel_Less,
		eRel_LessEqual,
		eRel_Equal,
		eRel_GreaterEqual,
		eRel_Greater,
		eRel_Has,
		eRel_HasOfType,
		eRel_Lacks,
		eRel_LacksOfType,
		eRel_VectorContains,
		eRel_StringContains,
		eRel_Regex
		};

/** \name Canonical methods
*/	//@{
	/// Constructs a spec that will match no tuple.
	ZTBSpec();

	/// Copy-construct from \a iOther.
	ZTBSpec(const ZTBSpec& iOther);

	/// Assign from \a iOther.
	ZTBSpec& operator=(const ZTBSpec& iOther);

	/// Destructor.
	~ZTBSpec();
	//@}


/** \name Constructors
*/	//@{
	/// Construct a spec that matches any (\a iAny == true) or no tuple.
	explicit ZTBSpec(bool iAny);

	/// Construct a spec which appropriately matches a single property of a tuple.
	explicit ZTBSpec(const Criterion& iCriterion);
	ZTBSpec(const ZTName& iPropName, ERel iRel, const ZTValue& iTV);
	ZTBSpec(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV);
	ZTBSpec(const ZTName& iPropName, Comparator iComparator, const ZTValue& iTV);

	/// Directly construct a spec which checks an intersection of criteria.
	explicit ZTBSpec(const CriterionSect& iCriterionSect);
	//@}


/** \name Conversion and assignment
*/	//@{
	/// Construct from tuple-ized representation.
	explicit ZTBSpec(const ZTuple& iTuple);

	/// Construct from serialized representation.
	explicit ZTBSpec(const ZStreamR& iStreamR);

	/// Return tuple-ized representation.
	ZTuple AsTuple() const;

	/// Write serialized representation.
	void ToStream(const ZStreamW& iStreamW) const;
	//@}


/** \name Comparison, to facilitate use as keys in associative containers.
*/	//@{
	operator operator_bool_type() const;

	int Compare(const ZTBSpec& iOther) const;

	bool operator==(const ZTBSpec& iOther) const;
	bool operator!=(const ZTBSpec& iOther) const;

	bool operator<(const ZTBSpec& iOther) const;
	bool operator<=(const ZTBSpec& iOther) const;
	bool operator>(const ZTBSpec& iOther) const;
	bool operator>=(const ZTBSpec& iOther) const;
	//@}


/** \name Combining operators
*/	//@{
	/// Return a ZTBSpec being the intersection of this and \a iOther.
	ZTBSpec operator&(const ZTBSpec& iOther) const;

	/// Update this to be the intersection of this and \a iOther.
	ZTBSpec& operator&=(const ZTBSpec& iOther);

	/// Return a ZTBSpec being the union of this and \a iOther.
	ZTBSpec operator|(const ZTBSpec& iOther) const;

	/// Update this to be the union of this and \a iOther.
	ZTBSpec& operator|=(const ZTBSpec& iOther);
	//@}

	/// Return true if the spec matches \a iTuple.
	bool Matches(const ZTuple& iTuple) const;

	/// Returns true if the spec will match any tuple.
	bool IsAny() const;

	/// Returns true if the spec will match no tuple.
	bool IsNone() const;


/** \name Access to underlying information
*/	//@{
	const CriterionUnion& GetCriterionUnion() const;

	void GetPropNames(std::set<std::string>& oNames) const;
	void GetPropNames(std::set<ZTName>& oNames) const;
	//@}


/** \name Pseudo constructors
Each returns a ZTBSpec that will match tuples satisfying the desired constraint.
They're basically more readable wrappers for the ZTBSpec constructor taking
a property name, comparator and ZTValue. Because ZTValue has conversion constructors
taking the various supported types we don't suffer combinatorial explosion.
*/	//@{
	/// Any tuple.
	static ZTBSpec sAny();

	/// No tuple.
	static ZTBSpec sNone();

	/// Tuples which have a property \a iPropName.
	static ZTBSpec sHas(const ZTName& iPropName);

	/// Tuples which have a property \a iPropName of type \a iType.
	static ZTBSpec sHas(const ZTName& iPropName, ZType iType);

	/// Tuples which do not have a property \a iPropName.
	static ZTBSpec sLacks(const ZTName& iPropName);

	/// Tuples which do not have a property \a iPropName or it is not of type \a iType.
	static ZTBSpec sLacks(const ZTName& iPropName, ZType iType);

	/// Tuples whose property \a iPropName has the value \a iValue.
	static ZTBSpec sEquals(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose property \a iPropName is not the value \a iValue.
	static ZTBSpec sNotEqual(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose property \a iPropName is less than the value \a iValue.
	static ZTBSpec sLess(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose property \a iPropName is less than or equal to the value \a iValue.
	static ZTBSpec sLessEqual(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose property \a iPropName is greater than the value \a iValue.
	static ZTBSpec sGreater(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose property \a iPropName is greater than or equal to the value \a iValue.
	static ZTBSpec sGreaterEqual(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose string property \a iPropName contains \a iString.
	static ZTBSpec sStringContains(
		const ZTName& iPropName, const std::string& iString, int iStrength);

	/// Tuples whose vector property \a iPropName contains a value matching \a iValue.
	static ZTBSpec sVectorContains(const ZTName& iPropName, const ZTValue& iValue);

	/// Tuples whose string property \a iPropName matches the regex \a iRegex.
	static ZTBSpec sRegex(const ZTName& iPropName, const std::string& iRegex, int iStrength);
	//@}

private:
	ZTBSpec(CriterionUnion& ioCriterionUnion, bool iKnowWhatIAmDoing);

	void MakeFresh();
	ZRef<Rep> fRep;
	};

inline bool ZTBSpec::operator==(const ZTBSpec& iOther) const
	{ return this->Compare(iOther) == 0; }

inline bool ZTBSpec::operator<(const ZTBSpec& iOther) const
	{ return this->Compare(iOther) < 0; }

template <>
inline int sCompare_T(const ZTBSpec& iL, const ZTBSpec& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Comparator

/// Holds a <ZTBSpec::ERel, strength> pair.

class ZTBSpec::Comparator
	{
public:
	explicit Comparator(const ZStreamR& iStreamR);
	Comparator();

	/// Comparator with relationship \a iRel and strength \a iStrength.
	Comparator(ERel iRel, int iStrength);

	/// Comparator with relationship \a iRel and strength 0.
	Comparator(ERel iRel);

/** \name Comparison, to facilitate use as keys in associative containers.
*/	//@{
	int Compare(const Comparator& iOther) const;
	bool operator<(const Comparator& iOther) const;
	//@}

	void ToStream(const ZStreamW& iStreamW) const;

	/// The relationship.
	ERel fRel;

	/// The strength to be used in applying that relationship.
	int fStrength;
	};

inline int ZTBSpec::Comparator::Compare(const Comparator& iOther) const
	{
	if (int compare = fRel - iOther.fRel)
		return compare;

	return fStrength - iOther.fStrength;
	}

inline bool ZTBSpec::Comparator::operator<(const Comparator& iOther) const
	{
	return fRel < iOther.fRel || (fRel == iOther.fRel && fStrength < iOther.fStrength);
	}

template <>
inline int sCompare_T(const ZTBSpec::Comparator& iL, const ZTBSpec::Comparator& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Criterion

/// Holds a <property name, ZTBSpec::Comparator, ZTValue> triple.

class ZTBSpec::Criterion
	{
private:
	// An unitialized Criterion has no meaning.
	Criterion();

public:
	explicit Criterion(const ZTuple& iTuple);
	explicit Criterion(const ZStreamR& iStreamR);

	/// The most general constructor, taking explicit property name, comparator and value.
	Criterion(const ZTName& iPropName, const Comparator& iComparator, const ZTValue& iTV);

	/// Equivalent to Criterion(iPropName, ZTBSpec::eRel_Has).
	Criterion(const ZTName& iPropName);

	/// Equivalent to Criterion(iPropName, ZTBSpec::eRel_HasofType, iType).
	Criterion(const ZTName& iPropName, ZType iType);

	/// Equivalent to Criterion(iPropName, ZTBSpec::Comparator(iRel), iTV).
	Criterion(const ZTName& iPropName, ERel iRel, const ZTValue& iTV);

	/// Equivalent to Criterion(iPropName, ZTBSpec::Comparator(iRel, iStrength), iTV).
	Criterion(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV);

	/// Returns a tuple-ized form of the Criterion.
	ZTuple AsTuple() const;

	void ToStream(const ZStreamW& iStreamW) const;

	/// Return true if iTuple satisfies the criterion.
	bool Matches(const ZTuple& iTuple) const;

/** \name Comparison, to facilitate use as keys in associative containers.
*/	//@{
	int Compare(const Criterion& iOther) const;
	bool operator<(const Criterion& iOther) const;
	//@}

	const ZTName& GetPropName() const;
	const Comparator& GetComparator() const;
	const ZTValue& GetTValue() const;

private:
	class Rep;
	ZRef<Rep> fRep;
	};

template <>
inline int sCompare_T(const ZTBSpec::Criterion& iL, const ZTBSpec::Criterion& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Criterion::Rep

class ZTBSpec::Criterion::Rep : public ZCountedWithoutFinalize
	{
private:
	Rep();

public:
	static bool sCheckAccessEnabled() { return false; }

	explicit Rep(const ZTuple& iTuple);
	explicit Rep(const ZStreamR& iStreamR);

	Rep(const ZTName& iPropName, const Comparator& iComparator, const ZTValue& iTV);
	Rep(const ZTName& iPropName);
	Rep(const ZTName& iPropName, ZType iType);
	Rep(const ZTName& iPropName, ERel iRel, const ZTValue& iTV);
	Rep(const ZTName& iPropName, ERel iRel, int iStrength, const ZTValue& iTV);

	ZTuple AsTuple() const;
	void ToStream(const ZStreamW& iStreamW) const;

private:
	friend class Criterion;

	// It is essential that these fields remain in this order,
	// as they may be initialized from a stream.

	ZTName fPropName;
	Comparator fComparator;
	ZTValue fTValue;

	#if ZCONFIG_API_Enabled(Regex)
		mutable ZRegex fRegex;
	#endif
	mutable ZTextCollator fTextCollator;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec inlines

inline ZTBSpec::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(!this->IsNone()); }

inline bool ZTBSpec::operator!=(const ZTBSpec& iOther) const
	{ return !(*this == iOther); }

inline bool ZTBSpec::operator<=(const ZTBSpec& iOther) const
	{ return !(iOther < *this); }

inline bool ZTBSpec::operator>(const ZTBSpec& iOther) const
	{ return iOther < *this; }

inline bool ZTBSpec::operator>=(const ZTBSpec& iOther) const
	{ return !(*this < iOther); }

inline ZTBSpec ZTBSpec::sAny()
	{ return ZTBSpec(true); }

inline ZTBSpec ZTBSpec::sNone()
	{ return ZTBSpec(); }

inline ZTBSpec ZTBSpec::sHas(const ZTName& iPropName)
	{ return ZTBSpec(iPropName, eRel_Has, ZTValue()); }

inline ZTBSpec ZTBSpec::sHas(const ZTName& iPropName, ZType iType)
	{ return ZTBSpec(iPropName, eRel_HasOfType, iType); }

inline ZTBSpec ZTBSpec::sLacks(const ZTName& iPropName)
	{ return ZTBSpec(iPropName, eRel_Lacks, ZTValue()); }

inline ZTBSpec ZTBSpec::sLacks(const ZTName& iPropName, ZType iType)
	{ return ZTBSpec(iPropName, eRel_LacksOfType, iType); }

inline ZTBSpec ZTBSpec::sEquals(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_Equal, iValue); }

inline ZTBSpec ZTBSpec::sNotEqual(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_Less, iValue) | ZTBSpec(iPropName, eRel_Greater, iValue); }

inline ZTBSpec ZTBSpec::sLess(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_Less, iValue); }

inline ZTBSpec ZTBSpec::sLessEqual(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_LessEqual, iValue); }

inline ZTBSpec ZTBSpec::sGreater(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_Greater, iValue); }

inline ZTBSpec ZTBSpec::sGreaterEqual(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_GreaterEqual, iValue); }

inline ZTBSpec ZTBSpec::sStringContains(
	const ZTName& iPropName, const std::string& iString, int iStrength)
	{ return ZTBSpec(iPropName, eRel_StringContains, iStrength, iString); }

inline ZTBSpec ZTBSpec::sVectorContains(const ZTName& iPropName, const ZTValue& iValue)
	{ return ZTBSpec(iPropName, eRel_VectorContains, iValue); }

inline ZTBSpec ZTBSpec::sRegex(
	const ZTName& iPropName, const std::string& iRegex, int iStrength)
	{ return ZTBSpec(iPropName, eRel_Regex, iStrength, iRegex); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBSpec::Criterion inlines

inline bool ZTBSpec::Criterion::operator<(const Criterion& iOther) const
	{ return this->Compare(iOther) < 0; }

inline const ZTName& ZTBSpec::Criterion::GetPropName() const
	{ return fRep->fPropName; }

inline const ZTBSpec::Comparator& ZTBSpec::Criterion::GetComparator() const
	{ return fRep->fComparator; }

inline const ZTValue& ZTBSpec::Criterion::GetTValue() const
	{ return fRep->fTValue; }

} // namespace ZooLib

#endif // __ZTBSpec__
