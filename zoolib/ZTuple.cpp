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

#include "zoolib/ZTuple.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZMemoryBlock.h"

#include <stdexcept> // For runtime_error

using std::min;
using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

#define kDebug_Tuple 1

static string sNilString;
static vector<ZTValue> sNilVector;
static ZTName sNilName;
static ZPointPOD sNilPoint;
static ZRectPOD sNilRect;
static ZTuple sNilTuple;
static ZTuple::PropList sEmptyProperties;
static ZTValue sNilValue;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static inline void sWriteCount(const ZStreamW& iStreamW, uint32 iCount)
	{ iStreamW.WriteCount(iCount); }

static inline uint32 sReadCount(const ZStreamR& iStreamR)
	{ return iStreamR.ReadCount(); }

static inline int sCompare(const void* iLeft, size_t iLeftLength,
	const void* iRight, size_t iRightLength)
	{
	if (int compare = memcmp(iLeft, iRight, min(iLeftLength, iRightLength)))
		return compare;
	// Strictly speaking the rules of two's complement mean that we
	// not need the casts, but let's be clear about what we're doing.
	return int(iLeftLength) - int(iRightLength);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleString

/** ZTupleString is a lighterweight implementation of immutable strings, used
for holding longer string properties in ZTValues. */

namespace ZANONYMOUS {

class ZTupleString
	{
	ZTupleString(); // Not implemented
	ZTupleString& operator=(const ZTupleString&); // Not implemented

public:
	~ZTupleString();

	ZTupleString(const ZTupleString& iOther);

	explicit ZTupleString(const string& iOther);

	ZTupleString(const char* iString, size_t iLength);

	ZTupleString(const ZStreamR& iStreamR, size_t iSize);

	int Compare(const ZTupleString& iOther) const;

	int Compare(const char* iString, size_t iSize) const;

	int Compare(const std::string& iString) const;

	bool Empty() const;

	void ToStream(const ZStreamW& iStreamW) const;

	void ToString(std::string& oString) const;

	std::string AsString() const;

private:
	const size_t fSize;
	char* fBuffer;
	};

} // namespace ZANONYMOUS

inline ZTupleString::~ZTupleString()
	{ delete[] fBuffer; }

inline bool ZTupleString::Empty() const
	{ return !fSize; }

inline std::string ZTupleString::AsString() const
	{ return string(fBuffer, fSize); }

ZTupleString::ZTupleString(const ZTupleString& iOther)
:	fSize(iOther.fSize)
	{
	fBuffer = new char[fSize];
	ZBlockCopy(iOther.fBuffer, fBuffer, fSize);
	}

ZTupleString::ZTupleString(const string& iOther)
:	fSize(iOther.size())
	{
	fBuffer = new char[fSize];
	if (fSize)
		ZBlockCopy(iOther.data(), fBuffer, fSize);
	}

ZTupleString::ZTupleString(const char* iString, size_t iLength)
:	fSize(iLength)
	{
	fBuffer = new char[fSize];
	ZBlockCopy(iString, fBuffer, fSize);
	}

ZTupleString::ZTupleString(const ZStreamR& iStreamR, size_t iSize)
:	fSize(iSize)
	{
	fBuffer = new char[iSize];
	try
		{
		iStreamR.Read(fBuffer, iSize);
		}
	catch (...)
		{
		delete fBuffer;
		throw;
		}
	}

inline int ZTupleString::Compare(const ZTupleString& iOther) const
	{ return sCompare(fBuffer, fSize, iOther.fBuffer, iOther.fSize); }

inline int ZTupleString::Compare(const char* iString, size_t iSize) const
	{ return sCompare(fBuffer, fSize, iString, iSize); }

inline int ZTupleString::Compare(const string& iString) const
	{
	if (size_t otherSize = iString.size())
		return sCompare(fBuffer, fSize, iString.data(), otherSize);

	// iString is empty. If fSize is zero, then fSize!=0 is true, and
	// will convert to 1. Otherwise it will be false, and convert
	// to zero, so we always return the correct result.
	return fSize != 0;
	}

void ZTupleString::ToStream(const ZStreamW& iStreamW) const
	{
	sWriteCount(iStreamW, fSize);
	iStreamW.Write(fBuffer, fSize);
	}

void ZTupleString::ToString(string& oString) const
	{
	oString.resize(fSize);
	if (fSize)
		ZBlockCopy(fBuffer, const_cast<char*>(oString.data()), fSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Placement-new

namespace ZANONYMOUS {

template <class T>
inline T* sConstruct_T(void* iBytes)
	{
	new(iBytes) T();
	return static_cast<T*>(iBytes);
	}

template <class T>
inline T* sConstruct_T(void* iBytes, const T& iOther)
	{
	new(iBytes) T(iOther);
	return static_cast<T*>(iBytes);
	}

template <class T, typename P0>
inline T* sConstruct_T(void* iBytes, const P0& iP0)
	{
	new(iBytes) T(iP0);
	return static_cast<T*>(iBytes);
	}

template <class T, typename P0, typename P1>
inline T* sConstruct_T(void* iBytes, const P0& iP0, const P1& iP1)
	{
	new(iBytes) T(iP0, iP1);
	return static_cast<T*>(iBytes);
	}

template <class T>
inline T* sCopyConstruct_T(const void* iSource, void* iBytes)
	{
	new(iBytes) T(*static_cast<const T*>(iSource));
	return static_cast<T*>(iBytes);
	}

template <class T>
inline void sDestroy_T(void* iBytes)
	{
	static_cast<T*>(iBytes)->~T();
	}

template <class T>
inline const T* sFetch_T(const void* iBytes)
	{
	return static_cast<const T*>(iBytes);
	}

template <class T>
inline T* sFetch_T(void* iBytes)
	{
	return static_cast<T*>(iBytes);
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue::Ex_IllegalType

ZTValue::Ex_IllegalType::Ex_IllegalType(int iType)
:	runtime_error("Ex_IllegalType"),
	fType(iType)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTValue

/** \class ZTValue
\nosubgrouping
*/

ZTValue::ZTValue()
	{
	fType.fType = eZType_Null;
	}

ZTValue::~ZTValue()
	{
	this->pRelease();
	}

ZTValue::ZTValue(const ZTValue& iOther)
	{
	this->pCopy(iOther);
	}

ZTValue& ZTValue::operator=(const ZTValue& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopy(iOther);
		}
	return *this;
	}

ZTValue::ZTValue(const ZStreamR& iStreamR)
	{
	fType.fType = eZType_Null;
	this->pFromStream(iStreamR);
	}

ZTValue::ZTValue(bool dummy, const ZStreamR& iStreamR)
	{
	fType.fType = eZType_Null;
	this->pFromStream(iStreamR);
	}

void ZTValue::ToStream(const ZStreamW& iStreamW) const
	{
	if (fType.fType < 0)
		{
		iStreamW.WriteUInt8(eZType_String);
		size_t theSize = -fType.fType-1;
		ZAssertStop(kDebug_Tuple, theSize <= kBytesSize);
		sWriteCount(iStreamW, theSize);
		if (theSize)
			iStreamW.Write(fType.fBytes, theSize);
		return;
		}

	iStreamW.WriteUInt8(fType.fType);
	switch (fType.fType)
		{
		case eZType_Null:
			{
			// No data to write.
			break;
			}
		case eZType_Type:
			{
			iStreamW.WriteUInt8(fData.fAs_Type);
			break;
			}
		case eZType_ID:
			{
			iStreamW.WriteUInt64(fData.fAs_ID);
			break;
			}
		case eZType_Int8:
			{
			iStreamW.WriteUInt8(fData.fAs_Int8);
			break;
			}
		case eZType_Int16:
			{
			iStreamW.WriteUInt16(fData.fAs_Int16);
			break;
			}
		case eZType_Int32:
			{
			iStreamW.WriteUInt32(fData.fAs_Int32);
			break;
			}
		case eZType_Int64:
			{
			iStreamW.WriteUInt64(fData.fAs_Int64);
			break;
			}
		case eZType_Bool:
			{
			iStreamW.WriteUInt8(fData.fAs_Bool ? 1 : 0);
			break;
			}
		case eZType_Float:
			{
			iStreamW.WriteFloat(fData.fAs_Float);
			break;
			}
		case eZType_Double:
			{
			iStreamW.WriteDouble(fData.fAs_Double);
			break;
			}
		case eZType_Time:
			{
			iStreamW.WriteDouble(fData.fAs_Time);
			break;
			}
		case eZType_Pointer:
			{
			iStreamW.WriteUInt32(reinterpret_cast<uint32>(fData.fAs_Pointer));
			break;
			}
		case eZType_Rect:
			{
			iStreamW.WriteUInt32(fData.fAs_Rect->left);
			iStreamW.WriteUInt32(fData.fAs_Rect->top);
			iStreamW.WriteUInt32(fData.fAs_Rect->right);
			iStreamW.WriteUInt32(fData.fAs_Rect->bottom);
			break;
			}
		case eZType_Point:
			{
			iStreamW.WriteUInt32(fData.fAs_Point.h);
			iStreamW.WriteUInt32(fData.fAs_Point.v);
			break;
			}
		case eZType_String:
			{
			sFetch_T<ZTupleString>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
#if 0//##
		case eZType_Name:
			{
			sFetch_T<ZTName>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
#endif//##
		case eZType_Tuple:
			{
			sFetch_T<ZTuple>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
		case eZType_RefCounted:
			{
			// Just do nothing. We'll construct a nil refcounted when we read.
			break;
			}
		case eZType_Raw:
			{
			const ZMemoryBlock* theMemoryBlock = sFetch_T<ZMemoryBlock>(fType.fBytes);
			sWriteCount(iStreamW, theMemoryBlock->GetSize());
			if (theMemoryBlock->GetSize())
				iStreamW.Write(theMemoryBlock->GetData(), theMemoryBlock->GetSize());
			break;
			}
		case eZType_Vector:
			{
			if (const vector<ZTValue>* theVector = fData.fAs_Vector)
				{
				size_t theCount = theVector->size();
				sWriteCount(iStreamW, theCount);
				for (size_t x = 0; x < theCount; ++x)
					{
					// Sigh, older linux headers don't have vector::at
					// theVector->at(x).ToStream(iStreamW);
					theVector->operator[](x).ToStream(iStreamW);
					}
				}
			else
				{
				// Empty vector optimization
				sWriteCount(iStreamW, 0);
				}
			break;
			}
		default:
			{
			ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

void ZTValue::FromStream(const ZStreamR& iStreamR)
	{
	this->pRelease();
	fType.fType = eZType_Null;
	this->pFromStream(iStreamR);
	}

void ZTValue::ToStreamOld(const ZStreamW& iStreamW) const
	{
	switch (fType.fType)
		{
		case eZType_Time:
			{
			ZDebugStopf(kDebug_Tuple, ("Times must not have ToStreamOld called on them"));
			break;
			}
#if 0//##
		case eZType_Name:
			{
			ZDebugStopf(kDebug_Tuple, ("Names must not have ToStreamOld called on them"));
			break;
			}
#endif//##
		case eZType_Vector:
			{
			ZDebugStopf(kDebug_Tuple, ("Vectors must not have ToStreamOld called on them"));
			break;
			}
		case eZType_Tuple:
			{
			iStreamW.WriteUInt8(eZType_Tuple);
			sFetch_T<ZTuple>(fType.fBytes)->ToStreamOld(iStreamW);
			break;
			}
		default:
			{
			this->ToStream(iStreamW);
			break;
			}
		}
	}

void ZTValue::FromStreamOld(const ZStreamR& iStreamR)
	{
	// This differs from ZTuple::FromStream only in its handling of values that are
	// tuples and that it does not support eZType_Time or eZType_Vector (which didn't exist
	// when this format was originally written).
	this->pRelease();
	int myType = iStreamR.ReadUInt8();
	switch (myType)
		{
		case eZType_Null:
			{
			// No data to read.
			break;
			}
		case eZType_Type:
			{
			fData.fAs_Type = ZType(iStreamR.ReadUInt8());
			break;
			}
		case eZType_ID:
			{
			fData.fAs_ID = iStreamR.ReadUInt64();
			break;
			}
		case eZType_Int8:
			{
			fData.fAs_Int8 = iStreamR.ReadUInt8();
			break;
			}
		case eZType_Int16:
			{
			fData.fAs_Int16 = iStreamR.ReadUInt16();
			break;
			}
		case eZType_Int32:
			{
			fData.fAs_Int32 = iStreamR.ReadUInt32();
			break;
			}
		case eZType_Int64:
			{
			fData.fAs_Int64 = iStreamR.ReadUInt64();
			break;
			}
		case eZType_Bool:
			{
			fData.fAs_Bool = iStreamR.ReadUInt8();
			break;
			}
		case eZType_Float:
			{
			fData.fAs_Float = iStreamR.ReadFloat();
			break;
			}
		case eZType_Double:
			{
			fData.fAs_Double = iStreamR.ReadDouble();
			break;
			}
		case eZType_Time:
			{
			// Not expecting a time.
			throw Ex_IllegalType(eZType_Time);
			break;
			}
		case eZType_Pointer:
			{
			fData.fAs_Pointer = reinterpret_cast<void*>(iStreamR.ReadUInt32());
			break;
			}
		case eZType_Rect:
			{
			fData.fAs_Rect = new ZRectPOD;
			fData.fAs_Rect->left = iStreamR.ReadUInt32();
			fData.fAs_Rect->top = iStreamR.ReadUInt32();
			fData.fAs_Rect->right = iStreamR.ReadUInt32();
			fData.fAs_Rect->bottom = iStreamR.ReadUInt32();
			break;
			}
		case eZType_Point:
			{
			fData.fAs_Point.h = iStreamR.ReadUInt32();
			fData.fAs_Point.v = iStreamR.ReadUInt32();
			break;
			}
		case eZType_String:
			{
			uint32 theSize = sReadCount(iStreamR);
			if (theSize <= kBytesSize)
				{
				myType = -theSize-1;
				if (theSize)
					iStreamR.Read(fType.fBytes, theSize);
				}
			else
				{
				sConstruct_T<ZTupleString>(fType.fBytes, iStreamR, theSize);
				}
			break;
			}
#if 0//##
		case eZType_Name:
			{
			// Not expecting a name.
			throw Ex_IllegalType(eZType_Name);
			break;
			}
#endif//##
		case eZType_Tuple:
			{
			ZTuple* theTuple = sConstruct_T<ZTuple>(fType.fBytes);
			// This is what makes ZTValue::FromStreamOld
			// different from ZTValue::FromStream.
			theTuple->FromStreamOld(iStreamR);
			break;
			}
		case eZType_RefCounted:
			{
			// Construct a nil refcounted.
			sConstruct_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);
			break;
			}
		case eZType_Raw:
			{
			uint32 size = sReadCount(iStreamR);
			ZMemoryBlock* theRaw = sConstruct_T<ZMemoryBlock>(fType.fBytes, size);
			if (size)
				iStreamR.Read(theRaw->GetData(), size);
			break;
			}
		case eZType_Vector:
			{
			// Not expecting a vector.
			throw Ex_IllegalType(eZType_Vector);
			break;
			}
		default:
			{
			throw Ex_IllegalType(myType);
			}
		}
	fType.fType = ZType(myType);
	}

ZTValue::ZTValue(const ZTValue& iVal, bool iAsVector)
	{
	ZAssertStop(kDebug_Tuple, iAsVector);
	fType.fType = eZType_Vector;
	fData.fAs_Vector = new vector<ZTValue>(1, iVal);
	}

ZTValue::ZTValue(ZType iVal)
	{
	fType.fType = eZType_Type;
	fData.fAs_Type = iVal;
	}

ZTValue::ZTValue(uint64 iVal, bool iIsID)
	{
	ZAssertStop(kDebug_Tuple, iIsID);
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

ZTValue::ZTValue(uint64 iVal)
	{
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

ZTValue::ZTValue(int8 iVal)
	{
	fType.fType = eZType_Int8;
	fData.fAs_Int8 = iVal;
	}

ZTValue::ZTValue(int16 iVal)
	{
	fType.fType = eZType_Int16;
	fData.fAs_Int16 = iVal;
	}

ZTValue::ZTValue(int32 iVal)
	{
	fType.fType = eZType_Int32;
	fData.fAs_Int32 = iVal;
	}

ZTValue::ZTValue(int64 iVal)
	{
	fType.fType = eZType_Int64;
	fData.fAs_Int64 = iVal;
	}

ZTValue::ZTValue(bool iVal)
	{
	fType.fType = eZType_Bool;
	fData.fAs_Bool = iVal;
	}

ZTValue::ZTValue(float iVal)
	{
	fType.fType = eZType_Float;
	fData.fAs_Float = iVal;
	}

ZTValue::ZTValue(double iVal)
	{
	fType.fType = eZType_Double;
	fData.fAs_Double = iVal;
	}

ZTValue::ZTValue(ZTime iVal)
	{
	fType.fType = eZType_Time;
	fData.fAs_Time = iVal.fVal;
	}

ZTValue::ZTValue(void* inPointer)
	{
	fType.fType = eZType_Pointer;
	fData.fAs_Pointer = inPointer;
	}

ZTValue::ZTValue(const ZRectPOD& iVal)
	{
	fType.fType = eZType_Rect;
	fData.fAs_Rect = new ZRectPOD(iVal);
	}

ZTValue::ZTValue(const ZPointPOD& iVal)
	{
	fType.fType = eZType_Point;
	fData.fAs_Point = iVal;
	}

ZTValue::ZTValue(const char* iVal)
	{
	size_t theSize = strlen(iVal);
	if (theSize <= kBytesSize)
		{
		fType.fType = -theSize-1;
		if (theSize)
			ZBlockCopy(iVal, fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ZTupleString>(fType.fBytes, iVal, theSize);
		}
	}

ZTValue::ZTValue(const string& iVal)
	{
	size_t theSize = iVal.size();
	if (theSize <= kBytesSize)
		{
		fType.fType = -theSize-1;
		if (theSize)
			ZBlockCopy(iVal.data(), fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ZTupleString>(fType.fBytes, iVal);
		}
	}

#if 0//##
ZTValue::ZTValue(const ZTName& iVal)
	{
	fType.fType = eZType_Name;
	sConstruct_T(fType.fBytes, iVal);
	}
#endif//##

ZTValue::ZTValue(const ZTuple& iVal)
	{
	fType.fType = eZType_Tuple;
	sConstruct_T(fType.fBytes, iVal);
	}

ZTValue::ZTValue(const ZRef<ZRefCountedWithFinalization>& iVal)
	{
	fType.fType = eZType_RefCounted;
	sConstruct_T(fType.fBytes, iVal);
	}

ZTValue::ZTValue(const ZMemoryBlock& iVal)
	{
	fType.fType = eZType_Raw;
	sConstruct_T(fType.fBytes, iVal);
	}

ZTValue::ZTValue(const void* iSource, size_t iSize)
	{
	fType.fType = eZType_Raw;
	sConstruct_T<ZMemoryBlock>(fType.fBytes, iSource, iSize);
	}

ZTValue::ZTValue(const ZStreamR& iStreamR, size_t iSize)
	{
	fType.fType = eZType_Raw;
	ZMemoryBlock* theRaw = sConstruct_T<ZMemoryBlock>(fType.fBytes);
	ZStreamRWPos_MemoryBlock(*theRaw).CopyFrom(iStreamR, iSize);
	}

ZTValue::ZTValue(const vector<ZTValue>& iVal)
	{
	fType.fType = eZType_Vector;
	if (iVal.empty())
		fData.fAs_Vector = nil;
	else
		fData.fAs_Vector = new vector<ZTValue>(iVal);
	}

int ZTValue::Compare(const ZTValue& iOther) const
	{
	if (this == &iOther)
		return 0;

	if (fType.fType < 0)
		{
		// We're a special string.
		if (iOther.fType.fType < 0)
			{
			// So is iOther
			return sCompare(fType.fBytes, -fType.fType-1,
				iOther.fType.fBytes, -iOther.fType.fType-1);
			}
		else if (iOther.fType.fType == eZType_String)
			{
			// iOther is a regular string.
			return -sFetch_T<ZTupleString>(iOther.fType.fBytes)
				->Compare(fType.fBytes, -fType.fType-1);
			}
		else
			{
			return int(eZType_String) - int(iOther.fType.fType);
			}
		}
	else
		{
		// We're not a special string.
		if (iOther.fType.fType < 0)
			{
			// iOther is a special string.
			if (fType.fType == eZType_String)
				{
				// We're a regular string.
				return sFetch_T<ZTupleString>(fType.fBytes)
					->Compare(iOther.fType.fBytes, -iOther.fType.fType-1);
				}
			else
				{
				return int(fType.fType) - int(eZType_String);
				}
			}
		}


	if (fType.fType < iOther.fType.fType)
		return -1;

	if (fType.fType > iOther.fType.fType)
		return 1;

	return this->pUncheckedCompare(iOther);
	}

bool ZTValue::operator==(const ZTValue& iOther) const
	{
	return this->Compare(iOther) == 0;
	}

bool ZTValue::operator<(const ZTValue& iOther) const
	{
	return this->Compare(iOther) < 0;
	}

bool ZTValue::IsSameAs(const ZTValue& iOther) const
	{
	if (this == &iOther)
		return true;

	if (fType.fType != iOther.fType.fType)
		return false;

	switch (fType.fType)
		{
		case eZType_Tuple:
			{
			return sFetch_T<ZTuple>(fType.fBytes)->IsSameAs(*sFetch_T<ZTuple>(iOther.fType.fBytes));
			}

		case eZType_Vector:
			{
			const vector<ZTValue>* thisVector = fData.fAs_Vector;
			const vector<ZTValue>* otherVector = iOther.fData.fAs_Vector;

			if (!thisVector || thisVector->empty())
				{
				// We're empty. We're the same if other is empty.
				return !otherVector || otherVector->empty();
				}
			else if (!otherVector || otherVector->empty())
				{
				// We're not empty, but other is, so we're different.
				return false;
				}

			vector<ZTValue>::const_iterator thisEnd = thisVector->end();
			vector<ZTValue>::const_iterator otherEnd = otherVector->end();
			
			vector<ZTValue>::const_iterator thisIter = thisVector->begin();
			vector<ZTValue>::const_iterator otherIter = otherVector->begin();
			
			for (;;)
				{
				if (thisIter == thisEnd)
					return otherIter == otherEnd;

				if (otherIter == otherEnd)
					return false;

				if (!(*thisIter).IsSameAs(*otherIter))
					return false;

				++thisIter;
				++otherIter;
				}
			}
		}

	return this->pUncheckedEqual(iOther);
	}

bool ZTValue::IsString(const char* iString) const
	{
	return this->GetString() == iString;
	}

bool ZTValue::IsString(const string& iString) const
	{
	return this->GetString() == iString;
	}

bool ZTValue::GetType(ZType& oVal) const
	{
	if (fType.fType == eZType_Type)
		{
		oVal = fData.fAs_Type;
		return true;
		}
	return false;
	}
	
ZType ZTValue::GetType() const
	{
	if (fType.fType == eZType_Type)
		return fData.fAs_Type;
	return eZType_Null;
	}

bool ZTValue::GetID(uint64& oVal) const
	{
	if (fType.fType == eZType_ID)
		{
		oVal = fData.fAs_ID;
		return true;
		}
	return false;
	}

uint64 ZTValue::GetID() const
	{
	if (fType.fType == eZType_ID)
		return fData.fAs_ID;
	return 0;
	}

bool ZTValue::GetInt8(int8& oVal) const
	{
	if (fType.fType == eZType_Int8)
		{
		oVal = fData.fAs_Int8;
		return true;
		}
	return false;
	}
	
int8 ZTValue::GetInt8() const
	{
	if (fType.fType == eZType_Int8)
		return fData.fAs_Int8;
	return 0;
	}

bool ZTValue::GetInt16(int16& oVal) const
	{
	if (fType.fType == eZType_Int16)
		{
		oVal = fData.fAs_Int16;
		return true;
		}
	return false;
	}

int16 ZTValue::GetInt16() const
	{
	if (fType.fType == eZType_Int16)
		return fData.fAs_Int16;
	return 0;
	}

bool ZTValue::GetInt32(int32& oVal) const
	{
	if (fType.fType == eZType_Int32)
		{
		oVal = fData.fAs_Int32;
		return true;
		}
	return false;
	}

int32 ZTValue::GetInt32() const
	{
	if (fType.fType == eZType_Int32)
		return fData.fAs_Int32;
	return 0;
	}

bool ZTValue::GetInt64(int64& oVal) const
	{
	if (fType.fType == eZType_Int64)
		{
		oVal = fData.fAs_Int64;
		return true;
		}
	return false;
	}

int64 ZTValue::GetInt64() const
	{
	if (fType.fType == eZType_Int64)
		return fData.fAs_Int64;
	return 0;
	}

bool ZTValue::GetBool(bool& oVal) const
	{
	if (fType.fType == eZType_Bool)
		{
		oVal = fData.fAs_Bool;
		return true;
		}
	return false;
	}

bool ZTValue::GetBool() const
	{
	if (fType.fType == eZType_Bool)
		return fData.fAs_Bool;
	return false;
	}

bool ZTValue::GetFloat(float& oVal) const
	{
	if (fType.fType == eZType_Float)
		{
		oVal = fData.fAs_Float;
		return true;
		}
	return false;
	}

float ZTValue::GetFloat() const
	{
	if (fType.fType == eZType_Float)
		return fData.fAs_Float;
	return 0.0f;
	}

bool ZTValue::GetDouble(double& oVal) const
	{
	if (fType.fType == eZType_Double)
		{
		oVal = fData.fAs_Double;
		return true;
		}
	return false;
	}

double ZTValue::GetDouble() const
	{
	if (fType.fType == eZType_Double)
		return fData.fAs_Double;
	return 0.0;
	}

bool ZTValue::GetTime(ZTime& oVal) const
	{
	if (fType.fType == eZType_Time)
		{
		oVal = fData.fAs_Time;
		return true;
		}
	return false;
	}

ZTime ZTValue::GetTime() const
	{
	if (fType.fType == eZType_Time)
		return fData.fAs_Time;
	return ZTime();
	}

bool ZTValue::GetPointer(void*& oVal) const
	{
	if (fType.fType == eZType_Pointer)
		{
		oVal = fData.fAs_Pointer;
		return true;
		}
	return false;
	}

void* ZTValue::GetPointer() const
	{
	if (fType.fType == eZType_Pointer)
		return fData.fAs_Pointer;
	return 0;
	}

bool ZTValue::GetRect(ZRectPOD& oVal) const
	{
	if (fType.fType == eZType_Rect)
		{
		oVal = *fData.fAs_Rect;
		return true;
		}
	return false;
	}

const ZRectPOD& ZTValue::GetRect() const
	{
	if (fType.fType == eZType_Rect)
		return *fData.fAs_Rect;
	return sNilRect;
	}

bool ZTValue::GetPoint(ZPointPOD& oVal) const
	{
	if (fType.fType == eZType_Point)
		{
		oVal = fData.fAs_Point;
		return true;
		}
	return false;
	}

const ZPointPOD& ZTValue::GetPoint() const
	{
	if (fType.fType == eZType_Point)
		return fData.fAs_Point;
	return sNilPoint;
	}

bool ZTValue::GetString(string& oVal) const
	{
	if (fType.fType == eZType_String)
		{
		sFetch_T<ZTupleString>(fType.fBytes)->ToString(oVal);
		return true;
		}
	else if (fType.fType < 0)
		{
		oVal = string(fType.fBytes, -fType.fType-1);
		return true;
		}
	return false;
	}

string ZTValue::GetString() const
	{
	if (fType.fType == eZType_String)
		{
		return sFetch_T<ZTupleString>(fType.fBytes)->AsString();
		}
	else if (fType.fType < 0)
		{
		return string(fType.fBytes, -fType.fType-1);
		}
	return sNilString;
	}

#if 0 //##
bool ZTValue::GetName(ZTName& oVal) const
	{
	if (fType.fType == eZType_Name)
		{
		oVal = *sFetch_T<ZTName>(fType.fBytes);
		return true;
		}
	return false;
	}

const ZTName& ZTValue::GetName() const
	{
	if (fType.fType == eZType_Name)
		return *sFetch_T<ZTName>(fType.fBytes);
	return sNilName;
	}
#endif //##

bool ZTValue::GetTuple(ZTuple& oVal) const
	{
	if (fType.fType == eZType_Tuple)
		{
		oVal = *sFetch_T<ZTuple>(fType.fBytes);
		return true;
		}
	return false;
	}

const ZTuple& ZTValue::GetTuple() const
	{
	if (fType.fType == eZType_Tuple)
		return *sFetch_T<ZTuple>(fType.fBytes);
	return sNilTuple;
	}

bool ZTValue::GetRefCounted(ZRef<ZRefCountedWithFinalization>& oVal) const
	{
	if (fType.fType == eZType_RefCounted)
		{
		oVal = *sFetch_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);
		return true;
		}
	return false;
	}

ZRef<ZRefCountedWithFinalization> ZTValue::GetRefCounted() const
	{
	if (fType.fType == eZType_RefCounted)
		return *sFetch_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);
	return ZRef<ZRefCountedWithFinalization>();
	}

bool ZTValue::GetRaw(ZMemoryBlock& oVal) const
	{
	if (fType.fType == eZType_Raw)
		{
		oVal = *sFetch_T<ZMemoryBlock>(fType.fBytes);
		return true;
		}
	return false;
	}

ZMemoryBlock ZTValue::GetRaw() const
	{
	if (fType.fType == eZType_Raw)
		return *sFetch_T<ZMemoryBlock>(fType.fBytes);
	return ZMemoryBlock();
	}

bool ZTValue::GetRaw(void* iDest, size_t iSize) const
	{
	if (fType.fType == eZType_Raw)
		{
		const ZMemoryBlock* theRaw = sFetch_T<ZMemoryBlock>(fType.fBytes);
		size_t sizeNeeded = theRaw->GetSize();
		if (iSize >= sizeNeeded)
			{
			theRaw->CopyTo(iDest, iSize);
			return true;
			}
		}
	return false;
	}

bool ZTValue::GetRawAttributes(const void** oAddress, size_t* oSize) const
	{
	if (fType.fType == eZType_Raw)
		{
		const ZMemoryBlock* theRaw = sFetch_T<ZMemoryBlock>(fType.fBytes);
		if (oAddress)
			*oAddress = theRaw->GetData();
		if (oSize)
			*oSize = theRaw->GetSize();
		return true;
		}
	return false;
	}

bool ZTValue::GetVector(vector<ZTValue>& oVal) const
	{
	if (fType.fType == eZType_Vector)
		{
		if (fData.fAs_Vector)
			oVal = *fData.fAs_Vector;
		else
			oVal.clear();
		return true;
		}
	return false;
	}

const vector<ZTValue>& ZTValue::GetVector() const
	{
	if (fType.fType == eZType_Vector)
		{
		if (fData.fAs_Vector)
			return *fData.fAs_Vector;
		}
	return sNilVector;
	}
	
ZType ZTValue::DGetType(ZType iDefault) const
	{
	if (fType.fType == eZType_Type)
		return fData.fAs_Type;
	return iDefault;
	}

uint64 ZTValue::DGetID(uint64 iDefault) const
	{
	if (fType.fType == eZType_ID)
		return fData.fAs_ID;
	return iDefault;
	}
	
int8 ZTValue::DGetInt8(int8 iDefault) const
	{
	if (fType.fType == eZType_Int8)
		return fData.fAs_Int8;
	return iDefault;
	}

int16 ZTValue::DGetInt16(int16 iDefault) const
	{
	if (fType.fType == eZType_Int16)
		return fData.fAs_Int16;
	return iDefault;
	}

int32 ZTValue::DGetInt32(int32 iDefault) const
	{
	if (fType.fType == eZType_Int32)
		return fData.fAs_Int32;
	return iDefault;
	}

int64 ZTValue::DGetInt64(int64 iDefault) const
	{
	if (fType.fType == eZType_Int64)
		return fData.fAs_Int64;
	return iDefault;
	}

bool ZTValue::DGetBool(bool iDefault) const
	{
	if (fType.fType == eZType_Bool)
		return fData.fAs_Bool;
	return iDefault;
	}

float ZTValue::DGetFloat(float iDefault) const
	{
	if (fType.fType == eZType_Float)
		return fData.fAs_Float;
	return iDefault;
	}

double ZTValue::DGetDouble(double iDefault) const
	{
	if (fType.fType == eZType_Double)
		return fData.fAs_Double;
	return iDefault;
	}

ZTime ZTValue::DGetTime(ZTime iDefault) const
	{
	if (fType.fType == eZType_Time)
		return fData.fAs_Time;
	return iDefault;
	}

ZRectPOD ZTValue::DGetRect(const ZRectPOD& iDefault) const
	{
	if (fType.fType == eZType_Rect)
		return *fData.fAs_Rect;
	return iDefault;
	}

ZPointPOD ZTValue::DGetPoint(const ZPointPOD& iDefault) const
	{
	if (fType.fType == eZType_Point)
		return fData.fAs_Point;
	return iDefault;
	}

string ZTValue::DGetString(const string& iDefault) const
	{
	if (fType.fType == eZType_String)
		{
		return sFetch_T<ZTupleString>(fType.fBytes)->AsString();
		}
	else if (fType.fType < 0)
		{
		return string(fType.fBytes, -fType.fType-1);
		}
	return iDefault;
	}

#if 0 //##
ZTName ZTValue::DGetName(const ZTName& iDefault) const
	{
	if (fType.fType == eZType_Name)
		return *sFetch_T<ZTName>(fType.fBytes);
	return iDefault;
	}
#endif //##

void ZTValue::SetNull()
	{
	this->pRelease();
	fType.fType = eZType_Null;
	}

void ZTValue::SetType(ZType iVal)
	{
	this->pRelease();
	fType.fType = eZType_Type;
	fData.fAs_Type = iVal;
	}

void ZTValue::SetID(uint64 iVal)
	{
	this->pRelease();
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

void ZTValue::SetInt8(int8 iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int8;
	fData.fAs_Int8 = iVal;
	}

void ZTValue::SetInt16(int16 iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int16;
	fData.fAs_Int16 = iVal;
	}

void ZTValue::SetInt32(int32 iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int32;
	fData.fAs_Int32 = iVal;
	}

void ZTValue::SetInt64(int64 iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int64;
	fData.fAs_Int64 = iVal;
	}

void ZTValue::SetBool(bool iVal)
	{
	this->pRelease();
	fType.fType = eZType_Bool;
	fData.fAs_Bool = iVal;
	}

void ZTValue::SetFloat(float iVal)
	{
	this->pRelease();
	fType.fType = eZType_Float;
	fData.fAs_Float = iVal;
	}

void ZTValue::SetDouble(double iVal)
	{
	this->pRelease();
	fType.fType = eZType_Double;
	fData.fAs_Double = iVal;
	}

void ZTValue::SetTime(ZTime iVal)
	{
	this->pRelease();
	fType.fType = eZType_Time;
	fData.fAs_Time = iVal.fVal;
	}

void ZTValue::SetPointer(void* iVal)
	{
	this->pRelease();
	fType.fType = eZType_Pointer;
	fData.fAs_Pointer = iVal;
	}

void ZTValue::SetRect(const ZRectPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Rect;
	fData.fAs_Rect = new ZRectPOD(iVal);
	}

void ZTValue::SetPoint(const ZPointPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Point;
	fData.fAs_Point = iVal;
	}

void ZTValue::SetString(const char* iVal)
	{
	this->pRelease();
	size_t theSize = strlen(iVal);
	if (theSize <= kBytesSize)
		{
		fType.fType = -theSize-1;
		if (theSize)
			ZBlockCopy(iVal, fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ZTupleString>(fType.fBytes, iVal, theSize);
		}
	}

void ZTValue::SetString(const string& iVal)
	{
	this->pRelease();
	size_t theSize = iVal.size();
	if (theSize <= kBytesSize)
		{
		fType.fType = -theSize-1;
		if (theSize)
			ZBlockCopy(iVal.data(), fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ZTupleString>(fType.fBytes, iVal);
		}
	}

#if 0//##
void ZTValue::SetName(const char* iVal)
	{
	this->pRelease();
	fType.fType = eZType_Name;
	sConstruct_T<ZTName>(fType.fBytes, iVal);
	}

void ZTValue::SetName(const std::string& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Name;
	sConstruct_T<ZTName>(fType.fBytes, iVal);
	}

void ZTValue::SetName(const ZTName& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Name;
	// AG 2007-08-06. For some reason sConstruct_T<ZTName> is ambiguous with CW.
	// It works okay for ZTuple et al, but not here.
	sConstruct_T<ZTName, ZTName>(fType.fBytes, iVal);
	}
#endif//##

void ZTValue::SetTuple(const ZTuple& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Tuple;
	sConstruct_T(fType.fBytes, iVal);
	}

void ZTValue::SetRefCounted(const ZRef<ZRefCountedWithFinalization>& iVal)
	{
	this->pRelease();
	fType.fType = eZType_RefCounted;
	sConstruct_T(fType.fBytes, iVal);
	}

void ZTValue::SetRaw(const ZMemoryBlock& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Raw;
	sConstruct_T(fType.fBytes, iVal);
	}

void ZTValue::SetRaw(const void* iSource, size_t iSize)
	{
	this->pRelease();
	fType.fType = eZType_Raw;
	sConstruct_T<ZMemoryBlock>(fType.fBytes, iSource, iSize);
	}

void ZTValue::SetRaw(const ZStreamR& iStreamR, size_t iSize)
	{
	ZMemoryBlock* theMemoryBlock;
	if (fType.fType != eZType_Raw)
		{
		this->pRelease();
		fType.fType = eZType_Raw;
		theMemoryBlock = sConstruct_T<ZMemoryBlock>(fType.fBytes, iSize);
		}
	else
		{
		theMemoryBlock = sFetch_T<ZMemoryBlock>(fType.fBytes);
		theMemoryBlock->SetSize(iSize);
		}
	ZStreamRWPos_MemoryBlock(*theMemoryBlock).CopyFrom(iStreamR, iSize);
	}

void ZTValue::SetRaw(const ZStreamR& iStreamR)
	{
	ZMemoryBlock* theMemoryBlock;
	if (fType.fType != eZType_Raw)
		{
		this->pRelease();
		fType.fType = eZType_Raw;
		theMemoryBlock = sConstruct_T<ZMemoryBlock>(fType.fBytes);
		}
	else
		{
		theMemoryBlock = sFetch_T<ZMemoryBlock>(fType.fBytes);
		theMemoryBlock->SetSize(0);
		}
	ZStreamRWPos_MemoryBlock(*theMemoryBlock).CopyAllFrom(iStreamR);
	}

void ZTValue::SetVector(const vector<ZTValue>& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Vector;
	if (iVal.empty())
		fData.fAs_Vector= nil;
	else
		fData.fAs_Vector = new vector<ZTValue>(iVal);
	}

ZTuple& ZTValue::GetMutableTuple()
	{
	// We're returning a non-const tuple, one that can be manipulated
	// by the caller, so we must actually *be* a tuple.
	ZAssertStop(kDebug_Tuple, fType.fType == eZType_Tuple);

	return *sFetch_T<ZTuple>(fType.fBytes);
	}

vector<ZTValue>& ZTValue::GetMutableVector()
	{
	// We're returning a non-const vector, one that can be manipulated
	// by the caller. So we must actually *be* a vector, we can't
	// return the usual dummy default value.
	ZAssertStop(kDebug_Tuple, fType.fType == eZType_Vector);

	if (!fData.fAs_Vector)
		{
		// The empty vector optimization is in effect, but
		// we need a real vector to return.
		fData.fAs_Vector = new vector<ZTValue>;
		}
	return *fData.fAs_Vector;
	}

ZTuple& ZTValue::EnsureMutableTuple()
	{
	if (fType.fType == eZType_Tuple)
		return *sFetch_T<ZTuple>(fType.fBytes);

	this->pRelease();
	fType.fType = eZType_Tuple;
	return *sConstruct_T<ZTuple>(fType.fBytes);
	}

vector<ZTValue>& ZTValue::EnsureMutableVector()
	{
	if (fType.fType == eZType_Vector)
		{
		if (!fData.fAs_Vector)
			fData.fAs_Vector = new vector<ZTValue>;
		}
	else
		{
		this->pRelease();
		fType.fType = eZType_Vector;
		fData.fAs_Vector = nil;
		fData.fAs_Vector = new vector<ZTValue>;
		}
	return *fData.fAs_Vector;	
	}

ZTValue& ZTValue::SetMutableNull()
	{
	this->pRelease();
	fType.fType = eZType_Null;
	return *this;
	}

ZTuple& ZTValue::SetMutableTuple()
	{
	this->pRelease();
	fType.fType = eZType_Tuple;
	return *sConstruct_T<ZTuple>(fType.fBytes);
	}

vector<ZTValue>& ZTValue::SetMutableVector()
	{
	// SetMutableVector is usually called so that external code
	// can populate the vector that's actually stored by us.
	// So we don't want to use the empty vector optimization
	// and thus must ensure we actually have a vector allocated.
	if (fType.fType == eZType_Vector)
		{
		if (fData.fAs_Vector)
			fData.fAs_Vector->clear();
		else
			fData.fAs_Vector = new vector<ZTValue>;
		}
	else
		{
		this->pRelease();
		fType.fType = eZType_Vector;
		fData.fAs_Vector = nil;
		fData.fAs_Vector = new vector<ZTValue>;
		}
	return *fData.fAs_Vector;
	}

void ZTValue::AppendValue(const ZTValue& iVal)
	{
	if (fType.fType == eZType_Vector)
		{
		if (!fData.fAs_Vector)
			fData.fAs_Vector = new vector<ZTValue>(1, iVal);
		else
			fData.fAs_Vector->push_back(iVal);
		}
	else if (fType.fType == eZType_Null)
		{
		fData.fAs_Vector = new vector<ZTValue>(1, iVal);
		fType.fType = eZType_Vector;
		}
	else
		{
		vector<ZTValue>* newVector = new vector<ZTValue>(1, *this);
		try
			{
			newVector->push_back(iVal);
			}
		catch (...)
			{
			delete newVector;
			throw;
			}

		this->pRelease();
		fType.fType = eZType_Vector;
		fData.fAs_Vector = newVector;
		}
	}

int ZTValue::pUncheckedCompare(const ZTValue& iOther) const
	{
	// Our types are assumed to be the same, and not special strings.
	ZAssertStop(kDebug_Tuple, fType.fType == iOther.fType.fType);
	switch (fType.fType)
		{
		case eZType_Null: return 0;
		case eZType_Type: return ZooLib::sCompare_T(fData.fAs_Type, iOther.fData.fAs_Type);
		case eZType_ID: return ZooLib::sCompare_T(fData.fAs_ID, iOther.fData.fAs_ID);
		case eZType_Int8: return ZooLib::sCompare_T(fData.fAs_Int8, iOther.fData.fAs_Int8);
		case eZType_Int16: return ZooLib::sCompare_T(fData.fAs_Int16, iOther.fData.fAs_Int16);
		case eZType_Int32: return ZooLib::sCompare_T(fData.fAs_Int32, iOther.fData.fAs_Int32);
		case eZType_Int64: return ZooLib::sCompare_T(fData.fAs_Int64, iOther.fData.fAs_Int64);
		case eZType_Bool: return ZooLib::sCompare_T(fData.fAs_Bool, iOther.fData.fAs_Bool);
		case eZType_Float:  return ZooLib::sCompare_T(fData.fAs_Float, iOther.fData.fAs_Float);
		case eZType_Double: return ZooLib::sCompare_T(fData.fAs_Double, iOther.fData.fAs_Double);
		case eZType_Time: return ZooLib::sCompare_T(fData.fAs_Time, iOther.fData.fAs_Time);
		case eZType_Pointer: return ZooLib::sCompare_T(fData.fAs_Pointer, iOther.fData.fAs_Pointer);
		case eZType_Rect: return ZooLib::sCompare_T(*fData.fAs_Rect, *iOther.fData.fAs_Rect);
		case eZType_Point: return ZooLib::sCompare_T(fData.fAs_Point, iOther.fData.fAs_Point);
		case eZType_String:
			{
			return sFetch_T<ZTupleString>(fType.fBytes)
				->Compare(*sFetch_T<ZTupleString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return sFetch_T<ZTName>(fType.fBytes)->Compare(*sFetch_T<ZTName>(iOther.fType.fBytes));
			}
#endif//##
		case eZType_Tuple:
			{
			return sFetch_T<ZTuple>(fType.fBytes)->Compare(*sFetch_T<ZTuple>(iOther.fType.fBytes));
			}
		case eZType_RefCounted:
			{
			const ZRef<ZRefCountedWithFinalization>* thisZRef
				= sFetch_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);

			const ZRef<ZRefCountedWithFinalization>* otherZRef
				= sFetch_T<ZRef<ZRefCountedWithFinalization> >(iOther.fType.fBytes);
			
			if (*thisZRef < *otherZRef)
				return -1;
			else if (*otherZRef < *thisZRef)
				return 1;
			return 0;
			}
		case eZType_Raw:
			{
			return sFetch_T<ZMemoryBlock>(fType.fBytes)->Compare(
				*sFetch_T<ZMemoryBlock>(iOther.fType.fBytes));
			}
		case eZType_Vector:
			{
			if (!fData.fAs_Vector || fData.fAs_Vector->empty())
				{
				// We're empty.
				if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
					{
					// And so is other, so we're equal.
					return 0;
					}
				else
					{
					// Other has data, so we're less than it.
					return -1;
					}
				}
			else if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
				{
				// We're not empty, but iOther is, so we're greater.
				return 1;
				}
			return ZooLib::sCompare_T(*fData.fAs_Vector, *iOther.fData.fAs_Vector);
			}
		}
	ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
	return 0;
	}

bool ZTValue::pUncheckedLess(const ZTValue& iOther) const
	{
	// Our types are assumed to be the same, and not special strings.
	ZAssertStop(kDebug_Tuple, fType.fType == iOther.fType.fType);
	switch (fType.fType)
		{
		case eZType_Null: return false;
		case eZType_Type: return fData.fAs_Type < iOther.fData.fAs_Type;
		case eZType_ID: return fData.fAs_ID < iOther.fData.fAs_ID;
		case eZType_Int8: return fData.fAs_Int8 < iOther.fData.fAs_Int8;
		case eZType_Int16: return fData.fAs_Int16 < iOther.fData.fAs_Int16;
		case eZType_Int32: return fData.fAs_Int32 < iOther.fData.fAs_Int32;
		case eZType_Int64: return fData.fAs_Int64 < iOther.fData.fAs_Int64;
		case eZType_Bool: return fData.fAs_Bool < iOther.fData.fAs_Bool;
		case eZType_Float: return fData.fAs_Float < iOther.fData.fAs_Float;
		case eZType_Double: return fData.fAs_Double < iOther.fData.fAs_Double;
		case eZType_Time: return fData.fAs_Time < iOther.fData.fAs_Time;
		case eZType_Pointer: return fData.fAs_Pointer < iOther.fData.fAs_Pointer;
		case eZType_Rect: return ZooLib::sCompare_T(*fData.fAs_Rect, *iOther.fData.fAs_Rect) < 0;
		case eZType_Point: return ZooLib::sCompare_T(fData.fAs_Point, iOther.fData.fAs_Point) < 0;

		case eZType_String:
			{
			return 0 > sFetch_T<ZTupleString>(fType.fBytes)
				->Compare(*sFetch_T<ZTupleString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return *sFetch_T<ZTName>(fType.fBytes) < *sFetch_T<ZTName>(iOther.fType.fBytes);
			}
#endif//##
		case eZType_Tuple:
			{
			return *sFetch_T<ZTuple>(fType.fBytes) < *sFetch_T<ZTuple>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes)
				< *sFetch_T<ZRef<ZRefCountedWithFinalization> >(iOther.fType.fBytes);
			}
		case eZType_Raw:
			{
			return *sFetch_T<ZMemoryBlock>(fType.fBytes)
				< *sFetch_T<ZMemoryBlock>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			if (!fData.fAs_Vector || fData.fAs_Vector->empty())
				{
				// We're empty.
				if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
					{
					// And so is other, so we're not less than it.
					return false;
					}
				else
					{
					// Other has data, so we're less than it.
					return true;
					}
				}
			else if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
				{
				// We're not empty, but iOther is, so we're not less than it.
				return false;
				}
			return (*fData.fAs_Vector) < (*iOther.fData.fAs_Vector);
			}
		}
	ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
	return false;
	}

bool ZTValue::pUncheckedEqual(const ZTValue& iOther) const
	{
	// Our types are assumed to be the same, and not special strings.
	ZAssertStop(kDebug_Tuple, fType.fType == iOther.fType.fType);
	switch (fType.fType)
		{
		case eZType_Null: return true;
		case eZType_Type: return fData.fAs_Type == iOther.fData.fAs_Type;
		case eZType_ID: return fData.fAs_ID == iOther.fData.fAs_ID;
		case eZType_Int8: return fData.fAs_Int8 == iOther.fData.fAs_Int8;
		case eZType_Int16: return fData.fAs_Int16 == iOther.fData.fAs_Int16;
		case eZType_Int32: return fData.fAs_Int32 == iOther.fData.fAs_Int32;
		case eZType_Int64: return fData.fAs_Int64 == iOther.fData.fAs_Int64;
		case eZType_Bool: return fData.fAs_Bool == iOther.fData.fAs_Bool;
		case eZType_Float: return fData.fAs_Float == iOther.fData.fAs_Float;
		case eZType_Double: return fData.fAs_Double == iOther.fData.fAs_Double;
		case eZType_Time: return fData.fAs_Time == iOther.fData.fAs_Time;
		case eZType_Pointer: return fData.fAs_Pointer == iOther.fData.fAs_Pointer;
		case eZType_Rect: return fData.fAs_Rect->left == iOther.fData.fAs_Rect->left
							&& fData.fAs_Rect->top == iOther.fData.fAs_Rect->top
							&& fData.fAs_Rect->right == iOther.fData.fAs_Rect->right
							&& fData.fAs_Rect->bottom == iOther.fData.fAs_Rect->bottom;
		case eZType_Point: return fData.fAs_Point.h == iOther.fData.fAs_Point.h
							&& fData.fAs_Point.v == iOther.fData.fAs_Point.v;

		case eZType_String:
			{
			return 0 == sFetch_T<ZTupleString>(fType.fBytes)
				->Compare(*sFetch_T<ZTupleString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return *sFetch_T<ZTName>(fType.fBytes) == *sFetch_T<ZTName>(iOther.fType.fBytes);
			}
#endif//##
		case eZType_Tuple:
			{
			return *sFetch_T<ZTuple>(fType.fBytes) == *sFetch_T<ZTuple>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes)
				== *sFetch_T<ZRef<ZRefCountedWithFinalization> >(iOther.fType.fBytes);
			}
		case eZType_Raw:
			{
			return *sFetch_T<ZMemoryBlock>(fType.fBytes)
				== *sFetch_T<ZMemoryBlock>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			if (!fData.fAs_Vector || fData.fAs_Vector->empty())
				{
				// We're empty.
				if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
					{
					// And so is other, so we're equal.
					return true;
					}
				else
					{
					// Other has data, so we're not equal.
					return false;
					}
				}
			else if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
				{
				// We're not empty, but iOther is, so we're not equal.
				return false;
				}
			return (*fData.fAs_Vector) == (*iOther.fData.fAs_Vector);
			}
		}
	ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
	return false;
	}

void ZTValue::pRelease()
	{
	switch (fType.fType)
		{
		case eZType_Null:
		case eZType_Type:
		case eZType_ID:
		case eZType_Int8:
		case eZType_Int16:
		case eZType_Int32:
		case eZType_Int64:
		case eZType_Bool:
		case eZType_Float:
		case eZType_Double:
		case eZType_Time:
		case eZType_Pointer:
		case eZType_Point:
			break;
		case eZType_Rect: delete fData.fAs_Rect; break;

		case eZType_String: sDestroy_T<ZTupleString>(fType.fBytes); break;
//##		case eZType_Name: sDestroy_T<ZTName>(fType.fBytes); break;
		case eZType_Tuple: sDestroy_T<ZTuple>(fType.fBytes); break;
		case eZType_RefCounted:
			{
			sDestroy_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);
			break;
			}
		case eZType_Raw: sDestroy_T<ZMemoryBlock>(fType.fBytes); break;
		case eZType_Vector: delete fData.fAs_Vector; break;
		default:
			{
			if (fType.fType >= 0)
				ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

void ZTValue::pCopy(const ZTValue& iOther)
	{
	if (iOther.fType.fType < 0)
		{
		fFastCopy = iOther.fFastCopy;
		}
	else
		{
		fType.fType = iOther.fType.fType;
		switch (fType.fType)
			{
			case eZType_Null:
				// No data to be copied.
				break;
			case eZType_Type:
				fData.fAs_Type = iOther.fData.fAs_Type;
				break;
			case eZType_ID:
				fData.fAs_ID = iOther.fData.fAs_ID;
				break;
			case eZType_Int8:
				fData.fAs_Int8 = iOther.fData.fAs_Int8;
				break;
			case eZType_Int16:
				fData.fAs_Int16 = iOther.fData.fAs_Int16;
				break;
			case eZType_Int32:
				fData.fAs_Int32 = iOther.fData.fAs_Int32;
				break;
			case eZType_Int64:
				fData.fAs_Int64 = iOther.fData.fAs_Int64;
				break;
			case eZType_Bool:
				fData.fAs_Bool = iOther.fData.fAs_Bool;
				break;
			case eZType_Float:
				fData.fAs_Float = iOther.fData.fAs_Float;
				break;
			case eZType_Double:
				fData.fAs_Double = iOther.fData.fAs_Double;
				break;
			case eZType_Time:
				fData.fAs_Time = iOther.fData.fAs_Time;
				break;
			case eZType_Pointer:
				fData.fAs_Pointer = iOther.fData.fAs_Pointer;
				break;
			case eZType_Rect:
				fData.fAs_Rect = new ZRectPOD(*iOther.fData.fAs_Rect);
				break;
			case eZType_Point:
				fData.fAs_Point = iOther.fData.fAs_Point;
				break;
			case eZType_String:
				sCopyConstruct_T<ZTupleString>(iOther.fType.fBytes, fType.fBytes);
				break;
#if 0//##
			case eZType_Name:
				sCopyConstruct_T<ZTName>(iOther.fType.fBytes, fType.fBytes);
				break;
#endif//##
			case eZType_Tuple:
				sCopyConstruct_T<ZTuple>(iOther.fType.fBytes, fType.fBytes);
				break;
			case eZType_RefCounted:
				sCopyConstruct_T<ZRef<ZRefCountedWithFinalization> >
					(iOther.fType.fBytes, fType.fBytes);				
				break;
			case eZType_Raw:
				sCopyConstruct_T<ZMemoryBlock>(iOther.fType.fBytes, fType.fBytes);
				break;
			case eZType_Vector:
				if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
					fData.fAs_Vector = nil;
				else
					fData.fAs_Vector = new vector<ZTValue>(*iOther.fData.fAs_Vector);
				break;
			default:
				ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

static vector<ZTValue>* sAllocVector(size_t iSize)
	{
	return new vector<ZTValue>(iSize, ZTValue());
	}

static ZRectPOD* sAllocRect()
	{
	return new ZRectPOD;
	}

void ZTValue::pFromStream(const ZStreamR& iStreamR)
	{
	int myType = iStreamR.ReadUInt8();
	switch (myType)
		{
		case eZType_Null:
			{
			// No data to read.
			break;
			}
		case eZType_Type:
			{
			fData.fAs_Type = ZType(iStreamR.ReadUInt8());
			break;
			}
		case eZType_ID:
			{
			fData.fAs_ID = iStreamR.ReadUInt64();
			break;
			}
		case eZType_Int8:
			{
			fData.fAs_Int8 = iStreamR.ReadUInt8();
			break;
			}
		case eZType_Int16:
			{
			fData.fAs_Int16 = iStreamR.ReadUInt16();
			break;
			}
		case eZType_Int32:
			{
			fData.fAs_Int32 = iStreamR.ReadUInt32();
			break;
			}
		case eZType_Int64:
			{
			fData.fAs_Int64 = iStreamR.ReadUInt64();
			break;
			}
		case eZType_Bool:
			{
			fData.fAs_Bool = iStreamR.ReadUInt8();
			break;
			}
		case eZType_Float:
			{
			fData.fAs_Float = iStreamR.ReadFloat();
			break;
			}
		case eZType_Double:
			{
			fData.fAs_Double = iStreamR.ReadDouble();
			break;
			}
		case eZType_Time:
			{
			fData.fAs_Time = iStreamR.ReadDouble();
			break;
			}
		case eZType_Pointer:
			{
			fData.fAs_Pointer = reinterpret_cast<void*>(iStreamR.ReadUInt32());
			break;
			}
		case eZType_Rect:
			{
			ZRectPOD* theRect = sAllocRect();
			try
				{
				theRect->left = iStreamR.ReadUInt32();
				theRect->top = iStreamR.ReadUInt32();
				theRect->right = iStreamR.ReadUInt32();
				theRect->bottom = iStreamR.ReadUInt32();
				}
			catch (...)
				{
				delete theRect;
				throw;
				}
			fData.fAs_Rect = theRect;
			break;
			}
		case eZType_Point:
			{
			fData.fAs_Point.h = iStreamR.ReadUInt32();
			fData.fAs_Point.v = iStreamR.ReadUInt32();
			break;
			}
		case eZType_String:
			{
			uint32 theSize = sReadCount(iStreamR);
			if (theSize <= kBytesSize)
				{
				myType = -theSize-1;
				if (theSize)
					iStreamR.Read(fType.fBytes, theSize);
				}
			else
				{
				sConstruct_T<ZTupleString>(fType.fBytes, iStreamR, theSize);
				}
			break;
			}
#if 0//##
		case eZType_Name:
			{
			sConstruct_T<ZTName>(fType.fBytes, iStreamR);
			break;
			}
#endif//##
		case eZType_Tuple:
			{
			sConstruct_T<ZTuple>(fType.fBytes, iStreamR);
			break;
			}
		case eZType_RefCounted:
			{
			// Construct a nil refcounted.
			sConstruct_T<ZRef<ZRefCountedWithFinalization> >(fType.fBytes);
			break;
			}
		case eZType_Raw:
			{
			uint32 size = sReadCount(iStreamR);
			ZMemoryBlock* theRaw = sConstruct_T<ZMemoryBlock>(fType.fBytes, size);
			if (size)
				{
				try
					{
					iStreamR.Read(theRaw->GetData(), size);
					}
				catch (...)
					{
					delete theRaw;
					throw;
					}
				}
			break;
			}
		case eZType_Vector:
			{
			if (uint32 theCount = sReadCount(iStreamR))
				{
				vector<ZTValue>* theVector = sAllocVector(theCount);
				for (size_t x = 0; x < theCount; ++x)
					{
					try
						{
						(*theVector)[x].FromStream(iStreamR);
						}
					catch (...)
						{
						delete theVector;
						throw;
						}
					}
				fData.fAs_Vector = theVector;
				}
			else
				{
				fData.fAs_Vector = nil;
				}
			break;
			}
		default:
			{
			throw Ex_IllegalType(myType);
			}
		}
	fType.fType = ZType(myType);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple

/** \class ZTuple
\nosubgrouping
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Constructing/assigning from stream

ZTuple::ZTuple(const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

ZTuple::ZTuple(bool dummy, const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple streaming

void ZTuple::ToStream(const ZStreamW& iStreamW) const
	{
	if (!fRep || fRep->fProperties.empty())
		{
		sWriteCount(iStreamW, 0);
		}
	else
		{
		const PropList& properties = fRep->fProperties;
		sWriteCount(iStreamW, properties.size());
		for (PropList::const_iterator i = properties.begin(); i != properties.end(); ++i)
			{
			(*i).fName.ToStream(iStreamW);
			(*i).fTV.ToStream(iStreamW);
			}
		}
	}

void ZTuple::FromStream(const ZStreamR& iStreamR)
	{ fRep = sRepFromStream(iStreamR); }

void ZTuple::ToStreamOld(const ZStreamW& iStreamW) const
	{
	if (!fRep || fRep->fProperties.empty())
		{
		sWriteCount(iStreamW, 0);
		}
	else
		{
		const PropList& properties = fRep->fProperties;
		sWriteCount(iStreamW, properties.size());
		for (PropList::const_iterator i = properties.begin(); i != properties.end(); ++i)
			{
			(*i).fName.ToStream(iStreamW);

			if (eZType_Vector == (*i).fTV.TypeOf())
				{
				const vector<ZTValue>& theVector = (*i).fTV.GetVector();
				sWriteCount(iStreamW, theVector.size());
				for (vector<ZTValue>::const_iterator j = theVector.begin();
					j != theVector.end(); ++j)
					{
					(*j).ToStreamOld(iStreamW);
					}
				}
			else
				{
				sWriteCount(iStreamW, 1);
				(*i).fTV.ToStreamOld(iStreamW);
				}
			}
		}
	}

void ZTuple::FromStreamOld(const ZStreamR& iStreamR)
	{
	ZRef<ZTupleRep> newRep;
	if (uint32 propertyCount = sReadCount(iStreamR))
		{
		newRep = new ZTupleRep;
		PropList& properties = newRep->fProperties;

		// It's a vector
		properties.reserve(propertyCount);
		properties.resize(propertyCount);
		for (PropList::iterator i = properties.begin(); i != properties.end(); ++i)
			{
			(*i).fName.FromStream(iStreamR);

			if (uint32 valueCount = sReadCount(iStreamR))
				{
				if (valueCount == 1)
					{
					(*i).fTV.FromStreamOld(iStreamR);
					}
				else
					{
					vector<ZTValue>& theVector = (*i).fTV.SetMutableVector();
					theVector.reserve(valueCount);
					theVector.resize(valueCount);
					for (vector<ZTValue>::iterator j = theVector.begin();
						j != theVector.end(); ++j)
						{
						(*j).FromStreamOld(iStreamR);
						}
					}
				}
			}
		}
	fRep = newRep;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple unioning

ZTuple ZTuple::Over(const ZTuple& iUnder) const
	{
	ZTuple result = iUnder;
	if (fRep)
		{
		for (PropList::const_iterator i = fRep->fProperties.begin(),
			theEnd = fRep->fProperties.end();
			i != theEnd; ++i)
			{
			result.SetValue((*i).fName, (*i).fTV);
			}
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple comparison

int ZTuple::Compare(const ZTuple& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We share the same rep, so we're identical.
		return 0;
		}

	if (!fRep)
		{
		// We have no rep, and iOther must have a rep (or fRep would be == iOther.fRep).
		if (iOther.fRep->fProperties.empty())
			{
			// And iOther's property list is empty, we're equivalent.
			return 0;
			}
		else
			{
			// iOther has properties, so we're less than it.
			return -1;
			}
		}

	if (!iOther.fRep)
		{
		// iOther has no rep.
		if (fRep->fProperties.empty())
			{
			// And our property list is empty, so we're equivalent.
			return 0;
			}
		else
			{
			// We have properties, so we're greater than iOther.
			return 1;
			}
		}

	for (PropList::const_iterator iterThis = fRep->fProperties.begin(),
			iterOther = iOther.fRep->fProperties.begin(),
			endThis = fRep->fProperties.end(),
			endOther = iOther.fRep->fProperties.end();
			/*no test*/; ++iterThis, ++iterOther)
		{
		if (iterThis != endThis)
			{
			// This is not exhausted.
			if (iterOther != endOther)
				{
				// Other is not exhausted either, so we compare their current values.
				if (int compare = (*iterThis).fName.Compare((*iterOther).fName))
					{
					// The names are different.
					return compare;
					}
				if (int compare = (*iterThis).fTV.Compare((*iterOther).fTV))
					{
					// The values are different.
					return compare;
					}
				}
			else
				{
				// Exhausted other, but still have this
				// remaining, so this is greater than other.
				return 1;
				}
			}
		else
			{
			// Exhausted this.
			if (iterOther != endOther)
				{
				// Still have other remaining, so this is less than other.
				return -1;
				}
			else
				{
				// Exhausted other. And as this is also
				// exhausted this equals other.
				return 0;
				}
			}
		}
	}

bool ZTuple::operator<(const ZTuple& iOther) const
	{
	return this->Compare(iOther) < 0;
	}

bool ZTuple::Contains(const ZTuple& iOther) const
	{
	if (fRep == iOther.fRep)
		{
		// We have the same rep, and so we contain other.
		return true;
		}

	if (!iOther.fRep || iOther.fRep->fProperties.empty())
		{
		// Other doesn't have a rep or doesn't have any properties, and thus we contain it.
		return true;
		}

	if (!fRep)
		{
		// Other has some properties, or we would have returned above, but
		// we don't have a rep so we can't contain other.
		return false;
		}


	if (fRep->fProperties.size() < iOther.fRep->fProperties.size())
		{
		// We have fewer properties than iOther, and thus can't contain it.
		return false;
		}

	for (PropList::const_iterator iterOther = iOther.fRep->fProperties.begin(),
			endOther = iOther.fRep->fProperties.end(),
			endThis = fRep->fProperties.end();
			iterOther != endOther; ++iterOther)
		{
		const_iterator iterThis = this->IteratorOf((*iterOther).fName);
		if (iterThis == endThis)
			return false;
		if (!(*iterThis).fTV.IsSameAs((*iterOther).fTV))
			return false;
		}

	return true;
	}

bool ZTuple::IsString(const_iterator iPropIter, const char* iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropIter))
		return theValue->IsString(iString);
	return false;
	}

bool ZTuple::IsString(const char* iPropName, const char* iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return theValue->IsString(iString);
	return false;
	}

bool ZTuple::IsString(const ZTName& iPropName, const char* iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return theValue->IsString(iString);
	return false;
	}

bool ZTuple::IsString(const_iterator iPropIter, const string& iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropIter))
		return theValue->IsString(iString);
	return false;
	}

bool ZTuple::IsString(const char* iPropName, const string& iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return theValue->IsString(iString);
	return false;
	}

bool ZTuple::IsString(const ZTName& iPropName, const string& iString) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return theValue->IsString(iString);
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple property meta-data

ZTuple::const_iterator ZTuple::begin() const
	{
	if (fRep)
		return fRep->fProperties.begin();
	return sEmptyProperties.end();
	}

ZTuple::const_iterator ZTuple::end() const
	{
	if (fRep)
		return fRep->fProperties.end();
	return sEmptyProperties.end();
	}

bool ZTuple::Empty() const
	{ return !fRep || fRep->fProperties.empty(); }

ZTuple::const_iterator ZTuple::IteratorOf(const char* iPropName) const
	{
	if (fRep)
		{
		size_t propNameLength = strlen(iPropName);
		const_iterator end = fRep->fProperties.end();
		for (const_iterator i = fRep->fProperties.begin(); i != end; ++i)
			{
			if ((*i).fName.Equals(iPropName, propNameLength))
				return i;
			}
		return end;
		}
	return sEmptyProperties.end();
	}

ZTuple::const_iterator ZTuple::IteratorOf(const ZTName& iPropName) const
	{
	if (fRep)
		{
		const_iterator end = fRep->fProperties.end();
		for (const_iterator i = fRep->fProperties.begin(); i != end; ++i)
			{
			if ((*i).fName.Equals(iPropName))
				return i;
			}
		return end;
		}
	return sEmptyProperties.end();
	}

size_t ZTuple::Count() const
	{
	if (fRep)
		return fRep->fProperties.size();
	return 0;
	}

const ZTName& ZTuple::NameOf(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return (*iPropIter).fName;
	return sNilName;
	}

bool ZTuple::Has(const char* iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

bool ZTuple::Has(const ZTName& iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

bool ZTuple::TypeOf(const_iterator iIterator, ZType& oPropertyType) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iIterator))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

bool ZTuple::TypeOf(const char* iPropName, ZType& oPropertyType) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

bool ZTuple::TypeOf(const ZTName& iPropName, ZType& oPropertyType) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

ZType ZTuple::TypeOf(const_iterator iPropIter) const
	{ return this->pLookupConst(iPropIter).TypeOf(); }

ZType ZTuple::TypeOf(const char* iPropName) const
	{ return this->pLookupConst(iPropName).TypeOf(); }

ZType ZTuple::TypeOf(const ZTName& iPropName) const
	{ return this->pLookupConst(iPropName).TypeOf(); }

void ZTuple::Erase(const_iterator iPropIter)
	{
	if (fRep)
		this->pErase(iPropIter);
	}

void ZTuple::Erase(const char* iPropName)
	{
	if (fRep)
		this->pErase(this->IteratorOf(iPropName));
	}

void ZTuple::Erase(const ZTName& iPropName)
	{
	if (fRep)
		this->pErase(this->IteratorOf(iPropName));
	}

ZTuple::const_iterator ZTuple::EraseAndReturn(const_iterator iPropIter)
	{
	if (!fRep)
		return sEmptyProperties.end();

	if (iPropIter == fRep->fProperties.end())
		return iPropIter;

	if (fRep->GetRefCount() == 1)
		{
		return fRep->fProperties.erase(iPropIter);
		}
	else
		{
		size_t theEraseIndex = iPropIter - fRep->fProperties.begin();

		ZRef<ZTupleRep> oldRep = fRep;
		fRep = new ZTupleRep;

		fRep->fProperties.reserve(oldRep->fProperties.size() - 1);

		copy(oldRep->fProperties.begin(), iPropIter, back_inserter(fRep->fProperties));
		copy(++iPropIter, oldRep->fProperties.end(), back_inserter(fRep->fProperties));

		return fRep->fProperties.begin() + theEraseIndex;
		}
	}

void ZTuple::Clear()
	{
	fRep.Clear();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Get

bool ZTuple::GetValue(const_iterator iPropIter, ZTValue& oVal) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropIter))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

bool ZTuple::GetValue(const char* iPropName, ZTValue& oVal) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

bool ZTuple::GetValue(const ZTName& iPropName, ZTValue& oVal) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

const ZTValue& ZTuple::GetValue(const_iterator iPropIter) const
	{ return this->pLookupConst(iPropIter); }

const ZTValue& ZTuple::GetValue(const char* iPropName) const
	{ return this->pLookupConst(iPropName); }

const ZTValue& ZTuple::GetValue(const ZTName& iPropName) const
	{ return this->pLookupConst(iPropName); }

bool ZTuple::GetTuple(const_iterator iPropIter, ZTuple& oVal) const
	{ return this->pLookupConst(iPropIter).GetTuple(oVal); }

const ZTuple& ZTuple::GetTuple(const_iterator iPropIter) const
	{ return this->pLookupConst(iPropIter).GetTuple(); }

bool ZTuple::GetTuple(const char* iPropName, ZTuple& oVal) const
	{ return this->pLookupConst(iPropName).GetTuple(oVal); }

const ZTuple& ZTuple::GetTuple(const char* iPropName) const
	{ return this->pLookupConst(iPropName).GetTuple(); }

bool ZTuple::GetTuple(const ZTName& iPropName, ZTuple& oVal) const
	{ return this->pLookupConst(iPropName).GetTuple(oVal); }

const ZTuple& ZTuple::GetTuple(const ZTName& iPropName) const
	{ return this->pLookupConst(iPropName).GetTuple(); }

bool ZTuple::GetRaw(const_iterator iPropIter, void* iDest, size_t iSize) const
	{ return this->pLookupConst(iPropIter).GetRaw(iDest, iSize); }

bool ZTuple::GetRaw(const char* iPropName, void* iDest, size_t iSize) const
	{ return this->pLookupConst(iPropName).GetRaw(iDest, iSize); }

bool ZTuple::GetRaw(const ZTName& iPropName, void* iDest, size_t iSize) const
	{ return this->pLookupConst(iPropName).GetRaw(iDest, iSize); }

bool ZTuple::GetRawAttributes(const_iterator iPropIter, const void** oAddress, size_t* oSize) const
	{ return this->pLookupConst(iPropIter).GetRawAttributes(oAddress, oSize); }

bool ZTuple::GetRawAttributes(const char* iPropName, const void** oAddress, size_t* oSize) const
	{ return this->pLookupConst(iPropName).GetRawAttributes(oAddress, oSize); }

bool ZTuple::GetRawAttributes(const ZTName& iPropName, const void** oAddress, size_t* oSize) const
	{ return this->pLookupConst(iPropName).GetRawAttributes(oAddress, oSize); }

bool ZTuple::GetVector(const_iterator iPropIter, vector<ZTValue>& oVal) const
	{ return this->pLookupConst(iPropIter).GetVector(oVal); }

bool ZTuple::GetVector(const char* iPropName, vector<ZTValue>& oVal) const
	{ return this->pLookupConst(iPropName).GetVector(oVal); }

bool ZTuple::GetVector(const ZTName& iPropName, vector<ZTValue>& oVal) const
	{ return this->pLookupConst(iPropName).GetVector(oVal); }

const vector<ZTValue>& ZTuple::GetVector(const_iterator iPropIter) const
	{ return this->pLookupConst(iPropIter).GetVector(); }

const vector<ZTValue>& ZTuple::GetVector(const char* iPropName) const
	{ return this->pLookupConst(iPropName).GetVector(); }

const vector<ZTValue>& ZTuple::GetVector(const ZTName& iPropName) const
	{ return this->pLookupConst(iPropName).GetVector(); }

const ZTValue ZTuple::DGetValue(const_iterator iPropIter, const ZTValue& iDefault) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropIter))
		return *theValue;
	return iDefault;
	}

const ZTValue ZTuple::DGetValue(const char* iPropName, const ZTValue& iDefault) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return iDefault;
	}

const ZTValue ZTuple::DGetValue(const ZTName& iPropName, const ZTValue& iDefault) const
	{
	if (const ZTValue* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return iDefault;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Set

bool ZTuple::SetNull(const_iterator iPropIter)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV.SetNull();
		return true;
		}
	return false;
	}

ZTuple& ZTuple::SetNull(const char* iPropName)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetNull();
	return *this;
	}

ZTuple& ZTuple::SetNull(const ZTName& iPropName)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetNull();
	return *this;
	}

bool ZTuple::SetRaw(const_iterator iPropIter, const void* iSource, size_t iSize)
	{ return this->pSet(iPropIter, ZTValue(iSource, iSize)); }

ZTuple& ZTuple::SetRaw(const char* iPropName, const void* iSource, size_t iSize)
	{ return this->pSet(iPropName, ZTValue(iSource, iSize)); }

ZTuple& ZTuple::SetRaw(const ZTName& iPropName, const void* iSource, size_t iSize)
	{ return this->pSet(iPropName, ZTValue(iSource, iSize)); }

bool ZTuple::SetRaw(const_iterator iPropIter, const ZStreamR& iStreamR, size_t iSize)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV.SetRaw(iStreamR, iSize);
		return true;
		}
	return false;
	}

ZTuple& ZTuple::SetRaw(const char* iPropName, const ZStreamR& iStreamR, size_t iSize)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetRaw(iStreamR, iSize);
	return *this;
	}

ZTuple& ZTuple::SetRaw(const ZTName& iPropName, const ZStreamR& iStreamR, size_t iSize)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetRaw(iStreamR, iSize);
	return *this;
	}

bool ZTuple::SetRaw(const_iterator iPropIter, const ZStreamR& iStreamR)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV.SetRaw(iStreamR);
		return true;
		}
	return false;
	}

ZTuple& ZTuple::SetRaw(const char* iPropName, const ZStreamR& iStreamR)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetRaw(iStreamR);
	return *this;
	}

