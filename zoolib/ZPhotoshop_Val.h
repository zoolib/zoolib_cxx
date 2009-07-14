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

#ifndef __ZPhotoshop_Val__
#define __ZPhotoshop_Val__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal.h"
#include "zoolib/ZValAccessors_Std.h"
#include "zoolib/ZValData_ZooLib.h"

#include <vector>

#include "PIActions.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop

namespace ZPhotoshop {

using std::string;

typedef ZValData_ZooLib Data;

class List;
class Map;
class Spec;
class Val;

typedef DescriptorClassID ClassID;
typedef DescriptorEnumID EnumID;
typedef DescriptorEnumTypeID EnumTypeID;
typedef DescriptorFormID FormID;
typedef DescriptorKeyID KeyID;
typedef DescriptorTypeID TypeID;
typedef DescriptorUnitID UnitID;

// =================================================================================================
#pragma mark -
#pragma mark * Public utilities

bool sAsRuntimeTypeID(const string8& iString, TypeID& oTypeID);

bool sFromRuntimeTypeID(TypeID iTypeID, string8& oString);

// =================================================================================================
#pragma mark -
#pragma mark * UnitFloat

struct UnitFloat
	{
	UnitFloat() {}
	UnitFloat(UnitID iUnitID, double iValue)
	:	fUnitID(iUnitID),
		fValue(iValue)
		{}

	UnitID fUnitID;
	double fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Enumerated

struct Enumerated
	{
	Enumerated()
	:	fEnumType(0),
		fValue(0)
		{}

	Enumerated(EnumTypeID iEnumType, EnumID iValue)
	:	fEnumType(iEnumType),
		fValue(iValue)
		{}

	Enumerated(EnumTypeID iEnumType, const string8& iValue);

	Enumerated(const string8& iEnumType, EnumID iValue);

	Enumerated(const string8& iEnumType, const string8& iValue);

	EnumTypeID fEnumType;
	EnumID fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * PSAlias

class PSAlias
	{
public:
	PSAlias();
	PSAlias(const PSAlias& iOther);

	~PSAlias();
	
	PSAlias& operator=(const PSAlias& iOther);

	PSAlias(Handle iHandle);
	PSAlias(Adopt_t<Handle> iHandle);

	PSAlias& operator=(Handle iHandle);
	PSAlias& operator=(Adopt_t<Handle> iHandle);

	Handle Get() const;
	Handle Orphan();
	Handle& OParam();

	size_t Size() const;

	string8 AsString() const;

private:
	Handle fHandle;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Spec

class Spec
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Spec,
		operator_bool_generator_type, operator_bool_type);

	struct Entry;

public:
	operator operator_bool_type() const;

	void swap(Spec& iOther);

	static Spec sClass(ClassID iClassID);
	static Spec sEnum(ClassID iClassID, EnumTypeID iEnumType, EnumID iValue);
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
	Spec(Adopt_t<PIActionReference> iOther);

	Spec& operator=(PIActionReference iOther);
	Spec& operator=(Adopt_t<PIActionReference> iOther);

	Spec operator+(const Spec& iOther);
	Spec& operator+=(const Spec& iOther);

	Map Get() const;

	PIActionReference MakeRef() const;

private:
	static void spConvert(PIActionReference iRef, std::vector<Entry>& oEntries);

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

		ClassID fClassID;
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

	std::vector<Entry> fEntries;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Val

class Val
:	public ZValR_T<Val>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Val,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	void swap(Val& iOther);

	Val();
	Val(const Val& iOther);
	~Val();
	Val& operator=(const Val& iOther);

	Val(int32 iVal);
	Val(double iVal);
	Val(bool iVal);
	Val(const string8& iVal);
	Val(const Data& iVal);
	Val(UnitFloat iVal);
	Val(Enumerated iVal);
	Val(const PSAlias& iVal);
	Val(const List& iVal);
	Val(const Map& iVal);
	Val(const Spec& iVal);

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(Val, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(Val, String, string8)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Data, Data)
	ZMACRO_ZValAccessors_Decl_Entry(Val, UnitFloat, UnitFloat)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Enumerated, Enumerated)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Alias, PSAlias)
	ZMACRO_ZValAccessors_Decl_Entry(Val, List, List)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Map, Map)
	ZMACRO_ZValAccessors_Decl_Entry(Val, Spec, Spec)

