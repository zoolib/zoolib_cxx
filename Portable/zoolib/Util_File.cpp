// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Util_File.h"

#include <map>
#include <vector>

namespace ZooLib {
namespace Util_File {

using std::map;
using std::pair;
using std::string;
using std::vector;

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

namespace { // anonymous

class RealRep_Sorted : public FileIterRep_Std::RealRep
	{
public:
	RealRep_Sorted(FileIter iSource)
		{
		map<string,FileSpec> temp;
		for (/*no init*/; iSource; iSource.Advance())
			temp[iSource.CurrentName()] = iSource.Current();
		copy(temp.begin(), temp.end(), back_inserter(fEntries));
		}

	virtual ~RealRep_Sorted()
		{}

// From FileIterRep_Std::RealRep
	virtual bool HasValue(size_t iIndex)
		{ return iIndex < fEntries.size(); }

	virtual FileSpec GetSpec(size_t iIndex)
		{
		ZAssert(iIndex < fEntries.size());
		return fEntries.at(iIndex).second;
		}

	virtual string GetName(size_t iIndex)
		{
		ZAssert(iIndex < fEntries.size());
		return fEntries.at(iIndex).first;
		}

private:
	vector<pair<string,FileSpec>> fEntries;
	};

} // anonymous namespace

FileIter sSorted(FileIter iSource)
	{ return FileIter(new FileIterRep_Std(new RealRep_Sorted(iSource), 0)); }

} // namespace Util_File
} // namespace ZooLib