ZTuple& ZTuple::SetRaw(const ZTName& iPropName, const ZStreamR& iStreamR)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetRaw(iStreamR);
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Mutable Get

ZTValue& ZTuple::GetMutableValue(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZTValue* theValue = this->pLookupAddress(iPropIter))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZTValue& ZTuple::GetMutableValue(const char* iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZTValue& ZTuple::GetMutableValue(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZTuple& ZTuple::GetMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZTValue* theValue = this->pLookupAddress(iPropIter))
		{
		if (theValue->fType.fType == eZType_Tuple)
			return *sFetch_T<ZTuple>(theValue->fType.fBytes);
		}
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

ZTuple& ZTuple::GetMutableTuple(const char* iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		{
		if (theValue->fType.fType == eZType_Tuple)
			return *sFetch_T<ZTuple>(theValue->fType.fBytes);
		}
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

ZTuple& ZTuple::GetMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		{
		if (theValue->fType.fType == eZType_Tuple)
			return *sFetch_T<ZTuple>(theValue->fType.fBytes);
		}
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

vector<ZTValue>& ZTuple::GetMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZTValue* theValue = this->pLookupAddress(iPropIter))
		{
		if (theValue->fType.fType == eZType_Vector)
			{
			if (!theValue->fData.fAs_Vector)
				theValue->fData.fAs_Vector = new vector<ZTValue>;
			return *theValue->fData.fAs_Vector;
			}
		}
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

vector<ZTValue>& ZTuple::GetMutableVector(const char* iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		{
		if (theValue->fType.fType == eZType_Vector)
			{
			if (!theValue->fData.fAs_Vector)
				theValue->fData.fAs_Vector = new vector<ZTValue>;
			return *theValue->fData.fAs_Vector;
			}
		}
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

vector<ZTValue>& ZTuple::GetMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZTValue* theValue = this->pLookupAddress(iPropName))
		{
		if (theValue->fType.fType == eZType_Vector)
			{
			if (!theValue->fData.fAs_Vector)
				theValue->fData.fAs_Vector = new vector<ZTValue>;
			return *theValue->fData.fAs_Vector;
			}
		}
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Mutable Set

ZTValue& ZTuple::SetMutableNull(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableNull();
	}

ZTValue& ZTuple::SetMutableNull(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableNull();
	}

ZTValue& ZTuple::SetMutableNull(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableNull();
	}

ZTuple& ZTuple::SetMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableTuple();
	}

