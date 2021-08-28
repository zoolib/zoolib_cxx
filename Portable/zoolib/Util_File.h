// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_File_h__
#define __ZooLib_Util_File_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"
#include "zoolib/ZQ.h"

namespace ZooLib {
namespace Util_File {

// =================================================================================================
#pragma mark - Util_File

ZQ<FileSpec> sQEnsureBranch(const FileSpec& iFS);

bool sDeleteTree(const FileSpec& iFS);

FileIter sSorted(FileIter iSource);

} // namespace Util_File
} // namespace ZooLib

#endif // __ZooLib_Util_File_h__
