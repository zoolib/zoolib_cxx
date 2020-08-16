// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Data_CF_h__
#define __ZooLib_Apple_Data_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Apple/ZP_CF.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Data_CF

class Data_CF
:	public ZP<CFDataRef>
	{
	typedef ZP<CFDataRef> inherited;
public:
	Data_CF();
	Data_CF(const Data_CF& iOther);
	~Data_CF();
	Data_CF& operator=(const Data_CF& iOther);

	Data_CF(CFMutableDataRef iOther);
	Data_CF(CFDataRef iOther);

	Data_CF(const Adopt_T<CFMutableDataRef>& iOther);
	Data_CF(const Adopt_T<CFDataRef>& iOther);

	Data_CF& operator=(CFMutableDataRef iOther);
	Data_CF& operator=(CFDataRef iOther);

	Data_CF& operator=(const Adopt_T<CFMutableDataRef>& iOther);
	Data_CF& operator=(const Adopt_T<CFDataRef>& iOther);

	Data_CF(size_t iSize);
	Data_CF(const void* iSource, size_t iSize);

// ZData protocol
	size_t GetSize() const;
	void SetSize(size_t iSize);

	const void* GetPtr() const;
	void* GetPtrMutable();

	void CopyFrom(size_t iOffset, const void* iSource, size_t iCount);
	void CopyFrom(const void* iSource, size_t iCount);

	void CopyTo(size_t iOffset, void* oDest, size_t iCount) const;
	void CopyTo(void* oDest, size_t iCount) const;

private:
	CFDataRef pData() const;
	CFMutableDataRef pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Data_CF_h__
