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

#ifndef __ZooLib_Apple_Val_NS_h__
#define __ZooLib_Apple_Val_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/UnicodeString.h"

#include "zoolib/Apple/Data_NS.h"
#include "zoolib/Apple/ZRef_NS.h"

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>

// TODO Watch for CFStringRef issues in Cocotron and other non-Apple Cocoa implementations.
typedef const struct __CFString * CFStringRef;

namespace ZooLib {

class Val_NS;
class Seq_NS;
class Map_NS;

// =================================================================================================
#pragma mark - Val_NS

class Val_NS
:	public ZRef<NSObject>
	{
	typedef ZRef<NSObject> inherited;
public:
	Val_NS();
	Val_NS(const Val_NS& iOther);
	~Val_NS();
	Val_NS& operator=(const Val_NS& iOther);

	Val_NS(NSObject* iVal)
	:	inherited(iVal)
		{}

	template <class S>
	Val_NS(const ZRef<S>& iVal)
	:	inherited(iVal)
		{}

	template <class S>
	Val_NS(const Adopt_T<S>& iVal)
	:	inherited(Adopt_T<NSObject*>(iVal.Get()))
		{}

	Val_NS(char iVal);
	Val_NS(signed char iVal);
	Val_NS(unsigned char iVal);
	Val_NS(wchar_t iVal);
	Val_NS(short iVal);
	Val_NS(unsigned short iVal);
	Val_NS(int iVal);
	Val_NS(unsigned int iVal);
	Val_NS(long iVal);
	Val_NS(unsigned long iVal);
	Val_NS(long long iVal);
	Val_NS(unsigned long long iVal);
	Val_NS(bool iVal);
	Val_NS(float iVal);
	Val_NS(double iVal);
	Val_NS(long double iVal);
	Val_NS(const char* iVal);
	Val_NS(const string8& iVal);
	Val_NS(const string16& iVal);
	Val_NS(CFStringRef iVal);

	Val_NS& operator=(id iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	Val_NS& operator=(const ZRef<S>& iVal)
		{
		inherited::operator=(iVal);
		return *this;
		}

	template <class S>
	Val_NS& operator=(const Adopt_T<S>& iVal)
		{
		inherited::operator=(Adopt_T<NSObject*>(iVal.Get()));
		return *this;
		}

// Val protocol
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

	const Val_NS Get(const string8& iName) const;
	const Val_NS Get(NSString* iName) const;
	const Val_NS Get(size_t iIndex) const;

	typedef __unsafe_unretained NSString* NSStringPtr;
	};

template <class S>
const ZQ<S> Val_NS::QGet() const
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
#pragma mark - Seq_NS

class Seq_NS
:	public ZRef<NSArray>
	{
	typedef ZRef<NSArray> inherited;

public:
	Seq_NS();
	Seq_NS(const Seq_NS& iOther);
	~Seq_NS();
	Seq_NS& operator=(const Seq_NS& iOther);

	Seq_NS(NSMutableArray* iOther);
	Seq_NS(NSArray* iOther);

	Seq_NS(const Adopt_T<NSMutableArray*>& iOther);
	Seq_NS(const Adopt_T<NSArray*>& iOther);

	Seq_NS& operator=(NSMutableArray* iOther);
	Seq_NS& operator=(NSArray* iOther);

	Seq_NS& operator=(const Adopt_T<NSMutableArray*>& iOther);
	Seq_NS& operator=(const Adopt_T<NSArray*>& iOther);

// ZSeq protocol
	size_t Size() const;

	using inherited::Clear;
	using inherited::Get;

	const ZQ<Val_NS> QGet(size_t iIndex) const;
	const Val_NS DGet(const Val_NS& iDefault, size_t iIndex) const;
	const Val_NS Get(size_t iIndex) const;

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, size_t iIndex) const
		{ return this->Get(iIndex).DGet<S>(iDefault); }

	template <class S>
	const S Get(size_t iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	Seq_NS& Set(size_t iIndex, const Val_NS& iVal);

	Seq_NS& Erase(size_t iIndex);

	Seq_NS& Insert(size_t iIndex, const Val_NS& iVal);

	Seq_NS& Append(const Val_NS& iVal);

private:
	NSArray* pArray() const;
	NSMutableArray* pTouch();
	bool fMutable;
	};

// =================================================================================================
#pragma mark - Map_NS

class Map_NS
:	public ZRef<NSDictionary>
	{
	typedef ZRef<NSDictionary> inherited;

public:
	Map_NS();
	Map_NS(const Map_NS& iOther);
	~Map_NS();
	Map_NS& operator=(const Map_NS& iOther);

	Map_NS(NSMutableDictionary* iOther);
	Map_NS(NSDictionary* iOther);

	Map_NS(const Adopt_T<NSMutableDictionary*>& iOther);
	Map_NS(const Adopt_T<NSDictionary*>& iOther);

	Map_NS& operator=(NSMutableDictionary* iOther);
	Map_NS& operator=(NSDictionary* iOther);

	Map_NS& operator=(const Adopt_T<NSMutableDictionary*>& iOther);
	Map_NS& operator=(const Adopt_T<NSDictionary*>& iOther);

// ZMap protocol
	using inherited::Clear;
	using inherited::Get;

	const ZQ<Val_NS> QGet(const string8& iName) const;
	const ZQ<Val_NS> QGet(NSString* iName) const;
	const ZQ<Val_NS> QGet(CFStringRef iName) const;

	const Val_NS DGet(const Val_NS& iDefault, const string8& iName) const;
	const Val_NS DGet(const Val_NS& iDefault, NSString* iName) const;
	const Val_NS DGet(const Val_NS& iDefault, CFStringRef iName) const;

	const Val_NS Get(const string8& iName) const;
	const Val_NS Get(NSString* iName) const;
	const Val_NS Get(CFStringRef iName) const;

	template <class S>
	const ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(NSString* iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(CFStringRef iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, NSString* iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, CFStringRef iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	const S Get(NSString* iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	const S Get(CFStringRef iName) const
		{ return this->Get(iName).Get<S>(); }

	Map_NS& Set(const string8& iName, const Val_NS& iVal);
	Map_NS& Set(NSString* iName, const Val_NS& iVal);
	Map_NS& Set(CFStringRef iName, const Val_NS& iVal);

	template <class S>
	Map_NS& Set(const string8& iName, const S& iVal)
		{ return this->Set(iName, Val_NS(iVal)); }

	template <class S>
	Map_NS& Set(NSString* iName, const Val_NS& iVal)
		{ return this->Set(iName, Val_NS(iVal)); }

	template <class S>
	Map_NS& Set(CFStringRef iName, const S& iVal)
		{ return this->Set(iName, Val_NS(iVal)); }

	Map_NS& Erase(const string8& iName);
	Map_NS& Erase(NSString* iName);
	Map_NS& Erase(CFStringRef iName);

private:
	NSDictionary* pDictionary() const;
	NSMutableDictionary* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_Val_NS_h__
