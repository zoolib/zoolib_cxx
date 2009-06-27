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

#ifndef ZCONFIG_Tuple_PackedTVs
#	define ZCONFIG_Tuple_PackedTVs 1
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

/// Holds a value which can be one of several different types.

#if ZCONFIG_Tuple_PackedTVs
#	pragma pack(4)
#endif

class ZVal_ZooLib
:	public ZValR_T<ZVal_ZooLib>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZVal_ZooLib,
		operator_bool_generator_type, operator_bool_type);

public:
	ZMACRO_ZValAccessors_Decl_Std(ZVal_ZooLib)
	ZMACRO_ZValAccessors_Decl_ZooLib(ZVal_ZooLib)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, Vector, std::vector<ZVal_ZooLib>)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, Tuple, ZValMap_ZooLib)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, List, ZValList_ZooLib)
	ZMACRO_ZValAccessors_Decl_Entry(ZVal_ZooLib, Map, ZValMap_ZooLib)

	class Ex_IllegalType;
	
/** \name Canonical Methods
*/	//@{
	ZVal_ZooLib();
	~ZVal_ZooLib();
	ZVal_ZooLib(const ZVal_ZooLib& iOther);
	ZVal_ZooLib& operator=(const ZVal_ZooLib& iOther);
	//@}


/** \name Constructors taking various types
*/	//@{
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
	ZVal_ZooLib(const ZValMap_ZooLib& iVal);
	ZVal_ZooLib(const ZRef<ZRefCountedWithFinalize>& iVal);
	ZVal_ZooLib(const ZMemoryBlock& iVal);
	ZVal_ZooLib(const void* iSource, size_t iSize);
	ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize);
	ZVal_ZooLib(const std::vector<ZVal_ZooLib>& iVal);
	//@}


/** \name Constructing from flattened data in a stream
*/	//@{
	explicit ZVal_ZooLib(const ZStreamR& iStreamR);
	explicit ZVal_ZooLib(ZType iType, const ZStreamR& iStreamR);
	ZVal_ZooLib(bool dummy, const ZStreamR& iStreamR);
	//@}


/** \name Efficient swapping with another instance
*/	//@{
	void swap(ZVal_ZooLib& iOther);
	//@}


/** \name Writing to and reading from a stream
*/	//@{
	void ToStream(const ZStreamW& iStreamW) const;
	void FromStream(const ZStreamR& iStreamR);
	void FromStream(ZType iType, const ZStreamR& iStreamR);
	//@}


/** \name Comparison
*/	//@{
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fType.fType != eZType_Null); }

	int Compare(const ZVal_ZooLib& iOther) const;

	bool operator==(const ZVal_ZooLib& iOther) const;
	bool operator!=(const ZVal_ZooLib& iOther) const;

	bool operator<(const ZVal_ZooLib& iOther) const;
	bool operator<=(const ZVal_ZooLib& iOther) const;
	bool operator>(const ZVal_ZooLib& iOther) const;
	bool operator>=(const ZVal_ZooLib& iOther) const;

	bool IsSameAs(const ZVal_ZooLib& iOther) const;
	//@}


/** \name Getting the type of the value
*/	//@{
	ZType TypeOf() const { return fType.fType < 0 ? eZType_String : ZType(fType.fType); }
	//@}


/** \name Getting the real value
*/	//@{
	template <class S>
	bool QGet_T(S& oResult) const;
	//@}


/** \name Setting type and value
*/	//@{
	template <class S>
	void Set_T(const S& iVal);

	void SetNull();
	void SetRaw(const void* iSource, size_t iSize);
	void SetRaw(const ZStreamR& iStreamR, size_t iSize);
	void SetRaw(const ZStreamR& iStreamR);
	//@}


/** \name Getting mutable reference to actual value, must be of correct type
*/	//@{
	ZValMap_ZooLib& GetMutableTuple();
	ZValList_ZooLib& GetMutableVector();
	//@}


/** \name Setting empty value and returning a mutable reference to it
*/	//@{
	ZVal_ZooLib& SetMutableNull();
	ZValMap_ZooLib& SetMutableTuple();
	ZValList_ZooLib& SetMutableVector();
	//@}


/** \name Getting mutable reference to actual value, changing type if necessary
*/	//@{
	ZValMap_ZooLib& EnsureMutableTuple();
	ZValList_ZooLib& EnsureMutableVector();
	//@}


	template <typename OutputIterator, typename T>
	void GetVector_T(OutputIterator iIter, const T& iDummy) const;

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
		ZValList_ZooLib* fAs_Vector;
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

