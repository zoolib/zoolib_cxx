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

#import <Foundation/NSArray.h>
#import <Foundation/NSDictionary.h>
//#import <Foundation/NSArray.h>

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_NS.h"
#include "zoolib/ZRef_NSObject.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors_Std.h"

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

	ZVal_NS(NSObject* iVal);
	ZVal_NS(const ZRef<NSObject>& iVal);
	ZVal_NS(const Adopt_T<NSObject>& iVal);

	ZVal_NS(int8 iVal);
	ZVal_NS(int16 iVal);
	ZVal_NS(int32 iVal);
	ZVal_NS(int64 iVal);
	ZVal_NS(bool iVal);
	ZVal_NS(float iVal);
	ZVal_NS(double iVal);
	ZVal_NS(const char* iVal);
	ZVal_NS(const string8& iVal);
	ZVal_NS(const string16& iVal);
	ZVal_NS(NSString* iVal);

	ZVal_NS(const ZData_NS& iVal);
	ZVal_NS(const ZSeq_NS& iVal);
	ZVal_NS(const ZMap_NS& iVal);

	explicit ZVal_NS(NSData* iVal);
	explicit ZVal_NS(NSArray* iVal);
	explicit ZVal_NS(NSDictionary* iVal);

	ZVal_NS& operator=(NSObject* iVal);
	ZVal_NS& operator=(const ZRef<NSObject>& iVal);
	ZVal_NS& operator=(const Adopt_T<NSObject>& iVal);

// ZVal protocol
	void Clear();

	template <class S>
	ZQ_T<S> QGet_T() const;

	template <class S>
	bool QGet_T(S& oVal) const
		{
		if (ZQ_T<S> theQ = this->QGet_T<S>())
			{
			oVal = theQ.Get();
			return true;
			}
		return false;
		}

	template <class S>
	S DGet_T(const S& iDefault) const
		{
		S result;
		if (this->QGet_T(result))
			return result;
		return iDefault;
		}

	template <class S>
	S Get_T() const
		{ return this->DGet_T(S()); }

	template <class S>
	void Set_T(const S& iVal);

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_Std(ZVal_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, NSString, ZRef<NSString>)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Data, ZData_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Seq, ZSeq_NS)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_NS, Map, ZMap_NS)
/// \endcond DoxygenIgnore
	};

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

	ZSeq_NS(const ZRef<NSMutableArray>& iOther);
	ZSeq_NS(const ZRef<NSArray>& iOther);

	ZSeq_NS(const Adopt_T<NSMutableArray>& iOther);
	ZSeq_NS(const Adopt_T<NSArray>& iOther);

	ZSeq_NS& operator=(NSMutableArray* iOther);
	ZSeq_NS& operator=(NSArray* iOther);

	ZSeq_NS& operator=(const ZRef<NSMutableArray>& iOther);
	ZSeq_NS& operator=(const ZRef<NSArray>& iOther);

	ZSeq_NS& operator=(const Adopt_T<NSMutableArray>& iOther);
	ZSeq_NS& operator=(const Adopt_T<NSArray>& iOther);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	ZQ_T<ZVal_NS> QGet(size_t iIndex) const;
	ZVal_NS DGet(const ZVal_NS& iDefault, size_t iIndex) const;
	ZVal_NS Get(size_t iIndex) const;

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

	ZMap_NS(const ZRef<NSMutableDictionary>& iOther);
	ZMap_NS(const ZRef<NSDictionary>& iOther);

	ZMap_NS(const Adopt_T<NSMutableDictionary>& iOther);
	ZMap_NS(const Adopt_T<NSDictionary>& iOther);

	ZMap_NS& operator=(NSMutableDictionary* iOther);
	ZMap_NS& operator=(NSDictionary* iOther);

	ZMap_NS& operator=(const ZRef<NSMutableDictionary>& iOther);
	ZMap_NS& operator=(const ZRef<NSDictionary>& iOther);

	ZMap_NS& operator=(const Adopt_T<NSMutableDictionary>& iOther);
	ZMap_NS& operator=(const Adopt_T<NSDictionary>& iOther);

// ZMap protocol
	void Clear();

	ZQ_T<ZVal_NS> QGet(const string8& iName) const;
	ZQ_T<ZVal_NS> QGet(NSString* iName) const;

	ZVal_NS DGet(const ZVal_NS& iDefault, const string8& iName) const;
	ZVal_NS DGet(const ZVal_NS& iDefault, NSString* iName) const;

	ZVal_NS Get(const string8& iName) const;
	ZVal_NS Get(NSString* iName) const;

	ZMap_NS& Set(const string8& iName, const ZVal_NS& iVal);
	ZMap_NS& Set(NSString* iName, const ZVal_NS& iVal);

	ZMap_NS& Erase(const string8& iName);
	ZMap_NS& Erase(NSString* iName);

private:
	NSDictionary* pDictionary() const;
	NSMutableDictionary* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZVal_NS__
