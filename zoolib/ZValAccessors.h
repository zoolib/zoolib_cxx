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

#ifndef __ZValAccessors_h__
#define __ZValAccessors_h__ 1
#include "zconfig.h"

// =================================================================================================
// MARK: - ZMACRO_ZValAccessors

#define ZMACRO_ZValAccessors_Decl_Get(T, TYPENAME, TYPE) \
	ZQ<TYPE> QGet##TYPENAME() const; \
	bool QGet##TYPENAME(TYPE& oVal) const; \
	TYPE DGet##TYPENAME(const TYPE& iDefault) const; \
	TYPE Get##TYPENAME() const; \

#define ZMACRO_ZValAccessors_Decl_Entry(T, TYPENAME, TYPE) \
	ZMACRO_ZValAccessors_Decl_Get(T, TYPENAME, TYPE) \
	void Set##TYPENAME(const TYPE& iVal); \

#define ZMACRO_ZValAccessors_Def_GetP(P, T, TYPENAME, TYPE) \
	P ZQ<TYPE> T::QGet##TYPENAME() const \
		{ return this->QGet<TYPE>(); } \
	P bool T::QGet##TYPENAME(TYPE& oVal) const \
		{ \
		if (ZQ<TYPE> theQ = this->QGet<TYPE>()) \
			{ oVal = theQ.Get(); return true; } \
		return false; \
		} \
	P TYPE T::DGet##TYPENAME(const TYPE& iDefault) const \
		{ return this->DGet<TYPE>(iDefault); } \
	P TYPE T::Get##TYPENAME() const \
		{ return this->Get<TYPE>(); } \

#define ZMACRO_ZValAccessors_Def_EntryP(P, T, TYPENAME, TYPE) \
	ZMACRO_ZValAccessors_Def_GetP(P, T, TYPENAME, TYPE) \
	P void T::Set##TYPENAME(const TYPE& iVal) \
		{ return this->Set<TYPE>(iVal); } \

#define ZMACRO_ZValAccessors_Def_Entry(T, TYPENAME, TYPE) \
	ZMACRO_ZValAccessors_Def_EntryP(,T,TYPENAME, TYPE)

#endif // __ZValAccessors_h__
