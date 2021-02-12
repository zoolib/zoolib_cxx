// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Zip_File_Zip_h__
#define __ZooLib_Zip_File_Zip_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

namespace ZooLib {

FileSpec sFileSpec_Zip(const string8& iZipFilePath);

FileSpec sFileSpec_Zip(FILE* iFILE);

} // namespace ZooLib

#endif // __ZooLib_Zip_File_Zip_h__
