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

#ifndef __ZPhotoshop_Val_h__
#define __ZPhotoshop_Val_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZTagVal.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors_Std.h"

#include "zoolib/photoshop/ZPhotoshop.h"
#include "zoolib/photoshop/ZPhotoshop_FileRef.h"

#include <vector>

#include "PIActions.h"

namespace ZooLib {
namespace ZPhotoshop {

// =================================================================================================
// MARK: - ZPhotoshop

using std::string;
using std::vector;

typedef ZData_Any Data;

class Map;
class Seq;
class Spec;
class Val;

typedef DescriptorEnumID EnumID;
typedef DescriptorEnumTypeID EnumTypeID;
typedef DescriptorFormID FormID;
typedef DescriptorKeyID KeyID;
typedef DescriptorTypeID TypeID;
typedef DescriptorUnitID UnitID;

// =================================================================================================
// MARK: - Public utilities

bool sAsRuntimeTypeID(const string8& iString, TypeID& oTypeID);

bool sFromRuntimeTypeID(TypeID iTypeID, string8& oString);

// =================================================================================================
// MARK: - ClassID

struct ClassID
	{
	ClassID() : fDCI(0)	{}

	ClassID(const ClassID& iOther) : fDCI(iOther.fDCI) {}

	ClassID(DescriptorClassID iDCI) : fDCI(iDCI) {}

	ClassID(const string8& iName);

	DescriptorClassID GetDCI() const { return fDCI; }

private:
	DescriptorClassID fDCI;
	};

// =================================================================================================
// MARK: - UnitFloat

struct UnitFloat
	{
	UnitFloat() : fUnitID(0), fValue(0.0) {}

	UnitFloat(UnitID iUnitID, double iValue)
	:	fUnitID(iUnitID),
		fValue(iValue)
		{}

	UnitID GetUnitID() const { return fUnitID; }
	double GetValue() const { return fValue; }

	UnitID fUnitID;
	double fValue;
	};

// =================================================================================================
// MARK: - Enumerated

struct Enumerated
	{
	Enumerated() : fEnumType(0), fValue(0) {}

	Enumerated(EnumTypeID iEnumType, EnumID iValue) : fEnumType(iEnumType), fValue(iValue) {}

	Enumerated(EnumTypeID iEnumType, const string8& iValue);

	Enumerated(const string8& iEnumType, EnumID iValue);

	Enumerated(const string8& iEnumType, const string8& iValue);

	EnumTypeID fEnumType;
	EnumID fValue;
	};

// =================================================================================================
// MARK: - Spec

class Spec
	{
	struct Entry;

public:
	ZMACRO_operator_bool(Spec, operator_bool) const;

	void swap(Spec& iOther);

	static Spec sClass(ClassID iClassID);
	static Spec sEnum(ClassID iClassID, const Enumerated& iEnum);
	static Spec sIdentifier(ClassID iClassID, uint32 iIdentifier);
	static Spec sIndex(ClassID iClassID, uint32 iIndex);
	static Spec sName(ClassID iClassID, const string8& iName);
	static Spec sName(ClassID iClassID, const ZRef<ASZString>& iName);
	static Spec sOffset(ClassID iClassID, int32 iOffset);
	static Spec sProperty(ClassID iClassID, KeyID iKeyID);
	static Spec sProperty(ClassID iClassID, const string8& iName);

	Spec();
	Spec(const Spec& iOther);
	~Spec();
	Spec& operator=(const Spec& iOther);

	Spec(const Entry& iEntry);

	Spec(PIActionReference iOther);
	Spec(Adopt_T<PIActionReference> iOther);

	Spec& operator=(PIActionReference iOther);
	Spec& operator=(Adopt_T<PIActionReference> iOther);

	Spec operator+(const Spec& iOther);
	Spec& operator+=(const Spec& iOther);

	const ZQ<Map> QGet() const;

	PIActionReference MakeRef() const;

private:
	static void spConvert(PIActionReference iRef, vector<Entry>& oEntries);

	struct Entry
		{
		Entry();
		Entry(const Entry& iOther);
		~Entry();
		Entry& operator=(const Entry& iOther);

		static Entry sClass(ClassID iClassID);
		static Entry sEnum(ClassID iClassID, const Enumerated& iEnum);
		static Entry sIdentifier(ClassID iClassID, uint32 iIdentifier);
		static Entry sIndex(ClassID iClassID, uint32 iIndex);
		static Entry sName(ClassID iClassID, const string8& iName);
		static Entry sName(ClassID iClassID, const ZRef<ASZString>& iName);
		static Entry sOffset(ClassID iClassID, int32 iOffset);
		static Entry sProperty(ClassID iClassID, KeyID iKeyID);

		void pRelease();
		void pCopyFrom(const Entry& iOther);

		DescriptorClassID fDCI;
		FormID fFormID;

		union
			{
			uint32 fAsIndex;
			uint32 fAsIdentifier;
			int32 fAsOffset;
			KeyID fAsProperty;
			char fBytes[1];
			// dummy fields to ensure the union is large enough.
			char spacer1[sizeof(Enumerated)];
			char spacer2[sizeof(string8)];
			} fData;
		};

