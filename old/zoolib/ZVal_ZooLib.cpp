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

#include "zoolib/ZCompare_Vector.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStream_Data_T.h"
#include "zoolib/ZVal_ZooLib.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"

#include <map>
#include <iterator>
#include <string.h>
#include <typeinfo>

using std::map;
using std::min;
using std::pair;
using std::string;
using std::type_info;
using std::vector;

#define kDebug_Tuple 1

namespace ZooLib {

static ZTName spNilName;
static ZMap_ZooLib::PropList spEmptyProperties;
static ZVal_ZooLib spNilVal;
static ZSeq_ZooLib spNilSeq;
static vector<ZVal_ZooLib> spNilVector;

// =================================================================================================

/**
Provides standard textual versions of ZType values.
*/
static const char* const spZTypeAsString[eZType_Max] =
	{
	"Null",
	"String",
	"!!NOTVALID!!", // Needed to maintain correct offsets.
	"Int8",
	"Int16",
	"Int32",
	"Int64",
	"Float",
	"Double",
	"Bool",
	"Pointer",
	"Raw",
	"Tuple",
	"RefCounted",
	"Rect",
	"Point",
	"Region",
	"ID",
	"Vector",
	"Type",
	"Time",
//##	"Name"
	};

const char* ZTypeAsString(ZType iType)
	{
	if (iType <= eZType_Time)
		return spZTypeAsString[iType];
	return "Unknown";
	}

// =================================================================================================
// MARK: - Helper functions

static inline bool spIsSpecialString(size_t iSize)
	{ return iSize <= ZVal_ZooLib::kBytesSize; }

// =================================================================================================
// MARK: - ValString

namespace { // anonymous

/** ValString is a lighterweight implementation of immutable strings, used
for holding longer string properties in ZVal_ZooLib instances. */

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

} // anonymous namespace

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
	ZMemCopy(fBuffer, iOther.fBuffer, fSize);
	}

ValString::ValString(const string& iOther)
:	fSize(iOther.size())
	{
	fBuffer = new char[fSize];
	if (fSize)
		ZMemCopy(fBuffer, iOther.data(), fSize);
	}