ZTuple& ZTuple::SetMutableTuple(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableTuple();
	}

ZTuple& ZTuple::SetMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableTuple();
	}

vector<ZTValue>& ZTuple::SetMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableVector();
	}

vector<ZTValue>& ZTuple::SetMutableVector(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableVector();
	}

vector<ZTValue>& ZTuple::SetMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableVector();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple ensure a property is of the type, and return a mutable reference

ZTValue& ZTuple::EnsureMutableValue(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV;
	}

ZTValue& ZTuple::EnsureMutableValue(const char* iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZTValue& ZTuple::EnsureMutableValue(const ZTName& iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZTuple& ZTuple::EnsureMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.EnsureMutableTuple();
	}

ZTuple& ZTuple::EnsureMutableTuple(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableTuple();
	}

ZTuple& ZTuple::EnsureMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableTuple();
	}

vector<ZTValue>& ZTuple::EnsureMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.EnsureMutableVector();
	}

vector<ZTValue>& ZTuple::EnsureMutableVector(const char * iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableVector();
	}

vector<ZTValue>& ZTuple::EnsureMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableVector();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple minimize

// Ensure that our vector's capacity is just enough to hold our data.
ZTuple& ZTuple::Minimize()
	{
	if (fRep)
		fRep = new ZTupleRep(fRep.GetObject());
	return *this;
	}

