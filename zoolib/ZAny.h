/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZAny__
#define __ZAny__
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"
#include "zoolib/ZTypes.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZAny as a typedef of boost::any

#if ZCONFIG_SPI_Enabled(boost)

#include "boost/any.hpp"

NAMESPACE_ZOOLIB_BEGIN

typedef boost::any ZAny;

template<typename ValueType>
ValueType* ZAnyCast(ZAny* operand)
	{ return boost::any_cast<ValueType>(operand); }

template<typename ValueType>
const ValueType* ZAnyCast(const ZAny* operand)
	{ return boost::any_cast<ValueType>(operand); }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(boost)

// =================================================================================================
#pragma mark -
#pragma mark * ZAny, copied/reworked from boost::any

#if ! ZCONFIG_SPI_Enabled(boost)

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "zoolib/ZCompat_algorithm.h" // For swap
#include <typeinfo>

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
	#if ZCONFIG(Compiler, CodeWarrior)
		#if __MWERKS__ <= 0x3206
			#define BOOST_NO_MEMBER_TEMPLATE_FRIENDS
		#endif
	#endif
#endif

NAMESPACE_ZOOLIB_BEGIN

class ZAny
	{
public:
	ZAny();
	ZAny(const ZAny& other);
	~ZAny();
	ZAny& operator=(ZAny rhs);

	template<typename ValueType>
	ZAny(const ValueType & value)
	:	content(new holder<ValueType>(value))
		{}

	template<typename ValueType>
	ZAny& operator=(const ValueType & rhs)
		{
		ZAny(rhs).swap(*this);
		return *this;
		}

	ZAny& swap(ZAny& rhs);

	bool empty() const;
	const std::type_info & type() const;

private:
	class placeholder
		{
	public:
		virtual ~placeholder() {}

		virtual const std::type_info& type() const = 0;
		virtual placeholder* clone() const = 0;
		};

	template<typename ValueType>
	class holder : public placeholder
		{
	public:
		holder(const ValueType& value) : held(value) {}

		virtual const std::type_info& type() const
			{ return typeid(ValueType); }

		virtual placeholder* clone() const
			{ return new holder(held); }

		ValueType held;

	private: // intentionally left unimplemented
		holder& operator=(const holder&);
		};

#ifdef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
    public: // so ZAnyCast can be non-friend
#else
private:
	template<typename ValueType>
	friend ValueType* ZAnyCast(ZAny*);

	template<typename ValueType>
	friend const ValueType* ZAnyCast(const ZAny*);
#endif

	placeholder* content;
	};

template<typename ValueType>
ValueType* ZAnyCast(ZAny* operand)
	{
	if (!operand || operand->type() != typeid(ValueType))
		return 0;
	return &static_cast<ZAny::holder<ValueType>*>(operand->content)->held;
	}

template<typename ValueType>
const ValueType* ZAnyCast(const ZAny* operand)
	{
	if (!operand || operand->type() != typeid(ValueType))
		return 0;
	return &static_cast<ZAny::holder<ValueType>*>(operand->content)->held;
	}

NAMESPACE_ZOOLIB_END

#endif // ! ZCONFIG_SPI_Enabled(boost)

// =================================================================================================
#pragma mark -
#pragma mark * ZAny coercion

NAMESPACE_ZOOLIB_BEGIN

bool sQCoerceInt(const ZAny& iAny, int64& oVal);
int64 sDCoerceInt(int64 iDefault, const ZAny& iAny);
int64 sCoerceInt(const ZAny& iAny);

bool sQCoerceReal(const ZAny& iAny, double& oVal);
double sDCoerceReal(double iDefault, const ZAny& iAny);
double sCoerceReal(const ZAny& iAny);

NAMESPACE_ZOOLIB_END

#endif // __ZAny__
