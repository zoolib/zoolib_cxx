// Copyright (c) 2000-2007 Andrew Green and Learning in Motion, Inc.
// Copyright (c) 2008-2019 Andrew Green.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_ZTypes_h__
#define __ZooLib_ZTypes_h__
#include "zconfig.h"

#include "zoolib/size_t.h"
#include "zoolib/UnicodeCU.h"

#include "zoolib/ZCONFIG_SPI.h"

#include <memory> // For std::pair

namespace ZooLib {

// =================================================================================================
// In many places we need a stack-based buffer. Ideally they'd be quite large
// but on MacClassic we don't want to blow the 24K - 32K that's normally available.
// This constant is 4096 on most platforms, 256 on MacOS 9.

#if ZCONFIG_SPI_Enabled(MacClassic)
	static const size_t sStackBufferSize = 256;
#else
	constexpr size_t sStackBufferSize = 4096;
#endif

// =================================================================================================
// countof
#if 0
	// This doesn't handle zero-length arrays:
	template <typename T, size_t N> constexpr size_t countof(T(&arr)[N]) noexcept { return N; }
#else
	// For a discussion of the implementation of countof See section 14.3 of
	// "Imperfect C++" by Matthew Wilson, published by Addison Wesley.

	#ifndef countof
		#if ZCONFIG_Compiler == ZCONFIG_Compiler_MSVC
			#define countof(x) _countof(x)
		#else
			template <class T, int N>
			char (&charArrayOneGreaterThan(const T(&)[N]))[N+1];

			// Switching to one based and adding this specialization lets us interpret
			// zero-sized arrays provided the compiler itself doesn't complain.
			char (&charArrayOneGreaterThan(const void*))[1];