ZTuple ZTuple::Minimized() const
	{ return ZTuple(*this).Minimize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Internals

ZTValue* ZTuple::pFindOrAllocate(const char* iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	size_t propNameLength = strlen(iPropName);
	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName, propNameLength))
			return &(*i).fTV;
		}
	fRep->fProperties.push_back(NameTV(iPropName));
	return &fRep->fProperties.back().fTV;
	}

ZTValue* ZTuple::pFindOrAllocate(const ZTName& iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName))
			return &(*i).fTV;
		}
	fRep->fProperties.push_back(NameTV(iPropName));
	return &fRep->fProperties.back().fTV;
	}

bool ZTuple::pSet(const_iterator iPropIter, const ZTValue& iVal)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV = iVal;
		return true;
		}
	return false;
	}

ZTuple& ZTuple::pSet(const char* iPropName, const ZTValue& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	return *this;
	}

ZTuple& ZTuple::pSet(const ZTName& iPropName, const ZTValue& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	return *this;
	}

ZTuple& ZTuple::pAppend(const char* iPropName, const ZTValue& iVal)
	{
	this->pTouch();
	pFindOrAllocate(iPropName)->AppendValue(iVal);
	return *this;
	}

ZTuple& ZTuple::pAppend(const ZTName& iPropName, const ZTValue& iVal)
	{
	this->pTouch();
	pFindOrAllocate(iPropName)->AppendValue(iVal);
	return *this;
	}

