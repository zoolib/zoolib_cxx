// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Any_T_h__
#define __ZooLib_Any_T_h__
#include "zconfig.h"

#include "zoolib/AnyBase.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Any_T

template <class Tag_p>
class Any_T : public AnyBase
	{
public:
	typedef Tag_p Tag_T;

	Any_T()
		{}

	Any_T(const Any_T& iOther)
	:	AnyBase((const AnyBase&)iOther)
		{}

	~Any_T()
		{}

	Any_T& operator=(const Any_T& iOther)
		{
		AnyBase::operator=((const AnyBase&)iOther);
		return *this;
		}

	Any_T(const null_t&)
		{}

	Any_T& operator=(const null_t&)
		{
		this->Clear();
		return *this;
		}

// Disallow construction and assignment from AnyBase derivatives with a different Tag.
    template <typename S,
    	typename EnableIfC<not std::is_base_of<AnyBase,S>::value, int>::type = 0>
	Any_T(const S& iVal)
	:	AnyBase(iVal)
		{}

    template <typename S,
    	typename EnableIfC<not std::is_base_of<AnyBase,S>::value, int>::type = 0>
	Any_T& operator=(const S& iVal)
		{
		AnyBase::operator=(iVal);
		return *this;
		}

	template <class OtherAny>
	typename EnableIfC<std::is_base_of<AnyBase,OtherAny>::value,const OtherAny&>::type
	As() const
		{ return *static_cast<const OtherAny*>(static_cast<const AnyBase*>(this)); }

	template <class OtherAny>
	typename EnableIfC<std::is_base_of<AnyBase,OtherAny>::value,OtherAny&>::type
	As()
		{ return *static_cast<OtherAny*>(static_cast<AnyBase*>(this)); }

// Special purpose constructors, called by sAny and sAnyCounted.
// Passing a single IKnowWhatIAmDoing_t indicates that the type of the pointed-to first
// param is the type to be default-constructed. Passing a second IKnowWhatIAmDoing_t
// indicates that the value is always to be counted rather than in-place.
	template <class S>
	Any_T(const S* dummy, const IKWIAD_t&)
	:	AnyBase(dummy, IKWIAD)
		{}

	template <class S, class P0>
	Any_T(const S* dummy, const P0& iP0, const IKWIAD_t&)
	:	AnyBase(dummy, iP0, IKWIAD)
		{}

	template <class S, class P0, class P1>
	Any_T(const S* dummy, const P0& iP0, const P1& iP1, const IKWIAD_t&)
	:	AnyBase(dummy, iP0, iP1, IKWIAD)
		{}

	template <class S>
	Any_T(const S* dummy, const IKWIAD_t&, const IKWIAD_t&)
	:	AnyBase(dummy, IKWIAD, IKWIAD)
		{}

	template <class S, class P0>
	Any_T(const S* dummy, const P0& iP0, const IKWIAD_t&, const IKWIAD_t&)
	:	AnyBase(dummy, iP0, IKWIAD, IKWIAD)
		{}

	template <class S, class P0, class P1>
	Any_T(const S* dummy, const P0& iP0, const P1& iP1, const IKWIAD_t&, const IKWIAD_t&)
	:	AnyBase(dummy, iP0, iP1, IKWIAD, IKWIAD)
		{}
	};

// =================================================================================================
#pragma mark - Pseudo-ctors

typedef Any_T<void> Any;

template <class S, class Tag_p=void>
Any_T<Tag_p> sAny()
	{ return Any(static_cast<S*>(0), IKWIAD); }

template <class S, class P0, class Tag_p=void>
Any_T<Tag_p> sAny(const P0& iP0)
	{ return Any_T<Tag_p>(static_cast<S*>(0), iP0, IKWIAD); }

template <class S, class P0, class P1, class Tag_p=void>
Any_T<Tag_p> sAny(const P0& iP0, const P1& iP1)
	{ return Any_T<Tag_p>(static_cast<S*>(0), iP0, iP1, IKWIAD); }

template <class S, class Tag_p=void>
Any_T<Tag_p> sAnyCounted()
	{ return Any_T<Tag_p>(static_cast<S*>(0), IKWIAD, IKWIAD); }

template <class S, class P0, class Tag_p=void>
Any_T<Tag_p> sAnyCounted(const P0& iP0)
	{ return Any_T<Tag_p>(static_cast<S*>(0), iP0, IKWIAD, IKWIAD); }

template <class S, class P0, class P1, class Tag_p=void>
Any_T<Tag_p> sAnyCounted(const P0& iP0, const P1& iP1)
	{ return Any_T<Tag_p>(static_cast<S*>(0), iP0, iP1, IKWIAD, IKWIAD); }

} // namespace ZooLib

#endif // __ZooLib_Any_T_h__
