// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_File.h"

namespace ZooLib {
namespace Util_File {

// =================================================================================================
#pragma mark - Util_File

ZQ<FileSpec> sQEnsureBranch(const FileSpec& iFS)
	{
	if (iFS.IsDir())
		return iFS;
	
	if (ZQ<FileSpec> newParentQ = sQEnsureBranch(iFS.Parent()))
		{
		if (const FileSpec theFS = newParentQ->Child(iFS.Name()).CreateDir())
			return theFS;
		}

	return null;
	}

bool sDeleteTree(const FileSpec& iFS)
	{
	for (FileIter iter = iFS; iter; iter.Advance())
		sDeleteTree(iter.Current());
	return iFS.Delete();
	}

} // namespace Util_File
} // namespace ZooLib

