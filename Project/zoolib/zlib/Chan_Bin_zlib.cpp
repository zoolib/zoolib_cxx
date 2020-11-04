// Copyright (c) 2002-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/zlib/Chan_Bin_zlib.h"
#include "zoolib/Util_STL.h"

#include <algorithm>

namespace ZooLib {

using Util_STL::sIsEmpty;
using Util_STL::sNotEmpty;

// =================================================================================================
#pragma mark - ChanR_Bin_zlib

ChanR_Bin_zlib::ChanR_Bin_zlib(bool iRaw, const ChanR_Bin& iChanR)
:	ChanR_Bin_zlib(iRaw ? zlib::eFormatR_Raw : zlib::eFormatR_Auto, 1024, iChanR)
	{}

ChanR_Bin_zlib::ChanR_Bin_zlib(zlib::EFormatR iFormatR, size_t iBufferSize, const ChanR_Bin& iChanR)
:	fChanR(iChanR)
,	fBuffer(std::max<size_t>(1024, iBufferSize))
	{
	fState.zalloc = nullptr;
	fState.zfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = &fBuffer[0];
	fState.avail_in = 0;

	fState.next_out = nullptr;
	fState.avail_out = 0;

	int windowBits;
	switch (iFormatR)
		{
		case zlib::eFormatR_GZip: windowBits = 16 | 15; break;
		case zlib::eFormatR_ZLib: windowBits = 15; break;
		case zlib::eFormatR_Raw: windowBits = -15; break;
		case zlib::eFormatR_Auto: windowBits = 32 | 15; break;
		}

	if (Z_OK != ::inflateInit2(&fState, windowBits))
		throw std::runtime_error("ChanR_Bin_zlib initialization problem");
	}

ChanR_Bin_zlib::~ChanR_Bin_zlib()
	{
	::inflateEnd(&fState);
	}

size_t ChanR_Bin_zlib::Read(byte* oDest, size_t iCount)
	{
	fState.avail_out = iCount;
	fState.next_out = oDest;

	for (;;)
		{
		::inflate(&fState, Z_NO_FLUSH);
		if (fState.avail_out == 0)
			{
			break;
			}
		else if (fState.avail_in == 0)
			{
			// Top up our input buffer
			size_t countReadable = sReadable(fChanR);
			if (countReadable == 0)
				countReadable = 1;

			size_t countToRead = std::min(countReadable, fBuffer.size());

			size_t countRead = sRead(fChanR, &fBuffer[0], countToRead);

			if (countRead == 0)
				break;

			fState.avail_in = countRead;
			fState.next_in = &fBuffer[0];
			}
		}

	return iCount - fState.avail_out;
	}

size_t ChanR_Bin_zlib::Readable()
	{ return fState.avail_out; }

// =================================================================================================
#pragma mark - ChanW_Bin_zlib

ChanW_Bin_zlib::ChanW_Bin_zlib(const ChanW_Bin& iChanW)
:	ChanW_Bin_zlib(zlib::eFormatW_Raw, 5, 1024, iChanW)
	{}

ChanW_Bin_zlib::ChanW_Bin_zlib(zlib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
	const ChanW_Bin& iChanW)
:	fChanW(iChanW)
,	fBuffer(std::max<size_t>(1024, iBufferSize))
	{
	fState.zalloc = nullptr;
	fState.zfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = nullptr;
	fState.avail_in = 0;

	fState.next_in = &fBuffer[0];
	fState.avail_out = fBuffer.size();

	int windowBits;
	switch (iFormatW)
		{
		case zlib::eFormatW_GZip: windowBits = 16 | 15; break;
		case zlib::eFormatW_ZLib: windowBits = 15; break;
		case zlib::eFormatW_Raw: windowBits = -15; break;
		}

	if (Z_OK != ::deflateInit2(&fState,
		iCompressionLevel, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY))
		{
		throw std::runtime_error("ChanW_Bin_zlib initialization  problem");
		}
	}

ChanW_Bin_zlib::~ChanW_Bin_zlib()
	{
	if (sNotEmpty(fBuffer))
		{
		try
			{
			this->pDeflate(true);
			sFlush(fChanW);
			}
		catch (...)
			{}
		}

	::deflateEnd(&fState);
	}

size_t ChanW_Bin_zlib::Write(const byte* iSource, size_t iCount)
	{
	if (sIsEmpty(fBuffer))
		return 0;

	fState.avail_in = iCount;
	fState.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(iSource));

	pDeflate(false);

	return iCount - fState.avail_in;
	}

void ChanW_Bin_zlib::Flush()
	{
	this->pDeflate(true);
	sFlush(fChanW);
	}

void ChanW_Bin_zlib::pDeflate(boolean iFlush)
	{
	for (;;)
		{
		::deflate(&fState, iFlush ? Z_SYNC_FLUSH : Z_NO_FLUSH);
		if (size_t countToWrite = fBuffer.size() - fState.avail_out)
			{
			if (countToWrite != sWriteFully(fChanW, &fBuffer[0], countToWrite))
				{
				fBuffer.clear();
				return;
				}
			fState.next_out = &fBuffer[0];
			fState.avail_out = fBuffer.size();
			}
		else
			{
			break;
			}
		}
	}


} // namespace ZooLib