#if ZCONFIG_Tuple_PackedTVs
#	pragma pack()
#endif

inline void ZVal_ZooLib::swap(ZVal_ZooLib& iOther)
	{ std::swap(fFastCopy, iOther.fFastCopy); }

inline bool ZVal_ZooLib::operator!=(const ZVal_ZooLib& iOther) const
	{ return !(*this == iOther); }

inline bool ZVal_ZooLib::operator<=(const ZVal_ZooLib& iOther) const
	{ return !(iOther < *this); }

inline bool ZVal_ZooLib::operator>(const ZVal_ZooLib& iOther) const
	{ return iOther < *this; }

inline bool ZVal_ZooLib::operator>=(const ZVal_ZooLib& iOther) const
	{ return !(*this < iOther); }

class ZVal_ZooLib::Ex_IllegalType : public std::runtime_error
	{
public:
	Ex_IllegalType(int iType);
	int fType;
	};

template <> inline int sCompare_T(const ZVal_ZooLib& iL, const ZVal_ZooLib& iR)
	{ return iL.Compare(iR); }

template <typename OutputIterator, typename T>
inline void ZVal_ZooLib::GetVector_T(OutputIterator iIter, const T& iDummy) const
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
#pragma mark * ZValList_ZooLib

class ZValList_ZooLib
:	public std::vector<ZVal_ZooLib>
,	public ZValListR_T<ZValList_ZooLib, ZVal_ZooLib>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValList_ZooLib,
		operator_bool_generator_type, operator_bool_type);

public:
	operator operator_bool_type() const;

	ZValList_ZooLib();
	ZValList_ZooLib(const ZValList_ZooLib& iOther);
	~ZValList_ZooLib();
	ZValList_ZooLib& operator=(const ZValList_ZooLib& iOther);

	ZValList_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton);
	ZValList_ZooLib(const std::vector<ZVal_ZooLib>& iOther);

	size_t Count() const;

	void Clear();

	bool QGet(size_t iIndex, ZVal_ZooLib& oVal) const;

	void Set(size_t iIndex, const ZVal_ZooLib& iVal);

	void Erase(size_t iIndex);

	void Insert(size_t iIndex, const ZVal_ZooLib& iVal);

	void Append(const ZVal_ZooLib& iVal);
	};

// =================================================================================================
#pragma mark -
#pragma mark * NameTV

/// The type used to store name/property pairs.
struct NameTV
	{
	NameTV();
	NameTV(const NameTV& iOther);

public:
	NameTV(const char* iName, const ZVal_ZooLib& iTV);
	NameTV(const ZTName& iName, const ZVal_ZooLib& iTV);

	NameTV(const char* iName);
	NameTV(const ZTName& iName);

	ZVal_ZooLib fTV;
	ZTName fName;
	};

/// The type used to store the list of name/property pairs.
typedef std::vector<NameTV> PropList;

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib

/// Associative array mapping names to ZVal_ZooLibs.

#define ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, TYPENAME, TYPE) \
	bool Get##TYPENAME(Name_t iName, TYPE& oVal) const; \
	bool QGet##TYPENAME(Name_t iName, TYPE& oVal) const; \
	TYPE DGet##TYPENAME(Name_t iName, const TYPE& iDefault) const; \
	TYPE Get##TYPENAME(Name_t iName) const; \
	void Set##TYPENAME(Name_t iName, const TYPE& iVal); \

#define ZMACRO_ZValMapAccessors_Decl_Std(T, Name_t) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, ID, uint64) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Int8, int8) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Int16, int16) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Int32, int32) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Int64, int64) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Bool, bool) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Float, float) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Double, double) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Pointer, VoidStar_t) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, String, std::string) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Tuple, ZValMap_ZooLib) \
	ZMACRO_ZValMapAccessors_Decl_Entry(T, Name_t, Vector, std::vector<ZVal_ZooLib>) \

class ZValMap_ZooLib
:	public ZValMapR_T<ZValMap_ZooLib, const char*, ZVal_ZooLib>
,	public ZValMapR_T<ZValMap_ZooLib, const ZTName&, ZVal_ZooLib>
,	public ZValMapR_T<ZValMap_ZooLib, PropList::iterator, ZVal_ZooLib>
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZValMap_ZooLib,
		operator_bool_generator_type, operator_bool_type);

	class Rep;

