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

#ifndef __ZNetscape_Variant_h__
#define __ZNetscape_Variant_h__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZValAccessors.h"

#include "zoolib/netscape/ZNetscape.h"

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPVariantBase

// NPVariantBase is only needed to work around our inability
// to have partial template member specialization.

class NPVariantBase
:	public NPVariant
	{
public:
// ZVal Get protocol
	template <class S>
	const ZQ<S> QGet() const;

	template <class S>
	const S DGet(const S& iDefault) const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return *theQ;
		return iDefault;
		}

	template <class S>
	const S Get() const
		{ return this->DGet(S()); }
	};

// =================================================================================================
// MARK: - NPVariant_T

template <class T>
void sVariantRelease_T(T&);

template <class T>
void* sMalloc_T(T&, size_t);

template <class T>
void sFree_T(void*);

template <class T>
class NPVariant_T
:	public NPVariantBase
	{
private:
	void pCopyFrom(const NPVariant& iOther);
	void pRelease();

public:
	ZMACRO_operator_bool_T(NPVariant_T<T>, operator_bool) const
		{ return operator_bool_gen::translate(type != NPVariantType_Void); }

	typedef T Object_t;

	ZAny AsAny() const;
	ZAny AsAny(const ZAny& iDefault) const;

	NPVariant_T();
	NPVariant_T(const NPVariant_T& iOther);
	~NPVariant_T();
	NPVariant_T& operator=(const NPVariant_T& iOther);

	NPVariant_T(const NPVariant& iOther);
	NPVariant_T& operator=(const NPVariant& iOther);

	explicit NPVariant_T(bool iValue);
	explicit NPVariant_T(int32 iValue);
	explicit NPVariant_T(double iValue);
	NPVariant_T(const char* iValue);
	NPVariant_T(const std::string& iValue);
	NPVariant_T(T* iValue);
	NPVariant_T(const ZRef<T>& iValue);

	NPVariant_T& operator=(bool iValue);
	NPVariant_T& operator=(int32 iValue);
	NPVariant_T& operator=(double iValue);
	NPVariant_T& operator=(const char* iValue);
	NPVariant_T& operator=(const std::string& iValue);
	NPVariant_T& operator=(T* iValue);
	NPVariant_T& operator=(const ZRef<T>& iValue);

	operator ZRef<T>() const;

	bool IsVoid() const;
	bool IsNull() const;
	bool IsBool() const;
	bool IsInt32() const;
	bool IsDouble() const;
	bool IsString() const;
	bool IsObject() const;

// Our protocol
	void SetVoid();
	void SetNull();
	void SetBool(bool iValue);
	void SetInt32(int32 iValue);
	void SetDouble(double iValue);
	void SetString(const std::string& iValue);
	void SetObject(T* iValue);
	void SetObject(const ZRef<T>& iValue);

// Typename get accessors
	ZMACRO_ZValAccessors_Decl_Get(NPVariant_T<T>, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Get(NPVariant_T<T>, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Get(NPVariant_T<T>, Double, double)
	ZMACRO_ZValAccessors_Decl_Get(NPVariant_T<T>, String, std::string)
	ZMACRO_ZValAccessors_Decl_Get(NPVariant_T<T>, Object, ZRef<T>)

private:
	void pSetString(const char* iChars, size_t iLength);
	void pSetString(const std::string& iString);
	};

} // namespace ZNetscape
} // namespace ZooLib

#endif // __ZNetscape_Variant_h__
