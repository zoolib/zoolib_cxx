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

#ifndef __ZVal_ZooLib_h__
#define __ZVal_ZooLib_h__ 1
#include "zconfig.h"

#include "zoolib/ZAny.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZGeomPOD.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZTName.h"
#include "zoolib/ZTypes.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/ZValAccessors_Std.h"

#include <stdexcept> // For runtime_error
#include <string>
#include <vector>

#ifndef ZCONFIG_Val_ZooLib_Packed
	#define ZCONFIG_Val_ZooLib_Packed 1
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - Forward and external declarations

class ZStreamR;
class ZStreamW;

class ZVal_ZooLib;
typedef ZData_Any ZData_ZooLib;
class ZSeq_ZooLib;
class ZMap_ZooLib;

typedef ZVal_ZooLib ZVal_Z;
typedef ZData_ZooLib ZData_Z;
typedef ZSeq_ZooLib ZSeq_Z;
typedef ZMap_ZooLib ZMap_Z;

// =================================================================================================

enum ZType
	{
	eZType_Null = 0,
	eZType_String = 1,
//	eZType_CString = 2,
	eZType_Int8 = 3,
	eZType_Int16 = 4,
	eZType_Int32 = 5,
	eZType_Int64 = 6,
	eZType_Float = 7,
	eZType_Double = 8,
	eZType_Bool = 9,
	eZType_Pointer = 10,
	eZType_Raw = 11,
	eZType_Tuple = 12,
	eZType_RefCounted = 13,
	eZType_Rect = 14,
	eZType_Point = 15,
	eZType_Region = 16,
	eZType_ID = 17,
	eZType_Vector = 18,
	eZType_Type = 19,
	eZType_Time = 20,
//##	eZType_Name = 21
	eZType_Max
	};

template <> inline int sCompare_T(const ZType& iL, const ZType& iR)
	{ return int(iL) - int(iR); }

const char* ZTypeAsString(ZType iType);

// =================================================================================================
// MARK: - ZVal_ZooLib

#if ZCONFIG_Val_ZooLib_Packed
	#pragma pack(4)
#endif

class ZVal_ZooLib
	{
public:
	static bool sQFromAny(const ZAny& iAny, ZVal_ZooLib& oVal);
	static ZVal_ZooLib sDFromAny(const ZVal_ZooLib& iDefault, const ZAny& iAny);
	static ZVal_ZooLib sFromAny(const ZAny& iAny);

	ZAny AsAny() const;
	ZAny DAsAny(const ZAny& iDefault) const;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	void swap(ZVal_ZooLib& iOther);

	class Ex_IllegalType;

	ZVal_ZooLib();
	~ZVal_ZooLib();
	ZVal_ZooLib(const ZVal_ZooLib& iOther);
	ZVal_ZooLib& operator=(const ZVal_ZooLib& iOther);

	explicit ZVal_ZooLib(const ZStreamR& iStreamR);
	ZVal_ZooLib(ZType iType, const ZStreamR& iStreamR);

	ZVal_ZooLib(const ZVal_ZooLib& iVal, bool iAsVector);
	ZVal_ZooLib(ZType iVal);
	ZVal_ZooLib(uint64 iVal, bool iIsID);
	ZVal_ZooLib(uint64 iVal);
	ZVal_ZooLib(int8 iVal);
	ZVal_ZooLib(int16 iVal);
	ZVal_ZooLib(int32 iVal);
	ZVal_ZooLib(int64 iVal);
	ZVal_ZooLib(bool iVal);
	ZVal_ZooLib(float iVal);
	ZVal_ZooLib(double iVal);
	ZVal_ZooLib(ZTime iVal);
	ZVal_ZooLib(void* iVal);
	ZVal_ZooLib(const ZRectPOD& iVal);
	ZVal_ZooLib(const ZPointPOD& iVal);
	ZVal_ZooLib(const char* iVal);
	ZVal_ZooLib(const std::string& iVal);
	ZVal_ZooLib(const ZData_ZooLib& iVal);
	ZVal_ZooLib(const ZSeq_ZooLib& iVal);
	ZVal_ZooLib(const ZMap_ZooLib& iVal);
	ZVal_ZooLib(const ZRef<ZCounted>& iVal);
	ZVal_ZooLib(const void* iSource, size_t iSize);
	ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize);

// ZVal protocol
	void Clear();

	template <class S>
	ZQ<S> QGet() const
		{
		S theVal;
		if (this->QGet<S>(theVal))
			return theVal;
		return null;
		}

	template <class S>
	bool QGet(S& oVal) const;

	template <class S>
	S DGet(const S& iDefault) const
		{
		if (ZQ<S> theQ = this->QGet<S>())
			return theQ.Get();
		return iDefault;
		}

	template <class S>
	S Get() const
		{ return this->DGet(S()); }

	template <class S>
	void Set(const S& iVal);