const ZTValue* ZTuple::pLookupAddressConst(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return &(*iPropIter).fTV;
	return nil;
	}

const ZTValue* ZTuple::pLookupAddressConst(const char* iPropName) const
	{
	if (fRep)
		{
		size_t propNameLength = strlen(iPropName);
		for (PropList::const_iterator i = fRep->fProperties.begin(),
			theEnd = fRep->fProperties.end();
			i != theEnd; ++i)
			{
			if ((*i).fName.Equals(iPropName, propNameLength))
				return &(*i).fTV;
			}
		}
	return nil;
	}

const ZTValue* ZTuple::pLookupAddressConst(const ZTName& iPropName) const
	{
	if (fRep)
		{
		for (PropList::const_iterator i = fRep->fProperties.begin(),
			theEnd = fRep->fProperties.end();
			i != theEnd; ++i)
			{
			if ((*i).fName.Equals(iPropName))
				return &(*i).fTV;
			}
		}
	return nil;
	}

ZTValue* ZTuple::pLookupAddress(const_iterator iPropIter)
	{
	// We're the non-const version, being called from a mutating operation
	// so pTouch must have been called and we must have a rep.
	ZAssertStop(kDebug_Tuple, fRep);

	if (fRep && iPropIter != fRep->fProperties.end())
		return &(*iPropIter).fTV;
	return nil;
	}

