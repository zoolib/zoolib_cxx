// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Data_ZZ_h__
#define __ZooLib_Data_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Compare_T.h"
#include "zoolib/Util_Relops.h"
#include "zoolib/ZP.h"

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

} // namespace ZooLib

#endif // __ZooLib_Data_ZZ_h__
