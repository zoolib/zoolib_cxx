/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTuple__
#define __ZTuple__ 1
#include "zconfig.h"

#include "zoolib/ZCompare.h"
#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZTime.h"
#include "zoolib/ZTName.h"
#include "zoolib/ZTypes.h"

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

class ZTuple;
class ZTupleRep;
class ZTValue;

typedef ZTName ZTuplePropName;
typedef ZTValue ZTupleValue;

class ZMemoryBlock;

class ZStreamR;
class ZStreamW;

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue

/// Holds a value which can be one of several different types.

#if ZCONFIG_Tuple_PackedTVs
#	pragma pack(4)
#endif

class ZTValue
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTValue, operator_bool_generator_type, operator_bool_type);
public:
	class Ex_IllegalType;

/** \name Canonical Methods
*/	//@{
	ZTValue();
	~ZTValue();
	ZTValue(const ZTValue& iOther);
	ZTValue& operator=(const ZTValue& iOther);
	//@}

/** \name Constructors taking various types
*/	//@{
	ZTValue(const ZTValue& iVal, bool iAsVector);
	ZTValue(ZType iVal);
	ZTValue(uint64 iVal, bool iIsID);
	ZTValue(uint64 iVal);
	ZTValue(int8 iVal);
	ZTValue(int16 iVal);
	ZTValue(int32 iVal);
	ZTValue(int64 iVal);
	ZTValue(bool iVal);
	ZTValue(float iVal);
	ZTValue(double iVal);
	ZTValue(ZTime iVal);
	ZTValue(void* iVal);
	ZTValue(const ZRectPOD& iVal);
	ZTValue(const ZPointPOD& iVal);
	ZTValue(const char* iVal);
	ZTValue(const std::string& iVal);
//##	ZTValue(const ZTName& iVal);
	ZTValue(const ZTuple& iVal);
	ZTValue(const ZRef<ZRefCountedWithFinalize>& iVal);
	ZTValue(const ZMemoryBlock& iVal);
	ZTValue(const void* iSource, size_t iSize);
	ZTValue(const ZStreamR& iStreamR, size_t iSize);
	ZTValue(const std::vector<ZTValue>& iVal);
	//@}

/** \name Constructing from flattened data in a stream
*/	//@{
	explicit ZTValue(const ZStreamR& iStreamR);
	explicit ZTValue(ZType iType, const ZStreamR& iStreamR);
	ZTValue(bool dummy, const ZStreamR& iStreamR);
	//@}

/** \name Efficient swapping with another instance
*/	//@{
	void swap(ZTValue& iOther);
	//@}

/** \name Writing to and reading from a stream
*/	//@{
	void ToStream(const ZStreamW& iStreamW) const;
	void FromStream(const ZStreamR& iStreamR);
	void FromStream(ZType iType, const ZStreamR& iStreamR);

	void ToStreamOld(const ZStreamW& iStreamW) const;
	void FromStreamOld(const ZStreamR& iStreamR);
	//@}

/** \name Comparison
*/	//@{
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fType.fType != eZType_Null); }

	int Compare(const ZTValue& iOther) const;

	bool operator==(const ZTValue& iOther) const;
	bool operator!=(const ZTValue& iOther) const;

	bool operator<(const ZTValue& iOther) const;
	bool operator<=(const ZTValue& iOther) const;
	bool operator>(const ZTValue& iOther) const;
	bool operator>=(const ZTValue& iOther) const;

	bool IsSameAs(const ZTValue& iOther) const;

	bool IsString(const char* iString) const;
	bool IsString(const std::string& iString) const;
	//@}


/** \name Getting the type of the value
*/	//@{
	ZType TypeOf() const { return fType.fType < 0 ? eZType_String : ZType(fType.fType); }
	//@}

/** \name Getting the real value
*/	//@{
	bool GetType(ZType& oVal) const;
	ZType GetType() const;

	bool GetID(uint64& oVal) const;
	uint64 GetID() const;

	bool GetInt8(int8& oVal) const;
	int8 GetInt8() const;

	bool GetInt16(int16& oVal) const;
	int16 GetInt16() const;

	bool GetInt32(int32& oVal) const;
	int32 GetInt32() const;

	bool GetInt64(int64& oVal) const;
	int64 GetInt64() const;

	bool GetBool(bool& oVal) const;
	bool GetBool() const;

	bool GetFloat(float& oVal) const;
	float GetFloat() const;

	bool GetDouble(double& oVal) const;
	double GetDouble() const;

	bool GetTime(ZTime& oVal) const;
	ZTime GetTime() const;

	bool GetPointer(void*& oVal) const;
	void* GetPointer() const;

	bool GetRect(ZRectPOD& oVal) const;
	const ZRectPOD& GetRect() const;

	bool GetPoint(ZPointPOD& oVal) const;
	const ZPointPOD& GetPoint() const;

	bool GetString(std::string& oVal) const;
	std::string GetString() const;

//##	bool GetName(ZTName& oVal) const;
//##	const ZTName& GetName() const;

	bool GetTuple(ZTuple& oVal) const;
	const ZTuple& GetTuple() const;

	bool GetRefCounted(ZRef<ZRefCountedWithFinalize>& oVal) const;
	ZRef<ZRefCountedWithFinalize> GetRefCounted() const;

	bool GetRaw(ZMemoryBlock& oVal) const;
	ZMemoryBlock GetRaw() const;

	bool GetRaw(void* iDest, size_t iSize) const;

	bool GetRawAttributes(const void** oAddress, size_t* oSize) const;

	bool GetVector(std::vector<ZTValue>& oVal) const;
	const std::vector<ZTValue>& GetVector() const;
	//@}

/** \name Getting, with a default value
*/	//@{
	ZType DGetType(ZType iDefault) const;

	uint64 DGetID(uint64 iDefault) const;

	int8 DGetInt8(int8 iDefault) const;

	int16 DGetInt16(int16 iDefault) const;

	int32 DGetInt32(int32 iDefault) const;

	int64 DGetInt64(int64 iDefault) const;

	bool DGetBool(bool iDefault) const;

	float DGetFloat(float iDefault) const;

	double DGetDouble(double iDefault) const;

	ZTime DGetTime(ZTime iDefault) const;

	ZRectPOD DGetRect(const ZRectPOD& iDefault) const;

	ZPointPOD DGetPoint(const ZPointPOD& iDefault) const;

	std::string DGetString(const std::string& iDefault) const;

//##	ZTName DGetName(const ZTName& iDefault) const;
	//@}

/** \name Setting type and value
*/	//@{
	void SetNull();
	void SetType(ZType iVal);
	void SetID(uint64 iVal);
	void SetInt8(int8 iVal);
	void SetInt16(int16 iVal);
	void SetInt32(int32 iVal);
	void SetInt64(int64 iVal);
	void SetBool(bool iVal);
	void SetFloat(float iVal);
	void SetDouble(double iVal);
	void SetTime(ZTime iVal);
	void SetPointer(void* iVal);
	void SetRect(const ZRectPOD& iVal);
	void SetPoint(const ZPointPOD& iVal);
	void SetString(const char* iVal);
	void SetString(const std::string& iVal);
//##	void SetName(const char* iVal);
//##	void SetName(const std::string& iVal);
//##	void SetName(const ZTName& iVal);
	void SetTuple(const ZTuple& iVal);
	void SetRefCounted(const ZRef<ZRefCountedWithFinalize>& iVal);
	void SetRaw(const ZMemoryBlock& iVal);
	void SetRaw(const void* iSource, size_t iSize);
	void SetRaw(const ZStreamR& iStreamR, size_t iSize);
	void SetRaw(const ZStreamR& iStreamR);
	void SetVector(const std::vector<ZTValue>& iVal);
	//@}

/** \name Appending a value
*/	//@{
	void AppendValue(const ZTValue& iVal);
	//@}

/** \name Getting mutable reference to actual value, must be of correct type
*/	//@{
	ZTuple& GetMutableTuple();
	std::vector<ZTValue>& GetMutableVector();
	//@}

/** \name Setting empty value and returning a mutable reference to it
*/	//@{
	ZTValue& SetMutableNull();
	ZTuple& SetMutableTuple();
	std::vector<ZTValue>& SetMutableVector();
	//@}

/** \name Getting mutable reference to actual value, changing type if necessary
*/	//@{
	ZTuple& EnsureMutableTuple();
	std::vector<ZTValue>& EnsureMutableVector();
	//@}

