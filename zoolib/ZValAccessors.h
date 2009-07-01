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

#ifndef __ZValAccessors__
#define __ZValAccessors__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZMACRO_ZValAccessors

#define ZMACRO_ZValAccessors_Decl_Entry(T, TYPENAME, TYPE) \
	bool QGet##TYPENAME(TYPE& oVal) const; \
	TYPE DGet##TYPENAME(const TYPE& iDefault) const; \
	TYPE Get##TYPENAME() const; \
	void Set##TYPENAME(const TYPE& iVal); \

#define ZMACRO_ZValAccessors_Def_Entry(T, TYPENAME, TYPE) \
	bool T::QGet##TYPENAME(TYPE& oVal) const \
		{ return this->QGet_T<>(oVal); } \
	TYPE T::DGet##TYPENAME(const TYPE& iDefault) const \
		{ return this->DGet_T<>(iDefault); } \
	TYPE T::Get##TYPENAME() const \
		{ return this->Get_T<TYPE>(); } \
	void T::Set##TYPENAME(const TYPE& iVal) \
		{ return this->Set_T<>(iVal); } \

// =================================================================================================
#pragma mark -
#pragma mark * ZMACRO_ZValMapAccessors

#define ZMACRO_ZValMapAccessors_Using(Map_t, Name_t, Val_t) \
	using ZValMapR_T<Map_t, Name_t, Val_t>::DGet; \
	using ZValMapR_T<Map_t, Name_t, Val_t>::Get; \
	using ZValMapR_T<Map_t, Name_t, Val_t>::QGet_T; \
	using ZValMapR_T<Map_t, Name_t, Val_t>::DGet_T; \
	using ZValMapR_T<Map_t, Name_t, Val_t>::Get_T; \

#endif // __ZValAccessors__
