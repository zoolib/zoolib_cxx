// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Data_ZZ_h__
#define __ZooLib_Data_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/Compare_T.h"
#include "zoolib/Util_Relops.h"
#include "zoolib/StdInt.h" // For byte
#include "zoolib/ZP.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Data_ZZ

class Data_ZZ
	{
	class Rep;

public:
	Data_ZZ();
	Data_ZZ(const Data_ZZ& iOther);
	~Data_ZZ();
	Data_ZZ& operator=(const Data_ZZ& iOther);

	Data_ZZ(size_t iSize);
	Data_ZZ(const void* iSourceData, size_t iSize);

	Data_ZZ(std::vector<byte>&& rVector);

	int Compare(const Data_ZZ& iOther) const;
	bool operator<(const Data_ZZ& iOther) const;
	bool operator==(const Data_ZZ& iOther) const;

	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetPtr() const;
	void* GetPtrMutable();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* oDest, size_t iCount) const;
	void CopyTo(void* oDest, size_t iCount) const;

private:
	void pTouch();

	ZP<Rep> fRep;
	};

template <> struct RelopsTraits_HasEQ<Data_ZZ> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Data_ZZ> : public RelopsTraits_Has {};

template <> inline int sCompare_T(const Data_ZZ& iL, const Data_ZZ& iR)
	{ return iL.Compare(iR); }

// =================================================================================================
#pragma mark - Data_ZZ pseudo-ctor from PaC

template <class T>
Data_ZZ sData_ZZ(const PaC<T>& iPaC)
	{ return Data_ZZ(sPointer(iPaC), sizeof(T) * sCount(iPaC)); }

template <>
inline Data_ZZ sData_ZZ<void>(const PaC<void>& iPaC)
	{ return Data_ZZ(sPointer(iPaC), sCount(iPaC)); }

template <>
inline Data_ZZ sData_ZZ<const void>(const PaC<const void>& iPaC)
	{ return Data_ZZ(sPointer(iPaC), sCount(iPaC)); }

// =================================================================================================
#pragma mark - sPac pseudo-ctor from Data_ZZ

template <class T>
PaC<T> sPaC(Data_ZZ& iData)
	{ return sPaC<T>(static_cast<T*>(iData.GetPtrMutable()), iData.GetSize() / sizeof(T)); }

template <class T>
PaC<const T> sPaC(const Data_ZZ& iData)
	{ return sPaC<const T>(static_cast<const T*>(iData.GetPtr()), iData.GetSize() / sizeof(T)); }

inline PaC<void> sPaC(Data_ZZ& iData)
	{ return sPaC<void>(iData.GetPtrMutable(), iData.GetSize()); }

inline PaC<const void> sPaC(const Data_ZZ& iData)
	{ return sPaC<const void>(iData.GetPtr(), iData.GetSize()); }

} // namespace ZooLib

#endif // __ZooLib_Data_ZZ_h__