ValString::ValString(const char* iString, size_t iLength)
:	fSize(iLength)
	{
	fBuffer = new char[fSize];
	ZMemCopy(fBuffer, iString, fSize);
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
	{ return ZMemCompare(fBuffer, fSize, iOther.fBuffer, iOther.fSize); }

inline int ValString::Compare(const char* iString, size_t iSize) const
	{ return ZMemCompare(fBuffer, fSize, iString, iSize); }

inline int ValString::Compare(const string& iString) const
	{
	if (size_t otherSize = iString.size())
		return ZMemCompare(fBuffer, fSize, iString.data(), otherSize);

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
		ZMemCopy(const_cast<char*>(oString.data()), fBuffer, fSize);
	}

// =================================================================================================
// MARK: - ZVal_ZooLib::Ex_IllegalType

ZVal_ZooLib::Ex_IllegalType::Ex_IllegalType(int iType)
:	runtime_error("Ex_IllegalType"),
	fType(iType)
	{}

// =================================================================================================
// MARK: - ZVal_ZooLib

/** \class ZVal_ZooLib
\nosubgrouping
*/

bool ZVal_ZooLib::sQFromAny(const ZAny& iAny, ZVal_ZooLib& oVal)
	{
	if (false)
		{}
	else if (iAny.IsNull())
		{
		oVal = ZVal_ZooLib();
		}
	else if (const string* theValue = iAny.PGet<string>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZType* theValue = iAny.PGet<ZType>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const uint64* theValue = iAny.PGet<uint64>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const char* theValue = iAny.PGet<char>())
		{
		oVal = ZVal_ZooLib(int8(*theValue));
		}
	else if (const unsigned char* theValue = iAny.PGet<unsigned char>())
		{
		oVal = ZVal_ZooLib(int8(*theValue));
		}
	else if (const signed char* theValue = iAny.PGet<signed char>())
		{
		oVal = ZVal_ZooLib(int8(*theValue));
		}
	else if (const short* theValue = iAny.PGet<short>())
		{
		oVal = ZVal_ZooLib(int16(*theValue));
		}
	else if (const unsigned short* theValue = iAny.PGet<unsigned short>())
		{
		oVal = ZVal_ZooLib(int16(*theValue));
		}
	else if (const int* theValue = iAny.PGet<int>())
		{
		if (ZIntIs32Bit)
			oVal = ZVal_ZooLib(int32(*theValue));
		else
			oVal = ZVal_ZooLib(int64(*theValue));
		}
	else if (const unsigned int* theValue = iAny.PGet<unsigned int>())
		{
		if (ZIntIs32Bit)
			oVal = ZVal_ZooLib(int32(*theValue));
		else
			oVal = ZVal_ZooLib(int64(*theValue));
		}
	else if (const long* theValue = iAny.PGet<long>())
		{
		if (ZLongIs32Bit)
			oVal = ZVal_ZooLib(int32(*theValue));
		else
			oVal = ZVal_ZooLib(int64(*theValue));
		}
	else if (const unsigned long* theValue = iAny.PGet<unsigned long>())
		{
		if (ZLongIs32Bit)
			oVal = ZVal_ZooLib(int32(*theValue));
		else
			oVal = ZVal_ZooLib(int64(*theValue));
		}
	else if (const int64* theValue = iAny.PGet<int64>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const float* theValue = iAny.PGet<float>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const double* theValue = iAny.PGet<double>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZTime* theValue = iAny.PGet<ZTime>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const VoidStar_t* theValue = iAny.PGet<VoidStar_t>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZRectPOD* theValue = iAny.PGet<ZRectPOD>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZPointPOD* theValue = iAny.PGet<ZPointPOD>())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const ZRef<ZCounted>* theValue =
		iAny.PGet<ZRef<ZCounted> >())
		{
		oVal = ZVal_ZooLib(*theValue);
		}
	else if (const vector<char>* theValue = iAny.PGet<vector<char> >())
		{
		if (size_t theSize = theValue->size())
			oVal = ZData_ZooLib(&(*theValue)[0], theSize);
		else
			oVal = ZData_ZooLib();
		}
	else if (const ZData_Any* theValue = iAny.PGet<ZData_Any>())
		{
		// This relies on the fact that ZData_ZooLib is a typedef of ZData_Any.
		oVal = *theValue;
		}
	else if (const vector<ZAny>* theValue = iAny.PGet<vector<ZAny> >())
		{
		ZSeq_ZooLib theSeq;
		for (vector<ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sQFromAny(*i, local))
				theSeq.Append(local);
			else
				theSeq.Append(ZVal_ZooLib());
			}
		oVal = theSeq;
		}
	else if (const ZSeq_Any* theValue = iAny.PGet<ZSeq_Any>())
		{
		ZSeq_ZooLib theSeq;
		for (int x = 0, count = theValue->Count(); x < count; ++x)
			{
			ZVal_ZooLib local;
			if (sQFromAny(theValue->Get(x), local))
				theSeq.Append(local);
			else
				theSeq.Append(ZVal_ZooLib());
			}
		oVal = theSeq;
		}
	else if (const map<string, ZAny>* theValue = iAny.PGet<map<string, ZAny> >())
		{
		ZMap_ZooLib theMap;
		for (map<string, ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sQFromAny((*i).second, local))
				theMap.Set((*i).first, local);
			}
		oVal = theMap;
		}
	else if (const ZMap_Any* theValue = iAny.PGet<ZMap_Any>())
		{
		ZMap_ZooLib theMap;
		for (ZMap_Any::Index_t i = theValue->Begin(), end = theValue->End();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sQFromAny(theValue->Get(i), local))
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

ZVal_ZooLib ZVal_ZooLib::sDFromAny(const ZVal_ZooLib& iDefault, const ZAny& iAny)
	{
	ZVal_ZooLib result;
	if (sQFromAny(iAny, result))
		return result;
	return iDefault;
	}

ZVal_ZooLib ZVal_ZooLib::sFromAny(const ZAny& iAny)
	{
	ZVal_ZooLib result;
	if (sQFromAny(iAny, result))
		return result;
	return ZVal_ZooLib();
	}

ZAny ZVal_ZooLib::AsAny() const
	{ return this->DAsAny(ZAny()); }

ZAny ZVal_ZooLib::DAsAny(const ZAny& iDefault) const
	{
	switch (fType.fType)
		{
		case eZType_Null:
			{
			return ZAny();
			}
		case eZType_Type:
			{
			return ZAny(fData.fAs_Type);
			}
		case eZType_ID:
			{
			return ZAny(fData.fAs_ID);
			}
		case eZType_Int8:
			{
			return ZAny(fData.fAs_Int8);
			}
		case eZType_Int16:
			{
			return ZAny(fData.fAs_Int16);
			}
		case eZType_Int32:
			{
			return ZAny(fData.fAs_Int32);
			}
		case eZType_Int64:
			{
			return ZAny(fData.fAs_Int64);
			}
		case eZType_Bool:
			{
			return ZAny(fData.fAs_Bool);
			}
		case eZType_Float:
			{
			return ZAny(fData.fAs_Float);
			}
		case eZType_Double:
			{
			return ZAny(fData.fAs_Double);
			}
		case eZType_Time:
			{
			return ZAny(ZTime(fData.fAs_Time));
			}
		case eZType_Pointer:
			{
			return ZAny(fData.fAs_Pointer);
			}
		case eZType_Rect:
			{
			return ZAny(fData.fAs_Rect);
			}
		case eZType_Point:
			{
			return ZAny(fData.fAs_Point);
			}
		case eZType_String:
			{
			return ZAny(sFetch_T<ValString>(fType.fBytes)->AsString());
			}
#if 0//##
		case eZType_Name:
			{
			sFetch_T<ZTName>(fType.fBytes)->ToStream(iStreamW));
			break;
			}
#endif//##
		case eZType_Tuple:
			{
			return ZAny(sFetch_T<ZMap_ZooLib>(fType.fBytes)->AsMap_Any(iDefault));
			}
		case eZType_RefCounted:
			{
			// Just do nothing. We'll construct a nil refcounted when we read.
			break;
			}
		case eZType_Raw:
			{
			return sFetch_T<ZData_ZooLib>(fType.fBytes)->AsAny();
			}
		case eZType_Vector:
			{
			return ZAny(sFetch_T<ZSeq_ZooLib>(fType.fBytes)->AsSeq_Any(iDefault));
			}
		default:
			{
			if (fType.fType < 0)
				return ZAny(string(fType.fBytes, -fType.fType-1));
			}
		}
	return iDefault;
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
	sCtor_T<ZSeq_ZooLib>(fType.fBytes, 1, iVal);
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
	if (spIsSpecialString(theSize))
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZMemCopy(fType.fBytes, iVal, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sCtor_T<ValString>(fType.fBytes, iVal, theSize);
		}
	}

