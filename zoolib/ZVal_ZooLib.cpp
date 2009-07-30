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

#include "zoolib/ZCompare.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStream_ValData_T.h"
#include "zoolib/ZVal_ZooLib.h"

#include <map>
#include <typeinfo>

using std::map;
using std::min;
using std::pair;
using std::string;
using std::type_info;
using std::vector;

#define kDebug_Tuple 1

NAMESPACE_ZOOLIB_BEGIN

static ZTName sNilName;
static ZValMap_ZooLib::PropList sEmptyProperties;
static ZVal_ZooLib sNilVal;
static ZValList_ZooLib sNilList;
static vector<ZVal_ZooLib> sNilVector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTypeAsTypeInfo

const type_info& ZTypeAsTypeInfo(ZType iType)
	{
	switch (iType)
		{
		case eZType_Null: return typeid(void);
		case eZType_Type: return typeid(ZType);
		case eZType_ID: return typeid(uint64);
		case eZType_Int8: return typeid(int8);
		case eZType_Int16: return typeid(int16);
		case eZType_Int32: return typeid(int32);
		case eZType_Int64: return typeid(int64);
		case eZType_Bool: return typeid(bool);
		case eZType_Float: return typeid(float);
		case eZType_Double: return typeid(double);
		case eZType_Time: return typeid(ZTime);
		case eZType_Pointer: return typeid(void*);
		case eZType_Rect: return typeid(ZRectPOD);
		case eZType_Point: return typeid(ZPointPOD);
		case eZType_String: return typeid(string);
//##		case eZType_Name: return typeid(uint64);
//		case eZType_RefCounted: return typeid(uint64);
//		case eZType_Raw: return typeid(uint64);
//		case eZType_Vector: return typeid(uint64);
		}
	ZUnimplemented();
	return typeid(void);
	}

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
for holding longer string properties in ZVal_ZooLib instances. */

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

	int Compare(const string& iString) const;

	bool Empty() const;

	void ToStream(const ZStreamW& iStreamW) const;

	void ToString(string& oString) const;

	string AsString() const;

private:
	const size_t fSize;
	char* fBuffer;
	};

} // namespace ZANONYMOUS

inline ValString::~ValString()
	{ delete[] fBuffer; }

inline bool ValString::Empty() const
	{ return !fSize; }

inline string ValString::AsString() const
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

bool ZVal_ZooLib::sFromAny(const ZAny& iAny, ZVal_ZooLib& oVal)
	{
	if (false)
		{}
	else if (iAny.type() == typeid(void))
		{
		oVal = ZVal_ZooLib();
		}
	else if (const ZType* theValue = ZAnyCast<ZType>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const uint64* theValue = ZAnyCast<uint64>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const int8* theValue = ZAnyCast<int8>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const int16* theValue = ZAnyCast<int16>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const int32* theValue = ZAnyCast<int32>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const int64* theValue = ZAnyCast<int64>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const bool* theValue = ZAnyCast<bool>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const float* theValue = ZAnyCast<float>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const double* theValue = ZAnyCast<double>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZTime* theValue = ZAnyCast<ZTime>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const VoidStar_t* theValue = ZAnyCast<VoidStar_t>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZRectPOD* theValue = ZAnyCast<ZRectPOD>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZPointPOD* theValue = ZAnyCast<ZPointPOD>(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZRef<ZRefCountedWithFinalization>* theValue
		= ZAnyCast<ZRef<ZRefCountedWithFinalization> >(&iAny))
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const vector<char>* theValue = ZAnyCast<vector<char> >(&iAny))
		{
		if (size_t theSize = theValue->size())
			oVal = ZValData_ZooLib(&(*theValue)[0], theSize);
		else
			oVal = ZValData_ZooLib();
		}
	else if (const vector<ZAny>* theValue = ZAnyCast<vector<ZAny> >(&iAny))
		{
		ZValList_ZooLib theList;
		for (vector<ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sFromAny(*i, local))
				theList.Append(local);
			else
				theList.Append(ZVal_ZooLib());
			}
		oVal = theList;
		}
	else if (const map<string, ZAny>* theValue = ZAnyCast<map<string, ZAny> >(&iAny))
		{
		ZValMap_ZooLib theMap;
		for (map<string, ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sFromAny((*i).second, local))
				theMap.Set((*i).first, local);
			}
		oVal = theMap;
		}
	else
		{
		return false;
		}
	return true;
	}