// Comparison
	int Compare(const ZVal_ZooLib& iOther) const;
	bool operator==(const ZVal_ZooLib& iOther) const;
	bool operator<(const ZVal_ZooLib& iOther) const;

// Our protocol
	ZSeq_ZooLib& MutableSeq();
	ZMap_ZooLib& MutableMap();

	ZType TypeOf() const;

	void ToStream(const ZStreamW& iStreamW) const;

// Typename accessors
/// \cond DoxygenIgnore
	ZMACRO_ZValAccessors_Decl_Std(ZVal_ZooLib)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, ID, uint64)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Type, ZType)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Time, ZTime)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Rect, ZRectPOD)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Point, ZPointPOD)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, RefCounted, ZRef<ZCounted>)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Pointer, VoidStar_t)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Data, ZData_ZooLib)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Seq, ZSeq_ZooLib)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Map, ZMap_ZooLib)

// Backwards compatibility
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Raw, ZData_ZooLib)
	ZMACRO_ZValAccessors_Decl_GetSet(ZVal_ZooLib, Tuple, ZMap_ZooLib)
/// \endcond DoxygenIgnore

private:
	int pUncheckedCompare(const ZVal_ZooLib& iOther) const;
	bool pUncheckedLess(const ZVal_ZooLib& iOther) const;
	bool pUncheckedEqual(const ZVal_ZooLib& iOther) const;

	void pRelease();
	void pCopy(const ZVal_ZooLib& iOther);
	void pFromStream(ZType iType, const ZStreamR& iStream);

	friend class ZMap_ZooLib;

	// Data is stored in one of several ways.
	// * For POD data <= 8 bytes in length, we simply store the
	// data directly, using named fields of fData.
	// * For ZRectPOD fData we store a pointer to a heap-allocated object,
	// * For most other types use placement-new and explicit destructor
	// invocation on fType.fBytes.

	// Finally, strings are funky. For short (<= 11 bytes) string
	// instances, we put the characters in fType.fBytes, and
	// store an encoded length in fType.fType.
	// If they're longer, we do placement new of a ZMap_ZooLibString.
public:
	static const int kBytesSize = 11;

	union Data
		{
		ZType fAs_Type;
		uint64 fAs_ID;
		int8 fAs_Int8;
		int16 fAs_Int16;
		int32 fAs_Int32;
		int64 fAs_Int64;
		bool fAs_Bool;
		float fAs_Float;
		double fAs_Double;
		double fAs_Time;
		VoidStar_t fAs_Pointer;
		ZPointPOD fAs_Point;

		ZRectPOD* fAs_Rect;
		};

	struct Type
		{
		char fBytes[kBytesSize];
		int8 fType;
		};

	struct FastCopy
		{
		char dummy[kBytesSize + 1];
		};

private:
	union
		{
		Data fData;
		Type fType;
		FastCopy fFastCopy;
		};
	};

#if ZCONFIG_Val_ZooLib_Packed
	#pragma pack()
#endif

// =================================================================================================
// MARK: - ZVal_ZooLib inlines

inline bool operator!=(const ZVal_ZooLib& l, const ZVal_ZooLib& r)
	{ return !(l == r); }

inline bool operator<=(const ZVal_ZooLib& l, const ZVal_ZooLib& r)
	{ return !(r < l); }

inline bool operator>(const ZVal_ZooLib& l, const ZVal_ZooLib& r)
	{ return r < l; }

inline bool operator>=(const ZVal_ZooLib& l, const ZVal_ZooLib& r)
	{ return !(l < r); }

template <> inline int sCompare_T(const ZVal_ZooLib& iL, const ZVal_ZooLib& iR)
	{ return iL.Compare(iR); }

inline void swap(ZVal_ZooLib& a, ZVal_ZooLib& b)
	{ a.swap(b); }

// =================================================================================================
// MARK: - ZVal_ZooLib::Ex_IllegalType

class ZVal_ZooLib::Ex_IllegalType : public std::runtime_error
	{
public:
	Ex_IllegalType(int iType);
	int fType;
	};

// =================================================================================================
// MARK: - ZSeq_ZooLib