ZTValue* ZTuple::pLookupAddress(const char* iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	size_t propNameLength = strlen(iPropName);
	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName, propNameLength))
			return &(*i).fTV;
		}
	return nil;
	}

ZTValue* ZTuple::pLookupAddress(const ZTName& iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName))
			return &(*i).fTV;
		}
	return nil;
	}

const ZTValue& ZTuple::pLookupConst(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return (*iPropIter).fTV;
	return sNilValue;
	}

const ZTValue& ZTuple::pLookupConst(const char* iPropName) const
	{
	if (fRep)
		{
		size_t propNameLength = strlen(iPropName);
		for (PropList::const_iterator i = fRep->fProperties.begin(),
			theEnd = fRep->fProperties.end();
			i != theEnd; ++i)
			{
			if ((*i).fName.Equals(iPropName, propNameLength))
				return (*i).fTV;
			}
		}
	return sNilValue;
	}

const ZTValue& ZTuple::pLookupConst(const ZTName& iPropName) const
	{
	if (fRep)
		{
		for (PropList::const_iterator i = fRep->fProperties.begin(),
			theEnd = fRep->fProperties.end();
			i != theEnd; ++i)
			{
			if ((*i).fName.Equals(iPropName))
				return (*i).fTV;
			}
		}
	return sNilValue;
	}