ZAny ZVal_ZooLib::AsAny() const
	{
	switch (fType.fType)
		{
		case eZType_Null:
			{
			return ZAny();
			}
		case eZType_Type:
			{
			return fData.fAs_Type;
			}
		case eZType_ID:
			{
			return fData.fAs_ID;
			}
		case eZType_Int8:
			{
			return fData.fAs_Int8;
			}
		case eZType_Int16:
			{
			return fData.fAs_Int16;
			}
		case eZType_Int32:
			{
			return fData.fAs_Int32;
			}
		case eZType_Int64:
			{
			return fData.fAs_Int64;
			}
		case eZType_Bool:
			{
			return fData.fAs_Bool;
			}
		case eZType_Float:
			{
			return fData.fAs_Float;
			}
		case eZType_Double:
			{
			return fData.fAs_Double;
			}
		case eZType_Time:
			{
			return fData.fAs_Time;
			}
		case eZType_Pointer:
			{
			return fData.fAs_Pointer;
			}
		case eZType_Rect:
			{
			return fData.fAs_Rect;
			}
		case eZType_Point:
			{
			return fData.fAs_Point;
			}
		case eZType_String:
			{
			return sFetch_T<ValString>(fType.fBytes)->AsString();
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
			return sFetch_T<ZValMap_ZooLib>(fType.fBytes)->AsAny();
			break;
			}
		case eZType_RefCounted:
			{
			// Just do nothing. We'll construct a nil refcounted when we read.
			break;
			}
		case eZType_Raw:
			{
			return sFetch_T<ZValData_ZooLib>(fType.fBytes)->AsAny();
			}
		case eZType_Vector:
			{
			return sFetch_T<ZValList_ZooLib>(fType.fBytes)->AsAny();
			}
		}
	return ZAny();
	}

ZVal_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fType.fType != eZType_Null); }

void ZVal_ZooLib::swap(ZVal_ZooLib& iOther)
	{ std::swap(fFastCopy, iOther.fFastCopy); }

ZVal_ZooLib::ZVal_ZooLib()
	{ fType.fType = eZType_Null; }

ZVal_ZooLib::~ZVal_ZooLib()
	{ this->pRelease(); }

ZVal_ZooLib::ZVal_ZooLib(const ZVal_ZooLib& iOther)
	{ this->pCopy(iOther); }

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

