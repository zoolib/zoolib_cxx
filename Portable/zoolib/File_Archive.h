// Copyright (c) 2019-21 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_File_Archive_h__
#define __ZooLib_File_Archive_h__ 1
#include "zconfig.h"

#include "zoolib/Archive.h"
#include "zoolib/File.h"

#include <map>

namespace ZooLib {

FileSpec sFileSpec_Archive(const ZP<Archive>& iArchive);

} // namespace ZooLib

#endif // __ZooLib_File_Archive_h__