ZVal_ZooLib::ZVal_ZooLib(const string& iVal)
	{
	size_t theSize = iVal.size();
	if (spIsSpecialString(theSize))
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZMemCopy(fType.fBytes, iVal.data(), theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sCtor_T<ValString>(fType.fBytes, iVal);
		}
	}

ZVal_ZooLib::ZVal_ZooLib(const ZData_ZooLib& iVal)
	{
	fType.fType = eZType_Raw;
	sCtor_T<ZData_ZooLib, ZData_ZooLib>(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZSeq_ZooLib& iVal)
	{
	fType.fType = eZType_Vector;
	sCtor_T<ZSeq_ZooLib, ZSeq_ZooLib>(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZMap_ZooLib& iVal)
	{
	fType.fType = eZType_Tuple;
	sCtor_T<ZMap_ZooLib, ZMap_ZooLib>(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZRef<ZCounted>& iVal)
	{
	fType.fType = eZType_RefCounted;
	sCtor_T<ZRef<ZCounted>, ZRef<ZCounted> >(fType.fBytes, iVal);
	}

ZVal_ZooLib::ZVal_ZooLib(const void* iSource, size_t iSize)
	{
	fType.fType = eZType_Raw;
	sCtor_T<ZData_ZooLib>(fType.fBytes, iSource, iSize);
	}

ZVal_ZooLib::ZVal_ZooLib(const ZStreamR& iStreamR, size_t iSize)
	{
	fType.fType = eZType_Raw;
	ZData_ZooLib* theRaw = sCtor_T<ZData_ZooLib>(fType.fBytes);
	sRead_T(*theRaw, iStreamR, iSize);
	}

void ZVal_ZooLib::Clear()
	{
	this->pRelease();
	fType.fType = eZType_Null;
	}

template <>
ZQ<ZType> ZVal_ZooLib::QGet<ZType>() const
	{
	if (fType.fType == eZType_Type)
		return fData.fAs_Type;
	return null;
	}

template <>
ZQ<uint64> ZVal_ZooLib::QGet<uint64>() const
	{
	if (fType.fType == eZType_ID)
		return fData.fAs_ID;
	return null;
	}

template <>
ZQ<int8> ZVal_ZooLib::QGet<int8>() const
	{
	if (fType.fType == eZType_Int8)
		return fData.fAs_Int8;
	return null;
	}

template <>
ZQ<int16> ZVal_ZooLib::QGet<int16>() const
	{
	if (fType.fType == eZType_Int16)
		return fData.fAs_Int16;
	return null;
	}

template <>
ZQ<int32> ZVal_ZooLib::QGet<int32>() const
	{
	if (fType.fType == eZType_Int32)
		return fData.fAs_Int32;
	return null;
	}

template <>
ZQ<int64> ZVal_ZooLib::QGet<int64>() const
	{
	if (fType.fType == eZType_Int64)
		return fData.fAs_Int64;
	return null;
	}

template <>
ZQ<bool> ZVal_ZooLib::QGet<bool>() const
	{
	if (fType.fType == eZType_Bool)
		return fData.fAs_Bool;
	return null;
	}

template <>
ZQ<float> ZVal_ZooLib::QGet<float>() const
	{
	if (fType.fType == eZType_Float)
		return fData.fAs_Float;
	return null;
	}

template <>
ZQ<double> ZVal_ZooLib::QGet<double>() const
	{
	if (fType.fType == eZType_Double)
		return fData.fAs_Double;
	return null;
	}

template <>
ZQ<ZTime> ZVal_ZooLib::QGet<ZTime>() const
	{
	if (fType.fType == eZType_Time)
		return fData.fAs_Time;
	return null;
	}

template <>
ZQ<void*> ZVal_ZooLib::QGet<void*>() const
	{
	if (fType.fType == eZType_Pointer)
		return fData.fAs_Pointer;
	return null;
	}

template <>
ZQ<ZRectPOD> ZVal_ZooLib::QGet<ZRectPOD>() const
	{
	if (fType.fType == eZType_Rect)
		return *fData.fAs_Rect;
	return null;
	}

template <>
ZQ<ZPointPOD> ZVal_ZooLib::QGet<ZPointPOD>() const
	{
	if (fType.fType == eZType_Point)
		return fData.fAs_Point;
	return null;
	}

template <>
ZQ<string> ZVal_ZooLib::QGet<string>() const
	{
	if (fType.fType == eZType_String)
		return sFetch_T<ValString>(fType.fBytes)->AsString();
	else if (fType.fType < 0)
		return string(fType.fBytes, -fType.fType-1);
	return null;
	}

template <>
ZQ<ZData_ZooLib> ZVal_ZooLib::QGet<ZData_ZooLib>() const
	{
	if (fType.fType == eZType_Raw)
		return *sFetch_T<ZData_ZooLib>(fType.fBytes);
	return null;
	}

template <>
ZQ<ZSeq_ZooLib> ZVal_ZooLib::QGet<ZSeq_ZooLib>() const
	{
	if (fType.fType == eZType_Vector)
		return *sFetch_T<ZSeq_ZooLib>(fType.fBytes);
	return null;
	}

template <>
ZQ<ZMap_ZooLib> ZVal_ZooLib::QGet<ZMap_ZooLib>() const
	{
	if (fType.fType == eZType_Tuple)
		return *sFetch_T<ZMap_ZooLib>(fType.fBytes);
	return null;
	}

template <>
ZQ<ZRef<ZCounted> > ZVal_ZooLib::QGet<ZRef<ZCounted> >() const
	{
	if (fType.fType == eZType_RefCounted)
		return *sFetch_T<ZRef<ZCounted> >(fType.fBytes);
	return null;
	}

template <>
void ZVal_ZooLib::Set<ZType>(const ZType& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Type;
	fData.fAs_Type = iVal;
	}

template <>
void ZVal_ZooLib::Set<uint64>(const uint64& iVal)
	{
	this->pRelease();
	fType.fType = eZType_ID;
	fData.fAs_ID = iVal;
	}

template <>
void ZVal_ZooLib::Set<int8>(const int8& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int8;
	fData.fAs_Int8 = iVal;
	}

template <>
void ZVal_ZooLib::Set<int16>(const int16& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int16;
	fData.fAs_Int16 = iVal;
	}

template <>
void ZVal_ZooLib::Set<int32>(const int32& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int32;
	fData.fAs_Int32 = iVal;
	}

template <>
void ZVal_ZooLib::Set<int64>(const int64& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Int64;
	fData.fAs_Int64 = iVal;
	}

template <>
void ZVal_ZooLib::Set<bool>(const bool& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Bool;
	fData.fAs_Bool = iVal;
	}

template <>
void ZVal_ZooLib::Set<float>(const float& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Float;
	fData.fAs_Float = iVal;
	}

template <>
void ZVal_ZooLib::Set<double>(const double& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Double;
	fData.fAs_Double = iVal;
	}

template <>
void ZVal_ZooLib::Set<ZTime>(const ZTime& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Time;
	fData.fAs_Time = iVal.fVal;
	}

template <>
void ZVal_ZooLib::Set<VoidStar_t>(const VoidStar_t& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Pointer;
	fData.fAs_Pointer = iVal;
	}

template <>
void ZVal_ZooLib::Set<ZRectPOD>(const ZRectPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Rect;
	fData.fAs_Rect = new ZRectPOD(iVal);
	}

template <>
void ZVal_ZooLib::Set<ZPointPOD>(const ZPointPOD& iVal)
	{
	this->pRelease();
	fType.fType = eZType_Point;
	fData.fAs_Point = iVal;
	}

typedef const char* ConstCharStar_t;
template <>
void ZVal_ZooLib::Set<ConstCharStar_t>(const ConstCharStar_t& iVal)
	{
	this->pRelease();
	size_t theSize = strlen(iVal);
	if (spIsSpecialString(theSize))
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZMemCopy(fType.fBytes, iVal, theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sCtor_T<ValString>(fType.fBytes, iVal, theSize);
		}
	}

template <>
void ZVal_ZooLib::Set<string>(const string& iVal)
	{
	this->pRelease();
	size_t theSize = iVal.size();
	if (spIsSpecialString(theSize))
		{
		fType.fType = -int(theSize)-1;
		if (theSize)
			ZMemCopy(fType.fBytes, iVal.data(), theSize);
		}
	else
		{
		fType.fType = eZType_String;
		sCtor_T<ValString>(fType.fBytes, iVal);
		}
	}

template <>
void ZVal_ZooLib::Set<ZData_ZooLib>(const ZData_ZooLib& iVal)
	{
	this->pRelease();
	sCtor_T<ZData_ZooLib, ZData_ZooLib>(fType.fBytes, iVal);
	fType.fType = eZType_Raw;
	}

template <>
void ZVal_ZooLib::Set<ZSeq_ZooLib>(const ZSeq_ZooLib& iVal)
	{
	this->pRelease();
	sCtor_T<ZSeq_ZooLib, ZSeq_ZooLib>(fType.fBytes, iVal);
	fType.fType = eZType_Vector;
	}

template <>
void ZVal_ZooLib::Set<ZMap_ZooLib>(const ZMap_ZooLib& iVal)
	{
	this->pRelease();
	sCtor_T<ZMap_ZooLib, ZMap_ZooLib>(fType.fBytes, iVal);
	fType.fType = eZType_Tuple;
	}

template <>
void ZVal_ZooLib::Set<ZRef<ZCounted> >(const ZRef<ZCounted>& iVal)
	{
	this->pRelease();
	fType.fType = eZType_RefCounted;
	sCtor_T<ZRef<ZCounted>, ZRef<ZCounted> >(fType.fBytes, iVal);
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
			return ZMemCompare(fType.fBytes, -fType.fType-1,
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

ZSeq_ZooLib& ZVal_ZooLib::MutableSeq()
	{
	if (fType.fType != eZType_Vector)
		{
		this->pRelease();
		sCtor_T<ZSeq_ZooLib>(fType.fBytes);
		fType.fType = eZType_Vector;
		}
	return *sFetch_T<ZSeq_ZooLib>(fType.fBytes);
	}

ZMap_ZooLib& ZVal_ZooLib::MutableMap()
	{
	if (fType.fType != eZType_Tuple)
		{
		this->pRelease();
		sCtor_T<ZMap_ZooLib>(fType.fBytes);
		fType.fType = eZType_Tuple;
		}
	return *sFetch_T<ZMap_ZooLib>(fType.fBytes);
	}

ZType ZVal_ZooLib::TypeOf() const
	{ return fType.fType < 0 ? eZType_String : ZType(fType.fType); }

void ZVal_ZooLib::ToStream(const ZStreamW& iStreamW) const
	{
	if (fType.fType < 0)
		{
		iStreamW.WriteUInt8(eZType_String);
		size_t theSize = -fType.fType-1;
		ZAssertStop(kDebug_Tuple, spIsSpecialString(theSize));
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
			sFetch_T<ZMap_ZooLib>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
		case eZType_RefCounted:
			{
			// Just do nothing. We'll construct a nil refcounted when we read.
			break;
			}
		case eZType_Raw:
			{
			const ZData_ZooLib* theMemoryBlock = sFetch_T<ZData_ZooLib>(fType.fBytes);
			iStreamW.WriteCount(theMemoryBlock->GetSize());
			if (theMemoryBlock->GetSize())
				iStreamW.Write(theMemoryBlock->GetPtr(), theMemoryBlock->GetSize());
			break;
			}
		case eZType_Vector:
			{
			sFetch_T<ZSeq_ZooLib>(fType.fBytes)->ToStream(iStreamW);
			break;
			}
		default:
			{
			ZDebugStopf(kDebug_Tuple, ("Unknown type (%d)", fType.fType));
			}
		}
	}

// =================================================================================================
// MARK: - ZVal_ZooLib typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_ZooLib)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, ID, uint64)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Type, ZType)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Time, ZTime)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Rect, ZRectPOD)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Point, ZPointPOD)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, RefCounted, ZRef<ZCounted>)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Pointer, VoidStar_t)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Data, ZData_ZooLib)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Seq, ZSeq_ZooLib)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Map, ZMap_ZooLib)

// Backwards compatibility
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Raw, ZData_ZooLib)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_ZooLib, Tuple, ZMap_ZooLib)

// =================================================================================================
// MARK: - ZVal_ZooLib internal implementation

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
		case eZType_Float: return sCompare_T(fData.fAs_Float, iOther.fData.fAs_Float);
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
			return sFetch_T<ZMap_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZMap_ZooLib>(iOther.fType.fBytes));
			}
		case eZType_RefCounted:
			{
			const ZRef<ZCounted>* thisZRef =
				sFetch_T<ZRef<ZCounted> >(fType.fBytes);

			const ZRef<ZCounted>* otherZRef =
				sFetch_T<ZRef<ZCounted> >(iOther.fType.fBytes);

			if (*thisZRef < *otherZRef)
				return -1;
			else if (*otherZRef < *thisZRef)
				return 1;
			return 0;
			}
		case eZType_Raw:
			{
			return sFetch_T<ZData_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZData_ZooLib>(iOther.fType.fBytes));
			}
		case eZType_Vector:
			{
			return sFetch_T<ZSeq_ZooLib>(fType.fBytes)
				->Compare(*sFetch_T<ZSeq_ZooLib>(iOther.fType.fBytes));
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
			return *sFetch_T<ZMap_ZooLib>(fType.fBytes) < *sFetch_T<ZMap_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZCounted> >(fType.fBytes)
				< *sFetch_T<ZRef<ZCounted> >(iOther.fType.fBytes);
			}
		case eZType_Raw:
			{
			return *sFetch_T<ZData_ZooLib>(fType.fBytes)
				< *sFetch_T<ZData_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			return *sFetch_T<ZSeq_ZooLib>(fType.fBytes)
				< *sFetch_T<ZSeq_ZooLib>(iOther.fType.fBytes);
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
			return *sFetch_T<ZMap_ZooLib>(fType.fBytes)
				== *sFetch_T<ZMap_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_RefCounted:
			{
			return *sFetch_T<ZRef<ZCounted> >(fType.fBytes)
				== *sFetch_T<ZRef<ZCounted> >(iOther.fType.fBytes);
			}
		case eZType_Raw:
			{
			return *sFetch_T<ZData_ZooLib>(fType.fBytes)
				== *sFetch_T<ZData_ZooLib>(iOther.fType.fBytes);
			}
		case eZType_Vector:
			{
			return *sFetch_T<ZSeq_ZooLib>(fType.fBytes)
				== *sFetch_T<ZSeq_ZooLib>(iOther.fType.fBytes);
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

		case eZType_String: sDtor_T<ValString>(fType.fBytes); break;
//##		case eZType_Name: sDtor_T<ZTName>(fType.fBytes); break;
		case eZType_Tuple: sDtor_T<ZMap_ZooLib>(fType.fBytes); break;
		case eZType_RefCounted:
			{
			sDtor_T<ZRef<ZCounted> >(fType.fBytes);
			break;
			}
		case eZType_Raw: sDtor_T<ZData_ZooLib>(fType.fBytes); break;
		case eZType_Vector: sDtor_T<ZSeq_ZooLib>(fType.fBytes); break;
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
				sCtorFromVoidStar_T<ValString>(fType.fBytes, iOther.fType.fBytes);
				break;
#if 0//##
			case eZType_Name:
				sCtorFromVoidStar_T<ZTName>(fType.fBytes, iOther.fType.fBytes);
				break;
#endif//##
			case eZType_Tuple:
				sCtorFromVoidStar_T<ZMap_ZooLib>(fType.fBytes, iOther.fType.fBytes);
				break;
			case eZType_RefCounted:
				sCtorFromVoidStar_T<ZRef<ZCounted> >
					(fType.fBytes, iOther.fType.fBytes);
				break;
			case eZType_Raw:
				sCtorFromVoidStar_T<ZData_ZooLib>(fType.fBytes, iOther.fType.fBytes);
				break;
			case eZType_Vector:
				sCtorFromVoidStar_T<ZSeq_ZooLib>(fType.fBytes, iOther.fType.fBytes);
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
			if (spIsSpecialString(theSize))
				{
				iType = (ZType)(-int(theSize)-1);
				if (theSize)
					iStreamR.Read(fType.fBytes, theSize);
				}
			else
				{
				sCtor_T<ValString>(fType.fBytes, iStreamR, theSize);
				}
			break;
			}
#if 0//##
		case eZType_Name:
			{
			sCtor_T<ZTName>(fType.fBytes, iStreamR);
			break;
			}
#endif//##
		case eZType_Tuple:
			{
			sCtor_T<ZMap_ZooLib>(fType.fBytes, iStreamR);
			break;
			}
		case eZType_RefCounted:
			{
			// Construct a nil refcounted.
			sCtor_T<ZRef<ZCounted> >(fType.fBytes);
			break;
			}
		case eZType_Raw:
			{
			uint32 size = iStreamR.ReadCount();
			ZData_ZooLib* theRaw = sCtor_T<ZData_ZooLib>(fType.fBytes, size);
			if (size)
				{
				try
					{
					iStreamR.Read(theRaw->GetPtrMutable(), size);
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
			sCtor_T<ZSeq_ZooLib>(fType.fBytes, iStreamR);
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
// MARK: - ZSeq_ZooLib::Rep

class ZSeq_ZooLib::Rep : public ZCountedWithoutFinalize
	{
public:
	Rep();
	Rep(const std::vector<ZVal_ZooLib>& iOther);
	virtual ~Rep();

	std::vector<ZVal_ZooLib> fVector;
	};

ZSeq_ZooLib::Rep::Rep()
	{}

ZSeq_ZooLib::Rep::Rep(const vector<ZVal_ZooLib>& iOther)
:	fVector(iOther)
	{}

ZSeq_ZooLib::Rep::~Rep()
	{}

// =================================================================================================
// MARK: - ZSeq_ZooLib

ZSeq_Any ZSeq_ZooLib::AsSeq_Any(const ZAny& iDefault) const
	{
	ZSeq_Any theSeq;
	if (fRep)
		{
		if (size_t theCount = fRep->fVector.size())
			{
			for (size_t x = 0; x < theCount; ++x)
				theSeq.Append(fRep->fVector[x].DAsAny(iDefault));
			}
		}
	return theSeq;
	}

ZSeq_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fVector.empty()); }

ZSeq_ZooLib::ZSeq_ZooLib()
	{}

ZSeq_ZooLib::ZSeq_ZooLib(const ZSeq_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

ZSeq_ZooLib::~ZSeq_ZooLib()
	{}

ZSeq_ZooLib& ZSeq_ZooLib::operator=(const ZSeq_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZSeq_ZooLib::ZSeq_ZooLib(const ZStreamR& iStreamR)
	{
	if (uint32 theCount = iStreamR.ReadCount())
		{
		fRep = new Rep;
		fRep->fVector.reserve(theCount);
		for (size_t x = 0; x < theCount; ++x)
			fRep->fVector.push_back(ZVal_ZooLib(iStreamR));
		}
	}

ZSeq_ZooLib::ZSeq_ZooLib(size_t iCount, const ZVal_ZooLib& iSingleton)
:	fRep(new Rep(vector<ZVal_ZooLib>(iCount, iSingleton)))
	{}

ZSeq_ZooLib::ZSeq_ZooLib(const vector<ZVal_ZooLib>& iOther)
:	fRep(new Rep(vector<ZVal_ZooLib>(iOther)))
	{}

size_t ZSeq_ZooLib::Count() const
	{
	if (fRep)
		return fRep->fVector.size();
	return 0;
	}

void ZSeq_ZooLib::Clear()
	{ fRep.Clear(); }

bool ZSeq_ZooLib::QGet(size_t iIndex, ZVal_ZooLib& oVal) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		{
		oVal = fRep->fVector.at(iIndex);
		return true;
		}
	return false;
	}

ZVal_ZooLib ZSeq_ZooLib::DGet(const ZVal_ZooLib& iDefault, size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return iDefault;
	}

ZVal_ZooLib ZSeq_ZooLib::Get(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return spNilVal;
	}

const ZVal_ZooLib& ZSeq_ZooLib::RGet(size_t iIndex) const
	{
	if (fRep && iIndex < fRep->fVector.size())
		return fRep->fVector.at(iIndex);
	return spNilVal;
	}

ZSeq_ZooLib& ZSeq_ZooLib::Set(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex < theVec.size())
		*(theVec.begin() + iIndex) = iVal;
	return *this;
	}

ZSeq_ZooLib& ZSeq_ZooLib::Erase(size_t iIndex)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex < theVec.size())
		theVec.erase(theVec.begin() + iIndex);
	return *this;
	}

