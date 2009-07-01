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
#include "zoolib/ZHandle_T.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZRef.h"
#include "zoolib/ZUnicodeString.h"
#include "zoolib/ZVal_T.h"
#include "zoolib/ZValAccessors_Std.h"

#include <vector>

#include "PIActions.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZPhotoshop {

class Value;
class List;
class Map;
class Spec;

typedef DescriptorClassID ClassID;
typedef DescriptorKeyID KeyID;
typedef DescriptorFormID FormID;

// =================================================================================================
#pragma mark -
#pragma mark * UnitFloat

struct UnitFloat
	{
	UnitFloat() {}
	UnitFloat(DescriptorUnitID iUnitID, double iValue)
	:	fUnitID(iUnitID),
		fValue(iValue)
		{}

	DescriptorUnitID fUnitID;
	double fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Enumerated

struct Enumerated
	{
	Enumerated() {}
	Enumerated(DescriptorEnumTypeID iEnumType, DescriptorEnumID iValue)
	:	fEnumType(iEnumType),
		fValue(iValue)
		{}

	DescriptorEnumTypeID fEnumType;
	DescriptorEnumID fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Value

class Value
:	public ZValR_T<Value>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Value,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValAccessors_Decl_Entry(Value, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(Value, String, string8)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Raw, ZMemoryBlock)
	ZMACRO_ZValAccessors_Decl_Entry(Value, UnitFloat, UnitFloat)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Enumerated, Enumerated)
	ZMACRO_ZValAccessors_Decl_Entry(Value, List, List)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Map, Map)
	ZMACRO_ZValAccessors_Decl_Entry(Value, Spec, Spec)

	operator operator_bool_type() const;

	Value();
	Value(const Value& iOther);
	~Value();
	Value& operator=(const Value& iOther);

	Value(int32 iVal);
	Value(double iVal);
	Value(bool iVal);
	Value(const string8& iVal);
	Value(const ZMemoryBlock& iVal);
	Value(UnitFloat iVal);
	Value(Enumerated iVal);
	Value(ClassID iType, const ZHandle_T<AliasHandle>& iHandle);
	Value(const List& iVal);
	Value(const Map& iVal);
	Value(const Spec& iVal);

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

private:
	void pRelease();
	void pCopy(const Value& iOther);

	union
		{
		bool fAsBool;
		int32 fAsInt32;
		double fAsDouble;
		ClassID fAsClassID;

		char fBytes[sizeof(string8)];
		} fData;

	KeyID fType;

	friend class List;
	friend class Map;
	};

// =================================================================================================
#pragma mark -
#pragma mark * List

class List
:	public ZValListR_T<List, Value>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(List,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	List();
	List(const List& iOther);
	~List();
	List& operator=(const List& iOther);

	List(PIActionList iOther);
	List(Adopt_t<PIActionList> iOther);

	List& operator=(PIActionList iOther);
	List& operator=(Adopt_t<PIActionList> iOther);

	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, Value& oVal) const;

	void Append(const Value& iVal);

	PIActionList GetActionList() const;

private:
	PIActionList fAL;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Map

class Map
:	public ZValMapR_T<Map, KeyID, Value>
,	public ZValMapR_T<Map, const string8&, Value>
,	public ZValMapR_T<Map, ZValMapIterator, Value>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(Map,
		operator_bool_generator_type, operator_bool_type);
public:
	ZMACRO_ZValMapAccessors_Using(Map, KeyID, Value)
	ZMACRO_ZValMapAccessors_Using(Map, const string8&, Value)
	ZMACRO_ZValMapAccessors_Using(Map, ZValMapIterator, Value)

	typedef ZValMapIterator const_iterator;

	operator operator_bool_type() const;

	Map();
	Map(const Map& iOther);
	~Map();
	Map& operator=(const Map& iOther);

	Map(PIActionDescriptor iOther);
	Map(Adopt_t<PIActionDescriptor> iOther);

	Map& operator=(PIActionDescriptor iOther);
	Map& operator=(Adopt_t<PIActionDescriptor> iOther);

	PIActionDescriptor* ParamO();

	const_iterator begin();
	const_iterator end();

	KeyID KeyOf(const_iterator iPropIter) const;
	std::string NameOf(const_iterator iPropIter) const;

	void Clear();

	bool QGet(KeyID iName, Value& oVal) const;
	bool QGet(const string8& iName, Value& oVal) const;
	bool QGet(const_iterator iName, Value& iVal);

	void Set(KeyID iName, const Value& iVal);
	void Set(const string8& iName, const Value& iVal);
	void Set(const_iterator iName, const Value& iVal);

	void Erase(KeyID iName);
	void Erase(const string8& iName);
	void Erase(const_iterator iName);

	PIActionDescriptor GetActionDescriptor() const;

private:
	size_t pCount() const;

	PIActionDescriptor fAD;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Spec

class Spec
	{
	struct Entry;

public:
	static Spec sClass(ClassID iClassID);
	static Spec sEnum(ClassID iClassID, DescriptorEnumTypeID iEnumType, DescriptorEnumID iValue);
	static Spec sEnum(ClassID iClassID, const Enumerated& iEnum);
	static Spec sIdentifier(ClassID iClassID, uint32 iIdentifier);
	static Spec sIndex(ClassID iClassID, uint32 iIndex);
	static Spec sName(ClassID iClassID, const string8& iName);
	static Spec sName(ClassID iClassID, const ZRef<ASZString>& iName);
	static Spec sOffset(ClassID iClassID, int32 iOffset);
	static Spec sProperty(ClassID iClassID, KeyID iKeyID);

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
	void pConvert(PIActionReference iRef, std::vector<Entry>& oEntries);
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
			char fBytes[sizeof(string8)];
			} fData;
		};

	std::vector<Entry> fEntries;
	};

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END

#endif // __ZPhotoshop_Val__