ZVal_ZooLib::ZVal_ZooLib(const ZVal_ZooLib& iVal, bool iAsVector)
	{
	ZAssertStop(kDebug_Tuple, iAsVector);
	sConstruct_T<ZValList_ZooLib>(fType.fBytes, 1, iVal);
	fType.fType = eZType_Vector;
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

ZVal_ZooLib::ZVal_ZooLib(const ZValData_ZooLib& iVal)
	{
	fType.fType = eZType_Raw;
	sConstruct_T(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZValList_ZooLib& iVal)
	{
	fType.fType = eZType_Vector;
	sConstruct_T(fType.fBytes, iVal);
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

ZVal_ZooLib::ZVal_ZooLib(const void* iSource, size_t iSize)
	{
	fType.fType = eZType_Raw;
	sConstruct_T<ZValData_ZooLib>(fType.fBytes, iSource, iSize);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize)
	{
	fType.fType = eZType_Raw;
	ZValData_ZooLib* theRaw = sConstruct_T<ZValData_ZooLib>(fType.fBytes);
	sRead_T(*theRaw, iStreamR, iSize);
	}

void ZVal_ZooLib::Clear()
	{
	this->pRelease();
	fType.fType = eZType_Null;	
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
bool ZVal_ZooLib::QGet_T<ZValData_ZooLib>(ZValData_ZooLib& oVal) const
	{
	if (fType.fType == eZType_Raw)
		{
		oVal = *sFetch_T<ZValData_ZooLib>(fType.fBytes);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZValList_ZooLib>(ZValList_ZooLib& oVal) const
	{
	if (fType.fType == eZType_Vector)
		{
		oVal = *sFetch_T<ZValList_ZooLib>(fType.fBytes);
		return true;
		}
	return false;
	}

template <>
bool ZVal_ZooLib::QGet_T<ZValMap_ZooLib>(ZValMap_ZooLib& oVal) const
	{
	if (fType.fType == eZType_Tuple)
		{
		oVal = *sFetch_T<ZValMap_ZooLib>(fType.fBytes);
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
void ZVal_ZooLib::Set_T<ZValData_ZooLib>(const ZValData_ZooLib& iVal)
	{
	this->pRelease();
	sConstruct_T(fType.fBytes, iVal);
	fType.fType = eZType_Raw;
	}

template <>
void ZVal_ZooLib::Set_T<ZValList_ZooLib>(const ZValList_ZooLib& iVal)
	{
	this->pRelease();
	sConstruct_T(fType.fBytes, iVal);
	fType.fType = eZType_Vector;
	}

template <>
void ZVal_ZooLib::Set_T<ZValMap_ZooLib>(const ZValMap_ZooLib& iVal)
	{
	this->pRelease();
	sConstruct_T(fType.fBytes, iVal);
	fType.fType = eZType_Tuple;
	}

template <>
void ZVal_ZooLib::Set_T<ZRef<ZRefCountedWithFinalize> >(const ZRef<ZRefCountedWithFinalize>& iVal)
	{
	this->pRelease();
	fType.fType = eZType_RefCounted;
	sConstruct_T(fType.fBytes, iVal);
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
	{ return this->Compare(iOther) == 0; }

bool ZVal_ZooLib::operator<(const ZVal_ZooLib& iOther) const
	{ return this->Compare(iOther) < 0; }

ZValList_ZooLib& ZVal_ZooLib::MutableList()
	{
	if (fType.fType != eZType_Vector)
		{
		this->pRelease();
		sConstruct_T<ZValList_ZooLib>(fType.fBytes);
		fType.fType = eZType_Vector;
		}
	return *sFetch_T<ZValList_ZooLib>(fType.fBytes);
	}

ZValMap_ZooLib& ZVal_ZooLib::MutableMap()
	{
	if (fType.fType != eZType_Tuple)
		{
		this->pRelease();
		sConstruct_T<ZValMap_ZooLib>(fType.fBytes);
		fType.fType = eZType_Tuple;
		}
	return *sFetch_T<ZValMap_ZooLib>(fType.fBytes);
	}

ZType ZVal_ZooLib::TypeOf() const
	{ return fType.fType < 0 ? eZType_String : ZType(fType.fType); }

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
			const ZValData_ZooLib* theMemoryBlock = sFetch_T<ZValData_ZooLib>(fType.fBytes);
			iStreamW.WriteCount(theMemoryBlock->GetSize());
			if (theMemoryBlock->GetSize())
				iStreamW.Write(theMemoryBlock->GetData(), theMemoryBlock->GetSize());
			break;
			}
		case eZType_Vector:
			{
			sFetch_T<ZValList_ZooLib>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
		default:
			{
			ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_ZooLib typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_ZooLib)
ZMACRO_ZValAccessors_Def_ZooLib(ZVal_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Data, ZValData_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, List, ZValList_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Map, ZValMap_ZooLib)

// Backwards compatibility
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Raw, ZValData_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(ZVal_ZooLib, Tuple, ZValMap_ZooLib)

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
			return sFetch_T<ZValMap_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes));
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
			return sFetch_T<ZValData_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZValData_ZooLib>(iOther.fType.fBytes));
			}
		case eZType_Vector:
			{
			return sFetch_T<ZValList_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZValList_ZooLib>(iOther.fType.fBytes));
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
			return *sFetch_T<ZValData_ZooLib>(fType.fBytes)
				< *sFetch_T<ZValData_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			return *sFetch_T<ZValList_ZooLib>(fType.fBytes)
				< *sFetch_T<ZValList_ZooLib>(iOther.fType.fBytes);
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
			return *sFetch_T<ZValMap_ZooLib>(fType.fBytes)
				== *sFetch_T<ZValMap_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZRefCountedWithFinalize> >(fType.fBytes)
				== *sFetch_T<ZRef<ZRefCountedWithFinalize> >(iOther.fType.fBytes);
			}
		case eZType_Raw:
			{
			return *sFetch_T<ZValData_ZooLib>(fType.fBytes)
				== *sFetch_T<ZValData_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			return *sFetch_T<ZValList_ZooLib>(fType.fBytes)
				== *sFetch_T<ZValList_ZooLib>(iOther.fType.fBytes);
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
		case eZType_Raw: sDestroy_T<ZValData_ZooLib>(fType.fBytes); break;
		case eZType_Vector: sDestroy_T<ZValList_ZooLib>(fType.fBytes); break;
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
				sCopyConstruct_T<ZValData_ZooLib>(iOther.fType.fBytes, fType.fBytes);
				break;
			case eZType_Vector:
				sCopyConstruct_T<ZValList_ZooLib>(iOther.fType.fBytes, fType.fBytes);
				break;
			default:
				ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
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
			ZRectPOD* theRect = new ZRectPOD;
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
			ZValData_ZooLib* theRaw = sConstruct_T<ZValData_ZooLib>(fType.fBytes, size);
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
			sConstruct_T<ZValList_ZooLib>(fType.fBytes, iStreamR);
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
#pragma mark * ZValList_ZooLib::Rep

class ZValList_ZooLib::Rep : public ZRefCounted
	{
public:
	Rep();
	Rep(const std::vector<ZVal_ZooLib>& iOther);
	virtual ~Rep();

	std::vector<ZVal_ZooLib> fVector;
	};

ZValList_ZooLib::Rep::Rep()
	{}

ZValList_ZooLib::Rep::Rep(const vector<ZVal_ZooLib>& iOther)
:	fVector(iOther)
	{}

ZValList_ZooLib::Rep::~Rep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_ZooLib

ZAny ZValList_ZooLib::AsAny() const
	{
	vector<ZAny> theVector;
	if (fRep)
		{
		if (size_t theCount = fRep->fVector.size())
			{
			theVector.reserve(theCount);
			for (size_t x = 0; x < theCount; ++x)
				theVector.push_back(fRep->fVector[x].AsAny());
			}
		}
	return theVector;
	}

ZValList_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fVector.empty()); }

ZValList_ZooLib::ZValList_ZooLib()
	{}

ZValList_ZooLib::ZValList_ZooLib(const ZValList_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

ZValList_ZooLib::~ZValList_ZooLib()
	{}

ZValList_ZooLib& ZValList_ZooLib::operator=(const ZValList_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZValList_ZooLib::ZValList_ZooLib(const ZStreamR& iStreamR)
	{
	if (uint32 theCount = iStreamR.ReadCount())
		{
		fRep = new Rep;
		fRep->fVector.reserve(theCount);
		for (size_t x = 0; x < theCount; ++x)
			fRep->fVector.push_back(ZVal_ZooLib(iStreamR));
		}
	}

ZValList_ZooLib::ZValList_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton)
:	fRep(new Rep(vector<ZVal_ZooLib>(iCount, iSingleton)))
	{}

ZValList_ZooLib::ZValList_ZooLib(const vector<ZVal_ZooLib>& iOther)
:	fRep(new Rep(vector<ZVal_ZooLib>(iOther)))
	{}

size_t ZValList_ZooLib::Count() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

void ZValList_ZooLib::Clear()
	{ fRep.Clear(); }

bool ZValList_ZooLib::QGet(size_t iIndex, ZVal_ZooLib& oVal) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		oVal = fRep->fVector.at(iIndex);
		return true;
		}
	return false;
	}

ZVal_ZooLib ZValList_ZooLib::DGet(size_t iIndex, const ZVal_ZooLib& iDefault) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return iDefault;
	}

ZVal_ZooLib ZValList_ZooLib::Get(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return sNilVal;
	}

const ZVal_ZooLib& ZValList_ZooLib::RGet(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return sNilVal;
	}

void ZValList_ZooLib::Set(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex < theVec.size())
		*(theVec.begin() + iIndex) = iVal;
	}

