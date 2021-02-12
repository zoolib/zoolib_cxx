// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Zip/File_Zip.h"
#include "zoolib/File_Archive.h"

#include "zoolib/Util_STL_map.h"

#include "zip.h"

// This is implemented in zip_open.c
extern "C" struct zip * zip_open_FILE(FILE* fp, int flags);

namespace ZooLib {

namespace { // anonymous

// =================================================================================================
#pragma mark - ArchiveToken_EntryNum (anonymous)

class ArchiveToken_EntryNum
:	public ArchiveToken
	{
public:
	ArchiveToken_EntryNum(size_t iEntryNum)
	:	fEntryNum(iEntryNum)
		{}

	const size_t fEntryNum;
	};

// =================================================================================================
#pragma mark - ZipHolder (anonymous)

class ZipHolder
:	public Archive
	{
public:
	ZipHolder(zip* i_zip, bool iAdopt)
	:	f_zip(i_zip)
	,	fOwned(iAdopt)
	,	fRoot(new ArchiveNode_Directory(nullptr, string8()))
		{
		for (size_t xx = 0, count = ::zip_get_num_files(f_zip); xx < count; ++xx)
			{
			const string8 theName = ::zip_get_name(f_zip, xx, 0);
			sGrowArchiveDirectoryTree(new ArchiveToken_EntryNum(xx), fRoot, Trail(theName), 0);
			}
		}

	virtual ~ZipHolder()
		{
		if (fOwned)
			::zip_close(f_zip);
		}

// From Archive
	virtual ZP<ArchiveNode> GetRoot()
		{ return fRoot; }

	virtual ZP<ChannerR_Bin> OpenR(const ZP<ArchiveNode_File>& iArchiveNode);

	struct zip* f_zip;
	bool fOwned;

	ZP<ArchiveNode_Directory> fRoot;
	};

// =================================================================================================
#pragma mark - ChannerR_Bin_Zip (anonymous)

struct ChannerR_Bin_Zip
:	public ChannerR_Bin
	{
	ZP<ZipHolder> fZipHolder;
	zip_file* f_zip_file;

	ChannerR_Bin_Zip(const ZP<ZipHolder>& iZipHolder, size_t iEntryNum)
	:	fZipHolder(iZipHolder)
	,	f_zip_file(::zip_fopen_index(fZipHolder->f_zip, iEntryNum, 0))
		{}

	virtual ~ChannerR_Bin_Zip()
		{ ::zip_fclose(f_zip_file); }

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount)
		{
		const ssize_t countRead = ::zip_fread(f_zip_file, oDest, iCount);
		return countRead <= 0 ? 0 : countRead;
		}
	};

ZP<ChannerR_Bin> ZipHolder::OpenR(const ZP<ArchiveNode_File>& iArchiveNode)
	{
	if (ZP<ArchiveToken_EntryNum> theATEN =
		iArchiveNode->GetToken().DynamicCast<ArchiveToken_EntryNum>())
		{
		return new ChannerR_Bin_Zip(this, theATEN->fEntryNum);
		}
	return null;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - sFileLoc_Zip

FileSpec sFileSpec_Zip(const std::string& iZipFilePath)
	{
	if (zip* the_zip = ::zip_open(iZipFilePath.c_str(), 0, nullptr))
		return sFileSpec_Archive(new ZipHolder(the_zip, true));
	return FileSpec();
	}

FileSpec sFileSpec_Zip(FILE* iFILE)
	{
	if (zip* the_zip = ::zip_open_FILE(iFILE, 0))
		return sFileSpec_Archive(new ZipHolder(the_zip, true));
	return FileSpec();
	}

} // namespace ZooLib
