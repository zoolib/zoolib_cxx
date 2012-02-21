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

#ifndef __ZVal_CF_h__
#define __ZVal_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_CF.h"
#include "zoolib/ZRef_CF.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors_Std.h"

namespace ZooLib {

class ZSeq_CF;
class ZMap_CF;

// =================================================================================================
// MARK: - ZVal_CF

class ZVal_CF
:	public ZRef<CFTypeRef>
	{
	typedef ZRef<CFTypeRef> inherited;
public:
	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	operator bool() const;

	ZVal_CF();
	ZVal_CF(const ZVal_CF& iOther);
	~ZVal_CF();
	ZVal_CF& operator=(const ZVal_CF& iOther);

	ZVal_CF(CFTypeRef iVal) : inherited(iVal) {}
	template <class S> ZVal_CF(const ZRef<S>& iVal) : inherited(iVal) {}
	template <class S> ZVal_CF(const Adopt_T<S>& iVal) : inherited(iVal) {}

	ZVal_CF(char iVal);
	ZVal_CF(signed char iVal);
	ZVal_CF(unsigned char iVal);
	ZVal_CF(wchar_t iVal);
	ZVal_CF(short iVal);
	ZVal_CF(unsigned short iVal);
	ZVal_CF(int iVal);
	ZVal_CF(unsigned int iVal);
	ZVal_CF(long iVal);
	ZVal_CF(unsigned long iVal);
	ZVal_CF(long long iVal);
	ZVal_CF(unsigned long long iVal);
	ZVal_CF(bool iVal);
	ZVal_CF(float iVal);
	ZVal_CF(double iVal);
	ZVal_CF(const char* iVal);
	ZVal_CF(const string8& iVal);
	ZVal_CF(const string16& iVal);
	ZVal_CF(CFStringRef iVal);

	ZVal_CF(const ZData_CF& iVal);
	ZVal_CF(const ZSeq_CF& iVal);
	ZVal_CF(const ZMap_CF& iVal);

	explicit ZVal_CF(CFDataRef iVal);
	explicit ZVal_CF(CFArrayRef iVal);
	explicit ZVal_CF(CFDictionaryRef iVal);

	ZVal_CF& operator=(CFTypeRef iVal);

	template <class S>
	ZVal_CF& operator=(const ZRef<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	ZVal_CF& operator=(const Adopt_T<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

// ZVal protocol
	using inherited::Clear;
	using inherited::Get;

	template <class S>
	ZQ<S> QGet() const;

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return iDefault;
		}

	template <class S>
	S Get() const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return S();
		}

	template <class S>
	void Set(const S& iVal);

	ZVal_CF Get(const string8& iName) const;
	ZVal_CF Get(CFStringRef iName) const;
	ZVal_CF Get(size_t iIndex) const;

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_Std(ZVal_CF)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_CF, CFString, ZRef<CFStringRef>)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_CF, Data, ZData_CF)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_CF, Seq, ZSeq_CF)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_CF, Map, ZMap_CF)
/// \endcond DoxygenIgnore
	};

// =================================================================================================
// MARK: - ZSeq_CF

class ZSeq_CF
:	public ZRef<CFArrayRef>
	{
	typedef ZRef<CFArrayRef> inherited;

public:
	typedef ZVal_CF Val;

	ZSeq_Any AsSeq_Any(const ZAny& iDefault) const;

	operator bool() const;

	ZSeq_CF();
	ZSeq_CF(const ZSeq_CF& iOther);
	~ZSeq_CF();
	ZSeq_CF& operator=(const ZSeq_CF& iOther);

	ZSeq_CF(CFMutableArrayRef iOther);
	ZSeq_CF(CFArrayRef iOther);

	ZSeq_CF(const Adopt_T<CFMutableArrayRef>& iOther);
	ZSeq_CF(const Adopt_T<CFArrayRef>& iOther);

	ZSeq_CF& operator=(CFMutableArrayRef iOther);
	ZSeq_CF& operator=(CFArrayRef iOther);

	ZSeq_CF& operator=(const Adopt_T<CFMutableArrayRef>& iOther);
	ZSeq_CF& operator=(const Adopt_T<CFArrayRef>& iOther);

// ZSeq protocol
	size_t Count() const;

	using inherited::Clear;
	using inherited::Get;

	ZQ<ZVal_CF> QGet(size_t iIndex) const;
	ZVal_CF DGet(const ZVal_CF& iDefault, size_t iIndex) const;
	ZVal_CF Get(size_t iIndex) const;

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	S DGet(const S& iDefault, size_t iIndex) const
		{ return this->Get(iIndex).DGet<S>(iDefault); }

	template <class S>
	S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	ZSeq_CF& Set(size_t iIndex, const ZVal_CF& iVal);

	ZSeq_CF& Erase(size_t iIndex);

	ZSeq_CF& Insert(size_t iIndex, const ZVal_CF& iVal);

	ZSeq_CF& Append(const ZVal_CF& iVal);

private:
	CFArrayRef pArray() const;
	CFMutableArrayRef pTouch();
	bool fMutable;
	};

// =================================================================================================
// MARK: - ZMap_CF

class ZMap_CF
:	public ZRef<CFDictionaryRef>
	{
	typedef ZRef<CFDictionaryRef> inherited;

public:
	typedef ZVal_CF Val;

	ZMap_Any AsMap_Any(const ZAny& iDefault) const;

	operator bool() const;

	ZMap_CF();
	ZMap_CF(const ZMap_CF& iOther);
	~ZMap_CF();
	ZMap_CF& operator=(const ZMap_CF& iOther);

	ZMap_CF(CFMutableDictionaryRef iOther);
	ZMap_CF(CFDictionaryRef iOther);

	ZMap_CF(const Adopt_T<CFMutableDictionaryRef>& iOther);
	ZMap_CF(const Adopt_T<CFDictionaryRef>& iOther);

	ZMap_CF& operator=(CFMutableDictionaryRef iOther);
	ZMap_CF& operator=(CFDictionaryRef iOther);

	ZMap_CF& operator=(const Adopt_T<CFMutableDictionaryRef>& iOther);
	ZMap_CF& operator=(const Adopt_T<CFDictionaryRef>& iOther);

// ZMap protocol
	using inherited::Clear;
	using inherited::Get;

	ZQ<ZVal_CF> QGet(const string8& iName) const;
	ZQ<ZVal_CF> QGet(CFStringRef iName) const;

	ZVal_CF DGet(const ZVal_CF& iDefault, const string8& iName) const;
	ZVal_CF DGet(const ZVal_CF& iDefault, CFStringRef iName) const;

	ZVal_CF Get(const string8& iName) const;
	ZVal_CF Get(CFStringRef iName) const;

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	ZMap_CF& Set(const string8& iName, const ZVal_CF& iVal);
	ZMap_CF& Set(CFStringRef iName, const ZVal_CF& iVal);

	template <class S>
	ZMap_CF& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, ZVal_CF(iVal)); }

	template <class S>
	ZMap_CF& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, ZVal_CF(iVal)); }

	ZMap_CF& Erase(const string8& iName);
	ZMap_CF& Erase(CFStringRef iName);

private:
	CFDictionaryRef pDictionary() const;
	CFMutableDictionaryRef pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZVal_CF_h__
