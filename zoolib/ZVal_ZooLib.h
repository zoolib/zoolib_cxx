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

#ifndef __ZVal_ZooLib__
#define __ZVal_ZooLib__ 1

#include "zoolib/ZCompare.h"
#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZTName.h"
#include "zoolib/ZTypes.h"
#include "zoolib/ZVal_T.h"
#include "zoolib/ZValAccessors_Std.h"
#include "zoolib/ZValAccessors_ZooLib.h"

#include <stdexcept> // For runtime_error
#include <string>
#include <vector>

#ifndef ZCONFIG_Val_ZooLib_Packed
#	define ZCONFIG_Val_ZooLib_Packed 1
#endif

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Forward and external declarations

class ZStreamR;
class ZStreamW;

class ZVal_ZooLib;
class ZValList_ZooLib;
class ZValMap_ZooLib;

typedef ZVal_ZooLib ZVal_Z;
typedef ZValList_ZooLib ZValList_Z;
typedef ZValMap_ZooLib ZValMap_Z;

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib

#if ZCONFIG_Val_ZooLib_Packed
#	pragma pack(4)
#endif

class ZVal_ZooLib
:	public ZValR_T<ZVal_ZooLib>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_ZooLib,
		operator_bool_generator_type, operator_bool_type);

public:
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
	ZVal_ZooLib(const ZMemoryBlock& iVal);
	ZVal_ZooLib(const ZValList_ZooLib& iVal);
	ZVal_ZooLib(const ZValMap_ZooLib& iVal);
	ZVal_ZooLib(const ZRef<ZRefCountedWithFinalize>& iVal);
	ZVal_ZooLib(const void* iSource, size_t iSize);
	ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize);

// ZVal protocol
	void Clear();

	template <class S>
	bool QGet_T(S& oResult) const;

	template <class S>
	void Set_T(const S& iVal);

// Comparison
	int Compare(const ZVal_ZooLib& iOther) const;
	bool operator==(const ZVal_ZooLib& iOther) const;
	bool operator<(const ZVal_ZooLib& iOther) const;

// Our protocol
	ZValList_ZooLib& MutableList();
	ZValMap_ZooLib& MutableMap();

	ZType TypeOf() const;

	void ToStream(const ZStreamW& iStreamW) const;

// Typename accessors
	ZMACRO_ZValAccessors_Decl_Std(ZVal_ZooLib)
	ZMACRO_ZValAccessors_Decl_ZooLib(ZVal_ZooLib)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, List, ZValList_ZooLib)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, Map, ZValMap_ZooLib)

// Backwards compatibility
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, Tuple, ZValMap_ZooLib)

private:
	int pUncheckedCompare(const ZVal_ZooLib& iOther) const;
	bool pUncheckedLess(const ZVal_ZooLib& iOther) const;
	bool pUncheckedEqual(const ZVal_ZooLib& iOther) const;

	void pRelease();
	void pCopy(const ZVal_ZooLib& iOther);
	void pFromStream(ZType iType, const ZStreamR& iStream);

	friend class ZValMap_ZooLib;
	
	// Data is stored in one of several ways.
	// * For POD data <= 8 bytes in length, we simply store the
	// data directly, using named fields of fData.
	// * For ZRectPOD and vector<ZVal_ZooLib> in fData we store a
	// pointer to a heap-allocated object, because they're 16 bytes
	// and probably 12 bytes in size, respectively.
	// * For ZValMap_ZooLib, ZRef and ZMemoryBlock we use placement-new and
	// explicit destructor invocation on fType.fBytes -- they're
	// all 4 bytes in size, but have constructors/destructors, so
	// we can't declare them in the union.

	// Finally, strings are funky. For short (<= 11 bytes) string
	// instances, we put the characters in fType.fBytes, and
	// store an encoded length in fType.fType.
	// If they're longer, we do placement new of a ZValMap_ZooLibString.
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
		void* fAs_Pointer;
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
#	pragma pack()
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib inlines

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

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib::Ex_IllegalType