/** \name Template member functions
*/	//@{
	template <typename T> bool Get_T(T& oVal) const;

	template <typename T> T Get_T() const;

	template <typename T> T DGet_T(T iDefault) const;

	template <typename OutputIterator>
	void GetVector_T(OutputIterator iIter) const;

	template <typename OutputIterator, typename T>
	void GetVector_T(OutputIterator iIter, const T& iDummy) const;

	template <typename InputIterator>
	void SetVector_T(InputIterator iBegin, InputIterator iEnd);

	template <typename InputIterator, typename T>
	void SetVector_T(InputIterator iBegin, InputIterator iEnd, const T& iDummy);
	//@}

private:
	int pUncheckedCompare(const ZTValue& iOther) const;
	bool pUncheckedLess(const ZTValue& iOther) const;
	bool pUncheckedEqual(const ZTValue& iOther) const;

	void pRelease();
	void pCopy(const ZTValue& iOther);
	void pFromStream(ZType iType, const ZStreamR& iStream);

	friend class ZTuple;
	
	// Data is stored in one of several ways.
	// * For POD data <= 8 bytes in length, we simply store the
	// data directly, using named fields of fData.
	// * For ZRectPOD and vector<ZTValue> in fData we store a
	// pointer to a heap-allocated object, because they're 16 bytes
	// and probably 12 bytes in size, respectively.
	// * For ZTuple, ZRef and ZMemoryBlock we use placement-new and
	// explicit destructor invocation on fType.fBytes -- they're
	// all 4 bytes in size, but have constructors/destructors, so
	// we can't declare them in the union.

	// Finally, strings are funky. For short (<= 11 bytes) string
	// instances, we put the characters in fType.fBytes, and
	// store an encoded length in fType.fType.
	// If they're longer, we do placement new of a ZTupleString.
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
		std::vector<ZTValue>* fAs_Vector;
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

inline void ZTValue::swap(ZTValue& iOther)
	{ std::swap(fFastCopy, iOther.fFastCopy); }

inline bool ZTValue::operator!=(const ZTValue& iOther) const
	{ return !(*this == iOther); }

inline bool ZTValue::operator<=(const ZTValue& iOther) const
	{ return !(iOther < *this); }

inline bool ZTValue::operator>(const ZTValue& iOther) const
	{ return iOther < *this; }

inline bool ZTValue::operator>=(const ZTValue& iOther) const
	{ return !(*this < iOther); }

/** \name Template member functions
*/	//@{
template <> inline bool ZTValue::Get_T<ZTValue>(ZTValue& oVal) const
	{ oVal = *this; return true; }

template <> inline bool ZTValue::Get_T<ZType>(ZType& oVal) const
	{ return this->GetType(oVal); }

template <> inline bool ZTValue::Get_T<uint64>(uint64& oVal) const
	{ return this->GetID(oVal); }

template <> inline bool ZTValue::Get_T<int8>(int8& oVal) const
	{ return this->GetInt8(oVal); }

template <> inline bool ZTValue::Get_T<int16>(int16& oVal) const
	{ return this->GetInt16(oVal); }

template <> inline bool ZTValue::Get_T<int32>(int32& oVal) const
	{ return this->GetInt32(oVal); }

template <> inline bool ZTValue::Get_T<int64>(int64& oVal) const
	{ return this->GetInt64(oVal); }

template <> inline bool ZTValue::Get_T<bool>(bool& oVal) const
	{ return this->GetBool(oVal); }

template <> inline bool ZTValue::Get_T<float>(float& oVal) const
	{ return this->GetFloat(oVal); }

template <> inline bool ZTValue::Get_T<double>(double& oVal) const
	{ return this->GetDouble(oVal); }

template <> inline bool ZTValue::Get_T<ZTime>(ZTime& oVal) const
	{ return this->GetTime(oVal); }

template <> inline bool ZTValue::Get_T<std::string>(std::string& oVal) const
	{ return this->GetString(oVal); }

//##template <> inline bool ZTValue::Get_T<ZTName>(ZTName& oVal) const
//##	{ return this->GetName(oVal); }


template <> inline ZTValue ZTValue::Get_T<ZTValue>() const
	{ return *this; }

template <> inline ZType ZTValue::Get_T<ZType>() const
	{ return this->GetType(); }

template <> inline uint64 ZTValue::Get_T<uint64>() const
	{ return this->GetID(); }

template <> inline int8 ZTValue::Get_T<int8>() const
	{ return this->GetInt8(); }

template <> inline int16 ZTValue::Get_T<int16>() const
	{ return this->GetInt16(); }

template <> inline int32 ZTValue::Get_T<int32>() const
	{ return this->GetInt32(); }

template <> inline int64 ZTValue::Get_T<int64>() const
	{ return this->GetInt64(); }

template <> inline bool ZTValue::Get_T<bool>() const
	{ return this->GetBool(); }

template <> inline float ZTValue::Get_T<float>() const
	{ return this->GetFloat(); }

template <> inline double ZTValue::Get_T<double>() const
	{ return this->GetDouble(); }

template <> inline ZTime ZTValue::Get_T<ZTime>() const
	{ return this->GetTime(); }

template <> inline std::string ZTValue::Get_T<std::string>() const
	{ return this->GetString(); }

//##template <> inline ZTName ZTValue::Get_T<ZTName>() const
//##	{ return this->GetName(); }


template <> inline ZType ZTValue::DGet_T<ZType>(ZType iDefault) const
	{ return this->DGetType(iDefault); }

template <> inline uint64 ZTValue::DGet_T<uint64>(uint64 iDefault) const
	{ return this->DGetID(iDefault); }

template <> inline int8 ZTValue::DGet_T<int8>(int8 iDefault) const
	{ return this->DGetInt8(iDefault); }

template <> inline int16 ZTValue::DGet_T<int16>(int16 iDefault) const
	{ return this->DGetInt16(iDefault); }

template <> inline int32 ZTValue::DGet_T<int32>(int32 iDefault) const
	{ return this->DGetInt32(iDefault); }

template <> inline int64 ZTValue::DGet_T<int64>(int64 iDefault) const
	{ return this->DGetInt64(iDefault); }

template <> inline bool ZTValue::DGet_T<bool>(bool iDefault) const
	{ return this->DGetBool(iDefault); }

template <> inline float ZTValue::DGet_T<float>(float iDefault) const
	{ return this->DGetFloat(iDefault); }

template <> inline double ZTValue::DGet_T<double>(double iDefault) const
	{ return this->DGetDouble(iDefault); }

template <> inline ZTime ZTValue::DGet_T<ZTime>(ZTime iDefault) const
	{ return this->DGetTime(iDefault); }

template <> inline std::string ZTValue::DGet_T<std::string>(std::string iDefault) const
	{ return this->DGetString(iDefault); }

//##template <> inline ZTName ZTValue::DGet_T<ZTName>(ZTName iDefault) const
//##	{ return this->DGetName(iDefault); }

template <typename OutputIterator>
inline void ZTValue::GetVector_T(OutputIterator iIter) const
	{
	const std::vector<ZTValue>& theVector = this->GetVector();
	for (std::vector<ZTValue>::const_iterator i = theVector.begin(), theEnd = theVector.end();
		i != theEnd; ++i)
		{
		(*i).Get_T(*iIter++);
		}
	}

template <typename OutputIterator, typename T>
inline void ZTValue::GetVector_T(OutputIterator iIter, const T& iDummy) const
	{
	const std::vector<ZTValue>& theVector = this->GetVector();
	for (std::vector<ZTValue>::const_iterator i = theVector.begin(), theEnd = theVector.end();
		i != theEnd; ++i)
		{
		*iIter++ = (*i).Get_T<T>();
		}
	}

template <typename InputIterator>
inline void ZTValue::SetVector_T(InputIterator iBegin, InputIterator iEnd)
	{ std::copy(iBegin, iEnd, back_inserter(this->SetMutableVector())); }

template <typename InputIterator, typename T>
inline void ZTValue::SetVector_T(InputIterator iBegin, InputIterator iEnd, const T& iDummy)
	{
	std::vector<ZTValue>& theVector = this->SetMutableVector();
	while (iBegin != iEnd)
		theVector.push_back(T(*iBegin++));
	}
//@}

class ZTValue::Ex_IllegalType : public std::runtime_error
	{
public:
	Ex_IllegalType(int iType);
	int fType;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

/// Associative array mapping names to ZTValues.

class ZTuple
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTuple, operator_bool_generator_type, operator_bool_type);

public:
	struct NameTV
		{
		NameTV();
		NameTV(const NameTV& iOther);

	public:
		NameTV(const char* iName, const ZTValue& iTV);
		NameTV(const ZTName& iName, const ZTValue& iTV);

		NameTV(const char* iName);
		NameTV(const ZTName& iName);

		ZTValue fTV;
		ZTName fName;
		};

	/// The type used to store properties.
	typedef std::vector<NameTV> PropList;

	/// Used to iterate through a tuple's properties.
	typedef PropList::iterator const_iterator;

