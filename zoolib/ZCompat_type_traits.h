/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCompat_type_traits_h__
#define __ZCompat_type_traits_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI_Definitions.h"

#ifndef ZCONFIG_SPI_Desired__type_traits
	#define ZCONFIG_SPI_Desired__type_traits 1
#endif

#if ZCONFIG_SPI_Desired__type_traits

	#if ZCONFIG_LIBCPP_2011
		#include <type_traits>

		#define ZMACRO_type_traits_namespace std
		#define ZCONFIG_SPI_Avail__type_traits 1

	#elif ZCONFIG_LIBCPP_TR1
		#include <tr1/type_traits>
		#define ZMACRO_type_traits_namespace std::tr1
		#define ZCONFIG_SPI_Avail__type_traits 1

		#if defined(ZCONFIG_GCC_Version) && ZCONFIG_GCC_Version < 45
			// No enable_if or conditional prior to 4.5
			namespace std { namespace tr1 {

				template<bool, typename _Tp = void> struct enable_if  {};
				template<typename _Tp> struct enable_if<true,_Tp> { typedef _Tp type; };

				template<bool _Cond, typename _Iftrue, typename _Iffalse>
				struct conditional { typedef _Iftrue type; };

				template<typename _Iftrue, typename _Iffalse>
				struct conditional<false,_Iftrue,_Iffalse> { typedef _Iffalse type; };

			}} // namespace tr1, namespace std
		#endif
	#endif

#endif // ZCONFIG_SPI_Desired__type_traits

#ifndef ZCONFIG_SPI_Avail__type_traits
	#define ZCONFIG_SPI_Avail__type_traits 0
#endif