private:
	void pRelease();
	void pCopy(const Val& iOther);

	union
		{
		bool fAsBool;
		int32 fAsInt32;
		double fAsDouble;
		ClassID fAsClassID;

		char fBytes[1];

		char spacer1[sizeof(string8)];
		char spacer2[sizeof(Data)];
		char spacer3[sizeof(UnitFloat)];
		char spacer4[sizeof(Enumerated)];
		char spacer5[sizeof(Spec)];
		} fData;

	KeyID fType;

	friend class List;
	friend class Map;
	};

// =================================================================================================
#pragma mark -
#pragma mark * List

class List
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(List,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	void swap(List& iOther);

	List();
	List(const List& iOther);
	~List();
	List& operator=(const List& iOther);

	List(PIActionList iOther);
	List(Adopt_t<PIActionList> iOther);

	List& operator=(PIActionList iOther);
	List& operator=(Adopt_t<PIActionList> iOther);

// ZValList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, Val& oVal) const;
	Val DGet(size_t iIndex, const Val& iDefault) const;
	Val Get(size_t iIndex) const;

	void Append(const Val& iVal);

// Our protocol
	PIActionList GetActionList() const;

private:
	PIActionList fAL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Map

class Map
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Map,
		operator_bool_generator_type, operator_bool_type);
public:
	typedef ZValMapIndex_T<Map> Index_t;

	operator operator_bool_type() const;

	void swap(Map& iOther);

	Map();
	Map(const Map& iOther);
	~Map();
	Map& operator=(const Map& iOther);

	Map(KeyID iType, PIActionDescriptor iOther);
	Map(const string8& iType, PIActionDescriptor iOther);

	Map(KeyID iType, Adopt_t<PIActionDescriptor> iOther);
	Map(const string8& iType, Adopt_t<PIActionDescriptor> iOther);

// ZValMap protocol
	void Clear();

	bool QGet(KeyID iKey, Val& oVal) const;
	bool QGet(const string8& iName, Val& oVal) const;
	bool QGet(Index_t iIndex, Val& iVal) const;

	Val DGet(KeyID iKey, const Val& iDefault) const;
	Val DGet(const string8& iName, const Val& iDefault) const;
	Val DGet(Index_t iIndex, const Val& iDefault) const;

	Val Get(KeyID iKey) const;
	Val Get(const string8& iName) const;
	Val Get(Index_t iIndex) const;

	void Set(KeyID iKey, const Val& iVal);
	void Set(const string8& iName, const Val& iVal);
	void Set(Index_t iIndex, const Val& iVal);

	void Erase(KeyID iKey);
	void Erase(const string8& iName);
	void Erase(Index_t iIndex);

// Our protocol
	PIActionDescriptor& OParam();
	PIActionDescriptor IParam() const;

	Index_t begin() const;
	Index_t end() const;

	KeyID KeyOf(Index_t iIndex) const;
	string8 NameOf(Index_t iIndex) const;

	Index_t IndexOf(KeyID iKey) const;
	Index_t IndexOf(const string8& iName) const;

	KeyID GetType() const;

	PIActionDescriptor Orphan();

private:
	size_t pCount() const;

	KeyID fType;
	PIActionDescriptor fAD;
	};

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * std::swap

namespace std {

inline void swap(ZOOLIB_PREFIX::ZPhotoshop::Spec& a, ZOOLIB_PREFIX::ZPhotoshop::Spec& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZPhotoshop::Val& a, ZOOLIB_PREFIX::ZPhotoshop::Val& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZPhotoshop::List& a, ZOOLIB_PREFIX::ZPhotoshop::List& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZPhotoshop::Map& a, ZOOLIB_PREFIX::ZPhotoshop::Map& b)
	{ a.swap(b); }

} // namespace std

#endif // __ZPhotoshop_Val__