class ZSeq_ZooLib
	{
	class Rep;

public:
	typedef ZVal_ZooLib Val_t;

	ZSeq_Any AsSeq_Any(const ZAny& iDefault) const;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZSeq_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	ZSeq_ZooLib();
	ZSeq_ZooLib(const ZSeq_ZooLib& iOther);
	~ZSeq_ZooLib();
	ZSeq_ZooLib& operator=(const ZSeq_ZooLib& iOther);

	explicit ZSeq_ZooLib(const ZStreamR& iStreamR);

	ZSeq_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton);
	ZSeq_ZooLib(const std::vector<ZVal_ZooLib>& iOther);

// ZSeq protocol
	size_t Count() const;

	void Clear();

	ZQ<ZVal_ZooLib> QGet(size_t iIndex) const;
	bool QGet(size_t iIndex, ZVal_ZooLib& oVal) const;
	ZVal_ZooLib DGet(const ZVal_ZooLib& iDefault, size_t iIndex) const;
	ZVal_ZooLib Get(size_t iIndex) const;
	const ZVal_ZooLib& RGet(size_t iIndex) const;

	ZSeq_ZooLib& Set(size_t iIndex, const ZVal_ZooLib& iVal);

	ZSeq_ZooLib& Erase(size_t iIndex);

	ZSeq_ZooLib& Insert(size_t iIndex, const ZVal_ZooLib& iVal);

	ZSeq_ZooLib& Append(const ZVal_ZooLib& iVal);

// Comparison
	int Compare(const ZSeq_ZooLib& iOther) const;
	bool operator==(const ZSeq_ZooLib& iOther) const;
	bool operator<(const ZSeq_ZooLib& iOther) const;

// Our protocol
	ZVal_ZooLib& Mutable(size_t iIndex);

	std::vector<ZVal_ZooLib>& MutableVector();

	const std::vector<ZVal_ZooLib>& GetVector() const;

	template <typename OutputIterator, typename T>
	void GetVector_T(OutputIterator iIter, const T& iDummy) const;

	void ToStream(const ZStreamW& iStreamW) const;

private:
	void pTouch();

	ZRef<Rep> fRep;
	};

// =================================================================================================
// MARK: - ZSeq_ZooLib inlines

inline bool operator!=(const ZSeq_ZooLib& l, const ZSeq_ZooLib& r)
	{ return !(l == r); }

inline bool operator<=(const ZSeq_ZooLib& l, const ZSeq_ZooLib& r)
	{ return !(r < l); }

inline bool operator>(const ZSeq_ZooLib& l, const ZSeq_ZooLib& r)
	{ return r < l; }

inline bool operator>=(const ZSeq_ZooLib& l, const ZSeq_ZooLib& r)
	{ return !(l < r); }

template <> inline int sCompare_T(const ZSeq_ZooLib& iL, const ZSeq_ZooLib& iR)
	{ return iL.Compare(iR); }

template <typename OutputIterator, typename T>
inline void ZSeq_ZooLib::GetVector_T(OutputIterator iIter, const T& iDummy) const
	{
	const std::vector<ZVal_ZooLib>& theVector = this->GetVector();
	for (std::vector<ZVal_ZooLib>::const_iterator i = theVector.begin(), theEnd = theVector.end();
		i != theEnd; ++i)
		{
		*iIter++ = (*i).Get<T>();
		}
	}

// =================================================================================================
// MARK: - NameVal

/// The type used to store name/property pairs.

struct NameVal
	{
	NameVal();
	NameVal(const NameVal& iOther);

public:
	NameVal(const ZStreamR& iStreamR);

	NameVal(const char* iName, const ZVal_ZooLib& iVal);
	NameVal(const ZTName& iName, const ZVal_ZooLib& iVal);

	NameVal(const char* iName);
	NameVal(const ZTName& iName);

	void ToStream(const ZStreamW& iStreamW) const;

	ZVal_ZooLib fVal;
	ZTName fName;
	};

// =================================================================================================
// MARK: - ZMap_ZooLib

/// Associative array mapping names to ZVal_ZooLibs.