#if ZCONFIG_SPI_Enabled(type_traits)
	namespace ZooLib {
	using ZMACRO_type_traits_namespace::integral_constant;
	using ZMACRO_type_traits_namespace::true_type;
	using ZMACRO_type_traits_namespace::false_type;

		// helper traits
	using ZMACRO_type_traits_namespace::enable_if;
	using ZMACRO_type_traits_namespace::conditional;

		// Primary classification traits:
	using ZMACRO_type_traits_namespace::is_void;
	using ZMACRO_type_traits_namespace::is_integral;
	using ZMACRO_type_traits_namespace::is_floating_point;
	using ZMACRO_type_traits_namespace::is_array;
	using ZMACRO_type_traits_namespace::is_pointer;
//		using ZMACRO_type_traits_namespace::is_lvalue_reference;
//		using ZMACRO_type_traits_namespace::is_rvalue_reference;
	using ZMACRO_type_traits_namespace::is_member_object_pointer;
	using ZMACRO_type_traits_namespace::is_member_function_pointer;
	using ZMACRO_type_traits_namespace::is_enum;
	using ZMACRO_type_traits_namespace::is_union;
	using ZMACRO_type_traits_namespace::is_class;
	using ZMACRO_type_traits_namespace::is_function;

		// Secondary classification traits:
	using ZMACRO_type_traits_namespace::is_reference;
	using ZMACRO_type_traits_namespace::is_arithmetic;
	using ZMACRO_type_traits_namespace::is_fundamental;
	using ZMACRO_type_traits_namespace::is_member_pointer;
	using ZMACRO_type_traits_namespace::is_scalar;
	using ZMACRO_type_traits_namespace::is_object;
	using ZMACRO_type_traits_namespace::is_compound;

		// Const-volatile properties and transformations:
	using ZMACRO_type_traits_namespace::is_const;
	using ZMACRO_type_traits_namespace::is_volatile;
	using ZMACRO_type_traits_namespace::remove_const;
	using ZMACRO_type_traits_namespace::remove_volatile;
	using ZMACRO_type_traits_namespace::remove_cv;
	using ZMACRO_type_traits_namespace::add_const;
	using ZMACRO_type_traits_namespace::add_volatile;
	using ZMACRO_type_traits_namespace::add_cv;

		// Reference transformations:
	using ZMACRO_type_traits_namespace::remove_reference;
	using ZMACRO_type_traits_namespace::add_reference;
//		using ZMACRO_type_traits_namespace::add_lvalue_reference;
//		using ZMACRO_type_traits_namespace::add_rvalue_reference;

		// Pointer transformations:
	using ZMACRO_type_traits_namespace::remove_pointer;
	using ZMACRO_type_traits_namespace::add_pointer;

		// Integral properties:
	using ZMACRO_type_traits_namespace::is_signed;
	using ZMACRO_type_traits_namespace::is_unsigned;
//		using ZMACRO_type_traits_namespace::make_signed;
//		using ZMACRO_type_traits_namespace::make_unsigned;

		// Array properties and transformations:
	using ZMACRO_type_traits_namespace::rank;
	using ZMACRO_type_traits_namespace::extent;
	using ZMACRO_type_traits_namespace::remove_extent;
	using ZMACRO_type_traits_namespace::remove_all_extents;

		// Member introspection:
	using ZMACRO_type_traits_namespace::is_pod;
//		using ZMACRO_type_traits_namespace::is_trivial;
//		using ZMACRO_type_traits_namespace::is_trivially_copyable;
//		using ZMACRO_type_traits_namespace::is_standard_layout;
//		using ZMACRO_type_traits_namespace::is_literal_type;
	using ZMACRO_type_traits_namespace::is_empty;
	using ZMACRO_type_traits_namespace::is_polymorphic;
	using ZMACRO_type_traits_namespace::is_abstract;

//		using ZMACRO_type_traits_namespace::is_constructible;
//		using ZMACRO_type_traits_namespace::is_default_constructible;
//		using ZMACRO_type_traits_namespace::is_copy_constructible;
//		using ZMACRO_type_traits_namespace::is_move_constructible;
//		using ZMACRO_type_traits_namespace::is_assignable;
//		using ZMACRO_type_traits_namespace::is_copy_assignable;
//		using ZMACRO_type_traits_namespace::is_move_assignable;
//		using ZMACRO_type_traits_namespace::is_destructible;

//		using ZMACRO_type_traits_namespace::is_trivially_constructible;
//		using ZMACRO_type_traits_namespace::is_trivially_default_constructible;
//		using ZMACRO_type_traits_namespace::is_trivially_copy_constructible;
//		using ZMACRO_type_traits_namespace::is_trivially_move_constructible;
//		using ZMACRO_type_traits_namespace::is_trivially_assignable;
//		using ZMACRO_type_traits_namespace::is_trivially_copy_assignable;
//		using ZMACRO_type_traits_namespace::is_trivially_move_assignable;
//		using ZMACRO_type_traits_namespace::is_trivially_destructible;

//		using ZMACRO_type_traits_namespace::is_nothrow_constructible;
//		using ZMACRO_type_traits_namespace::is_nothrow_default_constructible;
//		using ZMACRO_type_traits_namespace::is_nothrow_copy_constructible;
//		using ZMACRO_type_traits_namespace::is_nothrow_move_constructible;
//		using ZMACRO_type_traits_namespace::is_nothrow_assignable;
//		using ZMACRO_type_traits_namespace::is_nothrow_copy_assignable;
//		using ZMACRO_type_traits_namespace::is_nothrow_move_assignable;
//		using ZMACRO_type_traits_namespace::is_nothrow_destructible;

	using ZMACRO_type_traits_namespace::has_virtual_destructor;

		// Relationships between types:
	using ZMACRO_type_traits_namespace::is_same;
	using ZMACRO_type_traits_namespace::is_base_of;
	using ZMACRO_type_traits_namespace::is_convertible;

		// Alignment properties and transformations:
	using ZMACRO_type_traits_namespace::alignment_of;

	using ZMACRO_type_traits_namespace::is_pod;
	} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(type_traits)

#undef ZMACRO_type_traits_namespace

#endif // __ZCompat_type_traits_h__