void ZValList_ZooLib::Erase(size_t iIndex)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex < theVec.size())
		theVec.erase(theVec.begin() + iIndex);
	}

void ZValList_ZooLib::Insert(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex <= theVec.size())
		theVec.insert(theVec.begin() + iIndex, iVal);
	}

void ZValList_ZooLib::Append(const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(iVal);
	}

int ZValList_ZooLib::Compare(const ZValList_ZooLib& iOther) const
	{
	if (fRep)
		{
		if (iOther.fRep)
			{
			return sCompare_T(fRep->fVector.begin(), fRep->fVector.end(),
				iOther.fRep->fVector.begin(), iOther.fRep->fVector.end());
			}
		else
			{
			return 1;
			}
		}
	else if (iOther.fRep)
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

bool ZValList_ZooLib::operator==(const ZValList_ZooLib& iOther) const
	{ return this->Compare(iOther) == 0; }

bool ZValList_ZooLib::operator<(const ZValList_ZooLib& iOther) const
	{ return this->Compare(iOther) < 0; }

ZVal_ZooLib& ZValList_ZooLib::Mutable(size_t iIndex)
	{
	ZAssert(fRep && iIndex < fRep->fVector.size());
	return fRep->fVector.at(iIndex);
	}

vector<ZVal_ZooLib>& ZValList_ZooLib::MutableVector()
	{
	this->pTouch();
	return fRep->fVector;
	}

const vector<ZVal_ZooLib>& ZValList_ZooLib::GetVector() const
	{
	if (fRep)
		return fRep->fVector;
	return sNilVector;
	}

void ZValList_ZooLib::ToStream(const ZStreamW& iStreamW) const
	{
	if (fRep)
		{
		const vector<ZVal_ZooLib>& theVec = fRep->fVector;
		if (size_t theCount = theVec.size())
			{
			iStreamW.WriteCount(theCount);
			for (size_t x = 0; x < theCount; ++x)
				{
				// Sigh, older linux headers don't have vector::at
				// theVector->at(x).ToStream(iStreamW);
				theVec.operator[](x).ToStream(iStreamW);
				}
			return;
			}
		}
	iStreamW.WriteCount(0);
	}

void ZValList_ZooLib::pTouch()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = new Rep(fRep->fVector);
		}
	else
		{
		fRep = new Rep;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * NameVal inlines

NameVal::NameVal(const ZStreamR& iStreamR)
:	fName(iStreamR)
	{
	// We have to load the val here, because for layout reasons we need fVal
	// to come before fName, but for streaming we write fName before fVal.
	fVal = ZVal_ZooLib(iStreamR);
	}

inline NameVal::NameVal()
	{}

inline NameVal::NameVal(const NameVal& iOther)
:	fVal(iOther.fVal),
	fName(iOther.fName)
	{}

inline NameVal::NameVal(const char* iName, const ZVal_ZooLib& iVal)
:	fVal(iVal),
	fName(iName)
	{}

inline NameVal::NameVal(const ZTName& iName, const ZVal_ZooLib& iVal)
:	fVal(iVal),
	fName(iName)
	{}

inline NameVal::NameVal(const char* iName)
:	fName(iName)
	{}

inline NameVal::NameVal(const ZTName& iName)
:	fName(iName)
	{}

void NameVal::ToStream(const ZStreamW& iStreamW) const
	{
	fName.ToStream(iStreamW);
	fVal.ToStream(iStreamW);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib::Rep

class ZValMap_ZooLib::Rep : public ZRefCounted
	{
public:
	Rep();
	Rep(const ZValMap_ZooLib::PropList& iProperties);
	virtual ~Rep();

	ZValMap_ZooLib::PropList fProperties;
	};

ZValMap_ZooLib::Rep::Rep()
	{}

ZValMap_ZooLib::Rep::Rep(const ZValMap_ZooLib::PropList& iProperties)
:	fProperties(iProperties)
	{}

ZValMap_ZooLib::Rep::~Rep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib

ZAny ZValMap_ZooLib::AsAny() const
	{
	map<string, ZAny> theMap;
	if (fRep)
		{
		for (Index_t i = fRep->fProperties.begin(), end = fRep->fProperties.end();
			i != end; ++i)
			{
			theMap.insert(pair<string, ZAny>((*i).fName.AsString(), (*i).fVal));
			}
		}
	return theMap;
	}

ZValMap_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fProperties.empty()); }

