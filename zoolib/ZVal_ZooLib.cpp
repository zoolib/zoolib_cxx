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

#include "zoolib/ZVal_ZooLib.h"
#include "zoolib/ZMemory.h"

using std::min;
using std::string;
using std::vector;

#define kDebug_Tuple 1

NAMESPACE_ZOOLIB_BEGIN

static ZTName sNilName;
static ZValMap_ZooLib::PropList sEmptyProperties;
static ZVal_ZooLib sNilValue;
static ZValList_ZooLib sNilVector;
static ZValMap_ZooLib sNilTuple;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static inline int sCompare(const void* iLeft, size_t iLeftLength,
	const void* iRight, size_t iRightLength)
	{
	if (int compare = memcmp(iLeft, iRight, min(iLeftLength, iRightLength)))
		return compare;
	// Strictly speaking the rules of two's complement mean that we
	// don't need the casts, but let's be clear about what we're doing.
	return int(iLeftLength) - int(iRightLength);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ValString

/** ValString is a lighterweight implementation of immutable strings, used
for holding longer string properties in ZVal_ZooLibs. */

namespace ZANONYMOUS {

class ValString
	{
	ValString(); // Not implemented
	ValString& operator=(const ValString&); // Not implemented

public:
	~ValString();

	ValString(const ValString& iOther);

	explicit ValString(const string& iOther);

	ValString(const char* iString, size_t iLength);

	ValString(const ZStreamR& iStreamR, size_t iSize);

	int Compare(const ValString& iOther) const;

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

inline ValString::~ValString()
	{ delete[] fBuffer; }

inline bool ValString::Empty() const
	{ return !fSize; }

inline std::string ValString::AsString() const
	{ return string(fBuffer, fSize); }

ValString::ValString(const ValString& iOther)
:	fSize(iOther.fSize)
	{
	fBuffer = new char[fSize];
	ZBlockCopy(iOther.fBuffer, fBuffer, fSize);
	}

ValString::ValString(const string& iOther)
:	fSize(iOther.size())
	{
	fBuffer = new char[fSize];
	if (fSize)
		ZBlockCopy(iOther.data(), fBuffer, fSize);
	}

ValString::ValString(const char* iString, size_t iLength)
:	fSize(iLength)
	{
	fBuffer = new char[fSize];
	ZBlockCopy(iString, fBuffer, fSize);
	}

ValString::ValString(const ZStreamR& iStreamR, size_t iSize)
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

inline int ValString::Compare(const ValString& iOther) const
	{ return sCompare(fBuffer, fSize, iOther.fBuffer, iOther.fSize); }

inline int ValString::Compare(const char* iString, size_t iSize) const
	{ return sCompare(fBuffer, fSize, iString, iSize); }

inline int ValString::Compare(const string& iString) const
	{
	if (size_t otherSize = iString.size())
		return sCompare(fBuffer, fSize, iString.data(), otherSize);

	// iString is empty. If fSize is zero, then fSize!=0 is true, and
	// will convert to 1. Otherwise it will be false, and convert
	// to zero, so we always return the correct result.
	return fSize != 0;
	}

void ValString::ToStream(const ZStreamW& iStreamW) const
	{
	iStreamW.WriteCount(fSize);
	iStreamW.Write(fBuffer, fSize);
	}

void ValString::ToString(string& oString) const
	{
	oString.resize(fSize);
	if (fSize)
		ZBlockCopy(fBuffer, const_cast<char*>(oString.data()), fSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib::Ex_IllegalType

ZVal_ZooLib::Ex_IllegalType::Ex_IllegalType(int iType)
:	runtime_error("Ex_IllegalType"),
	fType(iType)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib

/** \class ZVal_ZooLib
\nosubgrouping
*/

ZVal_ZooLib::ZVal_ZooLib()
	{
	fType.fType = eZType_Null;
	}

ZVal_ZooLib::~ZVal_ZooLib()
	{
	this->pRelease();
	}

ZVal_ZooLib::ZVal_ZooLib(const ZVal_ZooLib& iOther)
	{
	this->pCopy(iOther);
	}

ZVal_ZooLib& ZVal_ZooLib::operator=(const ZVal_ZooLib& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopy(iOther);
		}
	return *this;
	}

ZVal_ZooLib::ZVal_ZooLib(const ZStreamR& iStreamR)
	{
	fType.fType = eZType_Null;
	this->pFromStream((ZType)iStreamR.ReadUInt8(), iStreamR);
	}

ZVal_ZooLib::ZVal_ZooLib(ZType iType, const ZStreamR& iStreamR)
	{
	fType.fType = eZType_Null;
	this->pFromStream(iType, iStreamR);
	}

ZVal_ZooLib::ZVal_ZooLib(bool dummy, const ZStreamR& iStreamR)
	{
	fType.fType = eZType_Null;
	this->pFromStream((ZType)iStreamR.ReadUInt8(), iStreamR);
	}

void ZVal_ZooLib::ToStream(const ZStreamW& iStreamW) const
	{
	if (fType.fType < 0)
		{
		iStreamW.WriteUInt8(eZType_String);
		size_t theSize = -fType.fType-1;
		ZAssertStop(kDebug_Tuple, theSize <= kBytesSize);
		iStreamW.WriteCount(theSize);
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
			if (sizeof(intptr_t) != sizeof(uint32))
				ZUnimplemented();
			else
				iStreamW.WriteUInt32(reinterpret_cast<intptr_t>(fData.fAs_Pointer));
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
			sFetch_T<ValString>(fType.fBytes)->ToStream(iStreamW);
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
			sFetch_T<ZValMap_ZooLib>(fType.fBytes)->ToStream(iStreamW);
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
			iStreamW.WriteCount(theMemoryBlock->GetSize());
			if (theMemoryBlock->GetSize())
				iStreamW.Write(theMemoryBlock->GetData(), theMemoryBlock->GetSize());
			break;
			}
		case eZType_Vector:
			{
			if (const ZValList_ZooLib* theVector = fData.fAs_Vector)
				{
				size_t theCount = theVector->size();
				iStreamW.WriteCount(theCount);
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
				iStreamW.WriteCount(0);
				}
			break;
			}
		default:
			{
			ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

void ZVal_ZooLib::FromStream(const ZStreamR& iStreamR)
	{
	this->pRelease();
	fType.fType = eZType_Null;
	this->pFromStream((ZType)iStreamR.ReadUInt8(), iStreamR);
	}

void ZVal_ZooLib::FromStream(ZType iType, const ZStreamR& iStreamR)
	{
	this->pRelease();
	fType.fType = eZType_Null;
	this->pFromStream(iType, iStreamR);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZVal_ZooLib& iVal, bool iAsVector)
	{
	ZAssertStop(kDebug_Tuple, iAsVector);
	fType.fType = eZType_Vector;
	fData.fAs_Vector = new ZValList_ZooLib(1, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(ZType iVal)
	{
	fType.fType = eZType_Type;
	fData.fAs_Type = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(uint64 iVal, bool iIsID)
	{
	ZAssertStop(kDebug_Tuple, iIsID);
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(uint64 iVal)
	{
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(int8 iVal)
	{
	fType.fType = eZType_Int8;
	fData.fAs_Int8 = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(int16 iVal)
	{
	fType.fType = eZType_Int16;
	fData.fAs_Int16 = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(int32 iVal)
	{
	fType.fType = eZType_Int32;
	fData.fAs_Int32 = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(int64 iVal)
	{
	fType.fType = eZType_Int64;
	fData.fAs_Int64 = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(bool iVal)
	{
	fType.fType = eZType_Bool;
	fData.fAs_Bool = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(float iVal)
	{
	fType.fType = eZType_Float;
	fData.fAs_Float = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(double iVal)
	{
	fType.fType = eZType_Double;
	fData.fAs_Double = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(ZTime iVal)
	{
	fType.fType = eZType_Time;
	fData.fAs_Time = iVal.fVal;
	}

ZVal_ZooLib::ZVal_ZooLib(void* inPointer)
	{
	fType.fType = eZType_Pointer;
	fData.fAs_Pointer = inPointer;
	}

ZVal_ZooLib::ZVal_ZooLib(const ZRectPOD& iVal)
	{
	fType.fType = eZType_Rect;
	fData.fAs_Rect = new ZRectPOD(iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZPointPOD& iVal)
	{
	fType.fType = eZType_Point;
	fData.fAs_Point = iVal;
	}

ZVal_ZooLib::ZVal_ZooLib(const char* iVal)
	{
	size_t theSize = strlen(iVal);
	if (theSize <= kBytesSize)
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZBlockCopy(iVal, fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ValString>(fType.fBytes, iVal, theSize);
		}
	}

ZVal_ZooLib::ZVal_ZooLib(const string& iVal)
	{
	size_t theSize = iVal.size();
	if (theSize <= kBytesSize)
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZBlockCopy(iVal.data(), fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ValString>(fType.fBytes, iVal);
		}
	}

ZVal_ZooLib::ZVal_ZooLib(const ZValMap_ZooLib& iVal)
	{
	fType.fType = eZType_Tuple;
	sConstruct_T(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZRef<ZRefCountedWithFinalize>& iVal)
	{
	fType.fType = eZType_RefCounted;
	sConstruct_T(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZMemoryBlock& iVal)
	{
	fType.fType = eZType_Raw;
	sConstruct_T(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const void* iSource, size_t iSize)
	{
	fType.fType = eZType_Raw;
	sConstruct_T<ZMemoryBlock>(fType.fBytes, iSource, iSize);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize)
	{
	fType.fType = eZType_Raw;
	ZMemoryBlock* theRaw = sConstruct_T<ZMemoryBlock>(fType.fBytes);
	ZStreamRWPos_MemoryBlock(*theRaw).CopyFrom(iStreamR, iSize);
	}

ZVal_ZooLib::ZVal_ZooLib(const vector<ZVal_ZooLib>& iVal)
	{
	fType.fType = eZType_Vector;
	if (iVal.empty())
		fData.fAs_Vector = nullptr;
	else
		fData.fAs_Vector = new ZValList_ZooLib(iVal);
	}

int ZVal_ZooLib::Compare(const ZVal_ZooLib& iOther) const
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
			return -sFetch_T<ValString>(iOther.fType.fBytes)
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
				return sFetch_T<ValString>(fType.fBytes)
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

bool ZVal_ZooLib::operator==(const ZVal_ZooLib& iOther) const
	{
	return this->Compare(iOther) == 0;
	}

bool ZVal_ZooLib::operator<(const ZVal_ZooLib& iOther) const
	{
	return this->Compare(iOther) < 0;
	}

bool ZVal_ZooLib::IsSameAs(const ZVal_ZooLib& iOther) const
	{
	if (this == &iOther)
		return true;

	if (fType.fType != iOther.fType.fType)
		return false;

	switch (fType.fType)
		{
		case eZType_Tuple:
			{
			return sFetch_T<ZValMap_ZooLib>(fType.fBytes)->IsSameAs(*sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes));
			}

		case eZType_Vector:
			{
			const vector<ZVal_ZooLib>* thisVector = fData.fAs_Vector;
			const vector<ZVal_ZooLib>* otherVector = iOther.fData.fAs_Vector;

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

			vector<ZVal_ZooLib>::const_iterator thisEnd = thisVector->end();
			vector<ZVal_ZooLib>::const_iterator otherEnd = otherVector->end();
			
			vector<ZVal_ZooLib>::const_iterator thisIter = thisVector->begin();
			vector<ZVal_ZooLib>::const_iterator otherIter = otherVector->begin();
			
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

template <>
bool ZVal_ZooLib::QGet_T<ZType>(ZType& oVal) const
	{
	if (fType.fType == eZType_Type)
		{
		oVal = fData.fAs_Type;
		return true;
		}
	return false;
	}
	
template <>
bool ZVal_ZooLib::QGet_T<uint64>(uint64& oVal) const
	{
	if (fType.fType == eZType_ID)
		{
		oVal = fData.fAs_ID;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<int8>(int8& oVal) const
	{
	if (fType.fType == eZType_Int8)
		{
		oVal = fData.fAs_Int8;
		return true;
		}
	return false;
	}
	
template <>
bool ZVal_ZooLib::QGet_T<int16>(int16& oVal) const
	{
	if (fType.fType == eZType_Int16)
		{
		oVal = fData.fAs_Int16;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<int32>(int32& oVal) const
	{
	if (fType.fType == eZType_Int32)
		{
		oVal = fData.fAs_Int32;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<int64>(int64& oVal) const
	{
	if (fType.fType == eZType_Int64)
		{
		oVal = fData.fAs_Int64;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<bool>(bool& oVal) const
	{
	if (fType.fType == eZType_Bool)
		{
		oVal = fData.fAs_Bool;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<float>(float& oVal) const
	{
	if (fType.fType == eZType_Float)
		{
		oVal = fData.fAs_Float;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<double>(double& oVal) const
	{
	if (fType.fType == eZType_Double)
		{
		oVal = fData.fAs_Double;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZTime>(ZTime& oVal) const
	{
	if (fType.fType == eZType_Time)
		{
		oVal = fData.fAs_Time;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<void*>(void*& oVal) const
	{
	if (fType.fType == eZType_Pointer)
		{
		oVal = fData.fAs_Pointer;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZRectPOD>(ZRectPOD& oVal) const
	{
	if (fType.fType == eZType_Rect)
		{
		oVal = *fData.fAs_Rect;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZPointPOD>(ZPointPOD& oVal) const
	{
	if (fType.fType == eZType_Point)
		{
		oVal = fData.fAs_Point;
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<string>(string& oVal) const
	{
	if (fType.fType == eZType_String)
		{
		sFetch_T<ValString>(fType.fBytes)->ToString(oVal);
		return true;
		}
	else if (fType.fType < 0)
		{
		oVal = string(fType.fBytes, -fType.fType-1);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZMemoryBlock>(ZMemoryBlock& oVal) const
	{
	if (fType.fType == eZType_Raw)
		{
		oVal = *sFetch_T<ZMemoryBlock>(fType.fBytes);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<vector<ZVal_ZooLib> >(vector<ZVal_ZooLib>& oVal) const
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

template <>
bool ZVal_ZooLib::QGet_T<ZRef<ZRefCountedWithFinalize> >(ZRef<ZRefCountedWithFinalize>& oVal) const
	{
	if (fType.fType == eZType_RefCounted)
		{
		oVal = *sFetch_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZValMap_ZooLib >(ZValMap_ZooLib& oVal) const
	{
	if (fType.fType == eZType_Tuple)
		{
		oVal = *sFetch_T<ZValMap_ZooLib>(fType.fBytes);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZValList_ZooLib >(ZValList_ZooLib& oVal) const
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

template <>
void ZVal_ZooLib::Set_T<ZType>(const ZType& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Type;
	fData.fAs_Type = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<uint64>(const uint64& iVal)
	{
	this->pRelease();
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<int8>(const int8& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int8;
	fData.fAs_Int8 = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<int16>(const int16& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int16;
	fData.fAs_Int16 = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<int32>(const int32& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int32;
	fData.fAs_Int32 = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<int64>(const int64& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int64;
	fData.fAs_Int64 = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<bool>(const bool& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Bool;
	fData.fAs_Bool = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<float>(const float& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Float;
	fData.fAs_Float = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<double>(const double& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Double;
	fData.fAs_Double = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<ZTime>(const ZTime& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Time;
	fData.fAs_Time = iVal.fVal;
	}

template <>
void ZVal_ZooLib::Set_T<VoidStar_t>(const VoidStar_t& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Pointer;
	fData.fAs_Pointer = iVal;
	}

template <>
void ZVal_ZooLib::Set_T<ZRectPOD>(const ZRectPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Rect;
	fData.fAs_Rect = new ZRectPOD(iVal);
	}

template <>
void ZVal_ZooLib::Set_T<ZPointPOD>(const ZPointPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Point;
	fData.fAs_Point = iVal;
	}

typedef const char* ConstCharStar_t;
template <>
void ZVal_ZooLib::Set_T<ConstCharStar_t>(const ConstCharStar_t& iVal)
	{
	this->pRelease();
	size_t theSize = strlen(iVal);
	if (theSize <= kBytesSize)
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZBlockCopy(iVal, fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ValString>(fType.fBytes, iVal, theSize);
		}
	}

template <>
void ZVal_ZooLib::Set_T<string>(const string& iVal)
	{
	this->pRelease();
	size_t theSize = iVal.size();
	if (theSize <= kBytesSize)
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZBlockCopy(iVal.data(), fType.fBytes, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sConstruct_T<ValString>(fType.fBytes, iVal);
		}
	}

template <>
void ZVal_ZooLib::Set_T<ZRef<ZRefCountedWithFinalize> >(const ZRef<ZRefCountedWithFinalize>& iVal)
	{
	this->pRelease();
	fType.fType = eZType_RefCounted;
	sConstruct_T(fType.fBytes, iVal);
	}

template <>
void ZVal_ZooLib::Set_T<ZMemoryBlock>(const ZMemoryBlock& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Raw;
	sConstruct_T(fType.fBytes, iVal);
	}

template <>
void ZVal_ZooLib::Set_T<vector<ZVal_ZooLib> >(const vector<ZVal_ZooLib>& iVal)
	{
	this->pRelease();
	sConstruct_T(fType.fBytes, iVal);
	fType.fType = eZType_Vector;
	if (iVal.empty())
		fData.fAs_Vector = nullptr;
	else
		fData.fAs_Vector = new ZValList_ZooLib(iVal);
	}

template <>
void ZVal_ZooLib::Set_T<ZValMap_ZooLib>(const ZValMap_ZooLib& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Tuple;
	sConstruct_T(fType.fBytes, iVal);
	}

template <>
void ZVal_ZooLib::Set_T<ZValList_ZooLib>(const ZValList_ZooLib& iVal)
	{
	this->pRelease();
	sConstruct_T(fType.fBytes, iVal);
	fType.fType = eZType_Vector;
	if (iVal.empty())
		fData.fAs_Vector = nullptr;
	else
		fData.fAs_Vector = new ZValList_ZooLib(iVal);
	}

void ZVal_ZooLib::SetNull()
	{
	this->pRelease();
	fType.fType = eZType_Null;
	}

void ZVal_ZooLib::SetRaw(const void* iSource, size_t iSize)
	{
	this->pRelease();
	fType.fType = eZType_Raw;
	sConstruct_T<ZMemoryBlock>(fType.fBytes, iSource, iSize);
	}

void ZVal_ZooLib::SetRaw(const ZStreamR& iStreamR, size_t iSize)
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

void ZVal_ZooLib::SetRaw(const ZStreamR& iStreamR)
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

ZValMap_ZooLib& ZVal_ZooLib::GetMutableTuple()
	{
	// We're returning a non-const tuple, one that can be manipulated
	// by the caller, so we must actually *be* a tuple.
	ZAssertStop(kDebug_Tuple, fType.fType == eZType_Tuple);

	return *sFetch_T<ZValMap_ZooLib>(fType.fBytes);
	}

ZValList_ZooLib& ZVal_ZooLib::GetMutableVector()
	{
	// We're returning a non-const vector, one that can be manipulated
	// by the caller. So we must actually *be* a vector, we can't
	// return the usual dummy default value.
	ZAssertStop(kDebug_Tuple, fType.fType == eZType_Vector);

	if (!fData.fAs_Vector)
		{
		// The empty vector optimization is in effect, but
		// we need a real vector to return.
		fData.fAs_Vector = new ZValList_ZooLib;
		}
	return *fData.fAs_Vector;
	}

ZVal_ZooLib& ZVal_ZooLib::SetMutableNull()
	{
	this->pRelease();
	fType.fType = eZType_Null;
	return *this;
	}

ZValMap_ZooLib& ZVal_ZooLib::SetMutableTuple()
	{
	this->pRelease();
	fType.fType = eZType_Tuple;
	return *sConstruct_T<ZValMap_ZooLib>(fType.fBytes);
	}

ZValList_ZooLib& ZVal_ZooLib::SetMutableVector()
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
			fData.fAs_Vector = new ZValList_ZooLib;
		}
	else
		{
		this->pRelease();
		fType.fType = eZType_Vector;
		fData.fAs_Vector = nullptr;
		fData.fAs_Vector = new ZValList_ZooLib;
		}
	return *fData.fAs_Vector;
	}

ZValMap_ZooLib& ZVal_ZooLib::EnsureMutableTuple()
	{
	if (fType.fType == eZType_Tuple)
		return *sFetch_T<ZValMap_ZooLib>(fType.fBytes);

	this->pRelease();
	fType.fType = eZType_Tuple;
	return *sConstruct_T<ZValMap_ZooLib>(fType.fBytes);
	}

ZValList_ZooLib& ZVal_ZooLib::EnsureMutableVector()
	{
	if (fType.fType == eZType_Vector)
		{
		if (!fData.fAs_Vector)
			fData.fAs_Vector = new ZValList_ZooLib;
		}
	else
		{
		this->pRelease();
		fType.fType = eZType_Vector;
		fData.fAs_Vector = nullptr;
		fData.fAs_Vector = new ZValList_ZooLib;
		}
	return *fData.fAs_Vector;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib standard accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_ZooLib)
ZMACRO_ZValAccessors_Def_ZooLib(ZVal_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Vector, std::vector<ZVal_ZooLib>)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Tuple, ZValMap_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, List, ZValList_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Map, ZValMap_ZooLib)

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib internal implementation

int ZVal_ZooLib::pUncheckedCompare(const ZVal_ZooLib& iOther) const
	{
	// Our types are assumed to be the same, and not special strings.
	ZAssertStop(kDebug_Tuple, fType.fType == iOther.fType.fType);
	switch (fType.fType)
		{
		case eZType_Null: return 0;
		case eZType_Type: return sCompare_T(fData.fAs_Type, iOther.fData.fAs_Type);
		case eZType_ID: return sCompare_T(fData.fAs_ID, iOther.fData.fAs_ID);
		case eZType_Int8: return sCompare_T(fData.fAs_Int8, iOther.fData.fAs_Int8);
		case eZType_Int16: return sCompare_T(fData.fAs_Int16, iOther.fData.fAs_Int16);
		case eZType_Int32: return sCompare_T(fData.fAs_Int32, iOther.fData.fAs_Int32);
		case eZType_Int64: return sCompare_T(fData.fAs_Int64, iOther.fData.fAs_Int64);
		case eZType_Bool: return sCompare_T(fData.fAs_Bool, iOther.fData.fAs_Bool);
		case eZType_Float:  return sCompare_T(fData.fAs_Float, iOther.fData.fAs_Float);
		case eZType_Double: return sCompare_T(fData.fAs_Double, iOther.fData.fAs_Double);
		case eZType_Time: return sCompare_T(fData.fAs_Time, iOther.fData.fAs_Time);
		case eZType_Pointer: return sCompare_T(fData.fAs_Pointer, iOther.fData.fAs_Pointer);
		case eZType_Rect: return sCompare_T(*fData.fAs_Rect, *iOther.fData.fAs_Rect);
		case eZType_Point: return sCompare_T(fData.fAs_Point, iOther.fData.fAs_Point);
		case eZType_String:
			{
			return sFetch_T<ValString>(fType.fBytes)
				->Compare(*sFetch_T<ValString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return sFetch_T<ZTName>(fType.fBytes)->Compare(*sFetch_T<ZTName>(iOther.fType.fBytes));
			}
#endif//##
		case eZType_Tuple:
			{
			return sFetch_T<ZValMap_ZooLib>(fType.fBytes)->
				Compare(*sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes));
			}
		case eZType_RefCounted:
			{
			const ZRef<ZRefCountedWithFinalize>* thisZRef
				= sFetch_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes);

			const ZRef<ZRefCountedWithFinalize>* otherZRef
				= sFetch_T<ZRef<ZRefCountedWithFinalize> >(iOther.fType.fBytes);
			
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
			return sCompare_T(fData.fAs_Vector->begin(), fData.fAs_Vector->end(),
				iOther.fData.fAs_Vector->begin(), iOther.fData.fAs_Vector->end());
//			return sCompare_T<std::vector<ZVal_ZooLib> >
//				(*fData.fAs_Vector, *iOther.fData.fAs_Vector);
			}
		}
	ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
	return 0;
	}

bool ZVal_ZooLib::pUncheckedLess(const ZVal_ZooLib& iOther) const
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
		case eZType_Rect: return sCompare_T(*fData.fAs_Rect, *iOther.fData.fAs_Rect) < 0;
		case eZType_Point: return sCompare_T(fData.fAs_Point, iOther.fData.fAs_Point) < 0;

		case eZType_String:
			{
			return 0 > sFetch_T<ValString>(fType.fBytes)
				->Compare(*sFetch_T<ValString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return *sFetch_T<ZTName>(fType.fBytes) < *sFetch_T<ZTName>(iOther.fType.fBytes);
			}
#endif//##
		case eZType_Tuple:
			{
			return *sFetch_T<ZValMap_ZooLib>(fType.fBytes) < *sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes)
				< *sFetch_T<ZRef<ZRefCountedWithFinalize> >(iOther.fType.fBytes);
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

bool ZVal_ZooLib::pUncheckedEqual(const ZVal_ZooLib& iOther) const
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
			return 0 == sFetch_T<ValString>(fType.fBytes)
				->Compare(*sFetch_T<ValString>(iOther.fType.fBytes));
			}
#if 0//##
		case eZType_Name:
			{
			return *sFetch_T<ZTName>(fType.fBytes) == *sFetch_T<ZTName>(iOther.fType.fBytes);
			}
#endif//##
		case eZType_Tuple:
			{
			return *sFetch_T<ZValMap_ZooLib>(fType.fBytes) == *sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes)
				== *sFetch_T<ZRef<ZRefCountedWithFinalize> >(iOther.fType.fBytes);
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

void ZVal_ZooLib::pRelease()
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

		case eZType_String: sDestroy_T<ValString>(fType.fBytes); break;
//##		case eZType_Name: sDestroy_T<ZTName>(fType.fBytes); break;
		case eZType_Tuple: sDestroy_T<ZValMap_ZooLib>(fType.fBytes); break;
		case eZType_RefCounted:
			{
			sDestroy_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes);
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

void ZVal_ZooLib::pCopy(const ZVal_ZooLib& iOther)
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
				sCopyConstruct_T<ValString>(iOther.fType.fBytes, fType.fBytes);
				break;
#if 0//##
			case eZType_Name:
				sCopyConstruct_T<ZTName>(iOther.fType.fBytes, fType.fBytes);
				break;
#endif//##
			case eZType_Tuple:
				sCopyConstruct_T<ZValMap_ZooLib>(iOther.fType.fBytes, fType.fBytes);
				break;
			case eZType_RefCounted:
				sCopyConstruct_T<ZRef<ZRefCountedWithFinalize> >
					(iOther.fType.fBytes, fType.fBytes);				
				break;
			case eZType_Raw:
				sCopyConstruct_T<ZMemoryBlock>(iOther.fType.fBytes, fType.fBytes);
				break;
			case eZType_Vector:
				if (!iOther.fData.fAs_Vector || iOther.fData.fAs_Vector->empty())
					fData.fAs_Vector = nullptr;
				else
					fData.fAs_Vector = new ZValList_ZooLib(*iOther.fData.fAs_Vector);
				break;
			default:
				ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

static ZValList_ZooLib* sAllocVector(size_t iSize)
	{
	return new ZValList_ZooLib(iSize, ZVal_ZooLib());
	}

static ZRectPOD* sAllocRect()
	{
	return new ZRectPOD;
	}

void ZVal_ZooLib::pFromStream(ZType iType, const ZStreamR& iStreamR)
	{
	switch (iType)
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
			uint32 theSize = iStreamR.ReadCount();
			if (theSize <= kBytesSize)
				{
				iType = (ZType)(-int(theSize)-1);
				if (theSize)
					iStreamR.Read(fType.fBytes, theSize);
				}
			else
				{
				sConstruct_T<ValString>(fType.fBytes, iStreamR, theSize);
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
			sConstruct_T<ZValMap_ZooLib>(fType.fBytes, iStreamR);
			break;
			}
		case eZType_RefCounted:
			{
			// Construct a nil refcounted.
			sConstruct_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes);
			break;
			}
		case eZType_Raw:
			{
			uint32 size = iStreamR.ReadCount();
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
			if (uint32 theCount = iStreamR.ReadCount())
				{
				ZValList_ZooLib* theVector = sAllocVector(theCount);
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
				fData.fAs_Vector = nullptr;
				}
			break;
			}
		default:
			{
			throw Ex_IllegalType(iType);
			}
		}
	fType.fType = ZType(iType);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_ZooLib

ZValList_ZooLib::operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(!this->empty()); }

ZValList_ZooLib::ZValList_ZooLib()
	{}

ZValList_ZooLib::ZValList_ZooLib(const ZValList_ZooLib& iOther)
:	vector<ZVal_ZooLib>(iOther)
	{}

ZValList_ZooLib::~ZValList_ZooLib()
	{}

ZValList_ZooLib& ZValList_ZooLib::operator=(const ZValList_ZooLib& iOther)
	{
	vector<ZVal_ZooLib>::operator=(iOther);
	return *this;
	}

ZValList_ZooLib::ZValList_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton)
:	vector<ZVal_ZooLib>(1, iSingleton)
	{}

ZValList_ZooLib::ZValList_ZooLib(const vector<ZVal_ZooLib>& iOther)
:	vector<ZVal_ZooLib>(iOther)
	{}

size_t ZValList_ZooLib::Count() const
	{ return size(); }

void ZValList_ZooLib::Clear()
	{ clear(); }

bool ZValList_ZooLib::QGet(size_t iIndex, ZVal_ZooLib& oVal) const
	{
	if (iIndex < size())
		{
		oVal = at(iIndex);
		return true;
		}
	return false;
	}

void ZValList_ZooLib::Set(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	if (iIndex < size())
		*(begin() + iIndex) = iVal;
	}

void ZValList_ZooLib::Erase(size_t iIndex)
	{
	if (iIndex < size())
		erase(begin() + iIndex);
	}

void ZValList_ZooLib::Insert(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	if (iIndex <= size())
		insert(begin() + iIndex, iVal);
	}

void ZValList_ZooLib::Append(const ZVal_ZooLib& iVal)
	{ push_back(iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib

/** \class ZValMap_ZooLib
\nosubgrouping
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib Constructing/assigning from stream

ZValMap_ZooLib::ZValMap_ZooLib(const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

ZValMap_ZooLib::ZValMap_ZooLib(bool dummy, const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib streaming

void ZValMap_ZooLib::ToStream(const ZStreamW& iStreamW) const
	{
	if (!fRep || fRep->fProperties.empty())
		{
		iStreamW.WriteCount(0);
		}
	else
		{
		const PropList& properties = fRep->fProperties;
		iStreamW.WriteCount(properties.size());
		for (PropList::const_iterator i = properties.begin(); i != properties.end(); ++i)
			{
			(*i).fName.ToStream(iStreamW);
			(*i).fTV.ToStream(iStreamW);
			}
		}
	}

void ZValMap_ZooLib::FromStream(const ZStreamR& iStreamR)
	{ fRep = sRepFromStream(iStreamR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib unioning

ZValMap_ZooLib ZValMap_ZooLib::Over(const ZValMap_ZooLib& iUnder) const
	{
	ZValMap_ZooLib result = iUnder;
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
#pragma mark * ZValMap_ZooLib comparison

int ZValMap_ZooLib::Compare(const ZValMap_ZooLib& iOther) const
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

bool ZValMap_ZooLib::operator<(const ZValMap_ZooLib& iOther) const
	{
	return this->Compare(iOther) < 0;
	}

bool ZValMap_ZooLib::Contains(const ZValMap_ZooLib& iOther) const
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

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib property meta-data

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::begin() const
	{
	if (fRep)
		return fRep->fProperties.begin();
	return sEmptyProperties.end();
	}

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::end() const
	{
	if (fRep)
		return fRep->fProperties.end();
	return sEmptyProperties.end();
	}

bool ZValMap_ZooLib::Empty() const
	{ return !fRep || fRep->fProperties.empty(); }

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::IteratorOf(const char* iPropName) const
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

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::IteratorOf(const ZTName& iPropName) const
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

size_t ZValMap_ZooLib::Count() const
	{
	if (fRep)
		return fRep->fProperties.size();
	return 0;
	}

const ZTName& ZValMap_ZooLib::NameOf(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return (*iPropIter).fName;
	return sNilName;
	}

bool ZValMap_ZooLib::Has(const char* iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

bool ZValMap_ZooLib::Has(const ZTName& iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

bool ZValMap_ZooLib::TypeOf(const_iterator iIterator, ZType& oPropertyType) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iIterator))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

bool ZValMap_ZooLib::TypeOf(const char* iPropName, ZType& oPropertyType) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

bool ZValMap_ZooLib::TypeOf(const ZTName& iPropName, ZType& oPropertyType) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		{
		oPropertyType = theValue->TypeOf();
		return true;
		}
	return false;
	}

ZType ZValMap_ZooLib::TypeOf(const_iterator iPropIter) const
	{ return this->pLookupConst(iPropIter).TypeOf(); }

ZType ZValMap_ZooLib::TypeOf(const char* iPropName) const
	{ return this->pLookupConst(iPropName).TypeOf(); }

ZType ZValMap_ZooLib::TypeOf(const ZTName& iPropName) const
	{ return this->pLookupConst(iPropName).TypeOf(); }

void ZValMap_ZooLib::Erase(const_iterator iPropIter)
	{
	if (fRep)
		this->pErase(iPropIter);
	}

void ZValMap_ZooLib::Erase(const char* iPropName)
	{
	if (fRep)
		this->pErase(this->IteratorOf(iPropName));
	}

void ZValMap_ZooLib::Erase(const ZTName& iPropName)
	{
	if (fRep)
		this->pErase(this->IteratorOf(iPropName));
	}

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::EraseAndReturn(const_iterator iPropIter)
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

		ZRef<Rep> oldRep = fRep;
		fRep = new Rep;

		fRep->fProperties.reserve(oldRep->fProperties.size() - 1);

		copy(oldRep->fProperties.begin(), iPropIter, back_inserter(fRep->fProperties));
		copy(++iPropIter, oldRep->fProperties.end(), back_inserter(fRep->fProperties));

		return fRep->fProperties.begin() + theEraseIndex;
		}
	}

void ZValMap_ZooLib::Clear()
	{
	fRep.Clear();
	}

// Do this explicitly here, otherwise CW7 chokes.
const ZVal_ZooLib& ZValMap_ZooLib::GetValue(const_iterator iPropIter) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropIter))
		return *theValue;
	return sNilValue;
	}

const ZVal_ZooLib& ZValMap_ZooLib::GetValue(const char* iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilValue;
	}

const ZVal_ZooLib& ZValMap_ZooLib::GetValue(const ZTName& iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilValue;
	}

template <class S, class Name_t>
bool ZValMap_ZooLib::QGet_T(Name_t iName, S& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iName))
		return theValue->QGet_T(oVal);
	return false;	
	}

void ZValMap_ZooLib::Set(const_iterator iPropIter, const ZVal_ZooLib& iVal)
	{ this->pSet(iPropIter, iVal); }

void ZValMap_ZooLib::Set(const char* iPropName, const ZVal_ZooLib& iVal)
	{ this->pSet(iPropName, iVal); }

void ZValMap_ZooLib::Set(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{ this->pSet(iPropName, iVal); }

bool ZValMap_ZooLib::SetNull(const_iterator iPropIter)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV.SetNull();
		return true;
		}
	return false;
	}

ZValMap_ZooLib& ZValMap_ZooLib::SetNull(const char* iPropName)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetNull();
	return *this;
	}

ZValMap_ZooLib& ZValMap_ZooLib::SetNull(const ZTName& iPropName)
	{
	this->pTouch();
	this->pFindOrAllocate(iPropName)->SetNull();
	return *this;
	}

bool ZValMap_ZooLib::SetValue(const_iterator iPropIter, const ZVal_ZooLib& iVal)
	{ return this->pSet(iPropIter, iVal); }

ZValMap_ZooLib& ZValMap_ZooLib::SetValue(const char* iPropName, const ZVal_ZooLib& iVal)
	{ return this->pSet(iPropName, iVal); }

ZValMap_ZooLib& ZValMap_ZooLib::SetValue(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{ return this->pSet(iPropName, iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib Mutable Get

ZVal_ZooLib& ZValMap_ZooLib::GetMutableValue(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropIter))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZVal_ZooLib& ZValMap_ZooLib::GetMutableValue(const char* iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZVal_ZooLib& ZValMap_ZooLib::GetMutableValue(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return *theValue;
	ZDebugStopf(0, ("GetMutableValue must only be called on extant properties"));
	return sNilValue;
	}

ZValMap_ZooLib& ZValMap_ZooLib::GetMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropIter))
		return theValue->GetMutableTuple();
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

ZValMap_ZooLib& ZValMap_ZooLib::GetMutableTuple(const char* iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return theValue->GetMutableTuple();
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

ZValMap_ZooLib& ZValMap_ZooLib::GetMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return theValue->GetMutableTuple();
	ZDebugStopf(0, ("GetMutableTuple must only be called on extant tuple properties"));
	return sNilTuple;
	}

ZValList_ZooLib& ZValMap_ZooLib::GetMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropIter))
		return theValue->GetMutableVector();
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

ZValList_ZooLib& ZValMap_ZooLib::GetMutableVector(const char* iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return theValue->GetMutableVector();
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

ZValList_ZooLib& ZValMap_ZooLib::GetMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	if (ZVal_ZooLib* theValue = this->pLookupAddress(iPropName))
		return theValue->GetMutableVector();
	ZDebugStopf(0, ("GetMutableVector must only be called on extant vector properties"));
	return sNilVector;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib Mutable Set

ZVal_ZooLib& ZValMap_ZooLib::SetMutableNull(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableNull();
	}

ZVal_ZooLib& ZValMap_ZooLib::SetMutableNull(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableNull();
	}

ZVal_ZooLib& ZValMap_ZooLib::SetMutableNull(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableNull();
	}

ZValMap_ZooLib& ZValMap_ZooLib::SetMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableTuple();
	}

ZValMap_ZooLib& ZValMap_ZooLib::SetMutableTuple(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableTuple();
	}

ZValMap_ZooLib& ZValMap_ZooLib::SetMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableTuple();
	}

ZValList_ZooLib& ZValMap_ZooLib::SetMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.SetMutableVector();
	}

ZValList_ZooLib& ZValMap_ZooLib::SetMutableVector(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableVector();
	}

ZValList_ZooLib& ZValMap_ZooLib::SetMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->SetMutableVector();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib ensure a property is of the type, and return a mutable reference

ZVal_ZooLib& ZValMap_ZooLib::EnsureMutableValue(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV;
	}

ZVal_ZooLib& ZValMap_ZooLib::EnsureMutableValue(const char* iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZVal_ZooLib& ZValMap_ZooLib::EnsureMutableValue(const ZTName& iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZValMap_ZooLib& ZValMap_ZooLib::EnsureMutableTuple(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.EnsureMutableTuple();
	}

ZValMap_ZooLib& ZValMap_ZooLib::EnsureMutableTuple(const char* iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableTuple();
	}

ZValMap_ZooLib& ZValMap_ZooLib::EnsureMutableTuple(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableTuple();
	}

ZValList_ZooLib& ZValMap_ZooLib::EnsureMutableVector(const_iterator iPropIter)
	{
	iPropIter = this->pTouch(iPropIter);
	return (*iPropIter).fTV.EnsureMutableVector();
	}

ZValList_ZooLib& ZValMap_ZooLib::EnsureMutableVector(const char * iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableVector();
	}

ZValList_ZooLib& ZValMap_ZooLib::EnsureMutableVector(const ZTName& iPropName)
	{
	this->pTouch();
	return this->pFindOrAllocate(iPropName)->EnsureMutableVector();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib minimize

// Ensure that our vector's capacity is just enough to hold our data.
ZValMap_ZooLib& ZValMap_ZooLib::Minimize()
	{
	if (fRep)
		fRep = new Rep(fRep.GetObject());
	return *this;
	}

ZValMap_ZooLib ZValMap_ZooLib::Minimized() const
	{ return ZValMap_ZooLib(*this).Minimize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib macros

#define ZMACRO_ZValMapAccessors_Def_Entry(Name_t, TYPENAME, TYPE) \
	bool ZValMap_ZooLib::Get##TYPENAME(Name_t iName, TYPE& oVal) const \
		{ return this->QGet##TYPENAME(iName, oVal); } \
	bool ZValMap_ZooLib::QGet##TYPENAME(Name_t iName, TYPE& oVal) const \
		{ \
		ZVal_ZooLib theVal; \
		if (this->QGet(iName, theVal)) \
			return theVal.QGet_T<TYPE>(oVal); \
		return false; \
		} \
	TYPE ZValMap_ZooLib::DGet##TYPENAME(Name_t iName, const TYPE& iDefault) const \
		{ return this->DGet_T<>(iName, iDefault); } \
	TYPE ZValMap_ZooLib::Get##TYPENAME(Name_t iName) const \
		{ return this->Get_T<TYPE>(iName); } \
	void ZValMap_ZooLib::Set##TYPENAME(Name_t iName, const TYPE& iVal) \
		{ return this->Set_T<>(iName, iVal); } \

#define ZMACRO_ZValMapAccessors_Def_Std(Name_t) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, ID, uint64) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int8, int8) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int16, int16) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int32, int32) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Int64, int64) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Bool, bool) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Float, float) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Double, double) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Pointer, VoidStar_t) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, String, std::string) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Tuple, ZValMap_ZooLib) \
	ZMACRO_ZValMapAccessors_Def_Entry(Name_t, Vector, vector<ZVal_ZooLib>) \

ZMACRO_ZValMapAccessors_Def_Std(const char*)
ZMACRO_ZValMapAccessors_Def_Std(const ZTName&)
ZMACRO_ZValMapAccessors_Def_Std(PropList::iterator)

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib Internals

ZVal_ZooLib* ZValMap_ZooLib::pFindOrAllocate(const char* iPropName)
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

ZVal_ZooLib* ZValMap_ZooLib::pFindOrAllocate(const ZTName& iPropName)
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

bool ZValMap_ZooLib::pSet(const_iterator iPropIter, const ZVal_ZooLib& iVal)
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		{
		iPropIter = this->pTouch(iPropIter);
		(*iPropIter).fTV = iVal;
		return true;
		}
	return false;
	}

ZValMap_ZooLib& ZValMap_ZooLib::pSet(const char* iPropName, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	return *this;
	}

ZValMap_ZooLib& ZValMap_ZooLib::pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	return *this;
	}

const ZVal_ZooLib* ZValMap_ZooLib::pLookupAddressConst(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return &(*iPropIter).fTV;
	return nullptr;
	}

const ZVal_ZooLib* ZValMap_ZooLib::pLookupAddressConst(const char* iPropName) const
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
	return nullptr;
	}

const ZVal_ZooLib* ZValMap_ZooLib::pLookupAddressConst(const ZTName& iPropName) const
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
	return nullptr;
	}

ZVal_ZooLib* ZValMap_ZooLib::pLookupAddress(const_iterator iPropIter)
	{
	// We're the non-const version, being called from a mutating operation
	// so pTouch must have been called and we must have a rep.
	ZAssertStop(kDebug_Tuple, fRep);

	if (fRep && iPropIter != fRep->fProperties.end())
		return &(*iPropIter).fTV;
	return nullptr;
	}

ZVal_ZooLib* ZValMap_ZooLib::pLookupAddress(const char* iPropName)
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
	return nullptr;
	}

ZVal_ZooLib* ZValMap_ZooLib::pLookupAddress(const ZTName& iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName))
			return &(*i).fTV;
		}
	return nullptr;
	}

const ZVal_ZooLib& ZValMap_ZooLib::pLookupConst(const_iterator iPropIter) const
	{
	if (fRep && iPropIter != fRep->fProperties.end())
		return (*iPropIter).fTV;
	return sNilValue;
	}

const ZVal_ZooLib& ZValMap_ZooLib::pLookupConst(const char* iPropName) const
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

const ZVal_ZooLib& ZValMap_ZooLib::pLookupConst(const ZTName& iPropName) const
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

void ZValMap_ZooLib::pErase(const_iterator iPropIter)
	{
	if (iPropIter == fRep->fProperties.end())
		return;

	if (fRep->GetRefCount() == 1)
		{
		fRep->fProperties.erase(iPropIter);
		}
	else
		{
		ZRef<Rep> oldRep = fRep;
		fRep = new Rep;

		fRep->fProperties.reserve(oldRep->fProperties.size() - 1);
		copy(oldRep->fProperties.begin(), iPropIter, back_inserter(fRep->fProperties));
		copy(++iPropIter, oldRep->fProperties.end(), back_inserter(fRep->fProperties));
		}
	}

void ZValMap_ZooLib::pTouch()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = new Rep(fRep.GetObject());
		}
	else
		{
		fRep = new Rep;
		}
	}

ZValMap_ZooLib::const_iterator ZValMap_ZooLib::pTouch(const_iterator iPropIter)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	if (fRep->GetRefCount() == 1)
		return iPropIter;

	size_t index = iPropIter - fRep->fProperties.begin();
	fRep = new Rep(fRep.GetObject());
	return fRep->fProperties.begin() + index;
	}

bool ZValMap_ZooLib::pIsEqual(const ZValMap_ZooLib& iOther) const
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

bool ZValMap_ZooLib::pIsSameAs(const ZValMap_ZooLib& iOther) const
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

ZRef<ZValMap_ZooLib::Rep> ZValMap_ZooLib::sRepFromStream(const ZStreamR& iStreamR)
	{
	ZRef<Rep> theRep;
	if (uint32 propertyCount = iStreamR.ReadCount())
		{
		theRep = new Rep;
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
#pragma mark * ZValMap_ZooLib

ZValMap_ZooLib::Rep::Rep()
	{}

ZValMap_ZooLib::Rep::Rep(const Rep* iOther)
:	fProperties(iOther->fProperties)
	{}

ZValMap_ZooLib::Rep::~Rep()
	{}

NAMESPACE_ZOOLIB_END
