// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Zip/Archive_Zip.h"

// This is implemented in zip_open.c
extern "C" struct zip * zip_open_FILE(FILE* fp, int flags);

namespace ZooLib {

// =================================================================================================
#pragma mark - ChannerR_Bin_Zip

class Archive_Zip::ChannerR_Bin_Zip
:	public ChannerR_Bin
	{
public:
	ZP<Archive_Zip> fArchive_Zip;
	zip_file* f_zip_file;

	ChannerR_Bin_Zip(const ZP<Archive_Zip>& iArchive_Zip, zip_file* i_zip_file)
	:	fArchive_Zip(iArchive_Zip)
	,	f_zip_file(i_zip_file)
		{}

	virtual ~ChannerR_Bin_Zip()
		{ ::zip_fclose(f_zip_file); }

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount)
		{
		ZAcqMtx acq(fArchive_Zip->fMtx);
		const ssize_t countRead = ::zip_fread(f_zip_file, oDest, iCount);
		return countRead <= 0 ? 0 : countRead;
		}
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

ZP<ChannerR_Bin> Archive_Zip::OpenR(size_t iIndex)
	{
	if (spIsFile(f_zip, iIndex))
		{
		if (zip_file* the_zip_file = ::zip_fopen_index(f_zip, iIndex, 0))
			{
			ZAcqMtx acq(fMtx);
			return new ChannerR_Bin_Zip(this, the_zip_file);
			}
		}
	return null;	
	}

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(const string8& iZipFilePath)
	{
	if (zip* the_zip = ::zip_open(iZipFilePath.c_str(), 0, nullptr))
		return new Archive_Zip(the_zip, true);
	return null;
	}

ZP<Archive_Zip> sArchive_Zip(FILE* iFILE)
	{
	if (zip* the_zip = ::zip_open_FILE(iFILE, 0))
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