class ZVal_ZooLib::Ex_IllegalType : public std::runtime_error
	{
public:
	Ex_IllegalType(int iType);
	int fType;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_ZooLib

class ZValList_ZooLib
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValList_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	operator operator_bool_type() const;

	void swap(ZValList_ZooLib& iOther);

	ZValList_ZooLib();
	ZValList_ZooLib(const ZValList_ZooLib& iOther);
	~ZValList_ZooLib();
	ZValList_ZooLib& operator=(const ZValList_ZooLib& iOther);

	explicit ZValList_ZooLib(const ZStreamR& iStreamR);

	ZValList_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton);
	ZValList_ZooLib(const std::vector<ZVal_ZooLib>& iOther);

// ZValList protocol
	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_ZooLib& oVal) const;
	ZVal_ZooLib DGet(size_t iIndex, const ZVal_ZooLib& iDefault) const;
	ZVal_ZooLib Get(size_t iIndex) const;
	const ZVal_ZooLib& RGet(size_t iIndex) const;

	void Set(size_t iIndex, const ZVal_ZooLib& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_ZooLib& iVal);

	void Append(const ZVal_ZooLib& iVal);

// Comparison
	int Compare(const ZValList_ZooLib& iOther) const;
	bool operator==(const ZValList_ZooLib& iOther) const;
	bool operator<(const ZValList_ZooLib& iOther) const;

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
#pragma mark -
#pragma mark * ZValList_ZooLib inlines

inline bool operator!=(const ZValList_ZooLib& l, const ZValList_ZooLib& r)
	{ return !(l == r); }

inline bool operator<=(const ZValList_ZooLib& l, const ZValList_ZooLib& r)
	{ return !(r < l); }

inline bool operator>(const ZValList_ZooLib& l, const ZValList_ZooLib& r)
	{ return r < l; }

inline bool operator>=(const ZValList_ZooLib& l, const ZValList_ZooLib& r)
	{ return !(l < r); }

template <> inline int sCompare_T(const ZValList_ZooLib& iL, const ZValList_ZooLib& iR)
	{ return iL.Compare(iR); }