void ZTuple::pErase(const_iterator iPropIter)
	{
	if (iPropIter == fRep->fProperties.end())
		return;

	if (fRep->GetRefCount() == 1)
		{
		fRep->fProperties.erase(iPropIter);
		}
	else
		{
		ZRef<ZTupleRep> oldRep = fRep;
		fRep = new ZTupleRep;

		fRep->fProperties.reserve(oldRep->fProperties.size() - 1);
		copy(oldRep->fProperties.begin(), iPropIter, back_inserter(fRep->fProperties));
		copy(++iPropIter, oldRep->fProperties.end(), back_inserter(fRep->fProperties));
		}
	}

void ZTuple::pTouch()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = new ZTupleRep(fRep.GetObject());
		}
	else
		{
		fRep = new ZTupleRep;
		}
	}

ZTuple::const_iterator ZTuple::pTouch(const_iterator iPropIter)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	if (fRep->GetRefCount() == 1)
		return iPropIter;

	size_t index = iPropIter - fRep->fProperties.begin();
	fRep = new ZTupleRep(fRep.GetObject());
	return fRep->fProperties.begin() + index;
	}

bool ZTuple::pIsEqual(const ZTuple& iOther) const
	{
	// The inline implementation of operator==, which is our only caller,
	// has already determined that fRep != iOther.fRep.
	ZAssertStop(kDebug_Tuple, fRep != iOther.fRep);

	if (!fRep || !iOther.fRep)
		return false;

	if (fRep->fProperties.size() != iOther.fRep->fProperties.size())
		return false;

	for (PropList::iterator iterThis = fRep->fProperties.begin(),
		iterOther = iOther.fRep->fProperties.begin(),
		endThis = fRep->fProperties.end();
		iterThis != endThis;
		++iterThis, ++iterOther)
		{
		if (!(*iterThis).fName.Equals((*iterOther).fName))
			return false;
		if ((*iterThis).fTV != (*iterOther).fTV)
			return false;
		}

	return true;
	}