ZValMap_ZooLib::ZValMap_ZooLib()
	{}

ZValMap_ZooLib::ZValMap_ZooLib(const ZValMap_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

ZValMap_ZooLib::~ZValMap_ZooLib()
	{}

ZValMap_ZooLib& ZValMap_ZooLib::operator=(const ZValMap_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZValMap_ZooLib::ZValMap_ZooLib(const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

ZValMap_ZooLib::ZValMap_ZooLib(ZRef<Rep> iRep)
:	fRep(iRep)
	{}

void ZValMap_ZooLib::Clear()
	{ fRep.Clear(); }


bool ZValMap_ZooLib::QGet(Index_t iIndex, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iIndex))
		{
		oVal = *theValue;
		return true;
		}
	return false;	
	}

bool ZValMap_ZooLib::QGet(const char* iPropName, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

bool ZValMap_ZooLib::QGet(const ZTName& iPropName, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

ZVal_ZooLib ZValMap_ZooLib::DGet(Index_t iIndex, const ZVal_ZooLib& iDefault) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iIndex))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZValMap_ZooLib::DGet(const char* iPropName, const ZVal_ZooLib& iDefault) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZValMap_ZooLib::DGet(const ZTName& iPropName, const ZVal_ZooLib& iDefault) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZValMap_ZooLib::Get(Index_t iIndex) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iIndex))
		return *theValue;
	return sNilVal;
	}

