// Copyright (c) 2019-21 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Archive_h__
#define __ZooLib_Archive_h__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/UnicodeString8.h"

#include <map>

namespace ZooLib {

// =================================================================================================
#pragma mark - Archive

class Archive
:	public Counted
	{
public:
// Our protocol
	virtual size_t Count() = 0;
	virtual bool IsFile(size_t iIndex) = 0;
	virtual string8 Name(size_t iIndex) = 0;
	virtual uint64 Size(size_t iIndex) = 0;
	virtual ZQ<uint32> QCRC(size_t iIndex);

	virtual ZP<ChannerR_Bin> OpenR(size_t iIndex);
	virtual ZP<ChannerRPos_Bin> OpenRPos(size_t iIndex);
	};

} // namespace ZooLib

#endif // __ZooLib_Archive_h__