ZSeq_ZooLib& ZSeq_ZooLib::Insert(size_t iIndex, const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	vector<ZVal_ZooLib>& theVec = fRep->fVector;
	if (iIndex <= theVec.size())
		theVec.insert(theVec.begin() + iIndex, iVal);
	return *this;
	}

ZSeq_ZooLib& ZSeq_ZooLib::Append(const ZVal_ZooLib& iVal)
	{
	this->pTouch();
	fRep->fVector.push_back(iVal);
	return *this;
	}

int ZSeq_ZooLib::Compare(const ZSeq_ZooLib& iOther) const
	{
	if (fRep)
		{
		if (iOther.fRep)
			{
			return sCompare_T(fRep->fVector, iOther.fRep->fVector);
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

bool ZSeq_ZooLib::operator==(const ZSeq_ZooLib& iOther) const
	{ return this->Compare(iOther) == 0; }

bool ZSeq_ZooLib::operator<(const ZSeq_ZooLib& iOther) const
	{ return this->Compare(iOther) < 0; }

ZVal_ZooLib& ZSeq_ZooLib::Mutable(size_t iIndex)
	{
	ZAssert(fRep && iIndex < fRep->fVector.size());
	return fRep->fVector.at(iIndex);
	}

vector<ZVal_ZooLib>& ZSeq_ZooLib::MutableVector()
	{
	this->pTouch();
	return fRep->fVector;
	}

const vector<ZVal_ZooLib>& ZSeq_ZooLib::GetVector() const
	{
	if (fRep)
		return fRep->fVector;
	return spNilVector;
	}

void ZSeq_ZooLib::ToStream(const ZStreamW& iStreamW) const
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

void ZSeq_ZooLib::pTouch()
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = new Rep(fRep->fVector);
		}
	else
		{
		fRep = new Rep;
		}
	}

// =================================================================================================
// MARK: - NameVal inlines

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
// MARK: - ZMap_ZooLib::Rep

class ZMap_ZooLib::Rep : public ZCountedWithoutFinalize
	{
public:
	Rep();
	Rep(const ZMap_ZooLib::PropList& iProperties);
	virtual ~Rep();

	ZMap_ZooLib::PropList fProperties;
	};

ZMap_ZooLib::Rep::Rep()
	{}

ZMap_ZooLib::Rep::Rep(const ZMap_ZooLib::PropList& iProperties)
:	fProperties(iProperties)
	{}

ZMap_ZooLib::Rep::~Rep()
	{}

// =================================================================================================
// MARK: - ZMap_ZooLib

ZMap_Any ZMap_ZooLib::AsMap_Any(const ZAny& iDefault) const
	{
	ZMap_Any theMap;
	if (fRep)
		{
		for (Index_t i = fRep->fProperties.begin(), end = fRep->fProperties.end();
			i != end; ++i)
			{
			theMap.Set((*i).fName.AsString(), (*i).fVal.DAsAny(iDefault));
			}
		}
	return theMap;
	}

ZMap_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && !fRep->fProperties.empty()); }