ZVal_ZooLib ZValMap_ZooLib::Get(const char* iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilVal;
	}

ZVal_ZooLib ZValMap_ZooLib::Get(const ZTName& iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilVal;
	}

const ZVal_ZooLib& ZValMap_ZooLib::RGet(Index_t iIndex) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iIndex))
		return *theValue;
	return sNilVal;
	}

const ZVal_ZooLib& ZValMap_ZooLib::RGet(const char* iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilVal;
	}

const ZVal_ZooLib& ZValMap_ZooLib::RGet(const ZTName& iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->pLookupAddressConst(iPropName))
		return *theValue;
	return sNilVal;
	}

void ZValMap_ZooLib::Set(Index_t iIndex, const ZVal_ZooLib& iVal)
	{ this->pSet(iIndex, iVal); }

void ZValMap_ZooLib::Set(const char* iPropName, const ZVal_ZooLib& iVal)
	{ this->pSet(iPropName, iVal); }

void ZValMap_ZooLib::Set(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{ this->pSet(iPropName, iVal); }

void ZValMap_ZooLib::Erase(Index_t iIndex)
	{
	if (fRep)
		this->pErase(iIndex);
	}

void ZValMap_ZooLib::Erase(const char* iPropName)
	{
	if (fRep)
		this->pErase(this->IndexOf(iPropName));
	}

void ZValMap_ZooLib::Erase(const ZTName& iPropName)
	{
	if (fRep)
		this->pErase(this->IndexOf(iPropName));
	}

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
				if (int compare = (*iterThis).fVal.Compare((*iterOther).fVal))
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

bool ZValMap_ZooLib::operator==(const ZValMap_ZooLib& iOther) const
	{ return this->Compare(iOther) == 0; }

bool ZValMap_ZooLib::operator<(const ZValMap_ZooLib& iOther) const
	{ return this->Compare(iOther) < 0; }

ZVal_ZooLib& ZValMap_ZooLib::Mutable(Index_t iIndex)
	{
	ZAssert(fRep && iIndex != fRep->fProperties.end());
	iIndex = this->pTouch(iIndex);
	return (*iIndex).fVal;
	}

ZVal_ZooLib& ZValMap_ZooLib::Mutable(const char* iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZVal_ZooLib& ZValMap_ZooLib::Mutable(const ZTName& iPropName)
	{
	this->pTouch();
	return *this->pFindOrAllocate(iPropName);
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::Begin() const
	{
	if (fRep)
		return fRep->fProperties.begin();
	return sEmptyProperties.end();
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::End() const
	{
	if (fRep)
		return fRep->fProperties.end();
	return sEmptyProperties.end();
	}

bool ZValMap_ZooLib::Empty() const
	{ return !fRep || fRep->fProperties.empty(); }

size_t ZValMap_ZooLib::Count() const
	{
	if (fRep)
		return fRep->fProperties.size();
	return 0;
	}

const ZTName& ZValMap_ZooLib::NameOf(Index_t iIndex) const
	{
	if (fRep && iIndex != fRep->fProperties.end())
		return (*iIndex).fName;
	return sNilName;
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::IndexOf(const char* iPropName) const
	{
	if (fRep)
		{
		size_t propNameLength = strlen(iPropName);
		Index_t end = fRep->fProperties.end();
		for (Index_t i = fRep->fProperties.begin(); i != end; ++i)
			{
			if ((*i).fName.Equals(iPropName, propNameLength))
				return i;
			}
		return end;
		}
	return sEmptyProperties.end();
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::IndexOf(const ZTName& iPropName) const
	{
	if (fRep)
		{
		Index_t end = fRep->fProperties.end();
		for (Index_t i = fRep->fProperties.begin(); i != end; ++i)
			{
			if ((*i).fName.Equals(iPropName))
				return i;
			}
		return end;
		}
	return sEmptyProperties.end();
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::IndexOf(
	const ZValMap_ZooLib& iOther, const Index_t& iOtherIndex) const
	{
	if (fRep == iOther.fRep)
		return iOtherIndex;

	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

bool ZValMap_ZooLib::Has(const char* iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

bool ZValMap_ZooLib::Has(const ZTName& iPropName) const
	{ return this->pLookupAddressConst(iPropName); }

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
			(*i).ToStream(iStreamW);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_ZooLib internal implementation

void ZValMap_ZooLib::pSet(Index_t iIndex, const ZVal_ZooLib& iVal)
	{
	if (fRep && iIndex != fRep->fProperties.end())
		{
		iIndex = this->pTouch(iIndex);
		(*iIndex).fVal = iVal;
		}
	}

void ZValMap_ZooLib::pSet(const char* iPropName, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	}

void ZValMap_ZooLib::pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	*pFindOrAllocate(iPropName) = iVal;
	}

ZVal_ZooLib* ZValMap_ZooLib::pFindOrAllocate(const char* iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	size_t propNameLength = strlen(iPropName);
	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName, propNameLength))
			return &(*i).fVal;
		}
	fRep->fProperties.push_back(NameVal(iPropName));
	return &fRep->fProperties.back().fVal;
	}

ZVal_ZooLib* ZValMap_ZooLib::pFindOrAllocate(const ZTName& iPropName)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName))
			return &(*i).fVal;
		}
	fRep->fProperties.push_back(NameVal(iPropName));
	return &fRep->fProperties.back().fVal;
	}

const ZVal_ZooLib* ZValMap_ZooLib::pLookupAddressConst(Index_t iIndex) const
	{
	if (fRep && iIndex != fRep->fProperties.end())
		return &(*iIndex).fVal;
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
				return &(*i).fVal;
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
				return &(*i).fVal;
			}
		}
	return nullptr;
	}

