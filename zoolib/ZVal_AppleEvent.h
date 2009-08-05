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
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

class ZVal_AppleEvent;
class ZList_AppleEvent;
class ZMap_AppleEvent;

typedef ZVal_AppleEvent ZVal_AE;
typedef ZList_AppleEvent ZList_AE;
typedef ZMap_AppleEvent ZMap_AE;

std::string sAEKeywordAsString(AEKeyword iKeyword);

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
	ZVal_Any AsVal_Any(const ZVal_Any& iDefault) const;

	operator operator_bool_type() const;

	void swap(ZVal_AppleEvent& iOther);

	ZVal_AppleEvent();
	ZVal_AppleEvent(const ZVal_AppleEvent& iOther);
	~ZVal_AppleEvent();
	ZVal_AppleEvent& operator=(const ZVal_AppleEvent& iOther);

	ZVal_AppleEvent(const AEDesc& iOther);
	ZVal_AppleEvent& operator=(const AEDesc& iOther);

	ZVal_AppleEvent(bool iVal);
	ZVal_AppleEvent(const std::string& iVal);
	ZVal_AppleEvent(const ZList_AppleEvent& iVal);
	ZVal_AppleEvent(const ZMap_AppleEvent& iVal);

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
	
// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oVal) const;

	template <class S>
	void Set_T(const S& iVal);

// Our protocol
	AEDesc& OParam();

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int16, int16)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int32, int32)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Int64, int64)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Bool, bool)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Float, float)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Double, double)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, String, std::string)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, List, ZList_AppleEvent)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, Map, ZMap_AppleEvent)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, FSSpec, FSSpec)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_AppleEvent, FSRef, FSRef)
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZList_AppleEvent

class ZList_AppleEvent
:	public AEDescList
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_AppleEvent,
		operator_bool_generator_type, operator_bool_type);

public:
	ZList_Any AsList_Any(const ZVal_Any& iDefault) const;

	operator operator_bool_type() const;

	void swap(ZList_AppleEvent& iOther);

	ZList_AppleEvent();
	ZList_AppleEvent(const ZList_AppleEvent& iOther);
	~ZList_AppleEvent();
	ZList_AppleEvent& operator=(const ZList_AppleEvent& iOther);

	ZList_AppleEvent(const AEDescList& iOther);
	ZList_AppleEvent& operator=(const AEDescList& iOther);

// ZList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_AppleEvent& oVal) const;
	ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, size_t iIndex) const;
	ZVal_AppleEvent Get(size_t iIndex) const;

	void Set(size_t iIndex, const AEDesc& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const AEDesc& iVal);

	void Append(const AEDesc& iVal);
	void Append(const ZVal_AppleEvent& iVal);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_AppleEvent

class ZMap_AppleEvent
:	public AERecord
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_AppleEvent,
		operator_bool_generator_type, operator_bool_type);

public:
	typedef ZMapIndex_T<ZMap_AppleEvent> Index_t;

	ZMap_Any AsMap_Any(const ZVal_Any& iDefault) const;

	operator operator_bool_type() const;

	void swap(ZMap_AppleEvent& iOther);

	ZMap_AppleEvent();
	ZMap_AppleEvent(const ZMap_AppleEvent& iOther);
	~ZMap_AppleEvent();
	ZMap_AppleEvent& operator=(const ZMap_AppleEvent& iOther);

	ZMap_AppleEvent(AEKeyword iType);
	ZMap_AppleEvent(AEKeyword iType, const AERecord& iOther);
	ZMap_AppleEvent(AEKeyword iType, const ZMap_AppleEvent& iOther);

	ZMap_AppleEvent(const AERecord& iOther);
	ZMap_AppleEvent& operator=(const AERecord& iOther);

// ZMap protocol
	void Clear();

	bool QGet(AEKeyword iName, ZVal_AppleEvent& oVal) const;
	bool QGet(const std::string& iName, ZVal_AppleEvent& oVal) const;
	bool QGet(Index_t iPropIter, ZVal_AppleEvent& oVal) const;

	ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, AEKeyword iName) const;
	ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, const std::string& iName) const;
	ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, Index_t iIndex) const;

	ZVal_AppleEvent Get(AEKeyword iName) const;
	ZVal_AppleEvent Get(const std::string& iName) const;
	ZVal_AppleEvent Get(Index_t iIndex) const;

	void Set(AEKeyword iName, const AEDesc& iVal);
	void Set(const std::string& iName, const AEDesc& iVal);
	void Set(Index_t iIndex, const AEDesc& iVal);

	void Erase(AEKeyword iName);
	void Erase(const std::string& iName);
	void Erase(Index_t iIndex);

// Our protocol
	AERecord& OParam();

	bool QGetAttr(AEKeyword iName, ZVal_AppleEvent& oVal) const;
	ZVal_AppleEvent DGetAttr(const ZVal_AppleEvent& iDefault, AEKeyword iName) const;
	ZVal_AppleEvent GetAttr(AEKeyword iName) const;
	void SetAttr(AEKeyword iName, const AEDesc& iVal);

	AEKeyword GetType() const;

	Index_t Begin() const;
	Index_t End() const;

	AEKeyword KeyOf(Index_t iIndex) const;
	std::string NameOf(Index_t iIndex) const;

	Index_t IndexOf(AEKeyword iName) const;
	Index_t IndexOf(const std::string& iName) const;

	Index_t IndexOf(const ZMap_AppleEvent& iOther, const Index_t& iOtherIndex) const;

private:
	size_t pCount() const;
	AEKeyword pKeyOf(size_t iIndex) const;
	};

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * std::swap

namespace std {

inline void swap(ZOOLIB_PREFIX::ZVal_AppleEvent& a, ZOOLIB_PREFIX::ZVal_AppleEvent& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZList_AppleEvent& a, ZOOLIB_PREFIX::ZList_AppleEvent& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZMap_AppleEvent& a, ZOOLIB_PREFIX::ZMap_AppleEvent& b)
	{ a.swap(b); }

} // namespace std

#endif // ZCONFIG_SPI_Enabled(AppleEvent)

#endif // __ZVal_AppleEvent__