ZMap_ZooLib::ZMap_ZooLib()
	{}

ZMap_ZooLib::ZMap_ZooLib(const ZMap_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

ZMap_ZooLib::~ZMap_ZooLib()
	{}

ZMap_ZooLib& ZMap_ZooLib::operator=(const ZMap_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZMap_ZooLib::ZMap_ZooLib(const ZStreamR& iStreamR)
:	fRep(sRepFromStream(iStreamR))
	{}

ZMap_ZooLib::ZMap_ZooLib(ZRef<Rep> iRep)
:	fRep(iRep)
	{}

void ZMap_ZooLib::Clear()
	{ fRep.Clear(); }

ZVal_ZooLib* ZMap_ZooLib::PGet(Index_t iIndex)
	{
	if (fRep && iIndex != fRep->fProperties.end())
		{
		iIndex = this->pTouch(iIndex);
		return &(*iIndex).fVal;
		}
	return nullptr;
	}

ZVal_ZooLib* ZMap_ZooLib::PGet(const char* iPropName)
	{
	this->pTouch();

	ZAssertStop(kDebug_Tuple, fRep);

	size_t propNameLength = strlen(iPropName);
	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName, propNameLength))
			return &(*i).fVal;
		}

	return nullptr;
	}

ZVal_ZooLib* ZMap_ZooLib::PGet(const ZTName& iPropName)
	{
	this->pTouch();

	ZAssertStop(kDebug_Tuple, fRep);

	for (PropList::iterator i = fRep->fProperties.begin(),
		theEnd = fRep->fProperties.end();
		i != theEnd; ++i)
		{
		if ((*i).fName.Equals(iPropName))
			return &(*i).fVal;
		}

	return nullptr;
	}

