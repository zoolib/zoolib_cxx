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

#ifndef __ZooLib_Apple_Val_CF_h__
#define __ZooLib_Apple_Val_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/UnicodeString.h"

#include "zoolib/Apple/Data_CF.h"
#include "zoolib/Apple/ZRef_CF.h"

namespace ZooLib {

class Seq_CF;
class Map_CF;

// =================================================================================================
#pragma mark -
#pragma mark Val_CF

class Val_CF
:	public ZRef<CFTypeRef>
	{
	typedef ZRef<CFTypeRef> inherited;
public:
	operator bool() const;

	Val_CF();
	Val_CF(const Val_CF& iOther);
	~Val_CF();
	Val_CF& operator=(const Val_CF& iOther);

	Val_CF(CFTypeRef iVal) : inherited(iVal) {}
	template <class S> Val_CF(const ZRef<S>& iVal) : inherited(iVal) {}
	template <class S> Val_CF(const Adopt_T<S>& iVal) : inherited(iVal) {}

	Val_CF(char iVal);
	Val_CF(signed char iVal);
	Val_CF(unsigned char iVal);
	Val_CF(wchar_t iVal);
	Val_CF(short iVal);
	Val_CF(unsigned short iVal);
	Val_CF(int iVal);
	Val_CF(unsigned int iVal);
	Val_CF(long iVal);
	Val_CF(unsigned long iVal);
	Val_CF(long long iVal);
	Val_CF(unsigned long long iVal);
	Val_CF(bool iVal);
	Val_CF(float iVal);
	Val_CF(double iVal);
	Val_CF(long double iVal);
	Val_CF(const char* iVal);
	Val_CF(const string8& iVal);
	Val_CF(const string16& iVal);
	Val_CF(CFStringRef iVal);

	Val_CF(const Data_CF& iVal);
	Val_CF(const Seq_CF& iVal);
	Val_CF(const Map_CF& iVal);

	explicit Val_CF(CFDataRef iVal);
	explicit Val_CF(CFArrayRef iVal);
	explicit Val_CF(CFDictionaryRef iVal);

	Val_CF& operator=(CFTypeRef iVal);

	template <class S>
	Val_CF& operator=(const ZRef<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	Val_CF& operator=(const Adopt_T<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

// ZVal protocol
	using inherited::Clear;
	using inherited::Get;

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
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return *theQ;
		return S();
		}

	template <class S>
	void Set(const S& iVal);

	const Val_CF Get(const string8& iName) const;
	const Val_CF Get(CFStringRef iName) const;
	const Val_CF Get(size_t iIndex) const;
	};

// =================================================================================================
#pragma mark -
#pragma mark Seq_CF

class Seq_CF
:	public ZRef<CFArrayRef>
	{
	typedef ZRef<CFArrayRef> inherited;

public:
	typedef Val_CF Val;

	operator bool() const;

	Seq_CF();
	Seq_CF(const Seq_CF& iOther);
	~Seq_CF();
	Seq_CF& operator=(const Seq_CF& iOther);

	Seq_CF(CFMutableArrayRef iOther);
	Seq_CF(CFArrayRef iOther);

	Seq_CF(const Adopt_T<CFMutableArrayRef>& iOther);
	Seq_CF(const Adopt_T<CFArrayRef>& iOther);

	Seq_CF& operator=(CFMutableArrayRef iOther);
	Seq_CF& operator=(CFArrayRef iOther);

	Seq_CF& operator=(const Adopt_T<CFMutableArrayRef>& iOther);
	Seq_CF& operator=(const Adopt_T<CFArrayRef>& iOther);

// ZSeq protocol
	size_t Size() const;

	using inherited::Clear;
	using inherited::Get;

	const ZQ<Val_CF> QGet(size_t iIndex) const;
	const Val_CF DGet(const Val_CF& iDefault, size_t iIndex) const;
	const Val_CF Get(size_t iIndex) const;

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, size_t iIndex) const
		{ return this->Get(iIndex).DGet<S>(iDefault); }

	template <class S>
	const S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	Seq_CF& Set(size_t iIndex, const Val_CF& iVal);

	Seq_CF& Erase(size_t iIndex);

	Seq_CF& Insert(size_t iIndex, const Val_CF& iVal);

	Seq_CF& Append(const Val_CF& iVal);

private:
	CFArrayRef pArray() const;
	CFMutableArrayRef pTouch();
	bool fMutable;
	};

// =================================================================================================
#pragma mark -
#pragma mark Map_CF

class Map_CF
:	public ZRef<CFDictionaryRef>
	{
	typedef ZRef<CFDictionaryRef> inherited;

public:
	typedef Val_CF Val;

	operator bool() const;

	Map_CF();
	Map_CF(const Map_CF& iOther);
	~Map_CF();
	Map_CF& operator=(const Map_CF& iOther);

	Map_CF(CFMutableDictionaryRef iOther);
	Map_CF(CFDictionaryRef iOther);

	Map_CF(const Adopt_T<CFMutableDictionaryRef>& iOther);
	Map_CF(const Adopt_T<CFDictionaryRef>& iOther);

	Map_CF& operator=(CFMutableDictionaryRef iOther);
	Map_CF& operator=(CFDictionaryRef iOther);

	Map_CF& operator=(const Adopt_T<CFMutableDictionaryRef>& iOther);
	Map_CF& operator=(const Adopt_T<CFDictionaryRef>& iOther);

// ZMap protocol
	using inherited::Clear;
	using inherited::Get;

	const ZQ<Val_CF> QGet(const string8& iName) const;
	const ZQ<Val_CF> QGet(CFStringRef iName) const;

	const Val_CF DGet(const Val_CF& iDefault, const string8& iName) const;
	const Val_CF DGet(const Val_CF& iDefault, CFStringRef iName) const;

	const Val_CF Get(const string8& iName) const;
	const Val_CF Get(CFStringRef iName) const;

	template <class S>
	const ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	const S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	Map_CF& Set(const string8& iName, const Val_CF& iVal);
	Map_CF& Set(CFStringRef iName, const Val_CF& iVal);

	template <class S>
	Map_CF& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, Val_CF(iVal)); }

	template <class S>
	Map_CF& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, Val_CF(iVal)); }

	Map_CF& Erase(const string8& iName);
	Map_CF& Erase(CFStringRef iName);

private:
	CFDictionaryRef pDictionary() const;
	CFMutableDictionaryRef pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Val_CF_h__