bool ZTuple::pIsSameAs(const ZTuple& iOther) const
	{
	// The inline implementation of IsSameAs, which is our only caller,
	// has already determined that fRep != iOther.fRep.
	ZAssertStop(kDebug_Tuple, fRep != iOther.fRep);

	if (!fRep || !iOther.fRep)
		return false;

	if (fRep->fProperties.size() != iOther.fRep->fProperties.size())
		return false;

	for (PropList::iterator iterThis = fRep->fProperties.begin(),
		endThis = fRep->fProperties.end();
		iterThis != endThis; ++iterThis)
		{
		const_iterator iterOther = iOther.IteratorOf((*iterThis).fName);
		if (iterOther == iOther.end())
			return false;
		if (!(*iterThis).fTV.IsSameAs((*iterOther).fTV))
			return false;
		}

	return true;
	}

ZRef<ZTupleRep> ZTuple::sRepFromStream(const ZStreamR& iStreamR)
	{
	ZRef<ZTupleRep> theRep;
	if (uint32 propertyCount = sReadCount(iStreamR))
		{
		theRep = new ZTupleRep;
		PropList& properties = theRep->fProperties;

		// It's a vector
		// Note that we set the rep's vector's size here, then iterate over it to
		// read in the data in place.
		properties.reserve(propertyCount);
		properties.resize(propertyCount);
		for (PropList::iterator i = properties.begin(); i != properties.end(); ++i)
			{
			(*i).fName.FromStream(iStreamR);
			(*i).fTV.FromStream(iStreamR);
			}
		}
	return theRep;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTupleRep

ZTupleRep::ZTupleRep()
	{}

ZTupleRep::ZTupleRep(const ZTupleRep* iOther)
:	fProperties(iOther->fProperties)
	{}

ZTupleRep::~ZTupleRep()
	{}

#ifndef SKIPOMPARSE
// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Get

#define ZTupleGetByIter(TYPENAME, TYPE_O, TYPE_RET) \
bool ZTuple::Get##TYPENAME(const_iterator iPropIter, TYPE_O& oVal) const \
	{ return this->pLookupConst(iPropIter).Get##TYPENAME(oVal); } \
TYPE_RET ZTuple::Get##TYPENAME(const_iterator iPropIter) const \
	{ return this->pLookupConst(iPropIter).Get##TYPENAME(); }

#define ZTupleGetByName(TYPENAME, STRINGTYPE, TYPE_O, TYPE_RET) \
bool ZTuple::Get##TYPENAME(STRINGTYPE iPropName, TYPE_O& oVal) const \
	{ return this->pLookupConst(iPropName).Get##TYPENAME(oVal); } \
TYPE_RET ZTuple::Get##TYPENAME(STRINGTYPE iPropName) const \
	{ return this->pLookupConst(iPropName).Get##TYPENAME(); } \

#define ZTupleGet(TYPENAME, TYPE) \
	ZTupleGetByIter(TYPENAME, TYPE, TYPE) \
	ZTupleGetByName(TYPENAME, const char*, TYPE, TYPE) \
	ZTupleGetByName(TYPENAME, const ZTName&, TYPE, TYPE)

#define ZTupleGetRet(TYPENAME, TYPE_O, TYPE_RET) \
	ZTupleGetByIter(TYPENAME, TYPE_O, TYPE_RET) \
	ZTupleGetByName(TYPENAME, const char*, TYPE_O, TYPE_RET) \
	ZTupleGetByName(TYPENAME, const ZTName&, TYPE_O, TYPE_RET)

ZTupleGet(Type, ZType)
ZTupleGet(ID, uint64)
ZTupleGet(Int8, int8)
ZTupleGet(Int16, int16)
ZTupleGet(Int32, int32)
ZTupleGet(Int64, int64)
ZTupleGet(Bool, bool)
ZTupleGet(Float, float)
ZTupleGet(Double, double)
ZTupleGet(Time, ZTime)
ZTupleGet(Pointer, void*)
ZTupleGetRet(Rect, ZRectPOD, const ZRectPOD&)
ZTupleGetRet(Point, ZPointPOD, const ZPointPOD&)
ZTupleGet(String, string)
//##ZTupleGet(Name, ZTName)
ZTupleGet(RefCounted, ZRef<ZRefCountedWithFinalization>)
ZTupleGet(Raw, ZMemoryBlock)

#undef ZTupleGet
#undef ZTupleGetRet
#undef ZTupleGetByIter
#undef ZTupleGetByName

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple DGet

#define ZTupleDGetByIter(TYPENAME, TYPE_I, TYPE_RET) \
TYPE_RET ZTuple::DGet##TYPENAME(const_iterator iPropIter, TYPE_I iDefault) const \
	{ return this->pLookupConst(iPropIter).DGet##TYPENAME(iDefault); }

#define ZTupleDGetByName(TYPENAME, STRINGTYPE, TYPE_I, TYPE_RET) \
TYPE_RET ZTuple::DGet##TYPENAME(STRINGTYPE iPropName, TYPE_I iDefault) const \
	{ return this->pLookupConst(iPropName).DGet##TYPENAME(iDefault); } \

#define ZTupleDGet(TYPENAME, TYPE) \
	ZTupleDGetByIter(TYPENAME, TYPE, TYPE) \
	ZTupleDGetByName(TYPENAME, const char*, TYPE, TYPE) \
	ZTupleDGetByName(TYPENAME, const ZTName&, TYPE, TYPE)

#define ZTupleDGetRet(TYPENAME, TYPE_I, TYPE_RET) \
	ZTupleDGetByIter(TYPENAME, TYPE_I, TYPE_RET) \
	ZTupleDGetByName(TYPENAME, const char*, TYPE_I, TYPE_RET) \
	ZTupleDGetByName(TYPENAME, const ZTName&, TYPE_I, TYPE_RET)

ZTupleDGet(Type, ZType)
ZTupleDGet(ID, uint64)
ZTupleDGet(Int8, int8)
ZTupleDGet(Int16, int16)
ZTupleDGet(Int32, int32)
ZTupleDGet(Int64, int64)
ZTupleDGet(Bool, bool)
ZTupleDGet(Float, float)
ZTupleDGet(Double, double)
ZTupleDGet(Time, ZTime)
ZTupleDGetRet(Rect, const ZRectPOD&, ZRectPOD)
ZTupleDGetRet(Point, const ZPointPOD&, ZPointPOD)
ZTupleDGetRet(String, const string&, string)
//##ZTupleDGetRet(Name, const ZTName&, ZTName)

#undef ZTupleDGet
#undef ZTupleDGetByIter
#undef ZTupleDGetByName

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Set

#define ZTupleSetByIter(TYPENAME, PARAMTYPE, STOREDVALUE) \
bool ZTuple::Set##TYPENAME(const_iterator iPropIter, PARAMTYPE iVal) \
	{ return this->pSet(iPropIter, STOREDVALUE); }

#define ZTupleSetByName(TYPENAME, STRINGTYPE, PARAMTYPE, STOREDVALUE) \
ZTuple& ZTuple::Set##TYPENAME(STRINGTYPE iPropName, PARAMTYPE iVal) \
	{ return this->pSet(iPropName, STOREDVALUE); }

#define ZTupleSet(TYPENAME, PARAMTYPE) \
	ZTupleSetByIter(TYPENAME, PARAMTYPE, ZTValue(iVal)) \
	ZTupleSetByName(TYPENAME, const char*, PARAMTYPE, ZTValue(iVal)) \
	ZTupleSetByName(TYPENAME, const ZTName&, PARAMTYPE, ZTValue(iVal))

ZTupleSetByIter(Value, const ZTValue&, iVal)
ZTupleSetByName(Value, const char*, const ZTValue&, iVal)
ZTupleSetByName(Value, const ZTName&, const ZTValue&, iVal)

ZTupleSetByIter(ID, uint64, ZTValue(iVal, true))
ZTupleSetByName(ID, const char*, uint64, ZTValue(iVal, true))
ZTupleSetByName(ID, const ZTName&, uint64, ZTValue(iVal, true))

ZTupleSetByIter(Type, ZType, ZTValue(iVal))
ZTupleSetByName(Type, const char*, ZType, ZTValue(iVal))
ZTupleSetByName(Type, const ZTName&, ZType, ZTValue(iVal))

ZTupleSet(Int8, int8)
ZTupleSet(Int16, int16)
ZTupleSet(Int32, int32)
ZTupleSet(Int64, int64)
ZTupleSet(Bool, bool)
ZTupleSet(Float, float)
ZTupleSet(Double, double)
ZTupleSet(Time, ZTime)
ZTupleSet(Pointer, void*)
ZTupleSet(Rect, const ZRectPOD&)
ZTupleSet(Point, const ZPointPOD&)
ZTupleSet(String, const char*)
ZTupleSet(String, const string&)
//##ZTupleSet(Name, const ZTName&)
ZTupleSet(Tuple, const ZTuple&)
ZTupleSet(RefCounted, const ZRef<ZRefCountedWithFinalization>&)
ZTupleSet(Raw, const ZMemoryBlock&)
ZTupleSet(Vector, const vector<ZTValue>&)

#undef ZTupleSetByIter
#undef ZTupleSetByName
#undef ZTupleSet

// =================================================================================================
#pragma mark -
#pragma mark * ZTuple Append

void ZTuple::AppendRaw(const char* iPropName, const void* iSource, size_t iSize)
	{ this->pAppend(iPropName, ZTValue(iSource, iSize)); }

void ZTuple::AppendRaw(const ZTName& iPropName, const void* iSource, size_t iSize)
	{ this->pAppend(iPropName, ZTValue(iSource, iSize)); }

#define ZTupleAppendByName(TYPENAME, STRINGTYPE, PARAMTYPE, STOREDVALUE) \
void ZTuple::Append##TYPENAME(STRINGTYPE iPropName, PARAMTYPE iVal) \
	{ this->pAppend(iPropName, STOREDVALUE); }

#define ZTupleAppend(TYPENAME, PARAMTYPE) \
	ZTupleAppendByName(TYPENAME, const char*, PARAMTYPE, ZTValue(iVal)) \
	ZTupleAppendByName(TYPENAME, const ZTName&, PARAMTYPE, ZTValue(iVal))

ZTupleAppendByName(Value, const char*, const ZTValue&, iVal)
ZTupleAppendByName(Value, const ZTName&, const ZTValue&, iVal)

ZTupleAppendByName(ID, const char*, uint64, ZTValue(iVal, true))
ZTupleAppendByName(ID, const ZTName&, uint64, ZTValue(iVal, true))

ZTupleAppendByName(Type, const char*, ZType, ZTValue(iVal))
ZTupleAppendByName(Type, const ZTName&, ZType, ZTValue(iVal))

ZTupleAppend(Int8, int8)
ZTupleAppend(Int16, int16)
ZTupleAppend(Int32, int32)
ZTupleAppend(Int64, int64)
ZTupleAppend(Bool, bool)
ZTupleAppend(Float, float)
ZTupleAppend(Double, double)
ZTupleAppend(Time, ZTime)
ZTupleAppend(Pointer, void*)
ZTupleAppend(Rect, const ZRectPOD&)
ZTupleAppend(Point, const ZPointPOD&)
ZTupleAppend(String, const char*)
ZTupleAppend(String, const string&)
//##ZTupleAppend(Name, const ZTName&)
ZTupleAppend(Tuple, const ZTuple&)
ZTupleAppend(RefCounted, const ZRef<ZRefCountedWithFinalization>&)
ZTupleAppend(Raw, const ZMemoryBlock&)

#undef ZTupleAppend
#undef ZTupleAppendByName

#endif // SKIPOMPARSE