public:
	ZMACRO_ZValMapAccessors_Using(ZValMap_ZooLib, const char*, ZVal_ZooLib)
	ZMACRO_ZValMapAccessors_Using(ZValMap_ZooLib, const ZTName&, ZVal_ZooLib)
	ZMACRO_ZValMapAccessors_Using(ZValMap_ZooLib, PropList::iterator, ZVal_ZooLib)

	ZMACRO_ZValMapAccessors_Decl_Std(ZValMap_ZooLib, const char*)
	ZMACRO_ZValMapAccessors_Decl_Std(ZValMap_ZooLib, const ZTName&)
	ZMACRO_ZValMapAccessors_Decl_Std(ZValMap_ZooLib, PropList::iterator)

	typedef std::vector<NameTV> PropList;

/// Used to iterate through a tuple's properties.
	typedef PropList::iterator const_iterator;

/** \name Canonical Methods
*/	//@{
	ZValMap_ZooLib();
	ZValMap_ZooLib(const ZValMap_ZooLib& iOther);
	~ZValMap_ZooLib();
	ZValMap_ZooLib& operator=(const ZValMap_ZooLib& iOther);
	//@}


/** \name Use with care
*/	//@{
	ZValMap_ZooLib(ZRef<Rep> iRep);
	//@}

/** \name Constructing from stream
*/	//@{
	explicit ZValMap_ZooLib(const ZStreamR& iStreamR);
	ZValMap_ZooLib(bool dummy, const ZStreamR& iStreamR);
	//@}


/** \name Streaming
*/	//@{
	void ToStream(const ZStreamW& iStreamW) const;
	void FromStream(const ZStreamR& iStreamR);
	//@}


/** \name Unioning tuples
*/	//@{
	ZValMap_ZooLib Over(const ZValMap_ZooLib& iUnder) const;
	ZValMap_ZooLib Under(const ZValMap_ZooLib& iOver) const;
	//@}


/** \name Comparison
*/	//@{
	operator operator_bool_type() const;

	int Compare(const ZValMap_ZooLib& iOther) const;

	bool operator==(const ZValMap_ZooLib& iOther) const;
	bool operator!=(const ZValMap_ZooLib& iOther) const;

	bool operator<(const ZValMap_ZooLib& iOther) const;
	bool operator<=(const ZValMap_ZooLib& iOther) const;
	bool operator>(const ZValMap_ZooLib& iOther) const;
	bool operator>=(const ZValMap_ZooLib& iOther) const;

	bool IsSameAs(const ZValMap_ZooLib& iOther) const;

	bool Contains(const ZValMap_ZooLib& iOther) const;
	//@}


/** \name Property meta-data
*/	//@{
	const_iterator begin() const;
	const_iterator end() const;

	bool Empty() const;
	size_t Count() const;

	const ZTName& NameOf(const_iterator iPropIter) const;

	const_iterator IteratorOf(const char* iPropName) const;
	const_iterator IteratorOf(const ZTName& iPropName) const;

	bool Has(const char* iPropName) const;
	bool Has(const ZTName& iPropName) const;

	bool TypeOf(const_iterator iPropIter, ZType& oPropertyType) const;
	bool TypeOf(const char* iPropName, ZType& oPropertyType) const;
	bool TypeOf(const ZTName& iPropName, ZType& oPropertyType) const;

	ZType TypeOf(const_iterator iPropIter) const;
	ZType TypeOf(const char* iPropName) const;
	ZType TypeOf(const ZTName& iPropName) const;
	//@}


/** \name Erasing
*/	//@{
	void Erase(const_iterator iPropIter);
	void Erase(const char* iPropName);
	void Erase(const ZTName& iPropName);

	const_iterator EraseAndReturn(const_iterator iPropIter);

	void Clear();
	//@}


/** \name Getting
*/	//@{
	const ZVal_ZooLib& GetValue(const_iterator iPropIter) const;
	const ZVal_ZooLib& GetValue(const char* iPropName) const;
	const ZVal_ZooLib& GetValue(const ZTName& iPropName) const;

	template <class Name_t>
	bool QGet(Name_t iName, ZVal_ZooLib& oVal) const;

	template <class S, class Name_t>
	bool QGet_T(Name_t iName, S& oVal) const;
	//@}