/** \name Canonical Methods
*/	//@{
	ZTuple();
	ZTuple(const ZTuple& iOther);
	~ZTuple();
	ZTuple& operator=(const ZTuple& iOther);
	//@}


/** \name Use with care
*/	//@{
	ZTuple(ZRef<ZTupleRep> iRep);
	//@}

/** \name Constructing from stream
*/	//@{
	explicit ZTuple(const ZStreamR& iStreamR);
	ZTuple(bool dummy, const ZStreamR& iStreamR);
	//@}


/** \name Streaming
*/	//@{
	void ToStream(const ZStreamW& iStreamW) const;
	void FromStream(const ZStreamR& iStreamR);

	void ToStreamOld(const ZStreamW& iStreamW) const;
	void FromStreamOld(const ZStreamR& iStreamR);
	//@}


/** \name Unioning tuples
*/	//@{
	ZTuple Over(const ZTuple& iUnder) const;
	ZTuple Under(const ZTuple& iOver) const;
	//@}


/** \name Comparison
*/	//@{
	operator operator_bool_type() const;

	int Compare(const ZTuple& iOther) const;

	bool operator==(const ZTuple& iOther) const;
	bool operator!=(const ZTuple& iOther) const;

	bool operator<(const ZTuple& iOther) const;
	bool operator<=(const ZTuple& iOther) const;
	bool operator>(const ZTuple& iOther) const;
	bool operator>=(const ZTuple& iOther) const;

	bool IsSameAs(const ZTuple& iOther) const;

	bool Contains(const ZTuple& iOther) const;
	//@}


/** \name Efficiently determining if a property is a particular string
*/	//@{
	bool IsString(const_iterator iPropIter, const char* iString) const;
	bool IsString(const char* iPropName, const char* iString) const;
	bool IsString(const ZTName& iPropName, const char* iString) const;

	bool IsString(const_iterator iPropIter, const std::string& iString) const;
	bool IsString(const char* iPropName, const std::string& iString) const;
	bool IsString(const ZTName& iPropName, const std::string& iString) const;
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
	bool GetValue(const_iterator iPropIter, ZTValue& oVal) const;
	bool GetValue(const char* iPropName, ZTValue& oVal) const;
	bool GetValue(const ZTName& iPropName, ZTValue& oVal) const;

	const ZTValue& GetValue(const_iterator iPropIter) const;
	const ZTValue& GetValue(const char* iPropName) const;
	const ZTValue& GetValue(const ZTName& iPropName) const;


	bool GetType(const_iterator iPropIter, ZType& oVal) const;
	bool GetType(const char* iPropName, ZType& oVal) const;
	bool GetType(const ZTName& iPropName, ZType& oVal) const;

	ZType GetType(const_iterator iPropIter) const;
	ZType GetType(const char* iPropName) const;
	ZType GetType(const ZTName& iPropName) const;


	bool GetID(const_iterator iPropIter, uint64& oVal) const;
	bool GetID(const char* iPropName, uint64& oVal) const;
	bool GetID(const ZTName& iPropName, uint64& oVal) const;

	uint64 GetID(const_iterator iPropIter) const;
	uint64 GetID(const char* iPropName) const;
	uint64 GetID(const ZTName& iPropName) const;


	bool GetInt8(const_iterator iPropIter, int8& oVal) const;
	bool GetInt8(const char* iPropName, int8& oVal) const;
	bool GetInt8(const ZTName& iPropName, int8& oVal) const;

	int8 GetInt8(const_iterator iPropIter) const;
	int8 GetInt8(const char* iPropName) const;
	int8 GetInt8(const ZTName& iPropName) const;


	bool GetInt16(const_iterator iPropIter, int16& oVal) const;
	bool GetInt16(const char* iPropName, int16& oVal) const;
	bool GetInt16(const ZTName& iPropName, int16& oVal) const;

	int16 GetInt16(const_iterator iPropIter) const;
	int16 GetInt16(const char* iPropName) const;
	int16 GetInt16(const ZTName& iPropName) const;


	bool GetInt32(const_iterator iPropIter, int32& oVal) const;
	bool GetInt32(const char* iPropName, int32& oVal) const;
	bool GetInt32(const ZTName& iPropName, int32& oVal) const;

	int32 GetInt32(const_iterator iPropIter) const;
	int32 GetInt32(const char* iPropName) const;
	int32 GetInt32(const ZTName& iPropName) const;


	bool GetInt64(const_iterator iPropIter, int64& oVal) const;
	bool GetInt64(const char* iPropName, int64& oVal) const;
	bool GetInt64(const ZTName& iPropName, int64& oVal) const;

	int64 GetInt64(const_iterator iPropIter) const;
	int64 GetInt64(const char* iPropName) const;
	int64 GetInt64(const ZTName& iPropName) const;


	bool GetBool(const_iterator iPropIter, bool& oVal) const;
	bool GetBool(const char* iPropName, bool& oVal) const;
	bool GetBool(const ZTName& iPropName, bool& oVal) const;

	bool GetBool(const_iterator iPropIter) const;
	bool GetBool(const char* iPropName) const;
	bool GetBool(const ZTName& iPropName) const;


	bool GetFloat(const_iterator iPropIter, float& oVal) const;
	bool GetFloat(const char* iPropName, float& oVal) const;
	bool GetFloat(const ZTName& iPropName, float& oVal) const;

	float GetFloat(const_iterator iPropIter) const;
	float GetFloat(const char* iPropName) const;
	float GetFloat(const ZTName& iPropName) const;


	bool GetDouble(const_iterator iPropIter, double& oVal) const;
	bool GetDouble(const char* iPropName, double& oVal) const;
	bool GetDouble(const ZTName& iPropName, double& oVal) const;

	double GetDouble(const_iterator iPropIter) const;
	double GetDouble(const char* iPropName) const;
	double GetDouble(const ZTName& iPropName) const;


	bool GetTime(const_iterator iPropIter, ZTime& oVal) const;
	bool GetTime(const char* iPropName, ZTime& oVal) const;
	bool GetTime(const ZTName& iPropName, ZTime& oVal) const;

	ZTime GetTime(const_iterator iPropIter) const;
	ZTime GetTime(const char* iPropName) const;
	ZTime GetTime(const ZTName& iPropName) const;


	bool GetPointer(const_iterator iPropIter, void*& oVal) const;
	bool GetPointer(const char* iPropName, void*& oVal) const;
	bool GetPointer(const ZTName& iPropName, void*& oVal) const;

	void* GetPointer(const_iterator iPropIter) const;
	void* GetPointer(const char* iPropName) const;
	void* GetPointer(const ZTName& iPropName) const;


	bool GetRect(const_iterator iPropIter, ZRectPOD& oVal) const;
	bool GetRect(const char* iPropName, ZRectPOD& oVal) const;
	bool GetRect(const ZTName& iPropName, ZRectPOD& oVal) const;

	const ZRectPOD& GetRect(const_iterator iPropIter) const;
	const ZRectPOD& GetRect(const char* iPropName) const;
	const ZRectPOD& GetRect(const ZTName& iPropName) const;


	bool GetPoint(const_iterator iPropIter, ZPointPOD& oVal) const;
	bool GetPoint(const char* iPropName, ZPointPOD& oVal) const;
	bool GetPoint(const ZTName& iPropName, ZPointPOD& oVal) const;

	const ZPointPOD& GetPoint(const_iterator iPropIter) const;
	const ZPointPOD& GetPoint(const char* iPropName) const;
	const ZPointPOD& GetPoint(const ZTName& iPropName) const;


	bool GetString(const_iterator iPropIter, std::string& oVal) const;
	bool GetString(const char* iPropName, std::string& oVal) const;
	bool GetString(const ZTName& iPropName, std::string& oVal) const;

	std::string GetString(const_iterator iPropIter) const;
	std::string GetString(const char* iPropName) const;
	std::string GetString(const ZTName& iPropName) const;


//##	bool GetName(const_iterator iPropIter, ZTName& oVal) const;
//##	bool GetName(const char* iPropName, ZTName& oVal) const;
//##	bool GetName(const ZTName& iPropName, ZTName& oVal) const;