template <typename OutputIterator, typename T>
inline void ZValList_ZooLib::GetVector_T(OutputIterator iIter, const T& iDummy) const
	{
	const std::vector<ZVal_ZooLib>& theVector = this->GetVector();
	for (std::vector<ZVal_ZooLib>::const_iterator i = theVector.begin(), theEnd = theVector.end();
		i != theEnd; ++i)
		{
		*iIter++ = (*i).Get_T<T>();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * NameVal

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
#pragma mark -
#pragma mark * ZValMap_ZooLib

/// Associative array mapping names to ZVal_ZooLibs.

class ZValMap_ZooLib
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	typedef std::vector<NameVal> PropList;
	typedef PropList::iterator const_iterator;

	operator operator_bool_type() const;

	void swap(ZValMap_ZooLib& iOther);

	ZValMap_ZooLib();
	ZValMap_ZooLib(const ZValMap_ZooLib& iOther);
	~ZValMap_ZooLib();
	ZValMap_ZooLib& operator=(const ZValMap_ZooLib& iOther);

	explicit ZValMap_ZooLib(const ZStreamR& iStreamR);
	ZValMap_ZooLib(ZRef<Rep> iRep);

// ZValMap protocol
	void Clear();

	bool QGet(const_iterator iPropIter, ZVal_ZooLib& oVal) const;
	bool QGet(const char* iPropName, ZVal_ZooLib& oVal) const;
	bool QGet(const ZTName& iPropName, ZVal_ZooLib& oVal) const;

	ZVal_ZooLib DGet(const_iterator iPropIter, const ZVal_ZooLib& iDefault) const;
	ZVal_ZooLib DGet(const char* iPropName, const ZVal_ZooLib& iDefault) const;
	ZVal_ZooLib DGet(const ZTName& iPropName, const ZVal_ZooLib& iDefault) const;

	ZVal_ZooLib Get(const_iterator iPropIter) const;
	ZVal_ZooLib Get(const char* iPropName) const;
	ZVal_ZooLib Get(const ZTName& iPropName) const;

	const ZVal_ZooLib& RGet(const_iterator iPropIter) const;
	const ZVal_ZooLib& RGet(const char* iPropName) const;
	const ZVal_ZooLib& RGet(const ZTName& iPropName) const;

	void Set(const_iterator iPropIter, const ZVal_ZooLib& iVal);
	void Set(const char* iPropName, const ZVal_ZooLib& iVal);
	void Set(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	void Erase(const_iterator iPropIter);
	void Erase(const char* iPropName);
	void Erase(const ZTName& iPropName);

// Comparison
	int Compare(const ZValMap_ZooLib& iOther) const;
	bool operator==(const ZValMap_ZooLib& iOther) const;
	bool operator<(const ZValMap_ZooLib& iOther) const;

// Our protocol
	ZVal_ZooLib& Mutable(const_iterator iPropIter);
	ZVal_ZooLib& Mutable(const char* iPropName);
	ZVal_ZooLib& Mutable(const ZTName& iPropName);

	const_iterator begin() const;
	const_iterator end() const;

	bool Empty() const;
	size_t Count() const;

	const ZTName& NameOf(const_iterator iPropIter) const;

	const_iterator IteratorOf(const char* iPropName) const;
	const_iterator IteratorOf(const ZTName& iPropName) const;

	bool Has(const char* iPropName) const;
	bool Has(const ZTName& iPropName) const;

	void ToStream(const ZStreamW& iStreamW) const;

protected:
	void pSet(const_iterator iPropIter, const ZVal_ZooLib& iVal);
	void pSet(const char* iPropName, const ZVal_ZooLib& iVal);
	void pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	ZVal_ZooLib* pFindOrAllocate(const char* iPropName);
	ZVal_ZooLib* pFindOrAllocate(const ZTName& iPropName);

	const ZVal_ZooLib* pLookupAddressConst(const_iterator iPropIter) const;
	const ZVal_ZooLib* pLookupAddressConst(const char* iPropName) const;
	const ZVal_ZooLib* pLookupAddressConst(const ZTName& iPropName) const;

	void pErase(const_iterator iPropIter);
	void pTouch();
	const_iterator pTouch(const_iterator iPropIter);

	static ZRef<Rep> sRepFromStream(const ZStreamR& iStreamR);

	ZRef<Rep> fRep;
	};

#undef ZMACRO_ZValMapAccessors_Decl_Entry
#undef ZMACRO_ZValMapAccessors_Decl

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib inlines

inline bool operator!=(const ZValMap_ZooLib& l, const ZValMap_ZooLib& r)
	{ return !(l == r); }

inline bool operator<=(const ZValMap_ZooLib& l, const ZValMap_ZooLib& r)
	{ return !(r < l); }

inline bool operator>(const ZValMap_ZooLib& l, const ZValMap_ZooLib& r)
	{ return r < l; }

inline bool operator>=(const ZValMap_ZooLib& l, const ZValMap_ZooLib& r)
	{ return !(l < r); }

template <> inline int sCompare_T(const ZValMap_ZooLib& iL, const ZValMap_ZooLib& iR)
	{ return iL.Compare(iR); }

NAMESPACE_ZOOLIB_END

// =================================================================================================
#pragma mark -
#pragma mark * std::swap

namespace std {

inline void swap(ZOOLIB_PREFIX::ZVal_ZooLib& a, ZOOLIB_PREFIX::ZVal_ZooLib& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZValList_ZooLib& a, ZOOLIB_PREFIX::ZValList_ZooLib& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZValMap_ZooLib& a, ZOOLIB_PREFIX::ZValMap_ZooLib& b)
	{ a.swap(b); }

} // namespace std

#endif // __ZVal_ZooLib__
