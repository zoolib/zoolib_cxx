// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Zip/Archive_Zip.h"

#include "zoolib/ChanR_Bin.h"

#include <string.h> // for strlen

#include "zip.h"
#include "zipint.h" // For zip_file.bytes_left

// This is implemented in zip_open.c
extern "C" struct zip * zip_open_FILE(FILE* fp, int flags);

namespace ZooLib {

// =================================================================================================
#pragma mark - ChannerRSize_Zip

class Archive_Zip::ChannerRSize_Bin_Zip
:	public ChannerRSize_Bin
	{
public:
	ZP<Archive_Zip> fArchive_Zip;
	zip_file* f_zip_file;
	uint64 fPos;

	ChannerRSize_Bin_Zip(const ZP<Archive_Zip>& iArchive_Zip, zip_file* i_zip_file)
	:	fArchive_Zip(iArchive_Zip)
	,	f_zip_file(i_zip_file)
	,	fPos(0)
		{}

	virtual ~ChannerRSize_Bin_Zip()
		{ ::zip_fclose(f_zip_file); }

// From ChanPos
	virtual uint64 Pos()
		{ return fPos; }

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount)
		{
		ZAcqMtx acq(fArchive_Zip->fMtx);
		const ssize_t countRead = ::zip_fread(f_zip_file, oDest, iCount);
		if (countRead < 0)
			return 0;
		fPos += countRead;
		return countRead;
		}

	virtual size_t Readable()
		{ return sClamped(f_zip_file->bytes_left); }

// From ChanSize
	virtual uint64 Size()
		{ return fPos + f_zip_file->bytes_left; }
	};

// =================================================================================================
#pragma mark -

Archive_Zip::Archive_Zip(zip* i_zip, bool iAdopt)
:	f_zip(i_zip)
,	fOwned(iAdopt)
	{}

Archive_Zip::~Archive_Zip()
	{
	if (fOwned)
		::zip_close(f_zip);
	}

size_t Archive_Zip::Count()
	{ return ::zip_get_num_files(f_zip); }

static bool spIsFile(zip* i_zip, size_t iIndex)
	{
	if (const char* theName = ::zip_get_name(i_zip, iIndex, 0))
		{
		if (size_t theLen = strlen(theName))
			{
			if (theName[theLen-1] != '/')
				return true;
			}
		}
	return false;
	}

bool Archive_Zip::IsFile(size_t iIndex)
	{ return spIsFile(f_zip, iIndex); }

string8 Archive_Zip::Name(size_t iIndex)
	{
	if (const char* theName = ::zip_get_name(f_zip, iIndex, 0))
		return theName;
	return string8();
	}

uint64 Archive_Zip::Size(size_t iIndex)
	{
	struct zip_stat st;
	if (0 != zip_stat_index(f_zip, iIndex, 0, &st))
		return 0;
	return st.size;
	}

ZQ<uint32> Archive_Zip::QCRC(size_t iIndex)
	{
	struct zip_stat st;
	if (0 != zip_stat_index(f_zip, iIndex, 0, &st))
		return null;

	return st.crc;
	}

ZP<ChannerRSize_Bin> Archive_Zip::OpenRSize(size_t iIndex)
	{
	if (spIsFile(f_zip, iIndex))
		{
		if (zip_file* the_zip_file = ::zip_fopen_index(f_zip, iIndex, 0))
			{
			ZAcqMtx acq(fMtx);
			// We could actually return a ChannerRSize -- implementing Aspect_Read and
			// Aspect_Size. There are circumstances where its helpful know the size even
			// if we can't random-access within.
			return new ChannerRSize_Bin_Zip(this, the_zip_file);
			}
		}
	return null;	
	}

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(const string8& iZipFilePath, bool iCheckConsistency)
	{
	const int flags = iCheckConsistency ? ZIP_CHECKCONS : 0;
	if (zip* the_zip = ::zip_open(iZipFilePath.c_str(), flags, nullptr))
		return new Archive_Zip(the_zip, true);
	return null;
	}

ZP<Archive_Zip> sArchive_Zip(FILE* iFILE, bool iCheckConsistency)
	{
	const int flags = iCheckConsistency ? ZIP_CHECKCONS : 0;
	if (zip* the_zip = ::zip_open_FILE(iFILE, flags))
		return new Archive_Zip(the_zip, true);
	return null;
	}

static const uint8 kSig[] = {0x50, 0x4B, 0x03, 0x04};

bool sIsZip(const ChanR_Bin& iChanR)
	{
	uint8 sig[sizeof(kSig)];
	if (sizeof(sig) != sReadMemFully(iChanR, sig, sizeof(sig)))
		return false;
	return 0 == memcmp(sig, kSig, sizeof(sig));
	}

} // namespace ZooLib
