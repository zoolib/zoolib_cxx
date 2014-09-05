/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCompare_h__
#define __ZCompare_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompare_T.h"

#include <typeinfo>

namespace ZooLib {

// =================================================================================================
// MARK: - ZCompare

class ZCompare
	{
protected:
	ZCompare(const char* iTypeName);
	virtual ~ZCompare();

	virtual int Compare(const void* iL, const void* iR) = 0;

public:
	static int sCompare(const char* iTypeName, const void* iL, const void* iR);
	const char* fTypeName;
	};

// =================================================================================================
// MARK: - ZCompareRegistration_T

#if defined(__MWERKS__) && (__MWERKS__ <= 0x2406)

	// CW7 doesn't handle function template parameters properly, so we indirect through a functor.

	template <class T, class CompareFunctor>
	class ZCompareRegistration_T : public ZCompare
		{
	public:
		ZCompareRegistration_T() : ZCompare(typeid(T).name()) {}

	// From ZCompare
		virtual int Compare(const void* iL, const void* iR)
			{ return CompareFunctor()(*static_cast<const T*>(iL), *static_cast<const T*>(iR)); }
		};

	template <class T>
	class ZCompareRegistrationFunctor_T
		{
	public:
		int operator()(const T& iL, const T& iR) const
			{ return sCompare_T<T>(iL, iR); }
		};

	#define ZMACRO_CompareRegistration_T_Real(t, CLASS, INST) \
		namespace { \
			class CLASS : public ZCompareRegistration_T<t, ZCompareRegistrationFunctor_T<t> > \
				{} INST; \
			}
#else

	template <class T, int (*CompareProc)(const T&, const T&)>
	class ZCompareRegistration_T : public ZCompare
		{
	public:
		ZCompareRegistration_T() : ZCompare(typeid(T).name()) {}

	// From ZCompare
		virtual int Compare(const void* iL, const void* iR)
			{ return CompareProc(*static_cast<const T*>(iL), *static_cast<const T*>(iR)); }
		};

	#define ZMACRO_CompareRegistration_T_Real(t, CLASS, INST) \
		namespace { class CLASS : public ZCompareRegistration_T<t, sCompare_T<t> > {} INST; }

#endif

// =================================================================================================
// MARK: - Macros

#define ZMACRO_CompareRegistration_T(t) \
	ZMACRO_CompareRegistration_T_Real(t, \
		ZMACRO_Concat(ZComparer_,__LINE__), \
		ZMACRO_Concat(sComparer_,__LINE__))

} // namespace ZooLib

#endif // __ZCompare_h__
