/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZMACRO_decltype_h__
#define __ZMACRO_decltype_h__ 1
#include "zconfig.h"

#if not defined(_MSC_VER)

// Just assume GCC for now.
#define ZMACRO_decltype(expr) __typeof__(expr)

#elif _MSC_VER >= 1600

// Visual C++ 2010
#define ZMACRO_decltype(expr) decltype(expr)


#elif _MSC_VER >= 1400
// The VC6-VC7 bug has been fixed, so we don't have a native
// implementation, nor do we have the tricky one.

#elif _MSC_VER >= 1300
// VC 7

/*
From <http://stackoverflow.com/questions/4292085/type-inference-in-visual-c-2008>,
originally from <http://rsdn.ru/forum/src/1094305.aspx>
Written by Igor Chesnokov
*/
// type_of() evil implementation for VC7
//
// (c) Chez
// mailto:chezu@pisem.net

//#include "stdafx.h"

// This file contains:
// 1) type_id(type)
// 2) var_type_id(expersssion)
// 3) type_of(expression)

namespace ZooLib {
namespace MACRO_decltype {

// IMPLEMENTATION
template<int ID>
class CTypeRegRoot
{
public:
    class id2type;
};

template<typename T, int ID>
class CTypeReg : public CTypeRegRoot<ID>
{
public:
    class CTypeRegRoot<ID>::id2type // This uses nice VC6-VC7 bugfeature
    {
    public:
        typedef T Type;
    };

    typedef void Dummy;
};

template<int N>
class CCounter;

// TUnused is required to force compiler to recompile CCountOf class
template<typename TUnused, int NTested = 0>
class CCountOf
{
public:
    enum
    {
        __if_exists(CCounter<NTested>) { count = CCountOf<TUnused, NTested + 1>::count }
        __if_not_exists(CCounter<NTested>) { count = NTested }
    };
};

template<class TTypeReg, class TUnused, int NValue> // Helper class
class CProvideCounterValue
{
public:
    enum { value = NValue };
};

// type_id
#define type_id(type) \
    (CProvideCounterValue< \
		typename CTypeReg<type, CCountOf<type >::count>::Dummy, \
		CCounter<CCountOf<type >::count>, \
		CCountOf<type >::count \
     >::value)

// Lets type_id() be > than 0
class __Increment_type_id { enum { value = type_id(__Increment_type_id) }; };

template<int NSize>
class sized
{
private:
    char m_pad[NSize];
};

template<typename T>
typename sized<type_id(T)> VarTypeID(T&);
template<typename T>
typename sized<type_id(const T)> VarTypeID(const T&);
template<typename T>
typename sized<type_id(volatile  T)> VarTypeID(volatile T&);
template<typename T>
typename sized<type_id(const volatile T)> VarTypeID(const volatile T&);

} // namespace MACRO_decltype
} // namespace ZooLib

#define ZMACRO_decltype(expr)\
    ZooLib::MACRO_decltype::CTypeRegRoot<var_type_id(expression)>::id2type::Type

#endif // _MSV_VER

#endif // __ZMACRO_decltype_h__