//##	ZTName GetName(const_iterator iPropIter) const;
//##	ZTName GetName(const char* iPropName) const;
//##	ZTName GetName(const ZTName& iPropName) const;


	bool GetTuple(const_iterator iPropIter, ZTuple& oVal) const;
	bool GetTuple(const char* iPropName, ZTuple& oVal) const;
	bool GetTuple(const ZTName& iPropName, ZTuple& oVal) const;

	const ZTuple& GetTuple(const_iterator iPropIter) const;
	const ZTuple& GetTuple(const char* iPropName) const;
	const ZTuple& GetTuple(const ZTName& iPropName) const;


	bool GetRefCounted(const_iterator iPropIter, ZRef<ZRefCountedWithFinalize>& oVal) const;
	bool GetRefCounted(const char* iPropName, ZRef<ZRefCountedWithFinalize>& oVal) const;
	bool GetRefCounted(const ZTName& iPropName, ZRef<ZRefCountedWithFinalize>& oVal) const;

	ZRef<ZRefCountedWithFinalize> GetRefCounted(const_iterator iPropIter) const;
	ZRef<ZRefCountedWithFinalize> GetRefCounted(const char* iPropName) const;
	ZRef<ZRefCountedWithFinalize> GetRefCounted(const ZTName& iPropName) const;


	bool GetRaw(const_iterator iPropIter, ZMemoryBlock& oVal) const;
	bool GetRaw(const char* iPropName, ZMemoryBlock& oVal) const;
	bool GetRaw(const ZTName& iPropName, ZMemoryBlock& oVal) const;

	ZMemoryBlock GetRaw(const_iterator iPropIter) const;
	ZMemoryBlock GetRaw(const char* iPropName) const;
	ZMemoryBlock GetRaw(const ZTName& iPropName) const;


	bool GetRaw(const_iterator iPropIter, void* iDest, size_t iSize) const;
	bool GetRaw(const char* iPropName, void* iDest, size_t iSize) const;
	bool GetRaw(const ZTName& iPropName, void* iDest, size_t iSize) const;

	bool GetRawAttributes(const_iterator iPropIter, const void** oAddress, size_t* oSize) const;
	bool GetRawAttributes(const char* iPropName, const void** oAddress, size_t* oSize) const;
	bool GetRawAttributes(const ZTName& iPropName, const void** oAddress, size_t* oSize) const;


	bool GetVector(const_iterator iPropIter, std::vector<ZTValue>& oVal) const;
	bool GetVector(const char* iPropName, std::vector<ZTValue>& oVal) const;
	bool GetVector(const ZTName& iPropName, std::vector<ZTValue>& oVal) const;

	const std::vector<ZTValue>& GetVector(const_iterator iPropIter) const;
	const std::vector<ZTValue>& GetVector(const char* iPropName) const;
	const std::vector<ZTValue>& GetVector(const ZTName& iPropName) const;

	template <typename OutputIterator>
	void GetVector_T(const_iterator iPropIter, OutputIterator iIter) const;

	template <typename OutputIterator>
	void GetVector_T(const char* iPropName, OutputIterator iIter) const;

	template <typename OutputIterator>
	void GetVector_T(const ZTName& iPropName, OutputIterator iIter) const;

	template <typename OutputIterator, typename T>
	void GetVector_T(const_iterator iPropIter, OutputIterator iIter, const T& iDummy) const;

	template <typename OutputIterator, typename T>
	void GetVector_T(const char* iPropName, OutputIterator iIter, const T& iDummy) const;

	template <typename OutputIterator, typename T>
	void GetVector_T(const ZTName& iPropName, OutputIterator iIter, const T& iDummy) const;
	//@}


/** \name Getting types specified by template parameter
*/	//@{
	template <typename T> bool Get_T(const_iterator iPropIter, T& oVal) const;
	template <typename T> bool Get_T(const char* iPropName, T& oVal) const;
	template <typename T> bool Get_T(const ZTName& iPropName, T& oVal) const;

	template <typename T> T Get_T(const_iterator iPropIter) const;
	template <typename T> T Get_T(const char* iPropName) const;
	template <typename T> T Get_T(const ZTName& iPropName) const;
	//@}


/** \name DGetting
*/	//@{
	const ZTValue DGetValue(const_iterator iPropIter, const ZTValue& iDefault) const;
	const ZTValue DGetValue(const char* iPropName, const ZTValue& iDefault) const;
	const ZTValue DGetValue(const ZTName& iPropName, const ZTValue& iDefault) const;


	ZType DGetType(const_iterator iPropIter, ZType iDefault) const;
	ZType DGetType(const char* iPropName, ZType iDefault) const;
	ZType DGetType(const ZTName& iPropName, ZType iDefault) const;


	uint64 DGetID(const_iterator iPropIter, uint64 iDefault) const;
	uint64 DGetID(const char* iPropName, uint64 iDefault) const;
	uint64 DGetID(const ZTName& iPropName, uint64 iDefault) const;


	int8 DGetInt8(const_iterator iPropIter, int8 iDefault) const;
	int8 DGetInt8(const char* iPropName, int8 iDefault) const;
	int8 DGetInt8(const ZTName& iPropName, int8 iDefault) const;


	int16 DGetInt16(const_iterator iPropIter, int16 iDefault) const;
	int16 DGetInt16(const char* iPropName, int16 iDefault) const;
	int16 DGetInt16(const ZTName& iPropName, int16 iDefault) const;


	int32 DGetInt32(const_iterator iPropIter, int32 iDefault) const;
	int32 DGetInt32(const char* iPropName, int32 iDefault) const;
	int32 DGetInt32(const ZTName& iPropName, int32 iDefault) const;


	int64 DGetInt64(const_iterator iPropIter, int64 iDefault) const;
	int64 DGetInt64(const char* iPropName, int64 iDefault) const;
	int64 DGetInt64(const ZTName& iPropName, int64 iDefault) const;


	bool DGetBool(const_iterator iPropIter, bool iDefault) const;
	bool DGetBool(const char* iPropName, bool iDefault) const;
	bool DGetBool(const ZTName& iPropName, bool iDefault) const;


	float DGetFloat(const_iterator iPropIter, float iDefault) const;
	float DGetFloat(const char* iPropName, float iDefault) const;
	float DGetFloat(const ZTName& iPropName, float iDefault) const;


	double DGetDouble(const_iterator iPropIter, double iDefault) const;
	double DGetDouble(const char* iPropName, double iDefault) const;
	double DGetDouble(const ZTName& iPropName, double iDefault) const;


	ZTime DGetTime(const_iterator iPropIter, ZTime iDefault) const;
	ZTime DGetTime(const char* iPropName, ZTime iDefault) const;
	ZTime DGetTime(const ZTName& iPropName, ZTime iDefault) const;


	ZRectPOD DGetRect(const_iterator iPropIter, const ZRectPOD& iDefault) const;
	ZRectPOD DGetRect(const char* iPropName, const ZRectPOD& iDefault) const;
	ZRectPOD DGetRect(const ZTName& iPropName, const ZRectPOD& iDefault) const;


	ZPointPOD DGetPoint(const_iterator iPropIter, const ZPointPOD& iDefault) const;
	ZPointPOD DGetPoint(const char* iPropName, const ZPointPOD& iDefault) const;
	ZPointPOD DGetPoint(const ZTName& iPropName, const ZPointPOD& iDefault) const;


	std::string DGetString(const_iterator iPropIter, const std::string& iDefault) const;
	std::string DGetString(const char* iPropName, const std::string& iDefault) const;
	std::string DGetString(const ZTName& iPropName, const std::string& iDefault) const;


//##	ZTName DGetName(const_iterator iPropIter, const ZTName& iDefault) const;
//##	ZTName DGetName(const char* iPropName, const ZTName& iDefault) const;
//##	ZTName DGetName(const ZTName& iPropName, const ZTName& iDefault) const;


	template <typename T> T DGet_T(const_iterator iPropIter, T iDefault) const;
	template <typename T> T DGet_T(const char* iPropName, T iDefault) const;
	template <typename T> T DGet_T(const ZTName& iPropName, T iDefault) const;
	//@}


