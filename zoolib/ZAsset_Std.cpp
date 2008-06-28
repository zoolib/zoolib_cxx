/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZAsset_Std.h"
#include "zoolib/ZByteSwap.h"
#include "zoolib/ZCompat_algorithm.h" // For lower_bound and find_first_of
#include "zoolib/ZStream_Memory.h"

using std::find_first_of;
using std::lower_bound;
using std::min;
using std::runtime_error;
using std::sort;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Utility routines

static uint32 sReadCount(const ZStreamR& inStream)
	{ return inStream.ReadCount(); }

static void sThrowBadFormat()
	{
	throw runtime_error("ZAssetTree_Std, bad format");
	}

static const char sEmptyString[] = "";
static const char sMagicText[] = "ZAO v1.0";
static const size_t sMagicTextSize = sizeof(sMagicText);

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std::CompareAgainstRep

struct ZAssetRep_Std::CompareAgainstRep
	{
	bool operator()(ZAssetRep_Std* left, ZAssetRep_Std* right) const
		{ return strcmp(left->fName, right->fName) < 0; }

	typedef ZAssetRep_Std* first_argument_type;
	typedef ZAssetRep_Std* second_argument_type;
	typedef bool result_type;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std::CompareAgainstPath

struct ZAssetRep_Std::CompareAgainstPath
	{
	CompareAgainstPath(size_t iPathLength) : fPathLength(iPathLength) {}

	bool operator()(ZAssetRep_Std* left, const char* right) const
		{ return strncmp(left->fName, right, fPathLength) < 0; }

	typedef ZAssetRep_Std* first_argument_type;
	typedef const char* second_argument_type;
	typedef bool result_type;
	size_t fPathLength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std

ZAssetRep_Std::ZAssetRep_Std(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength)
:   fName(inName),
	fNameLength(inNameLength),
	fParent(inParent)
	{}

ZAssetRep_Std::~ZAssetRep_Std()
	{
	ZAssertStop(2, !fAssetTree);
	}

void ZAssetRep_Std::Finalize()
	{ fAssetTree->RepBeingFinalized(this); }

string ZAssetRep_Std::GetName()
	{ return fName; }

string ZAssetRep_Std::GetPath()
	{
	string theName = fName;
	ZAssetRep_Std_Directory* currentDirectory = fParent;
	while (currentDirectory)
		{
		theName = currentDirectory->fName + string("|") + theName;
		currentDirectory = currentDirectory->fParent;
		}
	return theName;
	}

ZRef<ZAssetRep> ZAssetRep_Std::GetParentAssetRep()
	{
	if (fParent)
		return fAssetTree->UseRep(fParent);
	return ZRef<ZAssetRep>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Directory

ZAssetRep_Std_Directory::ZAssetRep_Std_Directory(ZAssetTree_Std* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets)
:	ZAssetRep_Std(inParent, inName, inNameLength)
	{
	fChildrenCount = sReadCount(inStream);
	if (fChildrenCount == 0)
		{
		fChildren = nil;
		}
	else if (fChildrenCount == 1)
		{
		uint32 childNameIndex = sReadCount(inStream);
		size_t nameOffset = ZByteSwap_ReadBig32(inNameOffsets + childNameIndex);
		size_t nextNameOffset = ZByteSwap_ReadBig32(inNameOffsets + childNameIndex + 1);
		fChild = inAssetTree->LoadAssetRep(this, inNameTable + nameOffset,
									nextNameOffset - nameOffset - 1,
									inStream, inNameTable, inNameOffsets);
		}
	else
		{
		fChildren = new ZAssetRep_Std*[fChildrenCount];
		for (size_t x = 0; x < fChildrenCount; ++x)
			{
			uint32 childNameIndex = sReadCount(inStream);
			size_t nameOffset = ZByteSwap_ReadBig32(inNameOffsets + childNameIndex);
			size_t nextNameOffset = ZByteSwap_ReadBig32(inNameOffsets + childNameIndex + 1);
			fChildren[x] = inAssetTree->LoadAssetRep(this, inNameTable + nameOffset,
									nextNameOffset - nameOffset - 1,
									inStream, inNameTable, inNameOffsets);
			}
		if (fChildrenCount > 7)
			{
			// We have more than seven children, so we sort them into name order
			// and will use lower_bound to binary chop into the list in ResolvePath.
			sort(fChildren, fChildren + fChildrenCount, CompareAgainstRep());
			}
		}
	}

ZAssetRep_Std_Directory::~ZAssetRep_Std_Directory()
	{
	if (fChildrenCount == 1)
		{
		delete fChild;
		}
	else
		{
		for (size_t x = 0; x < fChildrenCount; ++x)
			delete fChildren[x];
		delete[] fChildren;
		}
	}

ZRef<ZAssetRep> ZAssetRep_Std_Directory::ResolvePath(const char* inPath)
	{
	if (ZRef<ZAssetRep> theRep = ZAssetRep::ResolvePath(inPath))
		return theRep;

	if (fChildrenCount == 1)
		{
		if (strncmp(inPath, fChild->fName, fChild->fNameLength) == 0)
			{
			if (inPath[fChild->fNameLength] == '\0')
				return fAssetTree->UseRep(fChild);

			if (inPath[fChild->fNameLength] == '|')
				{
				ZRef<ZAssetRep_Std> theChild = fAssetTree->UseRep(fChild);
				return theChild->ResolvePath(inPath + fChild->fNameLength + 1);
				}
			}
		}
	else if (fChildrenCount <= 7)
		{
		// With seven or fewer children we do a linear scan of the list.
		for (size_t x = 0; x < fChildrenCount; ++x)
			{
			if (strncmp(inPath, fChildren[x]->fName, fChildren[x]->fNameLength) == 0)
				{
				if (inPath[fChildren[x]->fNameLength] == '\0')
					return fAssetTree->UseRep(fChildren[x]);

				if (inPath[fChildren[x]->fNameLength] == '|')
					{
					ZRef<ZAssetRep_Std> theChild = fAssetTree->UseRep(fChildren[x]);
					return theChild->ResolvePath(inPath + fChildren[x]->fNameLength + 1);
					}
				}
			}
		}
	else
		{
		// With more than seven children we do a binary chop into the list,
		// which was sorted in our constructor when we loaded up.
		static const char* const terminators = "|^";
		const char* pathEnd = find_first_of(inPath, inPath + strlen(inPath), terminators, terminators + 2);

		ZAssetRep_Std** childrenEnd = fChildren + fChildrenCount;
		ZAssetRep_Std** i = ::lower_bound(fChildren, childrenEnd, inPath, CompareAgainstPath(pathEnd - inPath));
		if (i != childrenEnd)
			{
			// We've found a child whose name is >= inPath...pathEnd.
			// There are three possibilities:
			// 1. It's strictly greater than inPath, and may be shorter or longer.
			// 2. It's a strict prefix of inPath.
			// 3. It's equal to inPath.
			// There's no standard function to distinguish these three scenarios,
			// at least without copying inPath...pathEnd into a zero-terminated string,
			// so we do it manually.

			const char* p = inPath;
			const char* t = (*i)->fName;
			for (; p != pathEnd && *t && *p == *t; ++p, ++t)
				{}

			if (p == pathEnd && !*t)
				{
				// inPath...pathEnd matched (*i)->fName and they're
				// the same length, so we've got a match.

				switch (*pathEnd)
					{
					case 0:
						{
						// pathEnd is the end of the whole of inPath, so we've found our leaf.
						return fAssetTree->UseRep(*i);
						}
					case '|':
						{
						// We found a pipe, so we've got another component
						ZRef<ZAssetRep_Std> theChild = fAssetTree->UseRep(*i);
						return theChild->ResolvePath(pathEnd + 1);
						}
					case '^':
						{
						// We found a child, but its name is followed by a hat,
						// indicating that we should move up to the child's parent,
						// which is us.
						return this->ResolvePath(pathEnd + 1);
						}
					default:
						{
						// Can't get here.
						ZUnimplemented();
						}
					}
				}
			}
		}
	return ZRef<ZAssetRep>();
	}

ZRef<ZAssetIterRep> ZAssetRep_Std_Directory::CreateIterRep()
	{
	if (fChildrenCount)
		return new ZAssetIterRep_Std_Directory(this, 0);
	return ZRef<ZAssetIterRep>();
	}

void ZAssetRep_Std_Directory::DoInitialization()
	{}

void ZAssetRep_Std_Directory::DoFinalization()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Union

ZAssetRep_Std_Union::ZAssetRep_Std_Union(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets)
:	ZAssetRep_Std(inParent, inName, inNameLength)
	{
	fResolved = false;
	size_t countOfPaths = sReadCount(inStream);
	for (size_t x = 0; x < countOfPaths; ++x)
		{
		uint32 pathNameIndex = sReadCount(inStream);
		fPaths.push_back(inNameTable + ZByteSwap_ReadBig32(inNameOffsets + pathNameIndex));
		}
	}

ZAssetRep_Std_Union::~ZAssetRep_Std_Union()
	{}

ZRef<ZAssetRep> ZAssetRep_Std_Union::ResolvePath(const char* inPath)
	{
	if (ZRef<ZAssetRep> theRep = ZAssetRep::ResolvePath(inPath))
		return theRep;

	if (!fResolved)
		this->pResolvePaths();

	if (fResolvedReps.empty())
		return ZRef<ZAssetRep>();

	if (fResolvedReps.size() == 1)
		{
		if (ZRef<ZAssetRep> theRep = fResolvedReps.front()->ResolvePath(inPath))
			return new ZAssetRep_Overlay(this, theRep);
		return ZRef<ZAssetRep>();
		}

	vector<ZRef<ZAssetRep> > overlaidReps;
	overlaidReps.reserve(fResolvedReps.size());
	for (vector<ZRef<ZAssetRep> >::iterator i = fResolvedReps.begin(); i != fResolvedReps.end(); ++i)
		{
		if (ZRef<ZAssetRep> theRep = (*i)->ResolvePath(inPath))
			overlaidReps.push_back(theRep);
		}

	if (overlaidReps.empty())
		return ZRef<ZAssetRep>();

	return new ZAssetRep_Overlay(this, overlaidReps);
	}

ZRef<ZAssetIterRep> ZAssetRep_Std_Union::CreateIterRep()
	{
	if (!fResolved)
		this->pResolvePaths();

	if (fResolvedReps.empty())
		return ZRef<ZAssetIterRep>();

	return new ZAssetIterRep_Union(this, fResolvedReps);
	}

void ZAssetRep_Std_Union::DoInitialization()
	{
	ZAssertStop(2, !fResolved);
	}

void ZAssetRep_Std_Union::DoFinalization()
	{
	if (fResolved)
		{
		fResolved = false;
		fResolvedReps.clear();
		}
	}

void ZAssetRep_Std_Union::pResolvePaths()
	{
	ZMutexLocker locker(fMutex);
	ZAssertStop(2, this->GetRefCount() > 0);
	if (!fResolved)
		{
		fResolved = true;
		if (ZRef<ZAssetRep> parent = this->GetParentAssetRep())
			{
			// Our paths are sorted from lowest priority to highest, but when we resolve them
			// we store them from highest to lowest because when they're passed to ZAssetRep_Overlay
			// it is significantly simpler for it to use the rep on the front of the list rather than
			// the rep on the back.
			for (vector<const char*>::reverse_iterator i = fPaths.rbegin(); i != fPaths.rend(); ++i)
				{
				if (ZRef<ZAssetRep> resolvedRep = parent->ResolvePath(*i))
					fResolvedReps.push_back(resolvedRep);
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetIterRep_Std_Directory

ZAssetIterRep_Std_Directory::ZAssetIterRep_Std_Directory(ZRef<ZAssetRep_Std_Directory> iAssetRep, size_t iIndex)
:	fAssetRep(iAssetRep),
	fCurrent(iIndex)
	{
	ZAssertStop(2, fAssetRep);
	}

bool ZAssetIterRep_Std_Directory::HasValue()
	{ return fCurrent < fAssetRep->fChildrenCount; }

void ZAssetIterRep_Std_Directory::Advance()
	{ ++fCurrent; }

ZRef<ZAssetRep> ZAssetIterRep_Std_Directory::Current()
	{
	if (fCurrent < fAssetRep->fChildrenCount)
		{
		if (fAssetRep->fChildrenCount == 1)
			return fAssetRep->fAssetTree->UseRep(fAssetRep->fChild);

		return fAssetRep->fAssetTree->UseRep(fAssetRep->fChildren[fCurrent]);
		}

	return ZRef<ZAssetRep>();
	}

string ZAssetIterRep_Std_Directory::CurrentName()
	{
	if (fCurrent < fAssetRep->fChildrenCount)
		{
		if (fAssetRep->fChildrenCount == 1)
			return fAssetRep->fChild->fName;

		return fAssetRep->fChildren[fCurrent]->fName;
		}

	return string();
	}

ZRef<ZAssetIterRep> ZAssetIterRep_Std_Directory::Clone()
	{ return new ZAssetIterRep_Std_Directory(fAssetRep, fCurrent); }

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std

ZAssetTree_Std::ZAssetTree_Std()
	{}

ZAssetTree_Std::~ZAssetTree_Std()
	{}

ZAssetRep_Std* ZAssetTree_Std::LoadAssetRep(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream, const char* inNameTable, const size_t* inNameOffsets)
	{
	switch (inStream.ReadUInt8())
		{
		case 0:
			{
			return new ZAssetRep_Std_Directory(this, inParent, inName, inNameLength, inStream, inNameTable, inNameOffsets);
			}
		case 1:
			{
			return this->LoadAssetRep_Data(inParent, inName, inNameLength, inStream);
			}
		case 2:
			{
			return new ZAssetRep_Std_Union(inParent, inName, inNameLength, inStream, inNameTable, inNameOffsets);
			}
		}
	ZUnimplemented();
	return nil;
	}

ZRef<ZAssetRep_Std> ZAssetTree_Std::UseRep(ZAssetRep_Std* iRep)
	{
	ZAssertStop(2, iRep);
	ZMutexLocker locker(fMutex);
	ZRef<ZAssetRep_Std> theRep = iRep;
	if (iRep->GetRefCount() == 1)
		{
		iRep->fAssetTree = this;
		iRep->DoInitialization();
		}
	return theRep;
	}

void ZAssetTree_Std::RepBeingFinalized(ZAssetRep_Std* iRep)
	{
	ZMutexLocker locker(fMutex);
	if (iRep->GetRefCount() == 1)
		{
		// Have the rep do whatever it needs to do when finalized.
		iRep->DoFinalization();
		iRep->FinalizationComplete();

		// Lose the rep's reference to us, but take a ref to ourselves first.
		ZRef<ZAssetTree_Std> myRef = this;
		iRep->fAssetTree.Clear();

		// Release our locker now, so that when myRef goes out of scope,
		// and potentially takes *our* refcount to zero locker will not
		// try to release our mutex, which might no longer be there.
		locker.Release();
		}
	else
		{
		iRep->FinalizationComplete();
		locker.Release();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * StreamerMemory

namespace ZANONYMOUS {

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
#pragma mark -
#pragma mark * ZAssetRep_Std_Data_Stream

class ZAssetRep_Std_Data_Stream : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Data_Stream(ZAssetTree_Std_Stream* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);
	virtual ~ZAssetRep_Std_Data_Stream();

// From ZAssetRep via ZAssetRep_Std
	virtual ZRef<ZStreamerRPos> OpenRPos();
	virtual void GetData(const void** outData, size_t* outSize);

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	ZAssetTree_Std_Stream* fAssetTree_Std_Stream;
	size_t fDataSize;
	size_t fDataOffset;
	char* fData;
	ZMutex fMutex_GetData;
	};

ZAssetRep_Std_Data_Stream::ZAssetRep_Std_Data_Stream(ZAssetTree_Std_Stream* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
:	ZAssetRep_Std(inParent, inName, inNameLength)
	{
	fAssetTree_Std_Stream = inAssetTree;
	fDataOffset = 0;
	fDataSize = sReadCount(inStream);
	if (fDataSize)
		fDataOffset = inStream.ReadUInt32();
	fData = nil;
	}

ZAssetRep_Std_Data_Stream::~ZAssetRep_Std_Data_Stream()
	{
	ZAssertStop(2, !fData);
	}

ZRef<ZStreamerRPos> ZAssetRep_Std_Data_Stream::OpenRPos()
	{
	// If we have loaded our data, then return a streamer that reads from it.
	if (fData)
		return new StreamerMemory(this, fData, fDataSize);

	// Otherwise return a streamer that accesses the store's underlying ZStreamRPos.
	return fAssetTree_Std_Stream->OpenRPos(fDataOffset, fDataSize);
	}

void ZAssetRep_Std_Data_Stream::GetData(const void** outData, size_t* outSize)
	{
	if (outSize)
		*outSize = fDataSize;

	if (outData)
		{
		if (!fData)
			{
			ZMutexLocker locker(fMutex_GetData);
			if (!fData)
				{
				fData = new char[fDataSize];
				fAssetTree_Std_Stream->ReadData(fDataOffset, fDataSize, fData);
				}
			}
		*outData = fData;
		}
	}

void ZAssetRep_Std_Data_Stream::DoInitialization()
	{
	ZAssertStop(2, !fData);
	}

void ZAssetRep_Std_Data_Stream::DoFinalization()
	{
	delete[] fData;
	fData = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Stream::StreamerRPos

class ZAssetTree_Std_Stream::StreamerRPos : public ZStreamerRPos,
								private ZStreamRPos
	{
public:
	StreamerRPos(const ZRef<ZAssetTree_Std_Stream>& inAssetTree, uint64 inStartOffset, size_t inSize);

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* inDest, size_t inCount, size_t* outCountRead);
	virtual void Imp_Skip(uint64 inCount, uint64* outCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();

private:
	ZRef<ZAssetTree_Std_Stream> fAssetTree;
	uint64 fStartOffset;
	uint64 fPosition;
	size_t fSize;
	};

ZAssetTree_Std_Stream::StreamerRPos::StreamerRPos(const ZRef<ZAssetTree_Std_Stream>& inAssetTree, uint64 inStartOffset, size_t inSize)
:	fAssetTree(inAssetTree),
	fStartOffset(inStartOffset),
	fPosition(0),
	fSize(inSize)
	{}

const ZStreamRPos& ZAssetTree_Std_Stream::StreamerRPos::GetStreamRPos()
	{ return *this; }

void ZAssetTree_Std_Stream::StreamerRPos::Imp_Read(void* inDest, size_t inCount, size_t* outCountRead)
	{
	size_t countToRead = min(uint64(inCount), sDiffPosR(fSize, fPosition));
	fAssetTree->ReadData(fStartOffset + fPosition, countToRead, inDest);
	fPosition += countToRead;
	if (outCountRead)
		*outCountRead = countToRead;
	}

void ZAssetTree_Std_Stream::StreamerRPos::Imp_Skip(uint64 inCount, uint64* outCountSkipped)
	{
	uint64 countToSkip = min(inCount, sDiffPosR(fSize, fPosition));
	fPosition += countToSkip;
	if (outCountSkipped)
		*outCountSkipped = countToSkip;
	}

uint64 ZAssetTree_Std_Stream::StreamerRPos::Imp_GetPosition()
	{ return fPosition; }

void ZAssetTree_Std_Stream::StreamerRPos::Imp_SetPosition(uint64 inPosition)
	{ fPosition = inPosition; }

uint64 ZAssetTree_Std_Stream::StreamerRPos::Imp_GetSize()
	{ return fSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Stream

ZAssetTree_Std_Stream::ZAssetTree_Std_Stream()
	{
	fStream = nil;
	fNameTable = nil;
	fRoot = nil;
	}

ZAssetTree_Std_Stream::ZAssetTree_Std_Stream(const ZStreamRPos& inStream, size_t inOffset, size_t inSize)
	{
	fStream = nil;
	fNameTable = nil;
	fRoot = nil;
	this->LoadUp(&inStream, inOffset, inSize);
	}

ZAssetTree_Std_Stream::~ZAssetTree_Std_Stream()
	{
	ZAssertStop(2, (fStream && fRoot && fNameTable) || (!fStream && !fRoot && !fNameTable));
	if (fStream)
		this->ShutDown();
	}

ZRef<ZAssetRep> ZAssetTree_Std_Stream::GetRoot()
	{ return this->UseRep(fRoot); }

ZAssetRep_Std* ZAssetTree_Std_Stream::LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
	{ return new ZAssetRep_Std_Data_Stream(this, inParent, inName, inNameLength, inStream); }

void ZAssetTree_Std_Stream::LoadUp(const ZStreamRPos* inStream, size_t inOffset, size_t inSize)
	{
	ZAssertStop(2, !fStream && !fNameTable && !fRoot);
	ZAssertStop(1, inSize != 0); // Minimal sanity check on size

	fStream = inStream;
	try
		{
		// Check that the stream is big enough to hold the magic text and header size
		size_t trailerSize = sizeof(uint32) + sMagicTextSize;
		if (inSize < trailerSize)
			sThrowBadFormat();

		// Get the header size and check the magic text
		fStream->SetPosition(inOffset + inSize - trailerSize);

		uint32 headerSize = fStream->ReadUInt32();

		char checkedText[sMagicTextSize];
		fStream->Read(checkedText, sMagicTextSize);
		if (0 != memcmp(checkedText, sMagicText, sMagicTextSize))
			sThrowBadFormat();

		// Move back to the beginning of the header
		fStream->SetPosition(inOffset + inSize - trailerSize - headerSize);

		// Read in the name table offsets. We suck in the data en masse for two reasons. First,
		// reading one offset at a time is slow. Second, we share the implementation of ZAssetRep_Std_Directory
		// with ZAssetTree_Std_Memory, and it's simpler to have non-conditional code in the directory
		// parsing stuff -- it always swaps from big-endian to host-endian as it interprets the
		// name table offsets.
		size_t nameCount = fStream->ReadUInt32();
		vector<size_t> nameOffsets(nameCount + 1);
		fStream->Read(&nameOffsets[0], sizeof(uint32) * (nameCount + 1));

		// Suck in the whole name table
		size_t nameTableSize = ZByteSwap_BigToHost32(nameOffsets.back());
		fNameTable = new char[nameTableSize];
		fStream->Read(fNameTable, nameTableSize);

		// Pull in the tree
		fRoot = this->LoadAssetRep(nil, sEmptyString, 0, *fStream, fNameTable, &nameOffsets[0]);

		// Remember where the start of the data is
		fDataOffset = inOffset;
		}
	catch (...)
		{
		delete[] fNameTable;
		fNameTable = nil;
		delete fRoot;
		fRoot = nil;
		fStream = nil;
		throw;
		}
	}

void ZAssetTree_Std_Stream::ShutDown()
	{
	ZAssertStop(2, fStream && fRoot && fNameTable);
	delete fRoot;
	fRoot = nil;
	delete[] fNameTable;
	fNameTable = nil;
	fStream = nil;
	}

ZRef<ZStreamerRPos> ZAssetTree_Std_Stream::OpenRPos(size_t inOffset, size_t inSize)
	{ return new StreamerRPos(this, inOffset, inSize); }

void ZAssetTree_Std_Stream::ReadData(size_t inOffset, size_t inSize, void* outData)
	{
	ZMutexLocker locker(fMutex);
	fStream->SetPosition(fDataOffset + inOffset);
	size_t countRead;
	fStream->Read(outData, inSize, &countRead);
	// It should not be possible to hit the end of stream, unless it gets truncated or was
	// badly formed to start with. In either case the programmer is at fault.
	ZAssertStop(2, countRead == inSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Streamer

ZAssetTree_Std_Streamer::ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> inStreamer)
:	fStreamer(inStreamer)
	{
	ZAssertStop(2, fStreamer);
	this->LoadUp(&fStreamer->GetStreamRPos(), 0, fStreamer->GetStreamRPos().GetSize());
	}

ZAssetTree_Std_Streamer::ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> inStreamer, size_t inOffset, size_t inSize)
:	fStreamer(inStreamer)
	{
	ZAssertStop(2, fStreamer);
	this->LoadUp(&fStreamer->GetStreamRPos(), inOffset, inSize);
	}

ZAssetTree_Std_Streamer::~ZAssetTree_Std_Streamer()
	{
	this->ShutDown();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Data_File

class ZAssetRep_Std_Data_File : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Data_File(ZAssetTree_Std_File* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);
	virtual ~ZAssetRep_Std_Data_File();

// From ZAssetRep via ZAssetRep_Std;
	virtual ZRef<ZStreamerRPos> OpenRPos();
	virtual void GetData(const void** outData, size_t* outSize);

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	ZAssetTree_Std_File* fAssetTree_Std_File;
	size_t fDataSize;
	size_t fDataOffset;
	char* fData;
	ZMutex fMutex_GetData;
	};

ZAssetRep_Std_Data_File::ZAssetRep_Std_Data_File(ZAssetTree_Std_File* inAssetTree, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
:	ZAssetRep_Std(inParent, inName, inNameLength)
	{
	fAssetTree_Std_File = inAssetTree;
	fDataOffset = 0;
	fDataSize = sReadCount(inStream);
	if (fDataSize)
		fDataOffset = inStream.ReadUInt32();
	fData = nil;
	}

ZAssetRep_Std_Data_File::~ZAssetRep_Std_Data_File()
	{
	ZAssertStop(2, !fData);
	}

ZRef<ZStreamerRPos> ZAssetRep_Std_Data_File::OpenRPos()
	{
	// If we have loaded our data, then return a streamer that reads from it.
	if (fData)
		return new StreamerMemory(this, fData, fDataSize);

	// Otherwise return a streamer that accesses the store's underlying ZStreamRPos.
	return fAssetTree_Std_File->OpenRPos(fDataOffset, fDataSize);
	}

void ZAssetRep_Std_Data_File::GetData(const void** outData, size_t* outSize)
	{
	if (outSize)
		*outSize = fDataSize;

	if (outData)
		{
		if (!fData)
			{
			ZMutexLocker locker(fMutex_GetData);
			if (!fData)
				{
				fData = new char[fDataSize];
				fAssetTree_Std_File->ReadData(fDataOffset, fDataSize, fData);
				}
			}
		*outData = fData;
		}
	}

void ZAssetRep_Std_Data_File::DoInitialization()
	{
	ZAssertStop(2, !fData);
	}

void ZAssetRep_Std_Data_File::DoFinalization()
	{
	delete[] fData;
	fData = nil;
	}


// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_File::StreamerRPos

class ZAssetTree_Std_File::StreamerRPos : public ZStreamerRPos,
								private ZStreamRPos
	{
public:
	StreamerRPos(const ZRef<ZFileR>& inFile, uint64 inStartOffset, size_t inSize);

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* inDest, size_t inCount, size_t* outCountRead);
	virtual void Imp_Skip(uint64 inCount, uint64* outCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();

private:
	ZRef<ZFileR> fFile;
	uint64 fStartOffset;
	uint64 fPosition;
	size_t fSize;
	};

ZAssetTree_Std_File::StreamerRPos::StreamerRPos(const ZRef<ZFileR>& inFile, uint64 inStartOffset, size_t inSize)
:	fFile(inFile),
	fStartOffset(inStartOffset),
	fPosition(0),
	fSize(inSize)
	{}

const ZStreamRPos& ZAssetTree_Std_File::StreamerRPos::GetStreamRPos()
	{ return *this; }

void ZAssetTree_Std_File::StreamerRPos::Imp_Read(void* inDest, size_t inCount, size_t* outCountRead)
	{
	size_t countToRead = min(uint64(inCount), sDiffPosR(fSize, fPosition));
	size_t countRead;
	fFile->ReadAt(fStartOffset + fPosition, inDest, countToRead, &countRead);
	fPosition += countRead;
	if (outCountRead)
		*outCountRead = countRead;
	}

void ZAssetTree_Std_File::StreamerRPos::Imp_Skip(uint64 inCount, uint64* outCountSkipped)
	{
	uint64 countToSkip = min(uint64(inCount), sDiffPosR(fSize, fPosition));
	fPosition += countToSkip;
	if (outCountSkipped)
		*outCountSkipped = countToSkip;
	}

uint64 ZAssetTree_Std_File::StreamerRPos::Imp_GetPosition()
	{ return fPosition; }

void ZAssetTree_Std_File::StreamerRPos::Imp_SetPosition(uint64 inPosition)
	{ fPosition = inPosition; }

uint64 ZAssetTree_Std_File::StreamerRPos::Imp_GetSize()
	{ return fSize; }

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_File

ZAssetTree_Std_File::ZAssetTree_Std_File(ZRef<ZFileR> inFile, uint64 inOffset, size_t inSize)
	{
	fNameTable = nil;
	fRoot = nil;

	ZAssertStop(1, inSize != 0); // Minimal sanity check on size

	try
		{
		// Check that the stream is big enough to hold the magic text and header size
		size_t trailerSize = sizeof(uint32) + sMagicTextSize;
		if (inSize < trailerSize)
			sThrowBadFormat();

		ZRef<StreamerRPos> theStreamer = new StreamerRPos(inFile, inOffset, inSize);
		const ZStreamRPos& theStream = theStreamer->GetStreamRPos();

		// Get the header size and check the magic text
		theStream.SetPosition(inSize - trailerSize);

		uint32 headerSize = theStream.ReadUInt32();

		char checkedText[sMagicTextSize];
		theStream.Read(checkedText, sMagicTextSize);
		if (0 != memcmp(checkedText, sMagicText, sMagicTextSize))
			sThrowBadFormat();

		// Move back to the beginning of the header
		theStream.SetPosition(inSize - trailerSize - headerSize);

		// Read in the name table offsets. We suck in the data en masse for two reasons. First,
		// reading one offset at a time is slow. Second, we share the implementation of ZAssetRep_Std_Directory
		// with ZAssetTree_Std_Memory, and it's simpler to have non-conditional code in the directory
		// parsing stuff -- it always swaps from big-endian to host-endian as it interprets the
		// name table offsets.
		size_t nameCount = theStream.ReadUInt32();
		vector<size_t> nameOffsets(nameCount + 1);
		theStream.Read(&nameOffsets[0], sizeof(uint32) * (nameCount + 1));

		// Suck in the whole name table
		size_t nameTableSize = ZByteSwap_BigToHost32(nameOffsets.back());
		fNameTable = new char[nameTableSize];
		theStream.Read(fNameTable, nameTableSize);

		// Pull in the tree
		fRoot = this->LoadAssetRep(nil, sEmptyString, 0, theStream, fNameTable, &nameOffsets[0]);

		// Remember where the start of the data is
		fDataOffset = inOffset;
		}
	catch (...)
		{
		delete[] fNameTable;
		fNameTable = nil;
		delete fRoot;
		fRoot = nil;
		throw;
		}
	}

ZAssetTree_Std_File::~ZAssetTree_Std_File()
	{
	ZAssertStop(2, (fRoot && fNameTable) || (!fRoot && !fNameTable));
	delete fRoot;
	fRoot = nil;
	delete[] fNameTable;
	fNameTable = nil;
	}

ZRef<ZAssetRep> ZAssetTree_Std_File::GetRoot()
	{ return this->UseRep(fRoot); }

ZAssetRep_Std* ZAssetTree_Std_File::LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
	{ return new ZAssetRep_Std_Data_File(this, inParent, inName, inNameLength, inStream); }

ZRef<ZStreamerRPos> ZAssetTree_Std_File::OpenRPos(uint64 inOffset, size_t inSize)
	{ return new StreamerRPos(fFile, inOffset, inSize); }

void ZAssetTree_Std_File::ReadData(uint64 inOffset, size_t inSize, void* outData)
	{ fFile->ReadAt(inOffset, outData, inSize, nil); }

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetRep_Std_Data_Memory

class ZAssetRep_Std_Data_Memory : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Data_Memory(const char* inBaseAddress, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream);
	virtual ~ZAssetRep_Std_Data_Memory();

// From ZAssetRep via ZAssetRep_Std;
	virtual ZRef<ZStreamerRPos> OpenRPos();
	virtual void GetData(const void** outData, size_t* outSize);

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	size_t fDataSize;
	const char* fDataAddress;
	};

ZAssetRep_Std_Data_Memory::ZAssetRep_Std_Data_Memory(const char* inBaseAddress, ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
:	ZAssetRep_Std(inParent, inName, inNameLength)
	{
	fDataSize = sReadCount(inStream);
	if (fDataSize)
		fDataAddress = inBaseAddress + inStream.ReadUInt32();
	else
		fDataAddress = nil;
	}

ZAssetRep_Std_Data_Memory::~ZAssetRep_Std_Data_Memory()
	{}

ZRef<ZStreamerRPos> ZAssetRep_Std_Data_Memory::OpenRPos()
	{ return new StreamerMemory(this, fDataAddress, fDataSize); }

void ZAssetRep_Std_Data_Memory::GetData(const void** outData, size_t* outSize)
	{
	if (outSize)
		*outSize = fDataSize;
	if (outData)
		*outData = fDataAddress;
	}

void ZAssetRep_Std_Data_Memory::DoInitialization()
	{}

void ZAssetRep_Std_Data_Memory::DoFinalization()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Memory

ZAssetTree_Std_Memory::ZAssetTree_Std_Memory()
	{
	fAddress = nil;
	fRoot = nil;
	}

ZAssetTree_Std_Memory::ZAssetTree_Std_Memory(const void* inAddress, size_t inSize)
	{
	fAddress = nil;
	fRoot = nil;
	this->LoadUp(inAddress, inSize);
	}

ZAssetTree_Std_Memory::~ZAssetTree_Std_Memory()
	{
	ZAssertStop(2, (fAddress && fRoot) || (!fAddress && !fRoot));
	if (fAddress)
		this->ShutDown();
	}

ZRef<ZAssetRep> ZAssetTree_Std_Memory::GetRoot()
	{ return this->UseRep(fRoot); }

ZAssetRep_Std* ZAssetTree_Std_Memory::LoadAssetRep_Data(ZAssetRep_Std_Directory* inParent, const char* inName, size_t inNameLength, const ZStreamRPos& inStream)
	{ return new ZAssetRep_Std_Data_Memory(fAddress, inParent, inName, inNameLength, inStream); }

void ZAssetTree_Std_Memory::LoadUp(const void* inAddress, size_t inSize)
	{
	ZAssertStop(2, !fAddress && !fRoot);
	ZAssertStop(1, inSize != 0); // Minimal sanity check on size

	fAddress = reinterpret_cast<const char*>(inAddress);

	try
		{
		// Ensure that the address is aligned at a 32 bit boundary
		ZAssertStop(2, (reinterpret_cast<uint32>(fAddress) & 3) == 0);

		// We use a stream to parse some of the header info in memory, and to make it easier
		// to determine the address of the various table boundaries.
		ZStreamRPos_Memory theSIM(fAddress, inSize);

		// Check that the stream is big enough to hold the magic text and header size
		size_t trailerSize = sizeof(int32) + sMagicTextSize;
		if (inSize < trailerSize)
			sThrowBadFormat();

		// Get the header size and check the magic text
		theSIM.SetPosition(inSize - trailerSize);

		uint32 headerSize = theSIM.ReadUInt32();

		char checkedText[sMagicTextSize];
		theSIM.Read(checkedText, sMagicTextSize);
		if (0 != memcmp(checkedText, sMagicText, sMagicTextSize))
			sThrowBadFormat();

		// Move back to the beginning of the header
		theSIM.SetPosition(inSize - trailerSize - headerSize);

		// Get the count of names. The count of offsets is one greater than that (to account
		// for the zero offsets at the beginning).
		size_t nameCount = theSIM.ReadUInt32();

		// Get the address of the name table offsets.
		const size_t* nameOffsets = reinterpret_cast<const size_t*>(theSIM.GetCurrentAddress());
		theSIM.Skip(sizeof(uint32) * (nameCount + 1));

		// The table of names itself
		const char* nameTable = reinterpret_cast<const char*>(theSIM.GetCurrentAddress());

		// Move the stream past the table of names
		theSIM.Skip(ZByteSwap_ReadBig32(nameOffsets + nameCount));

		// Pull in the tree
		fRoot = this->LoadAssetRep(nil, sEmptyString, 0, theSIM, nameTable, &nameOffsets[0]);
		}
	catch (...)
		{
		delete fRoot;
		fAddress = nil;
		fRoot = nil;
		throw;
		}
	}

void ZAssetTree_Std_Memory::ShutDown()
	{
	ZAssertStop(2, fAddress && fRoot);
	delete fRoot;
	fAddress = nil;
	fRoot = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZAssetTree_Std_Memory_StaticData

ZAssetTree_Std_Memory_StaticData::ZAssetTree_Std_Memory_StaticData(const void* inAddress, size_t inSize)
	{
	ZAssetTree_Std_Memory::LoadUp(inAddress, inSize);
	}

ZAssetTree_Std_Memory_StaticData::~ZAssetTree_Std_Memory_StaticData()
	{
	ZAssetTree_Std_Memory::ShutDown();
	}
