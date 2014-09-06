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

#ifndef __ZVal_AppleEvent_h__
#define __ZVal_AppleEvent_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(AppleEvent)

#include "zoolib/TagVal.h"

#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZUtil_AppleEvent.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors.h"

#include <string>

namespace ZooLib {

class ZVal_AppleEvent;
class ZSeq_AppleEvent;
class ZMap_AppleEvent;

typedef ZVal_AppleEvent ZVal_AE;
typedef ZSeq_AppleEvent ZSeq_AE;
typedef ZMap_AppleEvent ZMap_AE;

std::string sAEKeywordAsString(AEKeyword iKeyword);

// =================================================================================================
// MARK: - ZVal_AppleEvent

class ZVal_AppleEvent
:	public AEDesc
	{
public:
	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	ZMACRO_operator_bool(ZVal_AppleEvent, operator_bool) const;

	void swap(ZVal_AppleEvent& iOther);

	ZVal_AppleEvent();
	ZVal_AppleEvent(const ZVal_AppleEvent& iOther);
	~ZVal_AppleEvent();
	ZVal_AppleEvent& operator=(const ZVal_AppleEvent& iOther);

	ZVal_AppleEvent(const AEDesc& iOther);
	ZVal_AppleEvent& operator=(const AEDesc& iOther);

	ZVal_AppleEvent(bool iVal);
	ZVal_AppleEvent(const std::string& iVal);
	ZVal_AppleEvent(const ZSeq_AppleEvent& iVal);
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
	const ZQ<S> QGet() const;

	template <class S>
	const S DGet(const S& iDefault) const
		{
		if (ZQ<S> result = this->QGet<S>())
			return result.Get();
		return iDefault;
		}

	template <class S>
	const S Get() const
		{ return this->DGet(S()); }

	template <class S>
	void Set(const S& iVal);

// Our protocol
	AEDesc& OParam();

	const ZVal_AppleEvent Get(const std::string& iName) const;
	const ZVal_AppleEvent Get(size_t iIndex) const;

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Int16, int16)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Int32, int32)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Int64, int64)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Bool, bool)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Float, float)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Double, double)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, String, std::string)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Seq, ZSeq_AppleEvent)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, Map, ZMap_AppleEvent)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, FSRef, FSRef)

#if ZCONFIG_SPI_Enabled(Carbon)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_AppleEvent, FSSpec, FSSpec)
#endif
/// \endcond DoxygenIgnore
	};

inline void swap(ZVal_AppleEvent& a, ZVal_AppleEvent& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - ZSeq_AppleEvent

class ZSeq_AppleEvent
:	public AEDescList
	{
public:
	typedef ZVal_AppleEvent Val;

	ZSeq_Any AsSeq_Any(const ZAny& iDefault) const;

	void swap(ZSeq_AppleEvent& iOther);

	ZSeq_AppleEvent();
	ZSeq_AppleEvent(const ZSeq_AppleEvent& iOther);
	~ZSeq_AppleEvent();
	ZSeq_AppleEvent& operator=(const ZSeq_AppleEvent& iOther);

	ZSeq_AppleEvent(const AEDescList& iOther);
	ZSeq_AppleEvent& operator=(const AEDescList& iOther);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	const ZQ<ZVal_AppleEvent> QGet(size_t iIndex) const;
	const ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, size_t iIndex) const;
	const ZVal_AppleEvent Get(size_t iIndex) const;

	ZSeq_AppleEvent& Set(size_t iIndex, const AEDesc& iVal);

	ZSeq_AppleEvent& Erase(size_t iIndex);

	ZSeq_AppleEvent& Insert(size_t iIndex, const AEDesc& iVal);

	ZSeq_AppleEvent& Append(const AEDesc& iVal);
	ZSeq_AppleEvent& Append(const ZVal_AppleEvent& iVal);
	};

inline void swap(ZSeq_AppleEvent& a, ZSeq_AppleEvent& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - ZMap_AppleEvent

class ZMap_AppleEvent
:	public AERecord
	{
public:
	typedef TagVal<size_t,ZMap_AppleEvent> Index_t;
	typedef ZVal_AppleEvent Val;

	ZMap_Any AsMap_Any(const ZAny& iDefault) const;

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

	const ZQ<ZVal_AppleEvent> QGet(AEKeyword iName) const;
	const ZQ<ZVal_AppleEvent> QGet(const std::string& iName) const;
	const ZQ<ZVal_AppleEvent> QGet(Index_t iPropIter) const;

	const ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, AEKeyword iName) const;
	const ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, const std::string& iName) const;
	const ZVal_AppleEvent DGet(const ZVal_AppleEvent& iDefault, Index_t iIndex) const;

	const ZVal_AppleEvent Get(AEKeyword iName) const;
	const ZVal_AppleEvent Get(const std::string& iName) const;
	const ZVal_AppleEvent Get(Index_t iIndex) const;

	ZMap_AppleEvent& Set(AEKeyword iName, const AEDesc& iVal);
	ZMap_AppleEvent& Set(const std::string& iName, const AEDesc& iVal);
	ZMap_AppleEvent& Set(Index_t iIndex, const AEDesc& iVal);

	ZMap_AppleEvent& Erase(AEKeyword iName);
	ZMap_AppleEvent& Erase(const std::string& iName);
	ZMap_AppleEvent& Erase(Index_t iIndex);

// Our protocol
	AERecord& OParam();

	const ZQ<ZVal_AppleEvent> QGetAttr(AEKeyword iName) const;
	const ZVal_AppleEvent DGetAttr(const ZVal_AppleEvent& iDefault, AEKeyword iName) const;
	const ZVal_AppleEvent GetAttr(AEKeyword iName) const;
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

inline void swap(ZMap_AppleEvent& a, ZMap_AppleEvent& b)
	{ a.swap(b); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(AppleEvent)

#endif // __ZVal_AppleEvent_h__