/** \name Setting
*/	//@{
	bool SetValue(const_iterator iPropIter, const ZTValue& iVal);
	ZTuple& SetValue(const char* iPropName, const ZTValue& iVal);
	ZTuple& SetValue(const ZTName& iPropName, const ZTValue& iVal);


	bool SetNull(const_iterator iPropIter);
	ZTuple& SetNull(const char* iPropName);
	ZTuple& SetNull(const ZTName& iPropName);


	bool SetType(const_iterator iPropIter, ZType iVal);
	ZTuple& SetType(const char* iPropName, ZType iVal);
	ZTuple& SetType(const ZTName& iPropName, ZType iVal);


	bool SetID(const_iterator iPropIter, uint64 iVal);
	ZTuple& SetID(const char* iPropName, uint64 iVal);
	ZTuple& SetID(const ZTName& iPropName, uint64 iVal);


	bool SetInt8(const_iterator iPropIter, int8 iVal);
	ZTuple& SetInt8(const char* iPropName, int8 iVal);
	ZTuple& SetInt8(const ZTName& iPropName, int8 iVal);


	bool SetInt16(const_iterator iPropIter, int16 iVal);
	ZTuple& SetInt16(const char* iPropName, int16 iVal);
	ZTuple& SetInt16(const ZTName& iPropName, int16 iVal);


	bool SetInt32(const_iterator iPropIter, int32 iVal);
	ZTuple& SetInt32(const char* iPropName, int32 iVal);
	ZTuple& SetInt32(const ZTName& iPropName, int32 iVal);


	bool SetInt64(const_iterator iPropIter, int64 iVal);
	ZTuple& SetInt64(const char* iPropName, int64 iVal);
	ZTuple& SetInt64(const ZTName& iPropName, int64 iVal);


	bool SetBool(const_iterator iPropIter, bool iVal);
	ZTuple& SetBool(const char* iPropName, bool iVal);
	ZTuple& SetBool(const ZTName& iPropName, bool iVal);


	bool SetFloat(const_iterator iPropIter, float iVal);
	ZTuple& SetFloat(const char* iPropName, float iVal);
	ZTuple& SetFloat(const ZTName& iPropName, float iVal);


	bool SetDouble(const_iterator iPropIter, double iVal);
	ZTuple& SetDouble(const char* iPropName, double iVal);
	ZTuple& SetDouble(const ZTName& iPropName, double iVal);


	bool SetTime(const_iterator iPropIter, ZTime iVal);
	ZTuple& SetTime(const char* iPropName, ZTime iVal);
	ZTuple& SetTime(const ZTName& iPropName, ZTime iVal);


	bool SetPointer(const_iterator iPropIter, void* iVal);
	ZTuple& SetPointer(const char* iPropName, void* iVal);
	ZTuple& SetPointer(const ZTName& iPropName, void* iVal);


	bool SetRect(const_iterator iPropIter, const ZRectPOD& iVal);
	ZTuple& SetRect(const char* iPropName, const ZRectPOD& iVal);
	ZTuple& SetRect(const ZTName& iPropName, const ZRectPOD& iVal);


	bool SetPoint(const_iterator iPropIter, const ZPointPOD& iVal);
	ZTuple& SetPoint(const char* iPropName, const ZPointPOD& iVal);
	ZTuple& SetPoint(const ZTName& iPropName, const ZPointPOD& iVal);


	bool SetString(const_iterator iPropIter, const char* iVal);
	ZTuple& SetString(const char* iPropName, const char* iVal);
	ZTuple& SetString(const ZTName& iPropName, const char* iVal);


	bool SetString(const_iterator iPropIter, const std::string& iVal);
	ZTuple& SetString(const char* iPropName, const std::string& iVal);
	ZTuple& SetString(const ZTName& iPropName, const std::string& iVal);


//##	bool SetName(const_iterator iPropIter, const ZTName& iVal);
//##	ZTuple& SetName(const char* iPropName, const ZTName& iVal);
//##	ZTuple& SetName(const ZTName& iPropName, const ZTName& iVal);


	bool SetTuple(const_iterator iPropIter, const ZTuple& iVal);
	ZTuple& SetTuple(const char* iPropName, const ZTuple& iVal);
	ZTuple& SetTuple(const ZTName& iPropName, const ZTuple& iVal);

	bool SetRefCounted(const_iterator iPropIter, const ZRef<ZRefCountedWithFinalize>& iVal);
	ZTuple& SetRefCounted(const char* iPropName, const ZRef<ZRefCountedWithFinalize>& iVal);
	ZTuple& SetRefCounted(const ZTName& iPropName,
		const ZRef<ZRefCountedWithFinalize>& iVal);


	bool SetRaw(const_iterator iPropIter, const ZMemoryBlock& iVal);
	ZTuple& SetRaw(const char* iPropName, const ZMemoryBlock& iVal);
	ZTuple& SetRaw(const ZTName& iPropName, const ZMemoryBlock& iVal);


	bool SetRaw(const_iterator iPropIter, const void* iSource, size_t iSize);
	ZTuple& SetRaw(const char* iPropName, const void* iSource, size_t iSize);
	ZTuple& SetRaw(const ZTName& iPropName, const void* iSource, size_t iSize);


	bool SetRaw(const_iterator iPropIter, const ZStreamR& iStreamR, size_t iSize);
	ZTuple& SetRaw(const char* iPropName, const ZStreamR& iStreamR, size_t iSize);
	ZTuple& SetRaw(const ZTName& iPropName, const ZStreamR& iStreamR, size_t iSize);


	bool SetRaw(const_iterator iPropIter, const ZStreamR& iStreamR);
	ZTuple& SetRaw(const char* iPropName, const ZStreamR& iStreamR);
	ZTuple& SetRaw(const ZTName& iPropName, const ZStreamR& iStreamR);


	bool SetVector(const_iterator iPropIter, const std::vector<ZTValue>& iVal);
	ZTuple& SetVector(const char* iPropName, const std::vector<ZTValue>& iVal);
	ZTuple& SetVector(const ZTName& iPropName, const std::vector<ZTValue>& iVal);

	template <typename InputIterator>
	void SetVector_T(const_iterator iPropIter, InputIterator iBegin, InputIterator iEnd);

	template <typename InputIterator>
	void SetVector_T(const char* iPropName, InputIterator iBegin, InputIterator iEnd);

	template <typename InputIterator>
	void SetVector_T(const ZTName& iPropName, InputIterator iBegin, InputIterator iEnd);


	template <typename InputIterator, typename T>
	void SetVector_T(const_iterator iPropIter,
		InputIterator iBegin, InputIterator iEnd, const T& iDummy);

	template <typename InputIterator, typename T>
	void SetVector_T(const char* iPropName,
		InputIterator iBegin, InputIterator iEnd, const T& iDummy);

	template <typename InputIterator, typename T>
	void SetVector_T(const ZTName& iPropName,
		InputIterator iBegin, InputIterator iEnd, const T& iDummy);
	//@}


/** \name Getting mutable references to extant properties, must be of correct type
*/	//@{
	ZTValue& GetMutableValue(const_iterator iPropIter);
	ZTValue& GetMutableValue(const char* iPropName);
	ZTValue& GetMutableValue(const ZTName& iPropName);


	ZTuple& GetMutableTuple(const_iterator iPropIter);
	ZTuple& GetMutableTuple(const char* iPropName);
	ZTuple& GetMutableTuple(const ZTName& iPropName);


	std::vector<ZTValue>& GetMutableVector(const_iterator iPropIter);
	std::vector<ZTValue>& GetMutableVector(const char* iPropName);
	std::vector<ZTValue>& GetMutableVector(const ZTName& iPropName);
	//@}


/** \name Setting empty types and return reference to the actual entity
*/	//@{
	ZTValue& SetMutableNull(const_iterator iPropIter);
	ZTValue& SetMutableNull(const char* iPropName);
	ZTValue& SetMutableNull(const ZTName& iPropName);


	ZTuple& SetMutableTuple(const_iterator iPropIter);
	ZTuple& SetMutableTuple(const char* iPropName);
	ZTuple& SetMutableTuple(const ZTName& iPropName);


	std::vector<ZTValue>& SetMutableVector(const_iterator iPropIter);
	std::vector<ZTValue>& SetMutableVector(const char* iPropName);
	std::vector<ZTValue>& SetMutableVector(const ZTName& iPropName);
	//@}


/** \name Getting mutable reference to actual value, changing type if necessary
*/	//@{
	ZTValue& EnsureMutableValue(const_iterator iPropIter);
	ZTValue& EnsureMutableValue(const char* iPropName);
	ZTValue& EnsureMutableValue(const ZTName& iPropName);


	ZTuple& EnsureMutableTuple(const_iterator iPropIter);
	ZTuple& EnsureMutableTuple(const char* iPropName);
	ZTuple& EnsureMutableTuple(const ZTName& iPropName);


	std::vector<ZTValue>& EnsureMutableVector(const_iterator iPropIter);
	std::vector<ZTValue>& EnsureMutableVector(const char* iPropName);
	std::vector<ZTValue>& EnsureMutableVector(const ZTName& iPropName);
	//@}


