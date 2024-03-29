// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_zlib_Chan_Bin_zlib_h__
#define __ZooLib_zlib_Chan_Bin_zlib_h__ 1
#include "zconfig.h"

#include <vector>
#include <zlib.h>

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - zlib format enums

namespace zlib {

enum class EFormatR
	{
	Raw,
	ZLib,
	GZip,
	Auto
	};

enum class EFormatW
	{
	Raw,
	ZLib,
	GZip
	};

} // namespace zlib

// =================================================================================================
#pragma mark - ChanR_Bin_zlib

class ChanR_Bin_zlib
:	public virtual ChanR_Bin
	{
public:
	ChanR_Bin_zlib(bool iRaw, const ChanR_Bin& iChanR);
	ChanR_Bin_zlib(zlib::EFormatR iFormatR, size_t iBufferSize, const ChanR_Bin& iChanR);
	~ChanR_Bin_zlib();

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

protected:
	const ChanR_Bin& fChanR;
	z_stream fState;
	std::vector<Bytef> fBuffer;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_zlib

class ChanW_Bin_zlib
:	public virtual ChanW_Bin
	{
public:
	ChanW_Bin_zlib(bool iRaw, const ChanW_Bin& iChanW);
	ChanW_Bin_zlib(zlib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
		const ChanW_Bin& iChanW);
	~ChanW_Bin_zlib();

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	void pDeflate(int iFlushType);

	const ChanW_Bin& fChanW;
	z_stream fState;
	std::vector<Bytef> fBuffer;
	};

} // namespace ZooLib

#endif // __ZooLib_zlib_Chan_Bin_zlib_h__
