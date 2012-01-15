/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/ZAsset_FS.h"
#include "zoolib/ZStream_Memory.h" // For ZStreamerRPos_Memory

using std::string;
using std::vector;

namespace ZooLib {

// =================================================================================================
// MARK: - StreamerMemory

namespace { // anonymous

class StreamerMemory : public ZStreamerRPos_Memory
	{
public:
	StreamerMemory(ZRef<ZAssetRep> inAssetRep, const void* inAddress, size_t inSize);

private:
	ZRef<ZAssetRep> fAssetRep;
	};

StreamerMemory::StreamerMemory(ZRef<ZAssetRep> inAssetRep, const void* inAddress, size_t inSize)
:	ZStreamerRPos_Memory(inAddress, inSize),
	fAssetRep(inAssetRep)
	{}

} // anonymous namespace

// =================================================================================================
// MARK: - ZAssetIterRep_FS

class ZAssetIterRep_FS : public ZAssetIterRep
	{
public:
	ZAssetIterRep_FS(const ZFileIter& iIter);

	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZAssetRep> Current();
	virtual string CurrentName();

	virtual ZRef<ZAssetIterRep> Clone();

private:
	ZFileIter fIter;
	};

ZAssetIterRep_FS::ZAssetIterRep_FS(const ZFileIter& iIter)
:	fIter(iIter)
	{}

bool ZAssetIterRep_FS::HasValue()
	{ return fIter; }

void ZAssetIterRep_FS::Advance()
	{ fIter.Advance(); }

ZRef<ZAssetRep> ZAssetIterRep_FS::Current()
	{
	if (ZFileSpec theSpec = fIter.Current())
		return new ZAssetRep_FS(theSpec);
	return ZRef<ZAssetRep>();
	}

string ZAssetIterRep_FS::CurrentName()
	{ return fIter.CurrentName(); }

ZRef<ZAssetIterRep> ZAssetIterRep_FS::Clone()
	{ return new ZAssetIterRep_FS(fIter); }

// =================================================================================================
// MARK: - ZAssetRep_FS

ZAssetRep_FS::ZAssetRep_FS(const ZFileSpec& iSpec)
:	fSpec(iSpec),
	fData(nullptr),
	fDataSize(0)
	{}

ZAssetRep_FS::ZAssetRep_FS(const ZFileSpec& iSpec, const vector<string>& iComps)
:	fSpec(iSpec),
	fComps(iComps),
	fData(nullptr),
	fDataSize(0)
	{}

ZAssetRep_FS::ZAssetRep_FS(const ZFileSpec& iSpec,
	const vector<string>::iterator& iBegin, const vector<string>::iterator& iEnd)
:	fSpec(iSpec),
	fComps(iBegin, iEnd),
	fData(nullptr),
	fDataSize(0)
	{}

ZAssetRep_FS::~ZAssetRep_FS()
	{
	delete[] fData;
	}

string ZAssetRep_FS::GetName()
	{
	if (fComps.empty())
		return string();
	return fComps.back();
	}

string ZAssetRep_FS::GetPath()
	{
	string result;
	for (size_t x = 0; x < fComps.size(); ++x)
		{
		if (x)
			result += '|';
		result += fComps[x];
		}
	return result;
	}

ZRef<ZAssetRep> ZAssetRep_FS::GetParentAssetRep()
	{
	if (fComps.empty())
		return ZRef<ZAssetRep>();
	return new ZAssetRep_FS(fSpec, fComps.begin(), fComps.end() - 1);
	}

ZRef<ZAssetRep> ZAssetRep_FS::ResolvePath(const char* iPath)
	{
	const char* pathStart = iPath;

	if (pathStart == nullptr || pathStart[0] == 0)
		{
		// An empty path means us.
		return this;
		}

	vector<string> newComps;

	if (pathStart[0] == '|')
		{
		// A | prefix means we want our root, which is fSpec with an empty component vector.
		++pathStart;
		}
	else
		{
		newComps = fComps;
		}
		
	const char* iter = pathStart;
	for (;;)
		{		
		switch (*iter)
			{
			case 0:
			case '^':
			case '|':
				{
				if (iter > pathStart)
					newComps.push_back(string(pathStart, iter - pathStart));
				pathStart = iter + 1;
		
				if (*iter == '^')
					{
					if (newComps.empty())
						{
						// We're trying to get the parent of the root,
						// which is an illegal asset reference.
						return ZRef<ZAssetRep>();
						}
					newComps.pop_back();
					}
				}
			}

		if (*iter == 0)
			break;

		++iter;
		}

	if (newComps.empty())
		{
		if (fSpec.Exists())
			return new ZAssetRep_FS(fSpec);
		return ZRef<ZAssetRep>();
		}

	ZFileSpec branchSpec = fSpec.Trail(ZTrail(newComps.begin(), newComps.end() - 1));
	if (!branchSpec.Exists())
		{
		// The branch doesn't exist, so neither can the full path.
		return ZRef<ZAssetRep>();
		}

	// We know the branch exists.
	string leaf = newComps.back();
	if (leaf == "!binary" || leaf == "!string" || leaf == "!stringtable")
		{
		// We've got a leaf with a special name.
		if (branchSpec.IsFile())
			{
			// And the branch is a file, so we pretend that our specially named leaf exists.
			return new ZAssetRep_FS(fSpec, newComps);
			}
		}

	if (fSpec.Trail(ZTrail(newComps.begin(), newComps.end())).Exists())
		{
		// Our spec plus the new components is an extant entity.
		return new ZAssetRep_FS(fSpec, newComps);
		}

	return ZRef<ZAssetRep>();
	}

ZRef<ZAssetIterRep> ZAssetRep_FS::CreateIterRep()
	{
	return new ZAssetIterRep_FS(fSpec.Trail(ZTrail(fComps.begin(), fComps.end())));
	}

void ZAssetRep_FS::GetData(const void** oData, size_t* oSize)
	{
	if (!oData && !oSize)
		return;

	ZMutexLocker locker(fMutex);
	if (!fData)
		{
		if (ZRef<ZStreamerRPos> theStreamer = this->OpenRPos())
			{
			fDataSize = theStreamer->GetStreamRPos().GetSize();
			if (fDataSize)
				{
				fData = new char[fDataSize];
				size_t realSize;
				theStreamer->GetStreamR().Read(fData, fDataSize, &realSize);
				fDataSize = realSize;
				}
			}
		}

	if (oData)
		*oData = fData;

	if (oSize)
		*oSize = fDataSize;
	}

ZRef<ZStreamerRPos> ZAssetRep_FS::OpenRPos()
	{
	if (fData)
		return new StreamerMemory(this, fData, fDataSize);

	if (fComps.empty())
		return fSpec.OpenRPos();

	string leaf = fComps.back();
	if (leaf == "!binary" || leaf == "!string" || leaf == "!stringtable")
		{
		ZFileSpec branchSpec = fSpec.Trail(ZTrail(fComps.begin(), fComps.end() - 1));
		if (branchSpec.IsFile())
			{
			// And the branch is a file, so we pretend that our specially named leaf exists.
			return branchSpec.OpenRPos();
			}
		}

	return fSpec.Trail(ZTrail(fComps.begin(), fComps.end())).OpenRPos();
	}

} // namespace ZooLib