/** \name Appending data
*/	//@{
	void AppendValue(const char* iPropName, const ZTValue& iVal);
	void AppendValue(const ZTName& iPropName, const ZTValue& iVal);


	void AppendType(const char* iPropName, ZType iVal);
	void AppendType(const ZTName& iPropName, ZType iVal);


	void AppendID(const char* iPropName, uint64 iVal);
	void AppendID(const ZTName& iPropName, uint64 iVal);


	void AppendInt8(const char* iPropName, int8 iVal);
	void AppendInt8(const ZTName& iPropName, int8 iVal);


	void AppendInt16(const char* iPropName, int16 iVal);
	void AppendInt16(const ZTName& iPropName, int16 iVal);


	void AppendInt32(const char* iPropName, int32 iVal);
	void AppendInt32(const ZTName& iPropName, int32 iVal);


	void AppendInt64(const char* iPropName, int64 iVal);
	void AppendInt64(const ZTName& iPropName, int64 iVal);


	void AppendBool(const char* iPropName, bool iVal);
	void AppendBool(const ZTName& iPropName, bool iVal);


	void AppendFloat(const char* iPropName, float iVal);
	void AppendFloat(const ZTName& iPropName, float iVal);


	void AppendDouble(const char* iPropName, double iVal);
	void AppendDouble(const ZTName& iPropName, double iVal);


	void AppendTime(const char* iPropName, ZTime iVal);
	void AppendTime(const ZTName& iPropName, ZTime iVal);


	void AppendPointer(const char* iPropName, void* iVal);
	void AppendPointer(const ZTName& iPropName, void* iVal);


	void AppendRect(const char* iPropName, const ZRectPOD& iVal);
	void AppendRect(const ZTName& iPropName, const ZRectPOD& iVal);


	void AppendPoint(const char* iPropName, const ZPointPOD& iVal);
	void AppendPoint(const ZTName& iPropName, const ZPointPOD& iVal);


	void AppendString(const char* iPropName, const char* iVal);
	void AppendString(const ZTName& iPropName, const char* iVal);


	void AppendString(const char* iPropName, const std::string& iVal);
	void AppendString(const ZTName& iPropName, const std::string& iVal);


//##	void AppendName(const char* iPropName, const ZTName& iVal);
//##	void AppendName(const ZTName& iPropName, const ZTName& iVal);


	void AppendTuple(const char* iPropName, const ZTuple& iVal);
	void AppendTuple(const ZTName& iPropName, const ZTuple& iVal);


	void AppendRefCounted(const char* iPropName, const ZRef<ZRefCountedWithFinalize>& iVal);
	void AppendRefCounted(const ZTName& iPropName,
		const ZRef<ZRefCountedWithFinalize>& iVal);


	void AppendRaw(const char* iPropName, const ZMemoryBlock& iVal);
	void AppendRaw(const ZTName& iPropName, const ZMemoryBlock& iVal);


	void AppendRaw(const char* iPropName, const void* iSource, size_t iSize);
	void AppendRaw(const ZTName& iPropName, const void* iSource, size_t iSize);
	//@}


	void swap(ZTuple& iOther);

	ZTuple& Minimize();
	ZTuple Minimized() const;

protected:
	bool pSet(const_iterator iPropIter, const ZTValue& iVal);
	ZTuple& pSet(const char* iPropName, const ZTValue& iVal);
	ZTuple& pSet(const ZTName& iPropName, const ZTValue& iVal);

	ZTValue* pFindOrAllocate(const char* iPropName);
	ZTValue* pFindOrAllocate(const ZTName& iPropName);

	ZTuple& pAppend(const char* iPropName, const ZTValue& iVal);
	ZTuple& pAppend(const ZTName& iPropName, const ZTValue& iVal);

	const ZTValue* pLookupAddressConst(const_iterator iPropIter) const;
	const ZTValue* pLookupAddressConst(const char* iPropName) const;
	const ZTValue* pLookupAddressConst(const ZTName& iPropName) const;

	ZTValue* pLookupAddress(const_iterator iPropIter);
	ZTValue* pLookupAddress(const char* iPropName);
	ZTValue* pLookupAddress(const ZTName& iPropName);

	const ZTValue& pLookupConst(const_iterator iPropIter) const;
	const ZTValue& pLookupConst(const char* iPropName) const;
	const ZTValue& pLookupConst(const ZTName& iPropName) const;

	void pErase(const_iterator iPropIter);
	void pTouch();
	const_iterator pTouch(const_iterator iPropIter);
	bool pIsEqual(const ZTuple& iOther) const;
	bool pIsSameAs(const ZTuple& iOther) const;

	static ZRef<ZTupleRep> sRepFromStream(const ZStreamR& iStreamR);

	ZRef<ZTupleRep> fRep;
	};

template <typename OutputIterator>
inline void ZTuple::GetVector_T(const_iterator iPropIter, OutputIterator iIter) const
	{ this->GetValue(iPropIter).GetVector_T(iIter); }

template <typename OutputIterator>
inline void ZTuple::GetVector_T(const char* iPropName, OutputIterator iIter) const
	{ this->GetValue(iPropName).GetVector_T(iIter); }

template <typename OutputIterator>
inline void ZTuple::GetVector_T(const ZTName& iPropName, OutputIterator iIter) const
	{ this->GetValue(iPropName).GetVector_T(iIter); }

template <typename OutputIterator, typename T>
inline void ZTuple::GetVector_T(const_iterator iPropIter,
	OutputIterator iIter, const T& iDummy) const
	{ this->GetValue(iPropIter).GetVector_T(iIter, iDummy); }

template <typename OutputIterator, typename T>
inline void ZTuple::GetVector_T(const char* iPropName, OutputIterator iIter, const T& iDummy) const
	{ this->GetValue(iPropName).GetVector_T(iIter, iDummy); }

template <typename OutputIterator, typename T>
inline void ZTuple::GetVector_T(const ZTName& iPropName,
	OutputIterator iIter, const T& iDummy) const
	{ this->GetValue(iPropName).GetVector_T(iIter, iDummy); }

template <typename InputIterator>
inline void ZTuple::SetVector_T(const_iterator iPropIter, InputIterator iBegin, InputIterator iEnd)
	{ this->SetMutableNull(iPropIter).SetVector_T(iBegin, iEnd); }

template <typename InputIterator>
inline void ZTuple::SetVector_T(const char* iPropName, InputIterator iBegin, InputIterator iEnd)
	{ this->SetMutableNull(iPropName).SetVector_T(iBegin, iEnd); }

template <typename InputIterator>
inline void ZTuple::SetVector_T(const ZTName& iPropName,
	InputIterator iBegin, InputIterator iEnd)
	{ this->SetMutableNull(iPropName).SetVector_T(iBegin, iEnd); }

template <typename InputIterator, typename T>
inline void ZTuple::SetVector_T(const_iterator iPropIter,
	InputIterator iBegin, InputIterator iEnd, const T& iDummy)
	{ this->SetMutableNull(iPropIter).SetVector_T(iBegin, iEnd, iDummy); }

template <typename InputIterator, typename T>
inline void ZTuple::SetVector_T(const char* iPropName,
	InputIterator iBegin, InputIterator iEnd, const T& iDummy)
	{ this->SetMutableNull(iPropName).SetVector_T(iBegin, iEnd, iDummy); }

template <typename InputIterator, typename T>
inline void ZTuple::SetVector_T(const ZTName& iPropName,
	InputIterator iBegin, InputIterator iEnd, const T& iDummy)
	{ this->SetMutableNull(iPropName).SetVector_T(iBegin, iEnd, iDummy); }