const ZVal_ZooLib* ZMap_ZooLib::PGet(Index_t iIndex) const
	{
	if (fRep && iIndex != fRep->fProperties.end())
		return &(*iIndex).fVal;
	return nullptr;
	}

const ZVal_ZooLib* ZMap_ZooLib::PGet(const char* iPropName) const
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

const ZVal_ZooLib* ZMap_ZooLib::PGet(const ZTName& iPropName) const
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

bool ZMap_ZooLib::QGet(Index_t iIndex, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iIndex))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

bool ZMap_ZooLib::QGet(const char* iPropName, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

bool ZMap_ZooLib::QGet(const ZTName& iPropName, ZVal_ZooLib& oVal) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		{
		oVal = *theValue;
		return true;
		}
	return false;
	}

ZVal_ZooLib ZMap_ZooLib::DGet(const ZVal_ZooLib& iDefault, Index_t iIndex) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iIndex))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZMap_ZooLib::DGet(const ZVal_ZooLib& iDefault, const char* iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZMap_ZooLib::DGet(const ZVal_ZooLib& iDefault, const ZTName& iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		return *theValue;
	return iDefault;
	}

ZVal_ZooLib ZMap_ZooLib::Get(Index_t iIndex) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iIndex))
		return *theValue;
	return spNilVal;
	}

