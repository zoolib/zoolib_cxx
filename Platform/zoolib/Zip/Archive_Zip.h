// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Zip_Archive_Zip_h__
#define __ZooLib_Zip_Archive_Zip_h__ 1
#include "zconfig.h"

#include "zoolib/Archive.h"

struct zip;

namespace ZooLib {

// =================================================================================================
#pragma mark -

class Archive_Zip
:	public Archive
	{
public:
	Archive_Zip(struct zip* i_zip, bool iAdopt);
	~Archive_Zip() override;

// From Archive
	size_t Count() override;
	bool IsFile(size_t iIndex) override;
	string8 Name(size_t iIndex) override;
	uint64 Size(size_t iIndex) override;
	ZQ<uint32> QCRC(size_t iIndex) override;

	ZP<ChannerRSize_Bin> OpenRSize(size_t iIndex) override;

private:
	ZMtx fMtx;

	struct zip* f_zip;
	bool fOwned;
	
	class ChannerRSize_Bin_Zip;
	friend class ChannerRSize_Bin_Zip;
	};

// =================================================================================================
#pragma mark -

ZP<Archive_Zip> sArchive_Zip(const string8& iZipFilePath, bool iCheckConsistency);

ZP<Archive_Zip> sArchive_Zip(FILE* iFILE, bool iCheckConsistency);

bool sIsZip(const ChanR_Bin& iChanR);

} // namespace ZooLib

#endif // __ZooLib_Zip_Archive_Zip_h__
