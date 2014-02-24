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

	#if ZMACRO_Has_tr1

		#define ZCONFIG_SPI_Avail__type_traits 1

		#include ZMACRO_tr1_header(type_traits)

		#if ZMACRO_Has_tr1_early
			namespace std { namespace tr1 {

				template<bool, typename _Tp = void> struct enable_if {};
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
	using ZMACRO_namespace_tr1_prefix::integral_constant;
	using ZMACRO_namespace_tr1_prefix::true_type;
	using ZMACRO_namespace_tr1_prefix::false_type;

		// helper traits
	using ZMACRO_namespace_tr1_prefix::enable_if;
	using ZMACRO_namespace_tr1_prefix::conditional;

		// Primary classification traits:
	using ZMACRO_namespace_tr1_prefix::is_void;
	using ZMACRO_namespace_tr1_prefix::is_integral;
	using ZMACRO_namespace_tr1_prefix::is_floating_point;
	using ZMACRO_namespace_tr1_prefix::is_array;
	using ZMACRO_namespace_tr1_prefix::is_pointer;
//		using ZMACRO_namespace_tr1_prefix::is_lvalue_reference;
//		using ZMACRO_namespace_tr1_prefix::is_rvalue_reference;
	using ZMACRO_namespace_tr1_prefix::is_member_object_pointer;
	using ZMACRO_namespace_tr1_prefix::is_member_function_pointer;
	using ZMACRO_namespace_tr1_prefix::is_enum;
	using ZMACRO_namespace_tr1_prefix::is_union;
	using ZMACRO_namespace_tr1_prefix::is_class;
	using ZMACRO_namespace_tr1_prefix::is_function;

		// Secondary classification traits:
	using ZMACRO_namespace_tr1_prefix::is_reference;
	using ZMACRO_namespace_tr1_prefix::is_arithmetic;
	using ZMACRO_namespace_tr1_prefix::is_fundamental;
	using ZMACRO_namespace_tr1_prefix::is_member_pointer;
	using ZMACRO_namespace_tr1_prefix::is_scalar;
	using ZMACRO_namespace_tr1_prefix::is_object;
	using ZMACRO_namespace_tr1_prefix::is_compound;

		// Const-volatile properties and transformations:
	using ZMACRO_namespace_tr1_prefix::is_const;
	using ZMACRO_namespace_tr1_prefix::is_volatile;
	using ZMACRO_namespace_tr1_prefix::remove_const;
	using ZMACRO_namespace_tr1_prefix::remove_volatile;
	using ZMACRO_namespace_tr1_prefix::remove_cv;
	using ZMACRO_namespace_tr1_prefix::add_const;
	using ZMACRO_namespace_tr1_prefix::add_volatile;
	using ZMACRO_namespace_tr1_prefix::add_cv;

		// Reference transformations:
	using ZMACRO_namespace_tr1_prefix::remove_reference;
	#if ZMACRO_Has_tr1_early
		using ZMACRO_namespace_tr1_prefix::add_reference;
	#else
		using ZMACRO_namespace_tr1_prefix::add_lvalue_reference;
		template <class _Tp> using add_reference = add_lvalue_reference<_Tp>;
	#endif
//		using ZMACRO_namespace_tr1_prefix::add_rvalue_reference;

		// Pointer transformations:
	using ZMACRO_namespace_tr1_prefix::remove_pointer;
	using ZMACRO_namespace_tr1_prefix::add_pointer;

		// Integral properties:
	using ZMACRO_namespace_tr1_prefix::is_signed;
	using ZMACRO_namespace_tr1_prefix::is_unsigned;
//		using ZMACRO_namespace_tr1_prefix::make_signed;
//		using ZMACRO_namespace_tr1_prefix::make_unsigned;

		// Array properties and transformations:
	using ZMACRO_namespace_tr1_prefix::rank;
	using ZMACRO_namespace_tr1_prefix::extent;
	using ZMACRO_namespace_tr1_prefix::remove_extent;
	using ZMACRO_namespace_tr1_prefix::remove_all_extents;

		// Member introspection:
	using ZMACRO_namespace_tr1_prefix::is_pod;
//		using ZMACRO_namespace_tr1_prefix::is_trivial;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_copyable;
//		using ZMACRO_namespace_tr1_prefix::is_standard_layout;
//		using ZMACRO_namespace_tr1_prefix::is_literal_type;
	using ZMACRO_namespace_tr1_prefix::is_empty;
	using ZMACRO_namespace_tr1_prefix::is_polymorphic;
	using ZMACRO_namespace_tr1_prefix::is_abstract;

//		using ZMACRO_namespace_tr1_prefix::is_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_default_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_copy_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_move_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_copy_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_move_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_destructible;

//		using ZMACRO_namespace_tr1_prefix::is_trivially_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_default_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_copy_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_move_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_copy_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_move_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_trivially_destructible;

//		using ZMACRO_namespace_tr1_prefix::is_nothrow_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_default_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_copy_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_move_constructible;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_copy_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_move_assignable;
//		using ZMACRO_namespace_tr1_prefix::is_nothrow_destructible;

	using ZMACRO_namespace_tr1_prefix::has_virtual_destructor;

		// Relationships between types:
	using ZMACRO_namespace_tr1_prefix::is_same;
	using ZMACRO_namespace_tr1_prefix::is_base_of;
	using ZMACRO_namespace_tr1_prefix::is_convertible;

		// Alignment properties and transformations:
	using ZMACRO_namespace_tr1_prefix::alignment_of;

	using ZMACRO_namespace_tr1_prefix::is_pod;
	} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(type_traits)

#endif // __ZCompat_type_traits_h__