	vector<Entry> fEntries;
	};

inline void swap(Spec& a, Spec& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - Val

class Val : public ZAny
	{
public:
	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	Val();
	Val(const Val& iOther);
	~Val();
	Val& operator=(const Val& iOther);

	Val(const ZAny& iOther);

	Val(::int32 iVal);
	Val(double iVal);
	Val(bool iVal);
	Val(const string8& iVal);
	Val(const Data& iVal);
	Val(const UnitFloat& iVal);
	Val(const Enumerated& iVal);
	Val(const FileRef& iVal);
	Val(const Seq& iVal);
	Val(const Map& iVal);
	Val(const Spec& iVal);

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Int32, int32)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Double, double)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Bool, bool)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, String, string8)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Data, Data)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, ClassID, ClassID)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, UnitFloat, UnitFloat)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Enumerated, Enumerated)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, FileRef, FileRef)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Seq, Seq)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Map, Map)
	ZMACRO_ZValAccessors_Decl_GetSet(Val, Spec, Spec)
/// \endcond DoxygenIgnore
	};

inline void swap(Val& a, Val& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - Seq

class Seq
	{
public:
	typedef ZPhotoshop::Val Val;

	ZSeq_Any AsSeq_Any(const ZAny& iDefault) const;

	void swap(Seq& iOther);

	Seq();
	Seq(const Seq& iOther);
	~Seq();
	Seq& operator=(const Seq& iOther);

	Seq(PIActionList iOther);
	Seq(Adopt_T<PIActionList> iOther);

	Seq& operator=(PIActionList iOther);
	Seq& operator=(Adopt_T<PIActionList> iOther);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	const ZQ<Val> QGet(size_t iIndex) const;
	const Val DGet(const Val& iDefault, size_t iIndex) const;
	const Val Get(size_t iIndex) const;

	template <class S>
	const ZQ<S> QGet(size_t iIndex) const
		{
		if (ZQ<Val> theQ = this->QGet(iIndex))
			return theQ->QGet<S>();
		return null;
		}

	template <class S>
	const S DGet(const S& iDefault, size_t iIndex) const
		{
		if (ZQ<S> theQ = this->QGet<S>(iIndex))
			return *theQ;
		return iDefault;
		}

	template <class S>
	const S Get(size_t iIndex) const
		{ return this->DGet<S>(S(), iIndex); }

	Seq& Append(const Val& iVal);

// Our protocol
	PIActionList& OParam();
	PIActionList IParam() const;

	PIActionList Orphan();

private:
	PIActionList fAL;
	};

inline void swap(Seq& a, Seq& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - Map

class Map
	{
public:
	typedef ZTagVal<size_t,Map> Index_t;
	typedef ZPhotoshop::Val Val;

	ZMap_Any AsMap_Any(const ZAny& iDefault) const;

	void swap(Map& iOther);

	Map();
	Map(const Map& iOther);
	~Map();
	Map& operator=(const Map& iOther);

	Map(KeyID iType, const Map& iOther);
	Map(const string8& iType, const Map& iOther);

	Map(KeyID iType, PIActionDescriptor iOther);
	Map(const string8& iType, PIActionDescriptor iOther);

	Map(KeyID iType, Adopt_T<PIActionDescriptor> iOther);
	Map(const string8& iType, Adopt_T<PIActionDescriptor> iOther);

// ZMap protocol
	bool IsEmpty() const;

	void Clear();

	const ZQ<Val> QGet(KeyID iKey) const;
	const ZQ<Val> QGet(const string8& iName) const;
	const ZQ<Val> QGet(Index_t iIndex) const;

	const Val DGet(const Val& iDefault, KeyID iKey) const;
	const Val DGet(const Val& iDefault, const string8& iName) const;
	const Val DGet(const Val& iDefault, Index_t iIndex) const;

	const Val Get(KeyID iKey) const;
	const Val Get(const string8& iName) const;
	const Val Get(Index_t iIndex) const;

	template <class S>
	const ZQ<S> QGet(KeyID iKey) const
		{ return this->Get(iKey).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(const string8& iName) const
		{ return this->Get(iName).QGet<S>(); }

	template <class S>
	const ZQ<S> QGet(const Index_t& iIndex) const
		{ return this->Get(iIndex).QGet<S>(); }

	template <class S>
	const S DGet(const S& iDefault, KeyID iKey) const
		{ return this->Get(iKey).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, const string8& iName) const
		{ return this->Get(iName).DGet<S>(iDefault); }

	template <class S>
	const S DGet(const S& iDefault, const Index_t& iIndex) const
		{ return this->Get(iIndex).DGet<S>(iDefault); }

	template <class S>
	const S Get(KeyID iKey) const
		{ return this->Get(iKey).Get<S>(); }

	template <class S>
	const S Get(const string8& iName) const
		{ return this->Get(iName).Get<S>(); }

	template <class S>
	const S Get(const Index_t& iIndex) const
		{ return this->Get(iIndex).Get<S>(); }

	Map& Set(KeyID iKey, const Val& iVal);
	Map& Set(const string8& iName, const Val& iVal);
	Map& Set(Index_t iIndex, const Val& iVal);

	Map& Erase(KeyID iKey);
	Map& Erase(const string8& iName);
	Map& Erase(Index_t iIndex);

// Our protocol
	PIActionDescriptor& OParam();
	PIActionDescriptor IParam() const;

	Index_t Begin() const;
	Index_t End() const;

	KeyID KeyOf(Index_t iIndex) const;
	string8 NameOf(Index_t iIndex) const;

	Index_t IndexOf(KeyID iKey) const;
	Index_t IndexOf(const string8& iName) const;

	Index_t IndexOf(const Map& iOther, const Index_t& iOtherIndex) const;

	KeyID GetType() const;

	PIActionDescriptor Orphan();

private:
	size_t pCount() const;

	KeyID fType;
	PIActionDescriptor fAD;
	};

inline void swap(Map& a, Map& b)
	{ a.swap(b); }


} // namespace ZPhotoshop
} // namespace ZooLib

#endif // __ZPhotoshop_Val_h__