			#define countof(array) (sizeof(ZooLib::charArrayOneGreaterThan((array)))-1)
		#endif
	#endif
#endif

// =================================================================================================
// null is a marker value, used in places where we're wanting to be explicit about returning an
// empty value of some sort, but where we don't want to have to manually create the value each time.

const class notnull_t {} notnull;

const struct null_t
	{
	const notnull_t operator!() const { return notnull; }
	} null;

// =================================================================================================
// IKnowWhatIAmDoing is also a marker value, used in a few places generally to distinguish
// private ctors that would otherwise be identical.

const struct IKnowWhatIAmDoing_t {} IKnowWhatIAmDoing, IKWIAD;

// =================================================================================================
// Assuming 'type* ptr;' sConst(ptr) is a terser form of static_cast<const type*>(ptr).

template <class T>
const T* sConst(const T* iT) { return iT; }

template <class T>
const T* sConst(T* iT) { return iT; }

// =================================================================================================
// Assuming 'type& ref;' sConst(ref) is a terser form of static_cast<const type&>(ptr).

template <class T>
const T& sConst(const T& iT) { return iT; }

template <class T>
const T& sConst(T& iT) { return iT; }

// =================================================================================================
// Assuming 'const type* ptr;' sNonConst(ptr) is a terser form of const_cast<type*>(ptr).

template <class T>
T* sNonConst(const T* iT) { return const_cast<T*>(iT); }

template <class T>
T* sNonConst(T* iT) { return iT; }

// =================================================================================================
// Assuming 'const type& ref;' sNonConst(ref) is a terser form of const_cast<type&>(ref).

template <class T>
T& sNonConst(const T& iT) { return const_cast<T&>(iT); }

template <class T>
T& sNonConst(T& iT) { return iT; }

// =================================================================================================
// The sDynNonConst template functions combine const and dynamic casts into a single invocation.

template <class P, class T>
P* sDynNonConst(const T* iT) { return dynamic_cast<P*>(sNonConst(iT)); }

template <class P, class T>
P* sDynNonConst(T* iT) { return dynamic_cast<P*>(iT); }

template <class P, class T>
P& sDynNonConst(const T& iT) { return dynamic_cast<P&>(sNonConst(iT)); }

template <class P, class T>
P& sDynNonConst(T& iT) { return dynamic_cast<P&>(iT); }

// =================================================================================================
// 'sConstPtr(rvalue)' or 'sConstPtr& rvalue' can be used where a const pointer
// parameter is required, removing the need for a local variable in some cases.

const struct
	{
	template <class T>
	class Holder
		{
	public:
		Holder(const T& iT) : fT(iT) {}
		Holder(T&& iT) : fT(std::move(iT)) {}

		operator const T*() const { return &fT; }

		operator const void*() const { return &fT; }

	private:
		const T fT;
		};

	template <class T>
	Holder<T> operator()(const T& iT) const { return Holder<T>(iT); }

	template <class T>
	Holder<T> operator&(const T& iT) const { return Holder<T>(iT); }

	} sConstPtr;

// =================================================================================================
// 'sMutablePtr(rvalue)' or 'sMutablePtr& rvalue' is useful when working with socket APIs.

const struct
	{
	template <class T>
	class Holder
		{
	public:
		Holder(const T& iT) : fT(iT) {}
		Holder(T&& iT) : fT(std::move(iT)) {}

		operator T*() const { return &fT; }

	private:
		mutable T fT;
		};

	template <class T>
	Holder<T> operator()(const T& iT) const { return Holder<T>(iT); }

	template <class T>
	Holder<T> operator&(const T& iT) const { return Holder<T>(iT); }

	} sMutablePtr;

// =================================================================================================
// Adopt_T<type> or Adopt_T<type*> indicates to a ZP (mainly) that it should
// take ownership of the pointed-to refcounted entity.

template <class T>
class Adopt_T
	{
public:
	explicit Adopt_T(T* iP) : fP(iP) {}
	T* Get() const { return fP; }
private:
	T* fP;
	};

template <class T>
class Adopt_T<T*>
	{
public:
	explicit Adopt_T(T* iP) : fP(iP) {}
	T* Get() const { return fP; }
private:
	T* fP;
	};

// =================================================================================================
// 'sAdopt(pointer)' or 'sAdopt& pointer' returns an Adopt_T<pointee>.

const struct
	{
	template <class T>
	Adopt_T<T> operator()(T iT) const { return Adopt_T<T>(iT); }

	template <class T>
	Adopt_T<T> operator&(T iT) const { return Adopt_T<T>(iT); }
	} sAdopt;

// =================================================================================================

template <bool B, class T = void> struct EnableIfC {};
template <class T> struct EnableIfC<true, T> { typedef T type; };

template <bool B, class T> using EnableIf_t = typename EnableIfC<B,T>::type;

// =================================================================================================
#pragma mark - PaC (PointerAndCount)

template <class EE> using PaC = std::pair<EE*,size_t>;

// Accessors
template <class EE> EE* sPtr(const PaC<EE>& iPac) { return iPac.first; }
template <class EE> size_t sCount(const PaC<EE>& iPac) { return iPac.second; }

// Pseudo-ctor
template <class EE> PaC<EE> sPaC(EE* iPtr, size_t iCount) { return PaC<EE>(iPtr, iCount); }
template <class EE> PaC<EE> sPaC() { return PaC<EE>(); }

// Pseudo-ctor from PaC<void> to PaC<EE>, adjusting the count in the process
template <class EE>
PaC<EE> sPaC(PaC<void> iPaC)
	{ return sPaC<EE>(static_cast<EE*>(sPtr(iPaC)), sCount(iPaC) / sizeof(EE)); }

template <class EE>
PaC<const EE> sPaC(PaC<const void> iPaC)
	{ return sPaC<const EE>(static_cast<const EE*>(sPtr(iPaC)), sCount(iPaC) / sizeof(EE)); }

// Casting from PaC<EE> to PaC<RR>, provided their sizes match
template <class RR, class EE>
typename EnableIfC<sizeof(RR)==sizeof(EE),PaC<RR>>::type
sCastPaC(PaC<EE> iPaC)
	{ return sPaC(static_cast<RR*>(static_cast<void*>(sPtr(iPaC))), sCount(iPaC)); }

// Same, but for const pointers
template <class ConstRR, class EE>
typename EnableIfC<sizeof(ConstRR)==sizeof(EE),PaC<ConstRR>>::type
sCastPaC(PaC<const EE> iPaC)
	{ return sPaC(static_cast<ConstRR*>(static_cast<const void*>(sPtr(iPaC))), sCount(iPaC)); }

// Will be deprecated
template <class EE> EE* sPointer(const PaC<EE>& iPac) { return iPac.first; }

} // namespace ZooLib

#endif // __ZooLib_ZTypes_h__
