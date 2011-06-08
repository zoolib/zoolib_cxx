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

#ifndef __ZVal_NS__
#define __ZVal_NS__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_NS.h"
#include "zoolib/ZRef_NS.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZValAccessors_Std.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>

// TODO Watch for CFStringRef issues in Cocotron and other non-Apple Cocoa implementations.
typedef const struct __CFString * CFStringRef;

namespace ZooLib {

class ZVal_NS;
class ZSeq_NS;
class ZMap_NS;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_NS

class ZVal_NS
:	public ZRef<NSObject>
	{
	typedef ZRef<NSObject> inherited;
public:
	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	operator bool() const;

	ZVal_NS();
	ZVal_NS(const ZVal_NS& iOther);
	~ZVal_NS();
	ZVal_NS& operator=(const ZVal_NS& iOther);

	ZVal_NS(id iVal)
	:	inherited(iVal)
		{}

	template <class S>
	ZVal_NS(const ZRef<S>& iVal)
	:	inherited(iVal)
		{}

	template <class S>
	ZVal_NS(const Adopt_T<S>& iVal)
	:	inherited(Adopt_T<NSObject*>(iVal.Get()))
		{}

	ZVal_NS(char iVal);
	ZVal_NS(signed char iVal);
	ZVal_NS(unsigned char iVal);
	ZVal_NS(wchar_t iVal);
	ZVal_NS(short iVal);
	ZVal_NS(unsigned short iVal);
	ZVal_NS(int iVal);
	ZVal_NS(unsigned int iVal);
	ZVal_NS(long iVal);
	ZVal_NS(unsigned long iVal);
	ZVal_NS(long long iVal);
	ZVal_NS(unsigned long long iVal);
	ZVal_NS(bool iVal);
	ZVal_NS(float iVal);
	ZVal_NS(double iVal);
	ZVal_NS(const char* iVal);
	ZVal_NS(const string8& iVal);
	ZVal_NS(const string16& iVal);
	ZVal_NS(CFStringRef iVal);

	ZVal_NS& operator=(id iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	ZVal_NS& operator=(const ZRef<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	ZVal_NS& operator=(const Adopt_T<S>& iVal)
		{
		inherited::operator=(Adopt_T<NSObject*>(iVal.Get()));
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

	ZVal_NS Get(const string8& iName) const;
	ZVal_NS Get(NSString* iName) const;
	ZVal_NS Get(size_t iIndex) const;

// Typename accessors
/// \cond DoxygenIgnore
	typedef NSString* NSStringPtr;

	ZMACRO_ZValAccessors_Decl_Std(ZVal_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, NSString, NSStringPtr)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Data, ZData_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Seq, ZSeq_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Map, ZMap_NS)
/// \endcond DoxygenIgnore
	};

template <class S>
ZQ<S> ZVal_NS::QGet() const
	{
	ZQ<S> result;
	if (NSObject* theNSObject = inherited::Get())
		{
		if ([theNSObject isKindOfClass:[NSValue class]])
			{
			if (0 == strcmp(@encode(S), [(NSValue*)theNSObject objCType]))
				[(NSValue*)theNSObject getValue:&result.OParam()];
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_NS

class ZSeq_NS
:	public ZRef<NSArray>
	{
	typedef ZRef<NSArray> inherited;

public:
	operator bool() const;

	ZSeq_NS();
	ZSeq_NS(const ZSeq_NS& iOther);
	~ZSeq_NS();
	ZSeq_NS& operator=(const ZSeq_NS& iOther);

	ZSeq_NS(NSMutableArray* iOther);
	ZSeq_NS(NSArray* iOther);

	ZSeq_NS(const Adopt_T<NSMutableArray>& iOther);
	ZSeq_NS(const Adopt_T<NSArray>& iOther);

	ZSeq_NS& operator=(NSMutableArray* iOther);
	ZSeq_NS& operator=(NSArray* iOther);

	ZSeq_NS& operator=(const Adopt_T<NSMutableArray>& iOther);
	ZSeq_NS& operator=(const Adopt_T<NSArray>& iOther);

// ZSeq protocol
	size_t Count() const;

	using inherited::Clear;
	using inherited::Get;

	ZQ<ZVal_NS> QGet(size_t iIndex) const;
	ZVal_NS DGet(const ZVal_NS& iDefault, size_t iIndex) const;
	ZVal_NS Get(size_t iIndex) const;

	template <class S>
	ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	S DGet(const S& iDefault, size_t iIndex) const
		{ return this->Get(iIndex).DGet<S>(iDefault); }

	template <class S>
	S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	ZSeq_NS& Set(size_t iIndex, const ZVal_NS& iVal);

	ZSeq_NS& Erase(size_t iIndex);

	ZSeq_NS& Insert(size_t iIndex, const ZVal_NS& iVal);

	ZSeq_NS& Append(const ZVal_NS& iVal);

private:
	NSArray* pArray() const;
	NSMutableArray* pTouch();
	bool fMutable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_NS

class ZMap_NS
:	public ZRef<NSDictionary>
	{
	typedef ZRef<NSDictionary> inherited;

public:
	operator bool() const;

	ZMap_NS();
	ZMap_NS(const ZMap_NS& iOther);
	~ZMap_NS();
	ZMap_NS& operator=(const ZMap_NS& iOther);

	ZMap_NS(NSMutableDictionary* iOther);
	ZMap_NS(NSDictionary* iOther);

	ZMap_NS(const Adopt_T<NSMutableDictionary>& iOther);
	ZMap_NS(const Adopt_T<NSDictionary>& iOther);

	ZMap_NS& operator=(NSMutableDictionary* iOther);
	ZMap_NS& operator=(NSDictionary* iOther);

	ZMap_NS& operator=(const Adopt_T<NSMutableDictionary>& iOther);
	ZMap_NS& operator=(const Adopt_T<NSDictionary>& iOther);

// ZMap protocol
	using inherited::Clear;
	using inherited::Get;

	ZQ<ZVal_NS> QGet(const string8& iName) const;
	ZQ<ZVal_NS> QGet(NSString* iName) const;
	ZQ<ZVal_NS> QGet(CFStringRef iName) const;

	ZVal_NS DGet(const ZVal_NS& iDefault, const string8& iName) const;
	ZVal_NS DGet(const ZVal_NS& iDefault, NSString* iName) const;
	ZVal_NS DGet(const ZVal_NS& iDefault, CFStringRef iName) const;

	ZVal_NS Get(const string8& iName) const;
	ZVal_NS Get(NSString* iName) const;
	ZVal_NS Get(CFStringRef iName) const;

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	ZQ<S> QGet(NSString* iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S DGet(const S& iDefault, NSString* iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	S Get(NSString* iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	ZMap_NS& Set(const string8& iName, const ZVal_NS& iVal);
	ZMap_NS& Set(NSString* iName, const ZVal_NS& iVal);
	ZMap_NS& Set(CFStringRef iName, const ZVal_NS& iVal);

	template <class S>
	ZMap_NS& Set(const string8& iName, const ZVal_NS& iVal)
		{ return this->Set(iName, ZVal_NS(iVal)); }

	template <class S>
	ZMap_NS& Set(NSString* iName, const ZVal_NS& iVal)
		{ return this->Set(iName, ZVal_NS(iVal)); }

	template <class S>
	ZMap_NS& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, ZVal_NS(iVal)); }

	ZMap_NS& Erase(const string8& iName);
	ZMap_NS& Erase(NSString* iName);
	ZMap_NS& Erase(CFStringRef iName);

private:
	NSDictionary* pDictionary() const;
	NSMutableDictionary* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZVal_NS__