void ZValMap_ZooLib::pErase(Index_t iIndex)
	{
	if (iIndex == fRep->fProperties.end())
		return;

	if (fRep->GetRefCount() == 1)
		{
		fRep->fProperties.erase(iIndex);
		}
	else
		{
		ZRef<Rep> oldRep = fRep;
		fRep = new Rep;

		fRep->fProperties.reserve(oldRep->fProperties.size() - 1);
		copy(oldRep->fProperties.begin(), iIndex, back_inserter(fRep->fProperties));
		copy(++iIndex, oldRep->fProperties.end(), back_inserter(fRep->fProperties));
		}
	}

void ZValMap_ZooLib::pTouch()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = new Rep(fRep->fProperties);
		}
	else
		{
		fRep = new Rep;
		}
	}

ZValMap_ZooLib::Index_t ZValMap_ZooLib::pTouch(Index_t iIndex)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	if (fRep->GetRefCount() == 1)
		return iIndex;

	size_t index = iIndex - fRep->fProperties.begin();
	fRep = new Rep(fRep->fProperties);
	return fRep->fProperties.begin() + index;
	}

ZRef<ZValMap_ZooLib::Rep> ZValMap_ZooLib::sRepFromStream(const ZStreamR& iStreamR)
	{
	ZRef<Rep> theRep;
	if (uint32 propertyCount = iStreamR.ReadCount())
		{
		theRep = new Rep;
		PropList& properties = theRep->fProperties;

		properties.reserve(propertyCount);
		while (propertyCount--)
			properties.push_back(NameVal(iStreamR));
		}
	return theRep;
	}

NAMESPACE_ZOOLIB_END