/** \name Setting
*/	//@{
	void Set(const_iterator iPropIter, const ZVal_ZooLib& iVal);
	void Set(const char* iPropName, const ZVal_ZooLib& iVal);
	void Set(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	bool SetNull(const_iterator iPropIter);
	ZValMap_ZooLib& SetNull(const char* iPropName);
	ZValMap_ZooLib& SetNull(const ZTName& iPropName);

	bool SetValue(const_iterator iPropIter, const ZVal_ZooLib& iVal);
	ZValMap_ZooLib& SetValue(const char* iPropName, const ZVal_ZooLib& iVal);
	ZValMap_ZooLib& SetValue(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	template <class S>
	void Set_T(const_iterator iPropIter, const S& iVal);

	template <class S>
	void Set_T(const char* iPropName, const S& iVal);

	template <class S>
	void Set_T(const ZTName& iPropName, const S& iVal);
	//@}


/** \name Getting mutable references to extant properties, must be of correct type
*/	//@{
	ZVal_ZooLib& GetMutableValue(const_iterator iPropIter);
	ZVal_ZooLib& GetMutableValue(const char* iPropName);
	ZVal_ZooLib& GetMutableValue(const ZTName& iPropName);


	ZValMap_ZooLib& GetMutableTuple(const_iterator iPropIter);
	ZValMap_ZooLib& GetMutableTuple(const char* iPropName);
	ZValMap_ZooLib& GetMutableTuple(const ZTName& iPropName);


	ZValList_ZooLib& GetMutableVector(const_iterator iPropIter);
	ZValList_ZooLib& GetMutableVector(const char* iPropName);
	ZValList_ZooLib& GetMutableVector(const ZTName& iPropName);
	//@}


/** \name Setting empty types and return reference to the actual entity
*/	//@{
	ZVal_ZooLib& SetMutableNull(const_iterator iPropIter);
	ZVal_ZooLib& SetMutableNull(const char* iPropName);
	ZVal_ZooLib& SetMutableNull(const ZTName& iPropName);


	ZValMap_ZooLib& SetMutableTuple(const_iterator iPropIter);
	ZValMap_ZooLib& SetMutableTuple(const char* iPropName);
	ZValMap_ZooLib& SetMutableTuple(const ZTName& iPropName);


	ZValList_ZooLib& SetMutableVector(const_iterator iPropIter);
	ZValList_ZooLib& SetMutableVector(const char* iPropName);
	ZValList_ZooLib& SetMutableVector(const ZTName& iPropName);
	//@}


/** \name Getting mutable reference to actual value, changing type if necessary
*/	//@{
	ZVal_ZooLib& EnsureMutableValue(const_iterator iPropIter);
	ZVal_ZooLib& EnsureMutableValue(const char* iPropName);
	ZVal_ZooLib& EnsureMutableValue(const ZTName& iPropName);


	ZValMap_ZooLib& EnsureMutableTuple(const_iterator iPropIter);
	ZValMap_ZooLib& EnsureMutableTuple(const char* iPropName);
	ZValMap_ZooLib& EnsureMutableTuple(const ZTName& iPropName);


	ZValList_ZooLib& EnsureMutableVector(const_iterator iPropIter);
	ZValList_ZooLib& EnsureMutableVector(const char* iPropName);
	ZValList_ZooLib& EnsureMutableVector(const ZTName& iPropName);
	//@}


	void swap(ZValMap_ZooLib& iOther);

	ZValMap_ZooLib& Minimize();
	ZValMap_ZooLib Minimized() const;

protected:
	bool pSet(const_iterator iPropIter, const ZVal_ZooLib& iVal);
	ZValMap_ZooLib& pSet(const char* iPropName, const ZVal_ZooLib& iVal);
	ZValMap_ZooLib& pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal);

	ZVal_ZooLib* pFindOrAllocate(const char* iPropName);
	ZVal_ZooLib* pFindOrAllocate(const ZTName& iPropName);

	const ZVal_ZooLib* pLookupAddressConst(const_iterator iPropIter) const;
	const ZVal_ZooLib* pLookupAddressConst(const char* iPropName) const;
	const ZVal_ZooLib* pLookupAddressConst(const ZTName& iPropName) const;

	ZVal_ZooLib* pLookupAddress(const_iterator iPropIter);
	ZVal_ZooLib* pLookupAddress(const char* iPropName);
	ZVal_ZooLib* pLookupAddress(const ZTName& iPropName);

	const ZVal_ZooLib& pLookupConst(const_iterator iPropIter) const;
	const ZVal_ZooLib& pLookupConst(const char* iPropName) const;
	const ZVal_ZooLib& pLookupConst(const ZTName& iPropName) const;

	void pErase(const_iterator iPropIter);
	void pTouch();
	const_iterator pTouch(const_iterator iPropIter);
	bool pIsEqual(const ZValMap_ZooLib& iOther) const;
	bool pIsSameAs(const ZValMap_ZooLib& iOther) const;

	static ZRef<Rep> sRepFromStream(const ZStreamR& iStreamR);

	ZRef<Rep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib::Rep

/// ZValMap_ZooLibRep is an implementation detail. Try not to build anything that depends on it.

class ZValMap_ZooLib::Rep : public ZRefCounted
	{
public:
	static inline bool sCheckAccessEnabled() { return false; }

	Rep();
	Rep(const Rep* iOther);
	virtual ~Rep();

	/** fProperties is public to allow direct manipulation by utility code. But it
	is subject to change, so be careful. */
	ZValMap_ZooLib::PropList fProperties;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib inlines

inline ZValMap_ZooLib::ZValMap_ZooLib()
	{}

inline ZValMap_ZooLib::ZValMap_ZooLib(const ZValMap_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

inline ZValMap_ZooLib::~ZValMap_ZooLib()
	{}

inline ZValMap_ZooLib& ZValMap_ZooLib::operator=(const ZValMap_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

inline ZValMap_ZooLib::ZValMap_ZooLib(ZRef<Rep> iRep)
:	fRep(iRep)
	{}

inline ZValMap_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fProperties.empty()); }

inline ZValMap_ZooLib ZValMap_ZooLib::Under(const ZValMap_ZooLib& iOver) const
	{ return iOver.Over(*this); }

inline bool ZValMap_ZooLib::operator==(const ZValMap_ZooLib& iOther) const
	{ return fRep == iOther.fRep || this->pIsEqual(iOther); }

inline bool ZValMap_ZooLib::operator!=(const ZValMap_ZooLib& iOther) const
	{ return !(*this == iOther); }

inline bool ZValMap_ZooLib::operator<=(const ZValMap_ZooLib& iOther) const
	{ return !(iOther < *this); }

inline bool ZValMap_ZooLib::operator>(const ZValMap_ZooLib& iOther) const
	{ return iOther < *this; }

inline bool ZValMap_ZooLib::operator>=(const ZValMap_ZooLib& iOther) const
	{ return !(*this < iOther); }

inline bool ZValMap_ZooLib::IsSameAs(const ZValMap_ZooLib& iOther) const
	{ return fRep == iOther.fRep || this->pIsSameAs(iOther); }

inline void ZValMap_ZooLib::swap(ZValMap_ZooLib& iOther)
	{ std::swap(fRep, iOther.fRep); }

// =================================================================================================
#pragma mark -
#pragma mark * NameTV inlines

inline NameTV::NameTV()
	{}

inline NameTV::NameTV(const NameTV& iOther)
:	fTV(iOther.fTV),
	fName(iOther.fName)
	{}

inline NameTV::NameTV(const char* iName, const ZVal_ZooLib& iTV)
:	fTV(iTV),
	fName(iName)
	{}

inline NameTV::NameTV(const ZTName& iName, const ZVal_ZooLib& iTV)
:	fTV(iTV),
	fName(iName)
	{}

inline NameTV::NameTV(const char* iName)
:	fName(iName)
	{}

inline NameTV::NameTV(const ZTName& iName)
:	fName(iName)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib template inlines

template <class Name_t>
bool ZValMap_ZooLib::QGet(Name_t iName, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iName))
		{
		oVal = *theValue;
		return true;
		}
	return false;	
	}

template <class S>
void ZValMap_ZooLib::Set_T(const_iterator iPropIter, const S& iVal)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV.Set_T(iVal);
//		return true;
		}
//	return false;
	}

template <class S>
void ZValMap_ZooLib::Set_T(const char* iPropName, const S& iVal)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->Set_T(iVal);
	}

template <class S>
void ZValMap_ZooLib::Set_T(const ZTName& iPropName, const S& iVal)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->Set_T(iVal);
	}

NAMESPACE_ZOOLIB_END

namespace std {

inline void swap(ZOOLIB_PREFIX::ZVal_ZooLib& a, ZOOLIB_PREFIX::ZVal_ZooLib& b)
	{ a.swap(b); }

inline void swap(ZOOLIB_PREFIX::ZValMap_ZooLib& a, ZOOLIB_PREFIX::ZValMap_ZooLib& b)
	{ a.swap(b); }

} // namespace std

#endif // __ZVal_ZooLib__