template <>
inline bool ZTuple::Get_T<ZTValue>(const_iterator iPropIter, ZTValue& oVal) const
	{ return this->GetValue(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<ZType>(const_iterator iPropIter, ZType& oVal) const
	{ return this->GetType(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<uint64>(const_iterator iPropIter, uint64& oVal) const
	{ return this->GetID(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<int8>(const_iterator iPropIter, int8& oVal) const
	{ return this->GetInt8(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<int16>(const_iterator iPropIter, int16& oVal) const
	{ return this->GetInt16(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<int32>(const_iterator iPropIter, int32& oVal) const
	{ return this->GetInt32(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<int64>(const_iterator iPropIter, int64& oVal) const
	{ return this->GetInt64(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<bool>(const_iterator iPropIter, bool& oVal) const
	{ return this->GetBool(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<float>(const_iterator iPropIter, float& oVal) const
	{ return this->GetFloat(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<double>(const_iterator iPropIter, double& oVal) const
	{ return this->GetDouble(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<ZTime>(const_iterator iPropIter, ZTime& oVal) const
	{ return this->GetTime(iPropIter, oVal); }

template <>
inline bool ZTuple::Get_T<std::string>(const_iterator iPropIter, std::string& oVal) const
	{ return this->GetString(iPropIter, oVal); }

//##template <>
//##inline bool ZTuple::Get_T<ZTName>(const_iterator iPropIter, ZTName& oVal) const
//##	{ return this->GetName(iPropIter, oVal); }


template <>
inline bool ZTuple::Get_T<ZTValue>(const char* iPropName, ZTValue& oVal) const
	{ return this->GetValue(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<ZType>(const char* iPropName, ZType& oVal) const
	{ return this->GetType(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<uint64>(const char* iPropName, uint64& oVal) const
	{ return this->GetID(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int8>(const char* iPropName, int8& oVal) const
	{ return this->GetInt8(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int16>(const char* iPropName, int16& oVal) const
	{ return this->GetInt16(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int32>(const char* iPropName, int32& oVal) const
	{ return this->GetInt32(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int64>(const char* iPropName, int64& oVal) const
	{ return this->GetInt64(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<bool>(const char* iPropName, bool& oVal) const
	{ return this->GetBool(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<float>(const char* iPropName, float& oVal) const
	{ return this->GetFloat(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<double>(const char* iPropName, double& oVal) const
	{ return this->GetDouble(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<ZTime>(const char* iPropName, ZTime& oVal) const
	{ return this->GetTime(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<std::string>(const char* iPropName, std::string& oVal) const
	{ return this->GetString(iPropName, oVal); }

//##template <>
//##inline bool ZTuple::Get_T<ZTName>(const char* iPropName, ZTName& oVal) const
//##	{ return this->GetName(iPropName, oVal); }


template <>
inline bool ZTuple::Get_T<ZTValue>(const ZTName& iPropName, ZTValue& oVal) const
	{ return this->GetValue(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<ZType>(const ZTName& iPropName, ZType& oVal) const
	{ return this->GetType(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<uint64>(const ZTName& iPropName, uint64& oVal) const
	{ return this->GetID(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int8>(const ZTName& iPropName, int8& oVal) const
	{ return this->GetInt8(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int16>(const ZTName& iPropName, int16& oVal) const
	{ return this->GetInt16(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int32>(const ZTName& iPropName, int32& oVal) const
	{ return this->GetInt32(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<int64>(const ZTName& iPropName, int64& oVal) const
	{ return this->GetInt64(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<bool>(const ZTName& iPropName, bool& oVal) const
	{ return this->GetBool(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<float>(const ZTName& iPropName, float& oVal) const
	{ return this->GetFloat(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<double>(const ZTName& iPropName, double& oVal) const
	{ return this->GetDouble(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<ZTime>(const ZTName& iPropName, ZTime& oVal) const
	{ return this->GetTime(iPropName, oVal); }

template <>
inline bool ZTuple::Get_T<std::string>(const ZTName& iPropName, std::string& oVal) const
	{ return this->GetString(iPropName, oVal); }

//##template <>
//##inline bool ZTuple::Get_T<ZTName>(const ZTName& iPropName, ZTName& oVal) const
//##	{ return this->GetName(iPropName, oVal); }


template <>
inline ZTValue ZTuple::Get_T<ZTValue>(const_iterator iPropIter) const
	{ return this->GetValue(iPropIter); }

template <>
inline ZType ZTuple::Get_T<ZType>(const_iterator iPropIter) const
	{ return this->GetType(iPropIter); }

template <>
inline uint64 ZTuple::Get_T<uint64>(const_iterator iPropIter) const
	{ return this->GetID(iPropIter); }

template <>
inline int8 ZTuple::Get_T<int8>(const_iterator iPropIter) const
	{ return this->GetInt8(iPropIter); }

template <>
inline int16 ZTuple::Get_T<int16>(const_iterator iPropIter) const
	{ return this->GetInt16(iPropIter); }

template <>
inline int32 ZTuple::Get_T<int32>(const_iterator iPropIter) const
	{ return this->GetInt32(iPropIter); }

template <>
inline int64 ZTuple::Get_T<int64>(const_iterator iPropIter) const
	{ return this->GetInt64(iPropIter); }

template <>
inline bool ZTuple::Get_T<bool>(const_iterator iPropIter) const
	{ return this->GetBool(iPropIter); }

template <>
inline float ZTuple::Get_T<float>(const_iterator iPropIter) const
	{ return this->GetFloat(iPropIter); }

template <>
inline double ZTuple::Get_T<double>(const_iterator iPropIter) const
	{ return this->GetDouble(iPropIter); }

template <>
inline ZTime ZTuple::Get_T<ZTime>(const_iterator iPropIter) const
	{ return this->GetTime(iPropIter); }

template <>
inline std::string ZTuple::Get_T<std::string>(const_iterator iPropIter) const
	{ return this->GetString(iPropIter); }

//##template <>
//##inline ZTName ZTuple::Get_T<ZTName>(const_iterator iPropIter) const
//##	{ return this->GetName(iPropIter); }


template <>
inline ZTValue ZTuple::Get_T<ZTValue>(const char* iPropName) const
	{ return this->GetValue(iPropName); }

template <>
inline ZType ZTuple::Get_T<ZType>(const char* iPropName) const
	{ return this->GetType(iPropName); }

template <>
inline uint64 ZTuple::Get_T<uint64>(const char* iPropName) const
	{ return this->GetID(iPropName); }

template <>
inline int8 ZTuple::Get_T<int8>(const char* iPropName) const
	{ return this->GetInt8(iPropName); }

template <>
inline int16 ZTuple::Get_T<int16>(const char* iPropName) const
	{ return this->GetInt16(iPropName); }

template <>
inline int32 ZTuple::Get_T<int32>(const char* iPropName) const
	{ return this->GetInt32(iPropName); }

template <>
inline int64 ZTuple::Get_T<int64>(const char* iPropName) const
	{ return this->GetInt64(iPropName); }

template <>
inline bool ZTuple::Get_T<bool>(const char* iPropName) const
	{ return this->GetBool(iPropName); }

template <>
inline float ZTuple::Get_T<float>(const char* iPropName) const
	{ return this->GetFloat(iPropName); }

template <>
inline double ZTuple::Get_T<double>(const char* iPropName) const
	{ return this->GetDouble(iPropName); }

template <>
inline ZTime ZTuple::Get_T<ZTime>(const char* iPropName) const
	{ return this->GetTime(iPropName); }

template <>
inline std::string ZTuple::Get_T<std::string>(const char* iPropName) const
	{ return this->GetString(iPropName); }

//##template <>
//##inline ZTName ZTuple::Get_T<ZTName>(const char* iPropName) const
//##	{ return this->GetName(iPropName); }


template <>
inline ZTValue ZTuple::Get_T<ZTValue>(const ZTName& iPropName) const
	{ return this->GetValue(iPropName); }

template <>
inline ZType ZTuple::Get_T<ZType>(const ZTName& iPropName) const
	{ return this->GetType(iPropName); }

template <>
inline uint64 ZTuple::Get_T<uint64>(const ZTName& iPropName) const
	{ return this->GetID(iPropName); }

template <>
inline int8 ZTuple::Get_T<int8>(const ZTName& iPropName) const
	{ return this->GetInt8(iPropName); }

template <>
inline int16 ZTuple::Get_T<int16>(const ZTName& iPropName) const
	{ return this->GetInt16(iPropName); }

template <>
inline int32 ZTuple::Get_T<int32>(const ZTName& iPropName) const
	{ return this->GetInt32(iPropName); }

template <>
inline int64 ZTuple::Get_T<int64>(const ZTName& iPropName) const
	{ return this->GetInt64(iPropName); }

template <>
inline bool ZTuple::Get_T<bool>(const ZTName& iPropName) const
	{ return this->GetBool(iPropName); }

template <>
inline float ZTuple::Get_T<float>(const ZTName& iPropName) const
	{ return this->GetFloat(iPropName); }

template <>
inline double ZTuple::Get_T<double>(const ZTName& iPropName) const
	{ return this->GetDouble(iPropName); }

template <>
inline ZTime ZTuple::Get_T<ZTime>(const ZTName& iPropName) const
	{ return this->GetTime(iPropName); }

template <>
inline std::string ZTuple::Get_T<std::string>(const ZTName& iPropName) const
	{ return this->GetString(iPropName); }

//##template <>
//##inline ZTName ZTuple::Get_T<ZTName>(const ZTName& iPropName) const
//##	{ return this->GetName(iPropName); }


template <>
inline ZType ZTuple::DGet_T<ZType>(const_iterator iPropIter, ZType iDefault) const
	{ return this->DGetType(iPropIter, iDefault); }

template <>
inline uint64 ZTuple::DGet_T<uint64>(const_iterator iPropIter, uint64 iDefault) const
	{ return this->DGetID(iPropIter, iDefault); }

template <>
inline int8 ZTuple::DGet_T<int8>(const_iterator iPropIter, int8 iDefault) const
	{ return this->DGetInt8(iPropIter, iDefault); }

template <>
inline int16 ZTuple::DGet_T<int16>(const_iterator iPropIter, int16 iDefault) const
	{ return this->DGetInt16(iPropIter, iDefault); }

template <>
inline int32 ZTuple::DGet_T<int32>(const_iterator iPropIter, int32 iDefault) const
	{ return this->DGetInt32(iPropIter, iDefault); }

template <>
inline int64 ZTuple::DGet_T<int64>(const_iterator iPropIter, int64 iDefault) const
	{ return this->DGetInt64(iPropIter, iDefault); }

template <>
inline bool ZTuple::DGet_T<bool>(const_iterator iPropIter, bool iDefault) const
	{ return this->DGetBool(iPropIter, iDefault); }

template <>
inline float ZTuple::DGet_T<float>(const_iterator iPropIter, float iDefault) const
	{ return this->DGetFloat(iPropIter, iDefault); }

template <>
inline double ZTuple::DGet_T<double>(const_iterator iPropIter, double iDefault) const
	{ return this->DGetDouble(iPropIter, iDefault); }

template <>
inline ZTime ZTuple::DGet_T<ZTime>(const_iterator iPropIter, ZTime iDefault) const
	{ return this->DGetTime(iPropIter, iDefault); }

template <>
inline std::string ZTuple::DGet_T<std::string>(const_iterator iPropIter, std::string iDefault) const
	{ return this->DGetString(iPropIter, iDefault); }

//##template <>
//##inline ZTName ZTuple::DGet_T<ZTName>(const_iterator iPropIter, ZTName iDefault) const
//##	{ return this->DGetName(iPropIter, iDefault); }


template <>
inline ZType ZTuple::DGet_T<ZType>(const char* iPropName, ZType iDefault) const
	{ return this->DGetType(iPropName, iDefault); }

template <>
inline uint64 ZTuple::DGet_T<uint64>(const char* iPropName, uint64 iDefault) const
	{ return this->DGetID(iPropName, iDefault); }

template <>
inline int8 ZTuple::DGet_T<int8>(const char* iPropName, int8 iDefault) const
	{ return this->DGetInt8(iPropName, iDefault); }

template <>
inline int16 ZTuple::DGet_T<int16>(const char* iPropName, int16 iDefault) const
	{ return this->DGetInt16(iPropName, iDefault); }

template <>
inline int32 ZTuple::DGet_T<int32>(const char* iPropName, int32 iDefault) const
	{ return this->DGetInt32(iPropName, iDefault); }

template <>
inline int64 ZTuple::DGet_T<int64>(const char* iPropName, int64 iDefault) const
	{ return this->DGetInt64(iPropName, iDefault); }

template <>
inline bool ZTuple::DGet_T<bool>(const char* iPropName, bool iDefault) const
	{ return this->DGetBool(iPropName, iDefault); }

template <>
inline float ZTuple::DGet_T<float>(const char* iPropName, float iDefault) const
	{ return this->DGetFloat(iPropName, iDefault); }

template <>
inline double ZTuple::DGet_T<double>(const char* iPropName, double iDefault) const
	{ return this->DGetDouble(iPropName, iDefault); }

template <>
inline ZTime ZTuple::DGet_T<ZTime>(const char* iPropName, ZTime iDefault) const
	{ return this->DGetTime(iPropName, iDefault); }

template <>
inline std::string ZTuple::DGet_T<std::string>(const char* iPropName, std::string iDefault) const
	{ return this->DGetString(iPropName, iDefault); }

//##template <>
//##inline ZTName ZTuple::DGet_T<ZTName>(const char* iPropName, ZTName iDefault) const
//##	{ return this->DGetName(iPropName, iDefault); }


template <>
inline ZType ZTuple::DGet_T<ZType>(const ZTName& iPropName, ZType iDefault) const
	{ return this->DGetType(iPropName, iDefault); }

template <>
inline uint64 ZTuple::DGet_T<uint64>(const ZTName& iPropName, uint64 iDefault) const
	{ return this->DGetID(iPropName, iDefault); }

template <>
inline int8 ZTuple::DGet_T<int8>(const ZTName& iPropName, int8 iDefault) const
	{ return this->DGetInt8(iPropName, iDefault); }

template <>
inline int16 ZTuple::DGet_T<int16>(const ZTName& iPropName, int16 iDefault) const
	{ return this->DGetInt16(iPropName, iDefault); }

template <>
inline int32 ZTuple::DGet_T<int32>(const ZTName& iPropName, int32 iDefault) const
	{ return this->DGetInt32(iPropName, iDefault); }

template <>
inline int64 ZTuple::DGet_T<int64>(const ZTName& iPropName, int64 iDefault) const
	{ return this->DGetInt64(iPropName, iDefault); }

template <>
inline bool ZTuple::DGet_T<bool>(const ZTName& iPropName, bool iDefault) const
	{ return this->DGetBool(iPropName, iDefault); }

template <>
inline float ZTuple::DGet_T<float>(const ZTName& iPropName, float iDefault) const
	{ return this->DGetFloat(iPropName, iDefault); }

template <>
inline double ZTuple::DGet_T<double>(const ZTName& iPropName, double iDefault) const
	{ return this->DGetDouble(iPropName, iDefault); }

template <>
inline ZTime ZTuple::DGet_T<ZTime>(const ZTName& iPropName, ZTime iDefault) const
	{ return this->DGetTime(iPropName, iDefault); }

template <>
inline std::string ZTuple::DGet_T<std::string>(const ZTName& iPropName,
	std::string iDefault) const
	{ return this->DGetString(iPropName, iDefault); }

//##template <>
//##inline ZTName ZTuple::DGet_T<ZTName>(const ZTName& iPropName, ZTName iDefault) const
//##	{ return this->DGetName(iPropName, iDefault); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleRep

/// ZTupleRep is an implementation detail. Try not to build anything that depends on it.

class ZTupleRep : public ZRefCounted
	{
public:
	static inline bool sCheckAccessEnabled() { return false; }

	ZTupleRep();
	ZTupleRep(const ZTupleRep* iOther);
	virtual ~ZTupleRep();

	/** fProperties is public to allow direct manipulation by utility code. But it
	is subject to change, so be careful. */
	ZTuple::PropList fProperties;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple inlines

inline ZTuple::ZTuple()
	{}

inline ZTuple::ZTuple(const ZTuple& iOther)
:	fRep(iOther.fRep)
	{}

inline ZTuple::~ZTuple()
	{}

inline ZTuple& ZTuple::operator=(const ZTuple& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

inline ZTuple::ZTuple(ZRef<ZTupleRep> iRep)
:	fRep(iRep)
	{}

inline ZTuple::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fProperties.empty()); }

inline ZTuple ZTuple::Under(const ZTuple& iOver) const
	{ return iOver.Over(*this); }

inline bool ZTuple::operator==(const ZTuple& iOther) const
	{ return fRep == iOther.fRep || this->pIsEqual(iOther); }

inline bool ZTuple::operator!=(const ZTuple& iOther) const
	{ return !(*this == iOther); }

inline bool ZTuple::operator<=(const ZTuple& iOther) const
	{ return !(iOther < *this); }

inline bool ZTuple::operator>(const ZTuple& iOther) const
	{ return iOther < *this; }

inline bool ZTuple::operator>=(const ZTuple& iOther) const
	{ return !(*this < iOther); }

inline bool ZTuple::IsSameAs(const ZTuple& iOther) const
	{ return fRep == iOther.fRep || this->pIsSameAs(iOther); }

inline void ZTuple::swap(ZTuple& iOther)
	{ std::swap(fRep, iOther.fRep); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple::NameTV inlines

inline ZTuple::NameTV::NameTV()
	{}

inline ZTuple::NameTV::NameTV(const NameTV& iOther)
:	fTV(iOther.fTV),
	fName(iOther.fName)
	{}

inline ZTuple::NameTV::NameTV(const char* iName, const ZTValue& iTV)
:	fTV(iTV),
	fName(iName)
	{}

inline ZTuple::NameTV::NameTV(const ZTName& iName, const ZTValue& iTV)
:	fTV(iTV),
	fName(iName)
	{}

inline ZTuple::NameTV::NameTV(const char* iName)
:	fName(iName)
	{}

inline ZTuple::NameTV::NameTV(const ZTName& iName)
:	fName(iName)
	{}

template <> inline int sCompare_T(const ZTValue& iL, const ZTValue& iR)
	{ return iL.Compare(iR); }

NAMESPACE_ZOOLIB_END

namespace std {
inline void swap(ZOOLIB_PREFIX::ZTValue& a, ZOOLIB_PREFIX::ZTValue& b)
	{ a.swap(b); }
} // namespace std

namespace std {
inline void swap(ZOOLIB_PREFIX::ZTuple& a, ZOOLIB_PREFIX::ZTuple& b)
	{ a.swap(b); }
} // namespace std

#endif // __ZTuple__