ZVal_ZooLib ZMap_ZooLib::Get(const char* iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		return *theValue;
	return spNilVal;
	}

ZVal_ZooLib ZMap_ZooLib::Get(const ZTName& iPropName) const
	{
	if (const ZVal_ZooLib* theValue = this->PGet(iPropName))
		return *theValue;
	return spNilVal;
	}

ZMap_ZooLib& ZMap_ZooLib::Set(Index_t iIndex, const ZVal_ZooLib& iVal)
	{
	this->pSet(iIndex, iVal);
	return *this;
	}

ZMap_ZooLib& ZMap_ZooLib::Set(const char* iPropName, const ZVal_ZooLib& iVal)
	{
	this->pSet(iPropName, iVal);
	return *this;
	}

ZMap_ZooLib& ZMap_ZooLib::Set(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{
	this->pSet(iPropName, iVal);
	return *this;
	}

ZMap_ZooLib& ZMap_ZooLib::Erase(Index_t iIndex)
	{
	if (fRep)
		this->pErase(iIndex);
	return *this;
	}

ZMap_ZooLib& ZMap_ZooLib::Erase(const char* iPropName)
	{
	if (fRep)
		this->pErase(this->IndexOf(iPropName));
	return *this;
	}

ZMap_ZooLib& ZMap_ZooLib::Erase(const ZTName& iPropName)
	{
	if (fRep)
		this->pErase(this->IndexOf(iPropName));
	return *this;
	}

int ZMap_ZooLib::Compare(const ZMap_ZooLib& iOther) const
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

	if (not iOther.fRep)
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

bool ZMap_ZooLib::operator==(const ZMap_ZooLib& iOther) const
	{ return this->Compare(iOther) == 0; }

bool ZMap_ZooLib::operator<(const ZMap_ZooLib& iOther) const
	{ return this->Compare(iOther) < 0; }

ZVal_ZooLib& ZMap_ZooLib::Mutable(Index_t iIndex)
	{
	ZAssert(fRep && iIndex != fRep->fProperties.end());
	iIndex = this->pTouch(iIndex);
	return (*iIndex).fVal;
	}

ZVal_ZooLib& ZMap_ZooLib::Mutable(const char* iPropName)
	{ return *this->pFindOrAllocate(iPropName); }

ZVal_ZooLib& ZMap_ZooLib::Mutable(const ZTName& iPropName)
	{ return *this->pFindOrAllocate(iPropName); }

ZMap_ZooLib::Index_t ZMap_ZooLib::Begin() const
	{
	if (fRep)
		return fRep->fProperties.begin();
	return spEmptyProperties.end();
	}

ZMap_ZooLib::Index_t ZMap_ZooLib::End() const
	{
	if (fRep)
		return fRep->fProperties.end();
	return spEmptyProperties.end();
	}

bool ZMap_ZooLib::Empty() const
	{ return !fRep || fRep->fProperties.empty(); }

size_t ZMap_ZooLib::Count() const
	{
	if (fRep)
		return fRep->fProperties.size();
	return 0;
	}

const ZTName& ZMap_ZooLib::NameOf(Index_t iIndex) const
	{
	if (fRep && iIndex != fRep->fProperties.end())
		return (*iIndex).fName;
	return spNilName;
	}

ZMap_ZooLib::Index_t ZMap_ZooLib::IndexOf(const char* iPropName) const
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
	return spEmptyProperties.end();
	}