class ZMap_ZooLib
	{
	class Rep;

public:
	typedef std::vector<NameVal> PropList;
	typedef PropList::iterator Index_t;
	typedef ZVal_ZooLib Val_t;

	ZMap_Any AsMap_Any(const ZAny& iDefault) const;

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMap_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	ZMap_ZooLib();
	ZMap_ZooLib(const ZMap_ZooLib& iOther);
	~ZMap_ZooLib();
	ZMap_ZooLib& operator=(const ZMap_ZooLib& iOther);

	explicit ZMap_ZooLib(const ZStreamR& iStreamR);
	ZMap_ZooLib(ZRef<Rep> iRep);

// ZMap protocol
	void Clear();

	template <class S>
	S Get(const string8& iName) const;

	template <class S>
	S Get(const Index_t& iIndex) const;

	ZVal_ZooLib* PGet(Index_t iIndex);
	ZVal_ZooLib* PGet(const char* iPropName);
	ZVal_ZooLib* PGet(const ZTName& iPropName);

	const ZVal_ZooLib* PGet(Index_t iIndex) const;
	const ZVal_ZooLib* PGet(const char* iPropName) const;
	const ZVal_ZooLib* PGet(const ZTName& iPropName) const;

	ZQ<ZVal_ZooLib> QGet(Index_t iIndex) const;
	ZQ<ZVal_ZooLib> QGet(const char* iPropName) const;
	ZQ<ZVal_ZooLib> QGet(const ZTName& iPropName) const;

	bool QGet(Index_t iIndex, ZVal_ZooLib& oVal) const;
	bool QGet(const char* iPropName, ZVal_ZooLib& oVal) const;
	bool QGet(const ZTName& iPropName, ZVal_ZooLib& oVal) const;

	ZVal_ZooLib DGet(const ZVal_ZooLib& iDefault, Index_t iIndex) const;
	ZVal_ZooLib DGet(const ZVal_ZooLib& iDefault, const char* iPropName) const;
	ZVal_ZooLib DGet(const ZVal_ZooLib& iDefault, const ZTName& iPropName) const;

	ZVal_ZooLib Get(Index_t iIndex) const;
	ZVal_ZooLib Get(const char* iPropName) const;
	ZVal_ZooLib Get(const ZTName& iPropName) const;

	ZMap_ZooLib& Set(Index_t iIndex, const ZVal_ZooLib& iVal);
	ZMap_ZooLib& Set(const char* iPropName, const ZVal_ZooLib& iVal);
	ZMap_ZooLib& Set(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	ZMap_ZooLib& Erase(Index_t iIndex);
	ZMap_ZooLib& Erase(const char* iPropName);
	ZMap_ZooLib& Erase(const ZTName& iPropName);

// Comparison
	int Compare(const ZMap_ZooLib& iOther) const;
	bool operator==(const ZMap_ZooLib& iOther) const;
	bool operator<(const ZMap_ZooLib& iOther) const;

// Our protocol
	ZVal_ZooLib& Mutable(Index_t iIndex);
	ZVal_ZooLib& Mutable(const char* iPropName);
	ZVal_ZooLib& Mutable(const ZTName& iPropName);

	Index_t Begin() const;
	Index_t End() const;

	bool Empty() const;
	size_t Count() const;

	const ZTName& NameOf(Index_t iIndex) const;

	Index_t IndexOf(const char* iPropName) const;
	Index_t IndexOf(const ZTName& iPropName) const;

	Index_t IndexOf(const ZMap_ZooLib& iOther, const Index_t& iOtherIndex) const;

	bool Has(const char* iPropName) const;
	bool Has(const ZTName& iPropName) const;

	void ToStream(const ZStreamW& iStreamW) const;

protected:
	void pSet(Index_t iIndex, const ZVal_ZooLib& iVal);
	void pSet(const char* iPropName, const ZVal_ZooLib& iVal);
	void pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	ZVal_ZooLib* pFindOrAllocate(const char* iPropName);
	ZVal_ZooLib* pFindOrAllocate(const ZTName& iPropName);

	void pErase(Index_t iIndex);
	void pTouch();
	Index_t pTouch(Index_t iIndex);

	static ZRef<Rep> sRepFromStream(const ZStreamR& iStreamR);

	ZRef<Rep> fRep;
	};

template <class S>
inline
S ZMap_Z::Get(const string8& iName) const
	{ return this->Get(iName).Get<S>(); }

template <class S>
inline
S ZMap_Z::Get(const Index_t& iIndex) const
	{ return this->Get(iIndex).Get<S>(); }

// =================================================================================================
// MARK: - ZMap_ZooLib inlines

inline bool operator!=(const ZMap_ZooLib& l, const ZMap_ZooLib& r)
	{ return !(l == r); }

inline bool operator<=(const ZMap_ZooLib& l, const ZMap_ZooLib& r)
	{ return !(r < l); }

inline bool operator>(const ZMap_ZooLib& l, const ZMap_ZooLib& r)
	{ return r < l; }

inline bool operator>=(const ZMap_ZooLib& l, const ZMap_ZooLib& r)
	{ return !(l < r); }

template <> inline int sCompare_T(const ZMap_ZooLib& iL, const ZMap_ZooLib& iR)
	{ return iL.Compare(iR); }

} // namespace ZooLib

#endif // __ZVal_ZooLib_h__
