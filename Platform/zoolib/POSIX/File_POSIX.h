// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_POSIX_File_POSIX_h__
#define __ZooLib_POSIX_File_POSIX_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/File.h"

#ifndef ZCONFIG_API_Avail__File_POSIX
	#define ZCONFIG_API_Avail__File_POSIX ZCONFIG_SPI_Enabled(POSIX)
#endif

#ifndef ZCONFIG_API_Desired__File_POSIX
	#define ZCONFIG_API_Desired__File_POSIX 1
#endif

#if ZCONFIG_API_Enabled(File_POSIX)

ZMACRO_MSVCStaticLib_Reference(File_POSIX)

namespace ZooLib {

// =================================================================================================
#pragma mark - FileLoc_POSIX

class FileLoc_POSIX : public FileLoc
	{
public:
	static ZP<FileLoc_POSIX> sGet_CWD();
	static ZP<FileLoc_POSIX> sGet_Root();
	static ZP<FileLoc_POSIX> sGet_App();

	FileLoc_POSIX(bool iIsAtRoot);
	FileLoc_POSIX(bool iIsAtRoot, const std::vector<std::string>& iComps);
	FileLoc_POSIX(bool iIsAtRoot, const std::string* iComps, size_t iCount);
	FileLoc_POSIX(bool iIsAtRoot, std::vector<std::string>* ioComps, const IKnowWhatIAmDoing_t&);
	~FileLoc_POSIX() override;

// From FileLoc
	ZP<FileIterRep> CreateIterRep() override;

	std::string GetName() override;
	ZQ<Trail> QTrailTo(ZP<FileLoc> oDest) override;

	ZP<FileLoc> GetParent() override;
	ZP<FileLoc> GetDescendant(
		const std::string* iComps, size_t iCount) override;

	bool IsRoot() override;

	ZP<FileLoc> TraverseLink() override;

	std::string AsString_POSIX(const std::string* iComps, size_t iCount) override;
	std::string AsString_Native(const std::string* iComps, size_t iCount) override;

	EFileKind Kind() override;
	ZQ<uint64> QSize() override;
	ZQ<double> QTimeCreated() override;
	ZQ<double> QTimeModified() override;

	ZP<FileLoc> CreateDir() override;

	ZP<FileLoc> MoveTo(ZP<FileLoc> oDest) override;
	bool Delete() override;

	ZP<ChannerRPos_Bin> OpenRPos(bool iPreventWriters) override;
	ZP<ChannerWPos_Bin> OpenWPos(bool iPreventWriters) override;
	ZP<ChannerRWPos_Bin> OpenRWPos(bool iPreventWriters) override;

	ZP<ChannerWPos_Bin> CreateWPos(bool iOpenExisting, bool iPreventWriters) override;
	ZP<ChannerRWPos_Bin> CreateRWPos(bool iOpenExisting, bool iPreventWriters) override;

	std::string pGetPath();

private:
	bool fIsAtRoot;
	std::vector<std::string> fComps;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(File_POSIX)

#endif // __ZooLib_POSIX_File_POSIX_h__