ZMap_ZooLib::Index_t ZMap_ZooLib::IndexOf(const ZTName& iPropName) const
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
	return spEmptyProperties.end();
	}

ZMap_ZooLib::Index_t ZMap_ZooLib::IndexOf
	(const ZMap_ZooLib& iOther, const Index_t& iOtherIndex) const
	{
	if (fRep == iOther.fRep)
		return iOtherIndex;

	return this->IndexOf(iOther.NameOf(iOtherIndex));
	}

bool ZMap_ZooLib::Has(const char* iPropName) const
	{ return this->PGet(iPropName); }

bool ZMap_ZooLib::Has(const ZTName& iPropName) const
	{ return this->PGet(iPropName); }

void ZMap_ZooLib::ToStream(const ZStreamW& iStreamW) const
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
// MARK: - ZMap_ZooLib internal implementation

void ZMap_ZooLib::pSet(Index_t iIndex, const ZVal_ZooLib& iVal)
	{
	if (ZVal_ZooLib* theVal = this->PGet(iIndex))
		*theVal = iVal;
	}

void ZMap_ZooLib::pSet(const char* iPropName, const ZVal_ZooLib& iVal)
	{ *pFindOrAllocate(iPropName) = iVal; }

void ZMap_ZooLib::pSet(const ZTName& iPropName, const ZVal_ZooLib& iVal)
	{ *pFindOrAllocate(iPropName) = iVal; }

ZVal_ZooLib* ZMap_ZooLib::pFindOrAllocate(const char* iPropName)
	{
	if (ZVal_ZooLib* theVal = this->PGet(iPropName))
		return theVal;

	fRep->fProperties.push_back(NameVal(iPropName));
	return &fRep->fProperties.back().fVal;
	}

ZVal_ZooLib* ZMap_ZooLib::pFindOrAllocate(const ZTName& iPropName)
	{
	if (ZVal_ZooLib* theVal = this->PGet(iPropName))
		return theVal;

	fRep->fProperties.push_back(NameVal(iPropName));
	return &fRep->fProperties.back().fVal;
	}

void ZMap_ZooLib::pErase(Index_t iIndex)
	{
	if (iIndex == fRep->fProperties.end())
		return;

	if (!fRep->IsShared())
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

void ZMap_ZooLib::pTouch()
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = new Rep(fRep->fProperties);
		}
	else
		{
		fRep = new Rep;
		}
	}

ZMap_ZooLib::Index_t ZMap_ZooLib::pTouch(Index_t iIndex)
	{
	ZAssertStop(kDebug_Tuple, fRep);

	if (!fRep->IsShared())
		return iIndex;

	size_t index = iIndex - fRep->fProperties.begin();
	fRep = new Rep(fRep->fProperties);
	return fRep->fProperties.begin() + index;
	}

ZRef<ZMap_ZooLib::Rep> ZMap_ZooLib::sRepFromStream(const ZStreamR& iStreamR)
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

} // namespace ZooLib
