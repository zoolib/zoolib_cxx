// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Data_NS_h__
#define __ZooLib_Apple_Data_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Apple/ZP_NS.h"

#import <Foundation/NSData.h>

namespace ZooLib {

// =================================================================================================
#pragma mark - Data_NS

class Data_NS
:	public ZP<NSData>
	{
	typedef ZP<NSData> inherited;
public:
	Data_NS();
	Data_NS(const Data_NS& iOther);
	~Data_NS();
	Data_NS& operator=(const Data_NS& iOther);

	Data_NS(NSMutableData* iOther);
	Data_NS(NSData* iOther);

	Data_NS(const Adopt_T<NSMutableData>& iOther);
	Data_NS(const Adopt_T<NSData>& iOther);

	Data_NS& operator=(NSMutableData* iOther);
	Data_NS& operator=(NSData* iOther);

	Data_NS& operator=(const Adopt_T<NSMutableData>& iOther);
	Data_NS& operator=(const Adopt_T<NSData>& iOther);

	Data_NS(size_t iSize);
	Data_NS(const void* iSource, size_t iSize);

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
	NSData* pData() const;
	NSMutableData* pTouch();
	bool fMutable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZooLib_Apple_Data_NS_h__
