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

#ifndef __ZVal_AppleEvent__
#define __ZVal_AppleEvent__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(AppleEvent)

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZUtil_AppleEvent.h"
#include "zoolib/ZVal_T.h"
#include "zoolib/ZValAccessors.h"
#include "zoolib/ZValAccessors_Mac.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

class ZVal_AppleEvent;
class ZValList_AppleEvent;
class ZValMap_AppleEvent;

typedef ZVal_AppleEvent ZVal_AE;
typedef ZValList_AppleEvent ZValList_AE;
typedef ZValMap_AppleEvent ZValMap_AE;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_AppleEvent

class ZVal_AppleEvent
:	public AEDesc
,	public ZValR_T<ZVal_AppleEvent>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_AppleEvent,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int16, int16)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int64, int64)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Float, float)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, String, std::string)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, List, ZValList_AppleEvent)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Map, ZValMap_AppleEvent)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, FSSpec, FSSpec)

	ZMACRO_ZValAccessors_Decl_Mac(ZVal_AppleEvent)

	operator operator_bool_type() const;

	ZVal_AppleEvent();
	ZVal_AppleEvent(const ZVal_AppleEvent& iOther);
	~ZVal_AppleEvent();
	ZVal_AppleEvent& operator=(const ZVal_AppleEvent& iOther);

	ZVal_AppleEvent(const AEDesc& iOther);
	ZVal_AppleEvent& operator=(const AEDesc& iOther);

	ZVal_AppleEvent(const bool& iVal);
	ZVal_AppleEvent(const std::string& iVal);

	AEDesc* ParamO();

	template <class T>
	ZVal_AppleEvent(const T& iVal)
		{
		const ZAEValRef_T<T> theValRef(iVal);
		::AECreateDesc(theValRef.sDescType, theValRef.Ptr(), theValRef.Size(), this);
		}

	template <class T, int D>
	explicit ZVal_AppleEvent(const ZAEValRef_T<T, D>& iValRef)
		{ ::AECreateDesc(iValRef.sDescType, iValRef.Ptr(), iValRef.Size(), this); }

	template <class T>
	ZVal_AppleEvent(DescType iDescType, const T& iVal)
		{ ::AECreateDesc(iDescType, &iVal, sizeof(iVal), this); }

	ZVal_AppleEvent(DescType iDescType, const void* iVal, size_t iSize)
		{ ::AECreateDesc(iDescType, iVal, iSize, this); }
	
	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_AppleEvent

class ZValList_AppleEvent
:	public AEDescList
,	public ZValListR_T<ZValList_AppleEvent, ZVal_AppleEvent>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_AppleEvent,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	ZValList_AppleEvent();
	ZValList_AppleEvent(const ZValList_AppleEvent& iOther);
	~ZValList_AppleEvent();
	ZValList_AppleEvent& operator=(const ZValList_AppleEvent& iOther);

	ZValList_AppleEvent(const AEDescList& iOther);
	ZValList_AppleEvent& operator=(const AEDescList& iOther);

	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_AppleEvent& oVal) const;

	void Set(size_t iIndex, const AEDesc& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const AEDesc& iVal);

	void Append(const AEDesc& iVal);
	void Append(const ZVal_AppleEvent& iVal);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_AppleEvent

class ZValMap_AppleEvent
:	public AERecord
,	public ZValMapR_T<ZValMap_AppleEvent, AEKeyword, ZVal_AppleEvent>
,	public ZValMapR_T<ZValMap_AppleEvent, const std::string&, ZVal_AppleEvent>
,	public ZValMapR_T<ZValMap_AppleEvent, ZValMapIterator, ZVal_AppleEvent>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_AppleEvent,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValMapAccessors_Using(ZValMap_AppleEvent, AEKeyword, ZVal_AppleEvent)
	ZMACRO_ZValMapAccessors_Using(ZValMap_AppleEvent, const std::string&, ZVal_AppleEvent)
	ZMACRO_ZValMapAccessors_Using(ZValMap_AppleEvent, ZValMapIterator, ZVal_AppleEvent)

	typedef ZValMapIterator const_iterator;

	operator operator_bool_type() const;

	ZValMap_AppleEvent();
	ZValMap_AppleEvent(const ZValMap_AppleEvent& iOther);
	~ZValMap_AppleEvent();
	ZValMap_AppleEvent& operator=(const ZValMap_AppleEvent& iOther);

	ZValMap_AppleEvent(const AERecord& iOther);
	ZValMap_AppleEvent& operator=(const AERecord& iOther);

	AERecord* ParamO();

	const_iterator begin();
	const_iterator end();

	AEKeyword KeyOf(const_iterator iPropIter) const;
	std::string NameOf(const_iterator iPropIter) const;

	void Clear();

	bool QGet(AEKeyword iName, ZVal_AppleEvent& oVal) const;
	bool QGet(const std::string& iName, ZVal_AppleEvent& oVal) const;
	bool QGet(const_iterator iPropIter, ZVal_AppleEvent& oVal) const;

	void Set(AEKeyword iName, const AEDesc& iVal);
	void Set(const std::string& iName, const AEDesc& iVal);
	void Set(const_iterator iPropIter, const AEDesc& iVal);

	void Erase(AEKeyword iName);
	void Erase(const std::string& iName);
	void Erase(const_iterator iPropIter);

	bool QGetAttr(AEKeyword iName, ZVal_AppleEvent& oVal) const;
	ZVal_AppleEvent DGetAttr(AEKeyword iName, const ZVal_AppleEvent& iDefault) const;
	ZVal_AppleEvent GetAttr(AEKeyword iName) const;
	void SetAttr(AEKeyword iName, const AEDesc& iVal);
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(AppleEvent)

#endif // __ZVal_AppleEvent__
