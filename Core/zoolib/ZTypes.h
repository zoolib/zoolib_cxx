/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_ZTypes_h__
#define __ZooLib_ZTypes_h__
#include "zconfig.h"

#include "zoolib/UnicodeCU.h"

#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZStdInt.h"

namespace ZooLib {

// =================================================================================================
// There are several places where we need a buffer for some other code
// to dump data into, the content of which we don't care about. Rather
// than having multiple static buffers, or requiring wasteful use of
// stack space (a problem on MacOS 9) we instead have a shared garbage
// buffer. The key thing to remember in using it is that it must never
// be read from -- there's no way to know what other code is also using it.

extern char sGarbageBuffer[4096];

// =================================================================================================
// In many places we need a stack-based buffer. Ideally they'd be quite large
// but on MacOS 9 we don't want to blow the 24K - 32K that's normally available.
// This constant is 4096 on most platforms, 256 on MacOS 9.

#if ZCONFIG_SPI_Enabled(MacClassic)
	static const size_t sStackBufferSize = 256;
#else
	static const size_t sStackBufferSize = 4096;
#endif

// =================================================================================================
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

// =================================================================================================
// null is a marker value, used in places where we're wanting to be explicit about returning an
// empty value of some sort, but where we don't want to have to manually create the value each time.

const class notnull_t {} notnull = {};

const struct null_t
	{
	const notnull_t operator!() const { return notnull; }
	} null = {};

// =================================================================================================
// IKnowWhatIAmDoing is also a marker value, used in a few places generally to distinguish
// private ctors that would otherwise be identical.

const struct IKnowWhatIAmDoing_t {} IKnowWhatIAmDoing = {};

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

		operator const T*() const { return &fT; }

		operator const void*() const { return &fT; }

	private:
		const T fT;
		};

	template <class T>
	Holder<T> operator()(const T& iT) const { return Holder<T>(iT); }

	template <class T>
	Holder<T> operator&(const T& iT) const { return Holder<T>(iT); }

	} sConstPtr = {};

// =================================================================================================
// 'sMutablePtr(rvalue)' or 'sMutablePtr& rvalue' is useful when working with socket APIs.

const struct
	{
	template <class T>
	class Holder
		{
	public:
		Holder(const T& iT) : fT(iT) {}

		operator T*() const { return &fT; }

	private:
		mutable T fT;
		};

	template <class T>
	Holder<T> operator()(const T& iT) const { return Holder<T>(iT); }

	template <class T>
	Holder<T> operator&(const T& iT) const { return Holder<T>(iT); }

	} sMutablePtr = {};

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
	} sAdopt = {};

// =================================================================================================
// Availability of tr1 is still a bit patchy, use our own enable templates for now.

template <bool B, class T = void> struct EnableIfC {};

template <class T> struct EnableIfC<true, T> { typedef T type; };

} // namespace ZooLib

#endif // __ZooLib_ZTypes_h__
