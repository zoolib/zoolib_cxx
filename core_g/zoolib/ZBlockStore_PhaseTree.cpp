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

#include "zoolib/Memory.h"
//#include "zoolib/ZStream_Memory.h"

#include "zoolib/ZBlockStore_PhaseTree.h"
#include "zoolib/ZByteSwap.h"
#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#include <cstdio>

using std::lower_bound;
using std::upper_bound;
using std::map;
using std::min;
using std::set;
using std::vector;

namespace ZooLib {

using namespace ZUtil_STL;

#ifndef kDebug_PhaseTreeFile
	#define kDebug_PhaseTreeFile 3
#endif

#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
	#define PT_INLINE
	#define ASSERT_LOCKED(a) ZAssertStop(0, a.IsLocked())
	#define ASSERT_UNLOCKED(a) ZAssertStop(0, !a.IsLocked())
#else
	#define PT_INLINE inline
	#define ASSERT_LOCKED(a)
	#define ASSERT_UNLOCKED(a)
#endif

/**
\class ZBlockStore_PhaseTree

ZBlockStore_PhaseTree implements the ZBlockStore protocol, keeping its data in the instance
of ZFileR or ZFileRW passed to its constructor. Note that you don't have to pass a file,
just something that implements the ZFileR or ZFileRW protocols.

The entire data structure is a tree, although changes are made both in memory and on disk
those changes are not committed until all inferior nodes are safely written to disk
and then the root node written.
<http://slashdot.org/features/00/10/13/2117258.shtml>
*/

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Layout of nodes.

/*
Note that all fields in nodes are 32 bits in size, and are kept in big-endian order.

# of words     Description

	Internal Index Node
1              Count. This is the number of _keys_.
count          Keys.
count + 1      Child slots (other index slots)
count + 1      Child subtree sizes

	Leaf Index Node
1              Count
count          Keys
count          Child slots (block slots this time)
*/

// =================================================================================================

struct BigLessNative
	{
#if ZCONFIG(Endian, Big)
	PT_INLINE bool operator()(uint32 a, uint32 b)
		{ return a < b; }
#else
	PT_INLINE bool operator()(uint32 iBig, uint32 iNative)
		{ return ZByteSwap_BigToHost32(iBig) < iNative; }
#endif
	};

struct NativeLessBig
	{
#if ZCONFIG(Endian, Big)
	PT_INLINE bool operator()(uint32 a, uint32 b)
		{ return a < b; }
#else
	PT_INLINE bool operator()(uint32 iNative, uint32 iBig)
		{ return iNative < ZByteSwap_BigToHost32(iBig); }
#endif
	};

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree::Slot

class ZBlockStore_PhaseTree::Slot
	{
public:
	Slot(uint32 iSlotNumber, bool iForked);
	Slot(uint32 iSlotNumber);
	~Slot();

	void* operator new(size_t iObjectSize, size_t iSlotSize);
	void operator delete(void* iPtr);

protected:
	friend class ZBlockStore_PhaseTree;

	ZMtx fMutex;

	Slot* fCached_Prev;
	Slot* fCached_Next;

	uint32 fSlotNumber;
	int fUseCount;
	EState fState;
	char fData[1];
	};

ZBlockStore_PhaseTree::Slot::Slot(uint32 iSlotNumber, bool iForked)
:	fCached_Prev(nullptr),
	fCached_Next(nullptr),
	fSlotNumber(iSlotNumber),
	fUseCount(1),
	fState(iForked ? eStateForked : eStateClean)
	{}

ZBlockStore_PhaseTree::Slot::Slot(uint32 iSlotNumber)
:	fCached_Prev(nullptr),
	fCached_Next(nullptr),
	fSlotNumber(iSlotNumber),
	fUseCount(1),
	fState(eStateDirty)
	{}

ZBlockStore_PhaseTree::Slot::~Slot()
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fUseCount == 0);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fCached_Prev == nullptr);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fCached_Next == nullptr);
	}

void* ZBlockStore_PhaseTree::Slot::operator new(size_t iObjectSize, size_t iSlotSize)
	{
	return new char[iObjectSize - 1 + iSlotSize];
	}

void ZBlockStore_PhaseTree::Slot::operator delete(void* iPtr)
	{
	delete[] static_cast<char*>(iPtr);
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree::StreamRPos

class ZBlockStore_PhaseTree::StreamRPos : public ZStreamRPos
	{
public:
	StreamRPos(ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot);
	~StreamRPos();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	ZBlockStore_PhaseTree* fBlockStore;
	BlockID fBlockID;
	Slot* fBlockSlot;
	uint64 fPosition;
	};

ZBlockStore_PhaseTree::StreamRPos::StreamRPos(
	ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot)
	{
	fBlockStore = iBlockStore;
	fBlockID = iBlockID;
	fBlockSlot = iBlockSlot;
	fPosition = 0;
	}

ZBlockStore_PhaseTree::StreamRPos::~StreamRPos()
	{
	fBlockStore->StreamDisposing(fBlockSlot);
	}

void ZBlockStore_PhaseTree::StreamRPos::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fBlockStore->ReadFromBlock(fBlockID, fBlockSlot, fPosition, oDest, iCount, countRead);
	if (oCountRead)
		*oCountRead = countRead;
	fPosition += countRead;
	}

size_t ZBlockStore_PhaseTree::StreamRPos::Imp_CountReadable()
	{ return ZStream::sClampedSize(fBlockStore->GetBlockSize(fBlockID, fBlockSlot), fPosition); }

uint64 ZBlockStore_PhaseTree::StreamRPos::Imp_GetPosition()
	{ return fPosition; }

void ZBlockStore_PhaseTree::StreamRPos::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZBlockStore_PhaseTree::StreamRPos::Imp_GetSize()
	{ return fBlockStore->GetBlockSize(fBlockID, fBlockSlot); }

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree::StreamerRPos

class ZBlockStore_PhaseTree::StreamerRPos : public ZStreamerRPos
	{
public:
	StreamerRPos(ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot);
	virtual ~StreamerRPos();

// From ZStreamerRPos
	virtual ZStreamRPos& GetStreamRPos();

protected:
	ZBlockStore_PhaseTree::StreamRPos fStream;
	};

ZBlockStore_PhaseTree::StreamerRPos::StreamerRPos(
	ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot)
:	fStream(iBlockStore, iBlockID, iBlockSlot)
	{}

ZBlockStore_PhaseTree::StreamerRPos::~StreamerRPos()
	{}

ZStreamRPos& ZBlockStore_PhaseTree::StreamerRPos::GetStreamRPos()
	{ return fStream; }

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree::StreamRWPos

class ZBlockStore_PhaseTree::StreamRWPos : public ZStreamRWPos
	{
public:
	StreamRWPos(ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot);
	~StreamRWPos();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize);

private:
	ZBlockStore_PhaseTree* fBlockStore;
	BlockID fBlockID;
	Slot* fBlockSlot;
	size_t fPosition;
	};

ZBlockStore_PhaseTree::StreamRWPos::StreamRWPos(
	ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot)
	{
	fBlockStore = iBlockStore;
	fBlockID = iBlockID;
	fBlockSlot = iBlockSlot;
	fPosition = 0;
	}

ZBlockStore_PhaseTree::StreamRWPos::~StreamRWPos()
	{
	fBlockStore->StreamDisposing(fBlockSlot);
	}

void ZBlockStore_PhaseTree::StreamRWPos::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fBlockStore->ReadFromBlock(fBlockID, fBlockSlot, fPosition, oDest, iCount, countRead);
	if (oCountRead)
		*oCountRead = countRead;
	fPosition += countRead;
	}

void ZBlockStore_PhaseTree::StreamRWPos::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	fBlockStore->WriteToBlock(fBlockID, fBlockSlot, fPosition, iSource, iCount, countWritten);
	if (oCountWritten)
		*oCountWritten = countWritten;
	fPosition += countWritten;
	}

void ZBlockStore_PhaseTree::StreamRWPos::Imp_Flush()
	{
	// The general assumption with flush is that it provides a reasonable guarantee that
	// changes have been committed to safe storage, or passed out of any performance-enhancing
	// buffer onto the ultimate destination. But the flush operation on a phase tree is much
	// more heavyweight than that, and so the phase tree stream does nothing -- someone needs
	// to call the block store's flush method at some appropriate frequency.
	}

size_t ZBlockStore_PhaseTree::StreamRWPos::Imp_CountReadable()
	{ return ZStream::sClampedSize(fBlockStore->GetBlockSize(fBlockID, fBlockSlot), fPosition); }

uint64 ZBlockStore_PhaseTree::StreamRWPos::Imp_GetPosition()
	{ return fPosition; }

void ZBlockStore_PhaseTree::StreamRWPos::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZBlockStore_PhaseTree::StreamRWPos::Imp_GetSize()
	{ return fBlockStore->GetBlockSize(fBlockID, fBlockSlot); }

void ZBlockStore_PhaseTree::StreamRWPos::Imp_SetSize(uint64 iSize)
	{
	if (iSize > 0xFFFFFFFFU)
		sThrowBadSize();

	fBlockStore->SetBlockSize(fBlockID, fBlockSlot, iSize);
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree::StreamerRWPos

class ZBlockStore_PhaseTree::StreamerRWPos : public ZStreamerRWPos
	{
public:
	StreamerRWPos(ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot);
	virtual ~StreamerRWPos();

// From ZStreamerRWPos
	virtual ZStreamRWPos& GetStreamRWPos();

protected:
	ZBlockStore_PhaseTree::StreamRWPos fStream;
	};

ZBlockStore_PhaseTree::StreamerRWPos::StreamerRWPos(
	ZBlockStore_PhaseTree* iBlockStore, BlockID iBlockID, Slot* iBlockSlot)
:	fStream(iBlockStore, iBlockID, iBlockSlot)
	{}

ZBlockStore_PhaseTree::StreamerRWPos::~StreamerRWPos()
	{}

ZStreamRWPos& ZBlockStore_PhaseTree::StreamerRWPos::GetStreamRWPos()
	{ return fStream; }

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Potentially inlined low level field access.

PT_INLINE uint32 ZBlockStore_PhaseTree::ReadField(Slot* iSlot, size_t iOffset)
	{
	ASSERT_LOCKED(iSlot->fMutex);

	#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
		fMutex_Slots.Acquire();

		ZAssertStop(ZCONFIG_PhaseTree_Debug,
			iSlot->fState != eStateReleasedForked && iSlot->fState != eStateReleasedClean);

		fMutex_Slots.Release();
	#endif

	return ZByteSwap_ReadBig32(reinterpret_cast<uint32*>(iSlot->fData) + iOffset);
	}

PT_INLINE uint32* ZBlockStore_PhaseTree::FieldAddress(Slot* iSlot, size_t iOffset)
	{
	ASSERT_LOCKED(iSlot->fMutex);

	#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
		fMutex_Slots.Acquire();

		ZAssertStop(ZCONFIG_PhaseTree_Debug,
			iSlot->fState != eStateReleasedForked && iSlot->fState != eStateReleasedClean);

		fMutex_Slots.Release();
	#endif

	return reinterpret_cast<uint32*>(iSlot->fData) + iOffset;
	}

PT_INLINE void ZBlockStore_PhaseTree::WriteField(Slot* iSlot, size_t iOffset, uint32 iValue)
	{
	ASSERT_LOCKED(iSlot->fMutex);

	#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
		fMutex_Slots.Acquire();
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlot->fState == eStateDirty);
		fMutex_Slots.Release();
	#endif

	ZByteSwap_WriteBig32(reinterpret_cast<uint32*>(iSlot->fData) + iOffset, iValue);
	}

PT_INLINE void ZBlockStore_PhaseTree::Move(Slot* iSlot, size_t iSource, size_t iDest, size_t iCount)
	{
	ASSERT_LOCKED(iSlot->fMutex);

	#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
		fMutex_Slots.Acquire();
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlot->fState == eStateDirty);
		fMutex_Slots.Release();
	#endif

	uint32* start = reinterpret_cast<uint32*>(iSlot->fData);
	sMemMove(start + iDest, start + iSource, sizeof(uint32) * iCount);
	}

PT_INLINE void ZBlockStore_PhaseTree::Fill(Slot* iSlot, size_t iBegin, size_t iCount)
	{
	ASSERT_LOCKED(iSlot->fMutex);

	#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
		uint32* start = reinterpret_cast<uint32*>(iSlot->fData);
		sMemSet(start + iBegin, 0x55, sizeof(uint32) * iCount);
	#endif
	}

#if ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
#define VALIDATESLOT(a) this->ValidateSlot(a, __PRETTY_FUNCTION__, __LINE__)
void ZBlockStore_PhaseTree::ValidateSlot(Slot* iSlot, const char* iFunctionName, int iLineNumber)
	{
	size_t count = ReadField(iSlot, 0);
	for (size_t x = 2; x <= count; ++x)
		{
		BlockID currentKey = ReadField(iSlot, x);

		// AG 2001-11-29. This validation of the key's value only works for block stores where
		// you can be sure that all keys are >= 0x40000000 -- which was the case when I was
		// developing, but obviously is not any longer.

		// ZAssert(currentKey >= 0x40000000);

		BlockID priorKey = ReadField(iSlot, x - 1);
		ZAssertLogf(0, priorKey <= currentKey,
			("function: %s, line: %d, slot: %d priorKey: %06X, currentKey: %06X, count: %d, x:%d",
			iFunctionName, iLineNumber, iSlot->fSlotNumber, priorKey, currentKey, count, x));
		}
	}
#else // ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug
#define VALIDATESLOT(a)
inline void ZBlockStore_PhaseTree::ValidateSlot(
	Slot* iSlot, const char* iFunctionName, int iLineNumber)
	{}
#endif // ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Potentially inlined higher level field access.

// These methods are the ones that are used most
// heavily and thus that benefit most from being inlined.

PT_INLINE size_t ZBlockStore_PhaseTree::Index_GetCount(Slot* iSlot)
	{
	return ReadField(iSlot, 0);
	}

PT_INLINE bool ZBlockStore_PhaseTree::Index_IsFull(Slot* iSlot, bool iIsLeaf)
	{
	const size_t count = this->Index_GetCount(iSlot);
	if (iIsLeaf)
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, count <= fCapacity_IndexLeaf);
		return count == fCapacity_IndexLeaf;
		}
	else
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, count <= fCapacity_IndexInternal);
		return count == fCapacity_IndexInternal;
		}
	}

PT_INLINE ZBlockStore_PhaseTree::BlockID ZBlockStore_PhaseTree::Index_GetKey(
	Slot* iSlot, bool iIsLeaf, size_t iOffset)
	{
	if (iIsLeaf)
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
	else
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
	return ReadField(iSlot, 1 + iOffset);
	}

PT_INLINE uint32* ZBlockStore_PhaseTree::Index_GetKeysAddress(Slot* iSlot)
	{
	return FieldAddress(iSlot, 1);
	}

PT_INLINE uint32 ZBlockStore_PhaseTree::Index_GetChild(Slot* iSlot, bool iIsLeaf, size_t iOffset)
	{
	if (iIsLeaf)
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
		return ReadField(iSlot, 1 + fCapacity_IndexLeaf + iOffset);
		}
	else
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset <= ReadField(iSlot, 0));
		return ReadField(iSlot, 1 + fCapacity_IndexInternal + iOffset);
		}
	}

PT_INLINE uint32 ZBlockStore_PhaseTree::Index_GetSubTreeSize(Slot* iSlot, size_t iOffset)
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset <= ReadField(iSlot, 0));
	return ReadField(iSlot, 2 + 2 * fCapacity_IndexInternal + iOffset);
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree

// We embed CRLF, CR and LF into the magic text, if the file passes through
// a line-ending transforming gateway of some kind one or other (or all) of
// the endings will get munged and the magic text will be messed up.
static const char spMagicText[] = "PT1.0 CRLF\r\nCR\rLF\n";

static void spThrowBadFormat()
	{
	throw std::runtime_error("ZBlockStore_PhaseTree, bad format");
	}

/*
Layout of header

fUserHeaderSize bytes that are never touched.
the string defined by spMagicText (including the terminating zero.)
uint32 -- slot size
uint32 -- highwater
uint32 -- root slot number
uint32 -- index tree height
uint32 -- slot number of the head of the free list
uint32 -- count of number of slots in the free list
*/

ZBlockStore_PhaseTree::ZBlockStore_PhaseTree(
	ZRef<ZFileRW> iFile, size_t iSlotSize, size_t iUserHeaderSize)
:	fUserHeaderSize(iUserHeaderSize),
	fSlotSize(iSlotSize),
	fCount_Writes(0),
	fCount_WaitingFlushes(0),
	fFlushInProgress(false),
	fFlushInFirstStages(false),
	fRootSlotNumber(0),
	fHeight(0),
	fCached_Head(nullptr),
	fCached_Tail(nullptr),
	fHighWater(1),
	fAllocatedSpace(1),
	fUseRequests(0),
	fUseCached(0),
	fUseFailures(0),
	fFileRW(iFile),
	fFileR(iFile)
	{
	ZMACRO_ThreadSafe_Set(fStreamsInstantiated, 0);

	ZStreamRWPos_FileRW theStream(fFileRW);

	// We must allow for at least five keys in internal nodes because we do post-merge
	// on undersized nodes. With a minimum of three keys we don't have a wide enough
	// range of sizes to maintain our constraints. Five keys implies six children/sub tree
	// sizes, or 72 bytes.
	ZAssertStop(ZCONFIG_PhaseTree_Debug,
		iSlotSize >= sizeof(uint32) + (sizeof(uint32) * (5 + 6 + 6)));

	// Additionally slots must be large enough to accomodate the metaroot (43 bytes.)
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlotSize >= sizeof(spMagicText) + 6 * sizeof(uint32));

	fCapacity_BlockRoot_Byte = fSlotSize - sizeof(uint32);
	fCapacity_BlockRoot_Slots = (fSlotSize - sizeof(uint32)) / sizeof(uint32);

	fCapacity_IndexInternal = ((fSlotSize / sizeof(uint32)) / 3) - 1;
	// Force fCapacity_IndexInternal to be odd.
	fCapacity_IndexInternal = (((fCapacity_IndexInternal - 1) / 2) * 2) + 1;

	fCapacity_IndexLeaf = ((fSlotSize / sizeof(uint32)) - 1) / 2;
	// Force fCapacity_IndexLeaf to be odd.
	fCapacity_IndexLeaf = (((fCapacity_IndexLeaf - 1) / 2) * 2) + 1;

	fCapacity_Extent = fSlotSize / sizeof(uint32);
	fCapacity_Data = fSlotSize;

	theStream.SetSize(fUserHeaderSize + fSlotSize); // Truncate
	theStream.SetPosition(fUserHeaderSize);
	theStream.Write(spMagicText, sizeof(spMagicText));
	theStream.WriteUInt32(fSlotSize);
	theStream.WriteUInt32(fHighWater);
	theStream.WriteUInt32(fRootSlotNumber);
	theStream.WriteUInt32(fHeight);
	theStream.WriteUInt32(0); // Free list head slot number
	theStream.WriteUInt32(0); // Free list count
	theStream.Flush();
	}

ZBlockStore_PhaseTree::ZBlockStore_PhaseTree(ZRef<ZFileRW> iFile, size_t iUserHeaderSize)
:	fUserHeaderSize(iUserHeaderSize),
	fSlotSize(0),
	fCount_Writes(0),
	fCount_WaitingFlushes(0),
	fFlushInProgress(false),
	fFlushInFirstStages(false),
	fRootSlotNumber(0),
	fHeight(0),
	fCached_Head(nullptr),
	fCached_Tail(nullptr),
	fHighWater(0),
	fAllocatedSpace(0),
	fUseRequests(0),
	fUseCached(0),
	fUseFailures(0),
	fFileRW(iFile),
	fFileR(iFile)
	{
	ZMACRO_ThreadSafe_Set(fStreamsInstantiated, 0);

	ZAssertStop(1, fFileRW);

	ZStreamRWPos_FileRW theStream(fFileRW);

	if (theStream.GetSize() < fUserHeaderSize + sizeof(spMagicText) + 6 * sizeof(uint32))
		spThrowBadFormat();

	theStream.SetPosition(fUserHeaderSize);

	char dummy[sizeof(spMagicText)];
	theStream.Read(dummy, sizeof(spMagicText));
	if (0 != memcmp(dummy, spMagicText, sizeof(spMagicText)))
		spThrowBadFormat();

	fSlotSize = theStream.ReadUInt32();
	fHighWater = theStream.ReadUInt32();
	fAllocatedSpace = fHighWater;
	fRootSlotNumber = theStream.ReadUInt32();
	fHeight = theStream.ReadUInt32();

	// Any writes to the block store that were not flushed may (will) cause the
	// stream to be larger than what the header says it needs. However, it must
	// not have been made smaller.
	if (theStream.GetSize() < fUserHeaderSize + fSlotSize * fHighWater)
		spThrowBadFormat();
	theStream.SetSize(fUserHeaderSize + fSlotSize * fHighWater);

	uint32 freeListHeadSlotNumber = theStream.ReadUInt32();
	uint32 freeListCount = theStream.ReadUInt32();

	fCapacity_BlockRoot_Byte = fSlotSize - sizeof(uint32);
	fCapacity_BlockRoot_Slots = (fSlotSize - sizeof(uint32)) / sizeof(uint32);

	fCapacity_IndexInternal = ((fSlotSize / sizeof(uint32)) / 3) - 1;
	// Force fCapacity_IndexInternal to be odd.
	fCapacity_IndexInternal = (((fCapacity_IndexInternal - 1) / 2) * 2) + 1;

	fCapacity_IndexLeaf = ((fSlotSize / sizeof(uint32)) - 1) / 2;
	// Force fCapacity_IndexLeaf to be odd.
	fCapacity_IndexLeaf = (((fCapacity_IndexLeaf - 1) / 2) * 2) + 1;

	fCapacity_Extent = fSlotSize / sizeof(uint32);
	fCapacity_Data = fSlotSize;

	if (freeListHeadSlotNumber)
		{
		uint32 currentPage = freeListHeadSlotNumber;
		const uint32 slotsPerPage = fSlotSize / sizeof(uint32) - 1;
		while (true)
			{
			theStream.SetPosition(fUserHeaderSize + currentPage * fSlotSize);
			uint32 countInCurrent = min(freeListCount, slotsPerPage);
			for (size_t x = 0; x < countInCurrent; ++x)
				{
				uint32 theSlotNumber = theStream.ReadUInt32();
				fSlots_ReleasedForked.insert(theSlotNumber);
				}
			fSlots_ReleasedClean.insert(currentPage);
			if (countInCurrent < slotsPerPage)
				break;
			currentPage = theStream.ReadUInt32();
			freeListCount -= countInCurrent;
			}
		}
	}

ZBlockStore_PhaseTree::ZBlockStore_PhaseTree(ZRef<ZFileR> iFile, size_t iUserHeaderSize)
:	fUserHeaderSize(iUserHeaderSize),
	fSlotSize(0),
	fCount_Writes(0),
	fCount_WaitingFlushes(0),
	fFlushInProgress(false),
	fFlushInFirstStages(false),
	fRootSlotNumber(0),
	fHeight(0),
	fCached_Head(nullptr),
	fCached_Tail(nullptr),
	fHighWater(0),
	fAllocatedSpace(0),
	fUseRequests(0),
	fUseCached(0),
	fUseFailures(0),
	fFileR(iFile)
	{
	ZMACRO_ThreadSafe_Set(fStreamsInstantiated, 0);

	ZAssertStop(1, fFileR);

	ZStreamRPos_FileR theStream(fFileR);

	if (theStream.GetSize() < fUserHeaderSize + sizeof(spMagicText) + 6 * sizeof(uint32))
		spThrowBadFormat();

	theStream.SetPosition(fUserHeaderSize);

	char dummy[sizeof(spMagicText)];
	theStream.Read(dummy, sizeof(spMagicText));
	if (0 != memcmp(dummy, spMagicText, sizeof(spMagicText)))
		spThrowBadFormat();

	fSlotSize = theStream.ReadUInt32();
	fHighWater = theStream.ReadUInt32();
	fAllocatedSpace = fHighWater;
	fRootSlotNumber = theStream.ReadUInt32();
	fHeight = theStream.ReadUInt32();

	// Any writes to the block store that were not flushed may (will) cause the
	// stream to be larger than what the header says it needs. However, it must
	// not have been made smaller.
	if (theStream.GetSize() < fUserHeaderSize + fSlotSize * fHighWater)
		spThrowBadFormat();

	uint32 freeListHeadSlotNumber = theStream.ReadUInt32();
	uint32 freeListCount = theStream.ReadUInt32();

	fCapacity_BlockRoot_Byte = fSlotSize - sizeof(uint32);
	fCapacity_BlockRoot_Slots = (fSlotSize - sizeof(uint32)) / sizeof(uint32);

	fCapacity_IndexInternal = ((fSlotSize / sizeof(uint32)) / 3) - 1;
	// Force fCapacity_IndexInternal to be odd.
	fCapacity_IndexInternal = (((fCapacity_IndexInternal - 1) / 2) * 2) + 1;

	fCapacity_IndexLeaf = ((fSlotSize / sizeof(uint32)) - 1) / 2;
	// Force fCapacity_IndexLeaf to be odd.
	fCapacity_IndexLeaf = (((fCapacity_IndexLeaf - 1) / 2) * 2) + 1;

	fCapacity_Extent = fSlotSize / sizeof(uint32);
	fCapacity_Data = fSlotSize;

	if (freeListHeadSlotNumber)
		{
		uint32 currentPage = freeListHeadSlotNumber;
		const uint32 slotsPerPage = fSlotSize / sizeof(uint32) - 1;
		while (true)
			{
			theStream.SetPosition(fUserHeaderSize + currentPage * fSlotSize);
			uint32 countInCurrent = min(freeListCount, slotsPerPage);
			for (size_t x = 0; x < countInCurrent; ++x)
				{
				uint32 theSlotNumber = theStream.ReadUInt32();
				fSlots_ReleasedForked.insert(theSlotNumber);
				}
			fSlots_ReleasedClean.insert(currentPage);
			if (countInCurrent < slotsPerPage)
				break;
			currentPage = theStream.ReadUInt32();
			freeListCount -= countInCurrent;
			}
		}
	}

ZBlockStore_PhaseTree::~ZBlockStore_PhaseTree()
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, ZMACRO_ThreadSafe_Get(fStreamsInstantiated) == 0);
	this->Flush();

	for (map<uint32, Slot*>::iterator i = fSlots_Cached.begin(); i != fSlots_Cached.end(); ++i)
		{
		Slot* theSlot = (*i).second;
		theSlot->fCached_Prev = nullptr;
		theSlot->fCached_Next = nullptr;
		delete theSlot;
		}
	}

static void spCalcHighWaterAndMunge(uint32& ioHighWater, set<uint32>& ioSet)
	{
	if (ioSet.size())
		{
		set<uint32>::reverse_iterator current = ioSet.rbegin();
		while (current != ioSet.rend())
			{
			ZAssertStop(ZCONFIG_PhaseTree_Debug, ioHighWater > *current);
			if (ioHighWater != *current + 1)
				break;
			ioHighWater = *current;
			++current;
			}
		}
	set<uint32>::iterator iter = ioSet.lower_bound(ioHighWater);
	ioSet.erase(iter, ioSet.end());
	}

static void spSetAugment(const set<uint32>& iSet1, set<uint32>& ioSet2)
	{
	for (set<uint32>::const_iterator i = iSet1.begin(); i != iSet1.end(); ++i)
		ioSet2.insert(*i);
	}

void ZBlockStore_PhaseTree::Flush()
	{
	if (!fFileRW)
		return;

	ZFile::Error err;

/*
The straightforward implementation of flush would simply block all access to
the block store whilst flush was happening. In many cases this would not be a
big issue, however there are likely to be circumstances where flush could take
some time to complete, for example when the underlying file has poor latency.
This implementation of flush does _not_ lock out all access -- reads are allowed
to occur in parallel, but we disallow writes whilst the write-back of dirty slots
is happening, and we delay the start of the write-back until any extant writes are
completed. If we were to flush whilst mutating operations were happening then we
would be taking a reliable safe snapshot of half completed actions but it would be
without any saved state to complete those actions when we reopen the block store.

The code is broken into several stages; those that are processor-bound hold
fMutex_Slots and complete just as quickly as the processor can handle the job,
I/O-bound steps do not need to hold fMutex_Slots, instead they operate on local
data generated during the locked stages.
*/

	// Wait until any pending writes or flushes have completed.
	fMutex_Flush.Acquire();
	++fCount_WaitingFlushes;
	while (fCount_Writes || fFlushInProgress)
		fCondition_Flush.Wait(fMutex_Flush);
	--fCount_WaitingFlushes;
	fFlushInProgress = true;
	fFlushInFirstStages = true;
	fMutex_Flush.Release();

/*
Stage 1, fMutex_Slots is held.
Copy our meta information (fRootSlotNumber, fHeight, fHighWater,
fSlots_ReleasedClean and fSlots_ReleasedForked) into local variables. Any
dirty slots that are currently loaded have their use count incremented in
order to keep them loaded until they have been written back. We remember which
slots were dirty by putting them on a local vector. All loaded slots are
marked clean, so they can't be modified -- any code that needs to change their
contents will have to do so on a forked copy. All of this is done under the protection
of fMutex_Slots which will prevent any other thread from doing anything
to the meta information until we've finished the transcription and state update.
*/

	fMutex_MetaRoot.Acquire();

	fMutex_Slots.Acquire();

	uint32 writtenHighWater = fHighWater;
	uint32 writtenRootSlotNumber = fRootSlotNumber;
	uint32 writtenHeight = fHeight;

	fMutex_MetaRoot.Release();

	// Swap fSlots_ReleasedClean into a local, (after which fSlots_ReleasedClean will be empty).
	set<uint32> stageOneReleasedClean;
	fSlots_ReleasedClean.swap(stageOneReleasedClean);

	// Copy fSlots_ReleasedForked.
	set<uint32> stageOneReleasedForked = fSlots_ReleasedForked;

	// Transcribe any dirty slots to a local list, holding them in memory by incrementing their
	// use count. Also mark all slots as being clean.
	vector<Slot*> dirtySlots;
	for (map<uint32, Slot*>::iterator i = fSlots_Loaded.begin(); i != fSlots_Loaded.end(); ++i)
		{
		Slot* currentSlot = (*i).second;
		switch (currentSlot->fState)
			{
			case eStateClean:
			case eStateForked:
				{
				// Ignore clean or forked slots.
				break;
				}
			case eStateDirty:
				{
				// Dirty slots have their use count incremented to keep them loaded.
				dirtySlots.push_back(currentSlot);
				++currentSlot->fUseCount;
				break;
				}
			case eStateReleasedClean:
			case eStateReleasedForked:
				{
				// This can't happen.
				ZUnimplemented();
				break;
				}
			}
		// All slots are marked clean.
		currentSlot->fState = eStateClean;
		}

	for (map<uint32, Slot*>::iterator i = fSlots_Cached.begin(); i != fSlots_Cached.end(); ++i)
		{
		Slot* currentSlot = (*i).second;

		// Cached slots can only be forked or clean.
		ZAssertStop(kDebug_PhaseTreeFile,
			currentSlot->fState == eStateForked || currentSlot->fState == eStateClean);

		currentSlot->fState = eStateClean;
		}

	// Emptying fSlots_Forked will cause any subsequently loaded slot
	// to be marked as being clean rather than forked.
	fSlots_Forked.clear();

	fMutex_Slots.Release();

/*
Stage 2, fMutex_Slots is not held during the write back of slots, but is acquired/released
by each invocation of UnuseUnlockedSlot, which we call to normalize the slots' use counts.
We do not need to acquire the lock on each slot as no writes can be in progress nor will
any write start until we reset fFlushInFirstStages.
*/
	for (vector<Slot*>::iterator i = dirtySlots.begin(); i != dirtySlots.end(); ++i)
		{
		err = fFileRW->WriteAt(fUserHeaderSize + uint64((*i)->fSlotNumber) * fSlotSize,
			(*i)->fData, fSlotSize, nullptr);
		ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
		}

	for (vector<Slot*>::iterator i = dirtySlots.begin(); i != dirtySlots.end(); ++i)
		this->UnuseUnlockedSlot(*i);

	// Let writes know that they can proceed.
	fMutex_Flush.Acquire();
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fFlushInFirstStages);
	fFlushInFirstStages = false;
	fCondition_Flush.Broadcast();
	fMutex_Flush.Release();

/*
Stage 3, fMutex_Slots is held.
Build a list of all slots that are considered released for the purposes of the snapshot this flush
is effectively building. That includes all slots that were on fSlots_ReleasedForked and
fSlots_ReleasedClean when we entered stage one (ie stageOneReleasedForked and
stageOneReleasedClean.) Build 'pages', a list of slots that will have the list of released slots
written to them. For a page we can use any slot that is _currently_ in fSlots_ReleasedForked,
removing it from that set of course. We don't use stageOneReleasedForked as our source for pages
because ongoing access to the block store as we do this might make use of slots it references -- we
want to record that the slots in stageOneReleasedForked and stageOneReleasedClean are released, but
we mustn't actually write to any in stageOneReleasedClean at all or any in stageOneReleasedForked
that have been grabbed in the meantime.

We also determine two highwater values. The first is what we'll write to the header as part of the
consistent snapshot. The second is potentially larger, and will accomodate slots that have been
allocated since we started the flush and that will form part of the next snapshot.
*/

	fMutex_Slots.Acquire();

	// releasedAll is the union of stageOneReleasedClean and stageOneReleasedForked.
	set<uint32> releasedAll;
	set_union(stageOneReleasedClean.begin(), stageOneReleasedClean.end(),
			stageOneReleasedForked.begin(), stageOneReleasedForked.end(),
			inserter(releasedAll, releasedAll.begin()));

	// Chop off from the end of releasedAll any slots that are contiguous
	// with each other and the high water we're going to write to the
	// header, and adjust the written high water to match.
	spCalcHighWaterAndMunge(writtenHighWater, releasedAll);

	// Similarly, chop off from fSlots_ReleasedForked those slots contiguous with
	// the current highWater.
	spCalcHighWaterAndMunge(fHighWater, fSlots_ReleasedForked);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, writtenHighWater <= fHighWater);

	// Build the list of slots (the 'pages') that will actually hold the list of free slots.
	// We have to do this iteratively as we may have to increase the number of slots that
	// need to be recorded when we are forced to push up the highwater mark to get a page.

	// the -1 accounts for the next page link.
	const size_t slotsPerPage = fSlotSize / sizeof(uint32) - 1;

	set<uint32> pages;
	while ((releasedAll.size() + slotsPerPage - 1) / slotsPerPage > pages.size())
		{
		// We need a page.
		uint32 thePage;
		if (fSlots_ReleasedForked.size())
			{
			// Use one from fSlots_ReleasedForked.
			thePage = *fSlots_ReleasedForked.begin();
			fSlots_ReleasedForked.erase(fSlots_ReleasedForked.begin());
			// If we had it on our list to write out, remove it.
			releasedAll.erase(thePage);
			stageOneReleasedClean.erase(thePage);
			}
		else
			{
			// Take a slot at the current real highWater and bump it up.
			thePage = fHighWater++;
			}

		// If thePage lies beyond writtenHighWater, update it to match and
		// add the intervening slots to releasedAll.
		while (writtenHighWater < thePage)
			{
			releasedAll.insert(writtenHighWater);
			++writtenHighWater;
			}

		ZAssertStop(ZCONFIG_PhaseTree_Debug, writtenHighWater >= thePage);
		if (writtenHighWater == thePage)
			writtenHighWater = thePage + 1;
		pages.insert(thePage);
		}

	// From this point on our clean released slots are augmented by what's in pages.
	spSetAugment(pages, fSlots_ReleasedClean);

	// Our forked released slots are augmented by all the slots between
	// writtenHighWater (included) and fHighWater (not included).
	for (size_t x = writtenHighWater; x < fHighWater; ++x)
		fSlots_ReleasedForked.insert(x);

	// Truncate stageOneReleasedClean, for use later on.
	set<uint32>::iterator someIter = stageOneReleasedClean.lower_bound(writtenHighWater);
	stageOneReleasedClean.erase(someIter, stageOneReleasedClean.end());

	// No slot in fSlots_ReleasedForked can be at or past fHighWater.
	ZAssertStop(ZCONFIG_PhaseTree_Debug,
		fSlots_ReleasedForked.end() == fSlots_ReleasedForked.lower_bound(fHighWater));

	// We can't have moved writtenHighWater past fHighWater.
	ZAssertStop(ZCONFIG_PhaseTree_Debug, writtenHighWater <= fHighWater);

	// Set the file size to match fHighWater.
	err = fFileRW->SetSize(fUserHeaderSize + fHighWater * fSlotSize);
	fAllocatedSpace = fHighWater;
	ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);

	fMutex_Slots.Release();

/*
Stage 4, fMutex_Slots is not held.
Write out the linked list of pages, filling them with slot numbers from releasedAll.
*/
	vector<char> buffer(fSlotSize);

	set<uint32>::iterator pagesIter = pages.begin();
	set<uint32>::iterator releasedIter = releasedAll.begin();
	size_t slotsRemaining = releasedAll.size();
	while (slotsRemaining)
		{
		uint32* theAddress = reinterpret_cast<uint32*>(&buffer[0]);
		size_t slotsInCurrent = min(slotsPerPage, slotsRemaining);
		for (size_t x = 0; x < slotsInCurrent; ++x)
			ZByteSwap_WriteBig32(theAddress++, *releasedIter++);
		slotsRemaining -= slotsInCurrent;

		uint32 currentPage = *pagesIter++;
		if (pagesIter != pages.end())
			{
			// Note that slotsRemaining _can_ be zero, sometimes we overallocate
			// pages because we need a page to record one remaining free slot, and
			// the page that is allocated _is_ that free page.
			ZByteSwap_WriteBig32(theAddress++, *pagesIter);
			}

		err = fFileRW->WriteAt(fUserHeaderSize + uint64(currentPage) * fSlotSize,
			&buffer[0], fSlotSize, nullptr);

		ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
		}

/*
Stage 5, fMutex_Slots remains unheld.
We flush the file, to purge any pending writes of dirty slots or the free list. We
then assemble the header using information snapshotted in stage one and write the
header. This write is the absolute crux of the phase tree's resilience in the face of
failure -- it must write completely or not at all.
*/
	fFileRW->FlushVolume();

	ZStreamWPos_Memory bufferStream(&buffer[0], fSlotSize);

	bufferStream.Write(spMagicText, sizeof(spMagicText));
	bufferStream.WriteUInt32(fSlotSize);
	bufferStream.WriteUInt32(writtenHighWater);
	bufferStream.WriteUInt32(writtenRootSlotNumber);
	bufferStream.WriteUInt32(writtenHeight);
	if (pages.size())
		{
		bufferStream.WriteUInt32(*pages.begin()); // Free list head slot number
		bufferStream.WriteUInt32(releasedAll.size()); // Free list count
		}
	else
		{
		bufferStream.WriteUInt32(0);
		bufferStream.WriteUInt32(0);
		}

	err = fFileRW->WriteAt(fUserHeaderSize, &buffer[0], fSlotSize, nullptr);
	ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
	fFileRW->FlushVolume();

/*
Stage 6, fMutex_Slots is held
stageOneReleasedClean still references a bunch of slots that were free but that could not
be written to until the header had been written out. Add those slots into fSlots_ReleasedForked.
*/
	fMutex_Slots.Acquire();
	spSetAugment(stageOneReleasedClean, fSlots_ReleasedForked);
	fMutex_Slots.Release();

	// Let other calls to flush proceed.
	fMutex_Flush.Acquire();
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fFlushInFirstStages == false && fFlushInProgress == true);
	fFlushInProgress = false;
	fCondition_Flush.Broadcast();
	fMutex_Flush.Release();
	}

ZRef<ZStreamerRWPos> ZBlockStore_PhaseTree::Create(BlockID& oBlockID)
	{
	if (fFileRW)
		{
		this->WriteStart();
		if (Slot* theBlockSlot = this->CreateBlockImp(1, oBlockID))
			{
			int oldCount = ZMACRO_ThreadSafe_IncReturnOld(fStreamsInstantiated);
			ZAssertStop(ZCONFIG_PhaseTree_Debug, oldCount >= 0);

			this->WriteFinish(theBlockSlot);
			return new StreamerRWPos(this, oBlockID, theBlockSlot);
			}
		this->WriteFinish();
		}
	oBlockID = 0;
	return null;
	}

ZRef<ZStreamerRPos> ZBlockStore_PhaseTree::OpenRPos(BlockID iBlockID)
	{
	if (iBlockID)
		{
		if (Slot* theBlockSlot = this->FindBlockSlot(iBlockID))
			{
			int oldCount = ZMACRO_ThreadSafe_IncReturnOld(fStreamsInstantiated);
			ZAssertStop(ZCONFIG_PhaseTree_Debug, oldCount >= 0);

			this->ReadFinish(theBlockSlot);

			return new StreamerRPos(this, iBlockID, theBlockSlot);
			}
		}

	return null;
	}

ZRef<ZStreamerRWPos> ZBlockStore_PhaseTree::OpenRWPos(BlockID iBlockID)
	{
	if (fFileRW && iBlockID)
		{
		this->WriteStart();
		if (Slot* theBlockSlot = this->FindBlockSlotForWrite(iBlockID))
			{
			int oldCount = ZMACRO_ThreadSafe_IncReturnOld(fStreamsInstantiated);
			ZAssertStop(ZCONFIG_PhaseTree_Debug, oldCount >= 0);

			this->WriteFinish(theBlockSlot);
			return new StreamerRWPos(this, iBlockID, theBlockSlot);
			}

		this->WriteFinish();
		}
	return null;
	}

bool ZBlockStore_PhaseTree::Delete(BlockID iBlockID)
	{
	if (!fFileRW || !iBlockID)
		return false;

	this->WriteStart();

	// For most operations we walk down the tree, acquiring a child's lock then
	// releasing the parent's lock. Because delete can be called with an ID that
	// is not allocated, and because each ancestor records the total number
	// of allocated children in its subtrees, we have to lock the entire tree
	// until we know if the ID was allocated, and thus that subtree sizes
	// will need to be updated. The upshot being that we hold fMutex_MetaRoot
	// through the whole process.

	fMutex_MetaRoot.Acquire();
	Slot* rootSlot = this->UseRootSlotForked();

	bool result = false;
	Slot* blockSlot;
	Slot* newRoot;
	if (this->DeleteBlockRecurse(rootSlot, fHeight, iBlockID, blockSlot, &newRoot))
		{
		if (rootSlot != newRoot)
			{
			rootSlot = newRoot;
			if (rootSlot)
				{
				fRootSlotNumber = rootSlot->fSlotNumber;
				--fHeight;
				}
			else
				{
				fRootSlotNumber = 0;
				ZAssertStop(ZCONFIG_PhaseTree_Debug, fHeight == 0);
				}
			}
		result = true;
		fMutex_MetaRoot.Release();

		if (rootSlot)
			this->UnuseSlot(rootSlot);

		// We've released locks on all the intervening nodes, and the meta root.
		// Other calls are now free to proceed. We still hold the lock on
		// blockSlot, and now delete all the space it references.
		this->Block_KillTree(blockSlot);
		this->ReleaseAndUnuseSlot(blockSlot);
		}
	else
		{
		fMutex_MetaRoot.Release();

		if (rootSlot)
			this->UnuseSlot(rootSlot);
		}

	this->WriteFinish();

	return result;
	}

size_t ZBlockStore_PhaseTree::EfficientSize()
	{
	// Returns a size that is 'efficient' by some definition, in our case what
	// will fit in a block root without requiring allocation of data slots.
	return fSlotSize - sizeof(uint32);
	}

ZRef<ZStreamerRWPos> ZBlockStore_PhaseTree::CreateWithSpecificID(
	BlockID iDesiredBlockID, BlockID& oActualBlockID)
	{
	if (fFileRW && iDesiredBlockID)
		{
		this->WriteStart();
		if (Slot* theBlockSlot = this->CreateBlockImp(iDesiredBlockID, oActualBlockID))
			{
			int oldCount = ZMACRO_ThreadSafe_IncReturnOld(fStreamsInstantiated);
			ZAssertStop(ZCONFIG_PhaseTree_Debug, oldCount >= 0);

			this->WriteFinish(theBlockSlot);
			return new StreamerRWPos(this, oActualBlockID, theBlockSlot);
			}
		this->WriteFinish();
		}

	oActualBlockID = 0;
	return null;
	}

ZBlockStore::BlockID ZBlockStore_PhaseTree::GetNextBlockID(BlockID iBlockID)
	{
	fMutex_MetaRoot.Acquire();

	BlockID theID = 0;
	if (Slot* rootSlot = this->UseRootSlot())
		{
		theID = this->GetNextBlockIDRecurse(rootSlot, fHeight, iBlockID);
		this->UnuseSlot(rootSlot);
		}

	fMutex_MetaRoot.Release();
	return theID;
	}

void ZBlockStore_PhaseTree::Dump()
	{
	fMutex_MetaRoot.Acquire();
	if (Slot* rootSlot = this->UseRootSlot())
		{
		int currentHeight = fHeight;
		fMutex_MetaRoot.Release();

		this->DumpOne(rootSlot, currentHeight, 0);
		this->UnuseSlot(rootSlot);
		}
	else
		{
		fMutex_MetaRoot.Release();
		}
	}

void ZBlockStore_PhaseTree::DumpMeta()
	{
	fMutex_Slots.Acquire();
	fprintf(stdout, "Root Slot: %d, Height: %d, Highwater: %d, "
		"ReleasedClean: %zu, ReleasedForked: %zu, Forked: %zu\n",
		fRootSlotNumber, fHeight, fHighWater,
		fSlots_ReleasedClean.size(), fSlots_ReleasedForked.size(), fSlots_Forked.size());

	int loadedHitRate = 0;
	int cachedHitRate = 0;
	if (fUseRequests)
		{
		loadedHitRate = (fUseFailures * 100) / fUseRequests;
		cachedHitRate = (fUseCached * 100) / fUseRequests;
		}

	fprintf(stdout,
		"Loaded slots: %zu, use requests: %d, use cached %d, "
		"use failures: %d, loaded hit rate: %d, cached hit rate: %d\n",
		fSlots_Loaded.size(), fUseRequests, fUseCached,
		fUseFailures, 100 - loadedHitRate, 100 - cachedHitRate);

	fMutex_Slots.Release();
	}

void ZBlockStore_PhaseTree::ValidateTree()
	{
	fMutex_MetaRoot.Acquire();
	if (Slot* rootSlot = this->UseRootSlot())
		{
		int currentHeight = fHeight;
		fMutex_MetaRoot.Release();

		this->ValidateOne(rootSlot, currentHeight, 0, 0xFFFFFFFF);
		this->UnuseSlot(rootSlot);
		}
	else
		{
		fMutex_MetaRoot.Release();
		}
	}

static void spPrintIndent(int iIndent)
	{
	for (int i = 0; i < iIndent; ++i)
		fprintf(stdout, ". ");
	}

void ZBlockStore_PhaseTree::DumpOne(Slot* iSlot, int iHeight, int iIndent)
	{
	size_t count = this->Index_GetCount(iSlot);
	if (iHeight)
		{
		spPrintIndent(iIndent);
		fprintf(stdout, "Count: %zu Slot: %d\n", count, iSlot->fSlotNumber);
		for (size_t x = 0; x <= count; ++x)
			{
			if (x)
				{
				spPrintIndent(iIndent);
				fprintf(stdout, "Key: %08X\n", this->Index_GetKey(iSlot, false, x - 1));
				}
			spPrintIndent(iIndent);
			fprintf(stdout, "SubTree: %d\n", this->Index_GetSubTreeSize(iSlot, x));
			Slot* theSlot = this->UseSlot(this->Index_GetChild(iSlot, false, x));
			this->DumpOne(theSlot, iHeight - 1, iIndent + 1);
			this->UnuseSlot(theSlot);
			}
		}
	else
		{
		spPrintIndent(iIndent);
		fprintf(stdout, "Count: %zu Slot: %d\n", count, iSlot->fSlotNumber);
		for (size_t x = 0; x < count; ++x)
			{
			spPrintIndent(iIndent);
			fprintf(stdout, "Key: %08X ", this->Index_GetKey(iSlot, true, x));
			Slot* theBlockSlot = this->UseSlot(this->Index_GetChild(iSlot, true, x));
			this->DumpBlock(theBlockSlot, 0);
			this->UnuseSlot(theBlockSlot);
			}
		}
	}

void ZBlockStore_PhaseTree::DumpBlock(Slot* iSlot, int iIndent)
	{
	fprintf(stdout, "Slot: %d, Size: %d\n", iSlot->fSlotNumber, this->ReadField(iSlot, 0));
	}

size_t ZBlockStore_PhaseTree::ValidateOne(
	Slot* iSlot, int iHeight, uint32 iMinKeyMinusOne, uint32 iMaxKey)
	{
	size_t count = this->Index_GetCount(iSlot);
	uint32 priorKey = iMinKeyMinusOne;
	if (iHeight)
		{
		size_t accumulatedActual = 0;
		for (size_t x = 0; x <= count; ++x)
			{
			Slot* theSlot = this->UseSlot(this->Index_GetChild(iSlot, false, x));
			size_t expectedSubTreeSize = this->Index_GetSubTreeSize(iSlot, x);
			size_t actualSubTreeSize;
			if (x == count)
				{
				ZAssertLogf(0, iMaxKey - priorKey >= expectedSubTreeSize,
					"slotnumber: %d, priorKey: %X, iMaxKey: %X, expectedSubTreeSize: %d",
					iSlot->fSlotNumber, priorKey, iMaxKey, expectedSubTreeSize);
				actualSubTreeSize = this->ValidateOne(theSlot, iHeight - 1, priorKey, iMaxKey);
				}
			else
				{
				uint32 currentKey = this->Index_GetKey(iSlot, false, x);
				ZAssertLogf(0, currentKey - priorKey >= expectedSubTreeSize,
					"slotnumber: %d, priorKey: %X, currentKey: %X, expectedSubTreeSize: %d",
					iSlot->fSlotNumber, priorKey, currentKey, expectedSubTreeSize);
				actualSubTreeSize = this->ValidateOne(theSlot, iHeight - 1, priorKey, currentKey);
				priorKey = currentKey;
				}
			ZAssertLogf(0, expectedSubTreeSize == actualSubTreeSize,
				"slotnumber: %d, Expected: %d, actual:%d",
				iSlot->fSlotNumber, expectedSubTreeSize, actualSubTreeSize);
			accumulatedActual += actualSubTreeSize;
			this->UnuseSlot(theSlot);
			}
		return accumulatedActual;
		}
	else
		{
		for (size_t x = 0; x < count; ++x)
			{
			uint32 currentKey = this->Index_GetKey(iSlot, false, x);
			ZAssertLogf(0, currentKey > iMinKeyMinusOne && currentKey <= iMaxKey,
				"slotnumber: %d, iMinKeyMinusOne: %X, iMaxKey:%X, currentKey:%X",
				iSlot->fSlotNumber, iMinKeyMinusOne, iMaxKey, currentKey);
			}
		return count;
		}
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Methods called by streams or our block store API.

void ZBlockStore_PhaseTree::StreamDisposing(Slot* iBlockSlot)
	{
	if (iBlockSlot)
		{
		if (iBlockSlot->fSlotNumber)
			{
			iBlockSlot->fMutex.Acquire();
			this->UnuseSlot(iBlockSlot);
			}
		else
			{
			// The block slot has a zero slotnumber because the block was
			// deleted whilst one or more streams were open on it. The slotnumber
			// is zeroed in ReleaseAndUnuseSlot.
			this->UnuseUnlockedSlot(iBlockSlot);
			}
		}
	int newCount = ZMACRO_ThreadSafe_DecReturnNew(fStreamsInstantiated);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, newCount >= 0);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::FindBlockSlot(BlockID iBlockID)
	{
	fMutex_MetaRoot.Acquire();

	Slot* parentSlot = this->UseRootSlot();

	int currentHeight = fHeight;
	fMutex_MetaRoot.Release();

	if (!parentSlot)
		return nullptr;

	while (true)
		{
		bool isLeafNode = (currentHeight == 0);
		size_t offset;
		Slot* childSlot = this->Index_FindChild(parentSlot, isLeafNode, iBlockID, offset);
		if (!childSlot)
			{
			ZAssertStop(ZCONFIG_PhaseTree_Debug, isLeafNode);
			this->UnuseSlot(parentSlot);
			return nullptr;
			}

		this->UnuseSlot(parentSlot);

		if (isLeafNode)
			return childSlot;

		parentSlot = childSlot;
		--currentHeight;
		}
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::FindBlockSlotForWrite(BlockID iBlockID)
	{
	fMutex_MetaRoot.Acquire();

	Slot* parentSlot = this->UseRootSlotDirty();

	int currentHeight = fHeight;
	fMutex_MetaRoot.Release();

	if (!parentSlot)
		return nullptr;

	while (true)
		{
		bool isLeafNode = (currentHeight == 0);
		size_t offset;
		uint32 newChildSlotNumber;
		Slot* childSlot = this->Index_FindChildForked(
			parentSlot, isLeafNode, iBlockID, offset, newChildSlotNumber);

		if (!childSlot)
			{
			ZAssertStop(ZCONFIG_PhaseTree_Debug, isLeafNode);
			this->UnuseSlot(parentSlot);
			return nullptr;
			}

		if (newChildSlotNumber)
			{
			this->DirtySlot(parentSlot);
			this->Index_SetChild(parentSlot, isLeafNode, offset, newChildSlotNumber);
			}

		this->UnuseSlot(parentSlot);

		if (isLeafNode)
			{
			this->DirtySlot(childSlot);
			return childSlot;
			}

		parentSlot = childSlot;
		--currentHeight;
		}
	}

void ZBlockStore_PhaseTree::ReadFromBlock(BlockID iBlockID, Slot*& ioBlockSlot,
	uint64 iPosition, void* oDest, size_t iCount, size_t& oCountRead)
	{
	oCountRead = 0;
	if (iCount == 0)
		return;

	this->ReadStart(iBlockID, ioBlockSlot);

	if (ioBlockSlot)
		{
		size_t theSize = ReadField(ioBlockSlot, 0);
		size_t countRemaining = min(uint64(iCount), theSize > iPosition ? theSize - iPosition : 0);
		if (theSize <= fCapacity_BlockRoot_Byte)
			{
			sMemCopy(oDest,
				reinterpret_cast<char*>(FieldAddress(ioBlockSlot, 1)) + iPosition, countRemaining);
			oCountRead = countRemaining;
			}
		else
			{
			size_t localPosition = iPosition;
			char* localDest = reinterpret_cast<char*>(oDest);
			while (countRemaining)
				{
				Slot* dataSlot = this->Block_FindDataSlot(ioBlockSlot, localPosition);
				size_t positionInData = localPosition % fCapacity_Data;
				size_t countToMove = min(countRemaining, fCapacity_Data - positionInData);
				sMemCopy(localDest,
					reinterpret_cast<char*>(dataSlot->fData) + positionInData, countToMove);

				countRemaining -= countToMove;
				localDest += countToMove;
				localPosition += countToMove;
				oCountRead += countToMove;
				this->UnuseSlot(dataSlot);
				}
			}
		}

	this->ReadFinish(ioBlockSlot);
	}

void ZBlockStore_PhaseTree::WriteToBlock(BlockID iBlockID, Slot*& ioBlockSlot,
	uint64 iPosition, const void* iSource, size_t iCount, size_t& oCountWritten)
	{
	oCountWritten = 0;
	if (iCount == 0)
		return;

	this->WriteStart(iBlockID, ioBlockSlot);

	if (ioBlockSlot)
		{
		size_t theSize = ReadField(ioBlockSlot, 0);
		uint64 neededSize = min(uint64(0xFFFFFFFFU), iPosition + iCount);
		if (theSize < neededSize)
			{
			theSize = neededSize;
			this->SetBlockSizeImp(ioBlockSlot, theSize);
			}

		size_t countRemaining = min(uint64(iCount), theSize > iPosition ? theSize - iPosition : 0);

		if (theSize <= fCapacity_BlockRoot_Byte)
			{
			sMemCopy(reinterpret_cast<char*>(FieldAddress(ioBlockSlot, 1)) + iPosition,
				iSource, countRemaining);
			oCountWritten = countRemaining;
			}
		else
			{
			size_t localPosition = iPosition;
			const char* localSource = reinterpret_cast<const char*>(iSource);
			while (countRemaining)
				{
				size_t positionInData = localPosition % fCapacity_Data;
				size_t countToMove = min(countRemaining, fCapacity_Data - positionInData);
				Slot* dataSlot = this->Block_FindDataSlotForWrite(ioBlockSlot,
					localPosition, positionInData != 0 || countToMove != fCapacity_Data);
				sMemCopy(reinterpret_cast<char*>(dataSlot->fData) + positionInData,
					localSource, countToMove);
				countRemaining -= countToMove;
				localSource += countToMove;
				localPosition += countToMove;
				oCountWritten += countToMove;
				this->UnuseSlot(dataSlot);
				}
			}
		}

	this->WriteFinish(ioBlockSlot);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::CreateBlockImp(
	BlockID iBlockID, BlockID& oBlockID)
	{
	BlockID smallestKeyLessThan = 0;
	BlockID largestKeyGreaterOrEqual = 0xFFFFFFFF;

	fMutex_MetaRoot.Acquire();
	Slot* rootSlot = this->UseRootSlotDirty();

	if (this->Index_IsFull(rootSlot, fHeight == 0))
		{
		uint32 medianKey;
		size_t leftSubTreeSize, rightSubTreeSize;
		Slot* newChild;
		this->Index_Split(rootSlot, fHeight == 0,
			leftSubTreeSize, medianKey, newChild, rightSubTreeSize);

		++fHeight;

		Slot* newRoot = this->Index_CreateRoot(
			rootSlot, leftSubTreeSize, medianKey, newChild, rightSubTreeSize);

		this->UnuseSlot(newChild);
		this->UnuseSlot(rootSlot);
		fRootSlotNumber = newRoot->fSlotNumber;
		rootSlot = newRoot;
		}
	int currentHeight = fHeight;
	fMutex_MetaRoot.Release();

	Slot* currentSlot = rootSlot;
	for (;;)
		{
		VALIDATESLOT(currentSlot);
		this->DirtySlot(currentSlot);
		if (currentHeight)
			{
			size_t offset = 0;
			const size_t count = this->Index_GetCount(currentSlot);
			BlockID smallestKeyAtStart = smallestKeyLessThan;
			BlockID largestKeyAtStart = largestKeyGreaterOrEqual;
			for (;;)
				{
				if (offset == count)
					{
					ZAssertStop(ZCONFIG_PhaseTree_Debug, largestKeyGreaterOrEqual >= iBlockID);

					size_t curSubTreeSize = this->Index_GetSubTreeSize(currentSlot, count);

					if (largestKeyGreaterOrEqual - smallestKeyLessThan > curSubTreeSize)
						{
						// There are unallocated IDs in this branch (though
						// not necessarily iBlockID itself.)
						break;
						}
					else
						{
						// This branch is fully populated. We need to start over
						// at the very beginning, looking for smallestKeyAtStart + 1
						// instead of the requested ID.
						iBlockID = smallestKeyAtStart + 1;
						smallestKeyLessThan = smallestKeyAtStart;
						offset = 0;
						}
					}
				else
					{
					BlockID curKey = this->Index_GetKey(currentSlot, false, offset);

					ZAssertStop(ZCONFIG_PhaseTree_Debug, curKey > smallestKeyLessThan);
					ZAssertStop(ZCONFIG_PhaseTree_Debug, curKey <= largestKeyGreaterOrEqual);

					if (curKey < iBlockID)
						{
						smallestKeyLessThan = curKey;
						++offset;
						}
					else
						{
						size_t curSubTreeSize = this->Index_GetSubTreeSize(currentSlot, offset);

						if (curKey - smallestKeyLessThan > curSubTreeSize)
							{
							// There are unallocated IDs in this branch (though
							// not necessarily iBlockID itself.)
							largestKeyGreaterOrEqual = curKey;
							break;
							}
						else
							{
							// This branch is fully populated. Move on, but start
							// probing at curKey + 1.
							iBlockID = curKey + 1;
							smallestKeyLessThan = curKey;
							++offset;
							}
						}
					}
				}

			uint32 newSlotNumber;
			Slot* childSlot = this->UseSlotDirty(
				this->Index_GetChild(currentSlot, false, offset), true, newSlotNumber);

			if (newSlotNumber)
				this->Index_SetChild(currentSlot, false, offset, newSlotNumber);

			if (this->Index_IsFull(childSlot, currentHeight == 1))
				{
				// Split the child.
				// Don't need to dirty it -- it was returned dirty
				// this->DirtySlot(childSlot);
				uint32 medianKey;
				size_t leftSubTreeSize, rightSubTreeSize;
				Slot* newChild;
				this->Index_Split(childSlot, currentHeight == 1,
					leftSubTreeSize, medianKey, newChild, rightSubTreeSize);
				VALIDATESLOT(currentSlot);
				VALIDATESLOT(childSlot);
				VALIDATESLOT(newChild);

				if (ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug)
					{
					size_t oldSubTreeSize = this->Index_GetSubTreeSize(currentSlot, offset);
					ZAssertStopf(ZCONFIG_PhaseTree_Debug,
						leftSubTreeSize + rightSubTreeSize == oldSubTreeSize,
						"left: %d, right:%d, old:%d",
						leftSubTreeSize, rightSubTreeSize, oldSubTreeSize);
					}

				// And insert its new sibling (newChild) into currentSlot after offset.
				if (offset < count)
					{
					this->Index_InternalMakeOneSpaceBefore(currentSlot, offset + 1);

					this->Index_SetKey(
						currentSlot, false, offset + 1,
						this->Index_GetKey(currentSlot, false, offset));

					this->Index_SetKey(currentSlot, false, offset, medianKey);
					this->Index_SetChild(currentSlot, false, offset + 1, newChild->fSlotNumber);
					this->Index_SetSubTreeSize(currentSlot, offset, leftSubTreeSize);
					this->Index_SetSubTreeSize(currentSlot, offset + 1, rightSubTreeSize);
					}
				else
					{
					this->Index_InternalMakeSpaceOnRight(currentSlot, 1);

					this->Index_SetKey(currentSlot, false, offset, medianKey);
					this->Index_SetChild(currentSlot, false, offset + 1, newChild->fSlotNumber);
					this->Index_SetSubTreeSize(currentSlot, offset, leftSubTreeSize);
					this->Index_SetSubTreeSize(currentSlot, offset + 1, rightSubTreeSize);
					}

				VALIDATESLOT(currentSlot);

				// Release the children without changing currentSlot and revert the value of
				// largestKeyGreaterOrEqual and smallestKeyLessThan. We'll go round the loop,
				// but this time the child that's found won't need to be split.
				smallestKeyLessThan = smallestKeyAtStart;
				largestKeyGreaterOrEqual = largestKeyAtStart;
				this->UnuseSlot(newChild);
				this->UnuseSlot(childSlot);
				}
			else
				{
				// Update the sub tree size, doing so *after* any split of the child.
				this->Index_SetSubTreeSize(
					currentSlot, offset, this->Index_GetSubTreeSize(currentSlot, offset) + 1);
				this->UnuseSlot(currentSlot);
				--currentHeight;
				currentSlot = childSlot;
				}
			}
		else
			{
			// We're in a leaf, do the actual allocation.
			const size_t count = this->Index_GetCount(currentSlot);
			size_t offset;
			for (offset = 0; offset < count; ++offset)
				{
				BlockID curKey = this->Index_GetKey(currentSlot, false, offset);
				ZAssertStop(ZCONFIG_PhaseTree_Debug, curKey > smallestKeyLessThan);
				ZAssertStop(ZCONFIG_PhaseTree_Debug, curKey <= largestKeyGreaterOrEqual);
				if (curKey > iBlockID)
					{
					// We've found the location.
					break;
					}
				else if (curKey == iBlockID)
					{
					// Damn. The key iBlockID was already allocated. We have to rerun the loop
					// starting at smallestKeyLessThan + 1, where smallestKeyLessThan is the
					// largest number smaller than anything in this node.
					iBlockID = smallestKeyLessThan + 1;
					for (offset = 0; offset < count; ++offset)
						{
						curKey = this->Index_GetKey(currentSlot, true, offset);
						if (curKey > iBlockID)
							break;
						iBlockID = curKey + 1;
						}
					break;
					}
				}

			Slot* newBlockSlot = this->AllocateSlotDirty();
			WriteField(newBlockSlot, 0, 0);
			this->Index_LeafMakeOneSpaceBefore(currentSlot, offset);
			this->Index_SetKey(currentSlot, true, offset, iBlockID);
			this->Index_SetChild(currentSlot, true, offset, newBlockSlot->fSlotNumber);
			this->UnuseSlot(currentSlot);
			oBlockID = iBlockID;
			return newBlockSlot;
			}
		VALIDATESLOT(currentSlot);
		}
	}

bool ZBlockStore_PhaseTree::DeleteBlockRecurse(
	Slot* iParentSlot, int iHeight, BlockID iBlockID, Slot*& oBlockSlot, Slot** oNewRoot)
	{
	if (oNewRoot)
		*oNewRoot = iParentSlot;

	if (iHeight == 0)
		{
		size_t offset;
		if (Slot* childSlot = this->Index_FindChild(iParentSlot, true, iBlockID, offset))
			{
			oBlockSlot = childSlot;
			if (this->Index_GetCount(iParentSlot) == 1)
				{
				ZAssertStop(ZCONFIG_PhaseTree_Debug, oNewRoot);
				*oNewRoot = nullptr;
				this->ReleaseAndUnuseSlot(iParentSlot);
				}
			else
				{
				ZAssertStop(ZCONFIG_PhaseTree_Debug, this->Index_GetCount(iParentSlot) != 0);
				this->DirtySlot(iParentSlot);
				this->Index_LeafDeleteOne(iParentSlot, offset);
				}
			return true;
			}
		return false;
		}
	else
		{
		size_t offset;
		uint32 newChildSlotNumber;
		Slot* childSlot = this->Index_FindChildForked(
			iParentSlot, false, iBlockID, offset, newChildSlotNumber);

		ZAssertStop(ZCONFIG_PhaseTree_Debug, childSlot);
		if (newChildSlotNumber)
			{
			this->DirtySlot(iParentSlot);
			this->Index_SetChild(iParentSlot, false, offset, newChildSlotNumber);
			}

		if (!this->DeleteBlockRecurse(childSlot, iHeight - 1, iBlockID, oBlockSlot, nullptr))
			{
			this->UnuseSlot(childSlot);
			return false;
			}
		else
			{
			if (!newChildSlotNumber) // ???
				{
				// We already dirtied the parent slot above
				this->DirtySlot(iParentSlot);
				}

			this->Index_SetSubTreeSize(iParentSlot, offset,
				this->Index_GetSubTreeSize(iParentSlot, offset) - 1);

			const bool childrenAreLeaves = (iHeight == 1);

			size_t minimumCount = childrenAreLeaves
				? ((fCapacity_IndexLeaf + 1) / 2)
				: ((fCapacity_IndexInternal + 1) / 2);

			if (this->Index_GetCount(childSlot) < minimumCount)
				{
				this->DirtySlot(childSlot);

				size_t otherOffset = (offset == 0) ? 1 : offset - 1;

				uint32 newOtherChildSlotNumber;
				Slot* otherChild = this->UseSlotForked(
					this->Index_GetChild(iParentSlot, false, otherOffset),
					true, newOtherChildSlotNumber);

				if (newOtherChildSlotNumber)
					this->Index_SetChild(iParentSlot, false, otherOffset, newOtherChildSlotNumber);
				this->DirtySlot(otherChild);

				if (this->Index_GetCount(otherChild) < minimumCount)
					{
					this->Index_Merge(iParentSlot, childrenAreLeaves,
						offset, otherOffset, childSlot, otherChild);

					if (this->Index_GetCount(iParentSlot) == 0)
						{
						ZAssertStop(ZCONFIG_PhaseTree_Debug, oNewRoot);
						*oNewRoot = childSlot;
						this->ReleaseAndUnuseSlot(iParentSlot);
						return true;
						}
					}
				else
					{
					this->Index_Rotate(iParentSlot, childrenAreLeaves,
						offset, otherOffset, childSlot, otherChild);

					this->UnuseSlot(otherChild);
					}
				}
			this->UnuseSlot(childSlot);
			return true;
			}
		}
	}

ZBlockStore::BlockID ZBlockStore_PhaseTree::GetNextBlockIDRecurse(
	Slot* iParentSlot, int iHeight, BlockID iBlockID)
	{
	const size_t count = this->Index_GetCount(iParentSlot);
	const uint32* firstKey = this->Index_GetKeysAddress(iParentSlot);
	const uint32* i = upper_bound(firstKey, firstKey + count, iBlockID, NativeLessBig());
	size_t offset = i - firstKey;
	if (iHeight == 0)
		{
		// We're in a leaf.
		if (offset >= count)
			{
			// We've run off the end of the leaf.
			return 0;
			}
		return ZByteSwap_BigToHost32(*i);
		}
	else
		{
		// We're in an internal node.
		for (;;)
			{
			if (offset > count)
				{
				// We've run off the end of the internal node.
				return 0;
				}
			uint32 theSlotNumber = this->Index_GetChild(iParentSlot, false, offset);
			Slot* childSlot = this->UseSlot(theSlotNumber);
			BlockID result = this->GetNextBlockIDRecurse(childSlot, iHeight - 1, iBlockID);
			this->UnuseSlot(childSlot);
			if (result)
				return result;
			iBlockID = ZByteSwap_BigToHost32(*i);
			++offset;
			}
		}
	}

size_t ZBlockStore_PhaseTree::GetBlockSize(BlockID iBlockID, Slot*& ioBlockSlot)
	{
	size_t result = 0;

	this->ReadStart(iBlockID, ioBlockSlot);
	if (ioBlockSlot)
		result = ReadField(ioBlockSlot, 0);
	this->ReadFinish(ioBlockSlot);

	return result;
	}

void ZBlockStore_PhaseTree::SetBlockSize(BlockID iBlockID, Slot*& ioBlockSlot, size_t iSize)
	{
	this->WriteStart(iBlockID, ioBlockSlot);

	if (ioBlockSlot)
		this->SetBlockSizeImp(ioBlockSlot, iSize);

	this->WriteFinish(ioBlockSlot);
	}

void ZBlockStore_PhaseTree::SetBlockSizeIfLessThan(
	BlockID iBlockID, Slot*& ioBlockSlot, size_t iSize)
	{
	this->WriteStart(iBlockID, ioBlockSlot);

	if (ioBlockSlot)
		{
		size_t currentSize = ReadField(ioBlockSlot, 0);
		if (currentSize < iSize)
			this->SetBlockSizeImp(ioBlockSlot, iSize);
		}

	this->WriteFinish(ioBlockSlot);
	}

void ZBlockStore_PhaseTree::SetBlockSizeImp(Slot* iSlot, size_t iSize)
	{
	size_t currentSize = ReadField(iSlot, 0);

	if (iSize > currentSize)
		{
		// We're growing.
		if (iSize <= fCapacity_BlockRoot_Byte)
			{
			// Simple case. All our data is in the root node and will remain there.
			WriteField(iSlot, 0, iSize);
			}
		else
			{
			if (currentSize <= fCapacity_BlockRoot_Byte)
				{
				// Take a copy of the data in the root
				vector<char> tempBuffer(currentSize);
				sMemCopy(&tempBuffer[0], FieldAddress(iSlot, 1), currentSize);
				// Mark our root as being zero bytes in length
				WriteField(iSlot, 0, 0);
				// Grow our empty tree.
				this->Block_GrowTree(iSlot, iSize);
				// Update our block size.
				WriteField(iSlot, 0, iSize);
				// Find the leftmost data slot.
				Slot* leftMostSlot = this->Block_FindDataSlotForWrite(iSlot, 0, false);
				// And copy into it the data that was in the root.
				sMemCopy(leftMostSlot->fData, &tempBuffer[0], currentSize);
				this->UnuseSlot(leftMostSlot);
				}
			else
				{
				// Grow our tree.
				this->Block_GrowTree(iSlot, iSize);
				// Update our block size.
				WriteField(iSlot, 0, iSize);
				}
			}
		}
	else if (iSize < currentSize)
		{
		// We're shrinking
		if (currentSize <= fCapacity_BlockRoot_Byte)
			{
			// Simple case. All our data is in the root node.
			WriteField(iSlot, 0, iSize);
			}
		else
			{
			if (iSize <= fCapacity_BlockRoot_Byte)
				{
				// Find the leftmost data slot.
				Slot* leftMostSlot = this->Block_FindDataSlot(iSlot, 0);
				vector<char> tempBuffer(iSize);
				// Copy its first iSize bytes into a temporary buffer.
				sMemCopy(&tempBuffer[0], leftMostSlot->fData, iSize);
				// Release the leftmost slot.
				this->UnuseSlot(leftMostSlot);
				// Discard the entire tree.
				this->Block_KillTree(iSlot);
				// Copy the data into the root slot.
				sMemCopy(FieldAddress(iSlot, 1), &tempBuffer[0], iSize);
				// And update the block's size.
				WriteField(iSlot, 0, iSize);
				}
			else
				{
				// Prune our tree.
				this->Block_PruneTree(iSlot, iSize);
				// Update our block size.
				WriteField(iSlot, 0, iSize);
				}
			}
		}
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Slot management.

uint32 ZBlockStore_PhaseTree::AllocateSlotNumber(bool iForImmediateUse)
	{
	ASSERT_LOCKED(fMutex_Slots);

	uint32 theSlotNumber;

	if (fSlots_ReleasedForked.size())
		{
		// Ideally we re-use a slot that's been forked and then released.
		set<uint32>::iterator iter = fSlots_ReleasedForked.begin();
		theSlotNumber = *iter;
		fSlots_ReleasedForked.erase(iter);
		}
	else
		{
		// Otherwise we have to grow the file.
		theSlotNumber = fHighWater++;
		if (fHighWater > fAllocatedSpace)
			{
			fAllocatedSpace = fHighWater + 100;
			ZFile::Error err = fFileRW->SetSize(fUserHeaderSize + fAllocatedSpace * fSlotSize);
			ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
			}
		}

	if (not iForImmediateUse)
		fSlots_Forked.insert(theSlotNumber);

	return theSlotNumber;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::AllocateSlotDirty()
	{
	ASSERT_UNLOCKED(fMutex_Slots);
	fMutex_Slots.Acquire();

	uint32 theSlotNumber;

	if (fSlots_ReleasedForked.size())
		{
		// Ideally we re-use a slot that's been forked and then released.
		set<uint32>::iterator iter = fSlots_ReleasedForked.begin();
		theSlotNumber = *iter;
		fSlots_ReleasedForked.erase(iter);
		}
	else
		{
		// Otherwise we have to grow the file.
		theSlotNumber = fHighWater++;
		if (fHighWater > fAllocatedSpace)
			{
			fAllocatedSpace = fHighWater + 100;
			ZFile::Error err = fFileRW->SetSize(fUserHeaderSize + fAllocatedSpace * fSlotSize);
			ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
			}
		}

	Slot* theSlot = new(fSlotSize) Slot(theSlotNumber);

	fSlots_Loaded[theSlotNumber] = theSlot;

	if (ZCONFIG_Debug >= ZCONFIG_PhaseTree_Debug)
		sMemSet(theSlot->fData, 0xAA, fSlotSize);

	fMutex_Slots.Release();
	theSlot->fMutex.Acquire();
	return theSlot;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseRootSlot()
	{
	ASSERT_LOCKED(fMutex_MetaRoot);

	if (fRootSlotNumber)
		return this->UseSlot(fRootSlotNumber);
	return nullptr;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseRootSlotReal(bool iDirtied)
	{
	ASSERT_LOCKED(fMutex_MetaRoot);

	Slot* rootSlot;
	if (fRootSlotNumber)
		{
		uint32 newSlotNumber;
		rootSlot = this->UseSlotReal(iDirtied, fRootSlotNumber, true, newSlotNumber);
		if (newSlotNumber)
			fRootSlotNumber = newSlotNumber;
		}
	else
		{
		rootSlot = this->AllocateSlotDirty();
		this->Index_Initialize(rootSlot);
		fRootSlotNumber = rootSlot->fSlotNumber;
		}
	return rootSlot;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseRootSlotForked()
	{
	return this->UseRootSlotReal(false);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseRootSlotDirty()
	{
	return this->UseRootSlotReal(true);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseSlot(uint32 iSlotNumber)
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlotNumber);

	ASSERT_UNLOCKED(fMutex_Slots);

	// The caller already has the lock on the parent slot, so there can be no contention
	// as far as trying to _load_ the requested slot is concerned. If the requested slot
	// is already loaded then there may be contention on acquiring _its_ lock.

	// The use count of a slot is manipulated under the protection of fMutex_Slots.
	Slot* theSlot = nullptr;
	fMutex_Slots.Acquire();

	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlotNumber < fHighWater);

	if (fUseRequests >= 0x20000000 || fUseCached >= 0x20000000 || fUseFailures >= 0x20000000)
		{
		fUseRequests >>= 1;
		fUseCached >>= 1;
		fUseFailures >>= 1;
		}
	++fUseRequests;

	map<uint32, Slot*>::iterator iterLoaded = fSlots_Loaded.find(iSlotNumber);
	if (iterLoaded == fSlots_Loaded.end())
		{
		map<uint32, Slot*>::iterator iterCached = fSlots_Cached.find(iSlotNumber);
		if (iterCached == fSlots_Cached.end())
			{
			++fUseFailures;

			ZAssertStopf(ZCONFIG_PhaseTree_Debug,
				not sContains(fSlots_ReleasedForked, iSlotNumber),
				"Slot: %d", iSlotNumber);

			ZAssertStopf(ZCONFIG_PhaseTree_Debug,
				not sContains(fSlots_ReleasedClean, iSlotNumber),
				"Slot: %d", iSlotNumber);

			if (sQErase(fSlots_Forked, iSlotNumber))
				theSlot = new(fSlotSize) Slot(iSlotNumber, true);
			else
				theSlot = new(fSlotSize) Slot(iSlotNumber, false);

			fSlots_Loaded[iSlotNumber] = theSlot;

			fMutex_Slots.Release();
			ZFile::Error err = fFileR->ReadAt(fUserHeaderSize + uint64(iSlotNumber) * fSlotSize,
				theSlot->fData, fSlotSize, nullptr);

			ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
			}
		else
			{
			++fUseCached;
			theSlot = (*iterCached).second;

			ZAssertStop(ZCONFIG_PhaseTree_Debug, theSlot->fSlotNumber == iSlotNumber);

			fSlots_Cached.erase(iterCached);

			if (theSlot->fCached_Prev)
				theSlot->fCached_Prev->fCached_Next = theSlot->fCached_Next;
			if (theSlot->fCached_Next)
				theSlot->fCached_Next->fCached_Prev = theSlot->fCached_Prev;

			if (fCached_Head == theSlot)
				fCached_Head = theSlot->fCached_Next;
			if (fCached_Tail == theSlot)
				fCached_Tail = theSlot->fCached_Prev;

			theSlot->fCached_Prev = nullptr;
			theSlot->fCached_Next = nullptr;

			fSlots_Loaded[iSlotNumber] = theSlot;
			++theSlot->fUseCount;

			ZAssertStop(ZCONFIG_PhaseTree_Debug,
				theSlot->fState != eStateReleasedForked && theSlot->fState != eStateReleasedClean);
			fMutex_Slots.Release();
			}
		}
	else
		{
		theSlot = (*iterLoaded).second;
		ZAssertStop(ZCONFIG_PhaseTree_Debug, theSlot->fSlotNumber == iSlotNumber);
		++theSlot->fUseCount;

		ZAssertStop(ZCONFIG_PhaseTree_Debug,
			theSlot->fState != eStateReleasedForked && theSlot->fState != eStateReleasedClean);
		fMutex_Slots.Release();
		}

	theSlot->fMutex.Acquire();

	return theSlot;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseSlotReal(
	bool iDirtied, uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber)
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlotNumber);

	ASSERT_UNLOCKED(fMutex_Slots);

	// The caller already has the lock on the parent slot, so there can be no contention
	// as far as trying to _load_ the requested slot is concerned. If the requested slot
	// is already loaded then there may be contention on acquiring _its_ lock.

	// The use count of a slot is manipulated under the protection of fMutex_Slots.
	Slot* theSlot = nullptr;
	fMutex_Slots.Acquire();

	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlotNumber < fHighWater);

	if (fUseRequests >= 0x20000000 || fUseCached >= 0x20000000 || fUseFailures >= 0x20000000)
		{
		fUseRequests >>= 1;
		fUseCached >>= 1;
		fUseFailures >>= 1;
		}
	++fUseRequests;

	map<uint32, Slot*>::iterator iterLoaded = fSlots_Loaded.find(iSlotNumber);
	if (iterLoaded == fSlots_Loaded.end())
		{
		map<uint32, Slot*>::iterator iterCached = fSlots_Cached.find(iSlotNumber);
		if (iterCached == fSlots_Cached.end())
			{
			++fUseFailures;
			ZAssertStopf(ZCONFIG_PhaseTree_Debug,
				not sContains(fSlots_ReleasedForked, iSlotNumber),
				"Slot: %d", iSlotNumber);

			ZAssertStopf(ZCONFIG_PhaseTree_Debug,
				not sContains(fSlots_ReleasedClean, iSlotNumber),
				"Slot: %d", iSlotNumber);

			if (sQErase(fSlots_Forked, iSlotNumber))
				{
				oForkedSlotNumber = 0;
				theSlot = new(fSlotSize) Slot(iSlotNumber);
				fSlots_Loaded[iSlotNumber] = theSlot;
				}
			else
				{
				oForkedSlotNumber = this->AllocateSlotNumber(true);
				fSlots_ReleasedClean.insert(iSlotNumber);
				theSlot = new(fSlotSize) Slot(oForkedSlotNumber);
				fSlots_Loaded[oForkedSlotNumber] = theSlot;
				}
			fMutex_Slots.Release();

			if (iReadOriginalContent)
				{
				ZFile::Error err = fFileRW->ReadAt(
					fUserHeaderSize + uint64(iSlotNumber) * fSlotSize,
					theSlot->fData, fSlotSize, nullptr);

				ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
				}
			}
		else
			{
			++fUseCached;
			Slot* existingSlot = (*iterCached).second;
			ZAssertStop(ZCONFIG_PhaseTree_Debug, existingSlot->fSlotNumber == iSlotNumber);

			fSlots_Cached.erase(iterCached);

			if (existingSlot->fCached_Prev)
				existingSlot->fCached_Prev->fCached_Next = existingSlot->fCached_Next;
			if (existingSlot->fCached_Next)
				existingSlot->fCached_Next->fCached_Prev = existingSlot->fCached_Prev;

			if (fCached_Head == existingSlot)
				fCached_Head = existingSlot->fCached_Next;
			if (fCached_Tail == existingSlot)
				fCached_Tail = existingSlot->fCached_Prev;

			existingSlot->fCached_Prev = nullptr;
			existingSlot->fCached_Next = nullptr;

			fSlots_Loaded[iSlotNumber] = existingSlot;
			++existingSlot->fUseCount;

			if (existingSlot->fState == eStateClean)
				{
				fMutex_Slots.Release();
				existingSlot->fMutex.Acquire();
				fMutex_Slots.Acquire();
				oForkedSlotNumber = this->AllocateSlotNumber(true);
				theSlot = new(fSlotSize) Slot(oForkedSlotNumber);
				sMemCopy(theSlot->fData, existingSlot->fData, fSlotSize);

				fSlots_Loaded[oForkedSlotNumber] = theSlot;
				fMutex_Slots.Release();

				this->ReleaseAndUnuseSlot(existingSlot);
				}
			else
				{
				oForkedSlotNumber = 0;
				theSlot = existingSlot;
				ZAssertStop(ZCONFIG_PhaseTree_Debug,theSlot->fState != eStateReleasedForked
					&& theSlot->fState != eStateReleasedClean);
				if (iDirtied)
					theSlot->fState = eStateDirty;
				fMutex_Slots.Release();
				}
			}
		}
	else
		{
		Slot* existingSlot = (*iterLoaded).second;
		ZAssertStop(ZCONFIG_PhaseTree_Debug, existingSlot->fSlotNumber == iSlotNumber);
		++existingSlot->fUseCount;

		if (existingSlot->fState == eStateClean)
			{
			fMutex_Slots.Release();
			existingSlot->fMutex.Acquire();
			fMutex_Slots.Acquire();
			oForkedSlotNumber = this->AllocateSlotNumber(true);
			theSlot = new(fSlotSize) Slot(oForkedSlotNumber);
			sMemCopy(theSlot->fData, existingSlot->fData, fSlotSize);

			fSlots_Loaded[oForkedSlotNumber] = theSlot;
			fMutex_Slots.Release();

			this->ReleaseAndUnuseSlot(existingSlot);
			}
		else
			{
			oForkedSlotNumber = 0;
			theSlot = existingSlot;
			ZAssertStop(ZCONFIG_PhaseTree_Debug, theSlot->fState != eStateReleasedForked
				&& theSlot->fState != eStateReleasedClean);
			if (iDirtied)
				theSlot->fState = eStateDirty;
			fMutex_Slots.Release();
			}
		}

	theSlot->fMutex.Acquire();

	return theSlot;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseSlotForked(
	uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber)
	{
	return this->UseSlotReal(false, iSlotNumber, iReadOriginalContent, oForkedSlotNumber);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::UseSlotDirty(
	uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber)
	{
	return this->UseSlotReal(true, iSlotNumber, iReadOriginalContent, oForkedSlotNumber);
	}

void ZBlockStore_PhaseTree::UnuseSlot(Slot* iSlot, bool iPurge)
	{
	ASSERT_LOCKED(iSlot->fMutex);
	ASSERT_UNLOCKED(fMutex_Slots);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlot->fSlotNumber);

	fMutex_Slots.Acquire();

	if (iSlot->fUseCount == 1 && iSlot->fState == eStateDirty)
		{
		iSlot->fState = eStateForked;
		fMutex_Slots.Release();
		ZFile::Error err = fFileRW->WriteAt(
			fUserHeaderSize + uint64(iSlot->fSlotNumber) * fSlotSize, iSlot->fData, fSlotSize, nullptr);
		ZAssertStop(kDebug_PhaseTreeFile, err == ZFile::errorNone);
		fMutex_Slots.Acquire();
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlot->fSlotNumber);
		}

	if (--iSlot->fUseCount == 0)
		{
		switch (iSlot->fState)
			{
			case eStateDirty:
				{
				ZUnimplemented();
				break;
				}
			case eStateClean:
			case eStateForked:
				{
				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_Forked, iSlot->fSlotNumber));

				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_ReleasedForked, iSlot->fSlotNumber));

				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_ReleasedClean, iSlot->fSlotNumber));

				sEraseMust(ZCONFIG_PhaseTree_Debug,
					fSlots_Loaded, iSlot->fSlotNumber);

				if (true)
					{
					// Enable this section to use the cache.
					iSlot->fCached_Prev = fCached_Tail;
					if (fCached_Tail)
						fCached_Tail->fCached_Next = iSlot;
					else
						fCached_Head = iSlot;

					fCached_Tail = iSlot;

					ZAssertStop(ZCONFIG_PhaseTree_Debug,
						not sContains(fSlots_Cached, iSlot->fSlotNumber));

					fSlots_Cached[iSlot->fSlotNumber] = iSlot;

					iSlot->fMutex.Release();

					if (fSlots_Cached.size() > fCapacity_BlockRoot_Slots)
						{
						size_t cacheMax = fCapacity_BlockRoot_Slots / 2;
						while (fSlots_Cached.size() > cacheMax)
							{
							// Lose the head
							Slot* theSlot = fCached_Head;
							fCached_Head = theSlot->fCached_Next;
							if (fCached_Head)
								fCached_Head->fCached_Prev = nullptr;
							else
								fCached_Tail = nullptr;

							ZAssertStop(ZCONFIG_PhaseTree_Debug, theSlot->fSlotNumber);

							map<uint32, Slot*>::iterator cacheIter =
								fSlots_Cached.find(theSlot->fSlotNumber);
							ZAssertStopf(ZCONFIG_PhaseTree_Debug,
								cacheIter != fSlots_Cached.end(),
								"Slot: %d, state: %d", theSlot->fSlotNumber, theSlot->fState);

							fSlots_Cached.erase(cacheIter);

							if (theSlot->fState == eStateForked)
								fSlots_Forked.insert(theSlot->fSlotNumber);
							theSlot->fCached_Next = nullptr;
							theSlot->fCached_Prev = nullptr;
							delete theSlot;
							}
						}
					}
				else
					{
					// This variant does not put the slot into the cache.
					if (iSlot->fState == eStateForked)
						fSlots_Forked.insert(iSlot->fSlotNumber);
					delete iSlot;
					}
				break;
				}
			case eStateReleasedClean:
			case eStateReleasedForked:
				{
				// This slot should have been removed from
				// fSlots_Loaded when ReleaseAndUnuseSlot was called.
				ZAssertStopf(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_Loaded, iSlot->fSlotNumber),
					"Slot: %d", iSlot->fSlotNumber);
				delete iSlot;
				break;
				}
			}
		}
	else
		{
		iSlot->fMutex.Release();
		}
	fMutex_Slots.Release();
	}

void ZBlockStore_PhaseTree::UnuseUnlockedSlot(Slot* iSlot)
	{
	// For a change the slot must be _unlocked_
	ASSERT_UNLOCKED(iSlot->fMutex);
	ASSERT_UNLOCKED(fMutex_Slots);

	fMutex_Slots.Acquire();

	if (--iSlot->fUseCount == 0)
		{
		switch (iSlot->fState)
			{
			case eStateDirty:
			case eStateForked:
				{
				// There are three callers of UnuseUnlockedSlot: Flush, WriteStart
				// and StreamDisposing. Flush will have marked clean any slots which
				// it might pass to this method. WriteStart only calls this method
				// for slots which are not eStateForked and not eStateDirty. StreamDisposing
				// only calls it for block slots which are released whilst a stream is
				// referencing them, which thus have a zero fSlotNumber and whose fState
				// is either eStateReleasedClean or eStateReleasedForked.
				ZUnimplemented();
				break;
				}
			case eStateClean:
				{
				ZAssertStop(ZCONFIG_PhaseTree_Debug, iSlot->fSlotNumber);

				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_Forked, iSlot->fSlotNumber));

				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_ReleasedForked, iSlot->fSlotNumber));

				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					not sContains(fSlots_ReleasedClean, iSlot->fSlotNumber));

				sEraseMust(ZCONFIG_PhaseTree_Debug,
					fSlots_Loaded, iSlot->fSlotNumber);

				if (iSlot->fState == eStateForked)
					fSlots_Forked.insert(iSlot->fSlotNumber);
				delete iSlot;
				break;
				}
			case eStateReleasedClean:
			case eStateReleasedForked:
				{
				// This slot should have been removed from fSlots_Loaded when ReleaseAndUnuseSlot
				// was called. If it's a block slot which has been deleted out from under a
				// stream then its fSlotNumber will have been set to zero.
				ZAssertStop(ZCONFIG_PhaseTree_Debug,
					iSlot->fSlotNumber == 0
					|| not sContains(fSlots_Loaded, iSlot->fSlotNumber));
				delete iSlot;
				break;
				}
			}
		}
	fMutex_Slots.Release();
	}

void ZBlockStore_PhaseTree::DirtySlot(Slot* iSlot)
	{
	ASSERT_LOCKED(iSlot->fMutex);
	ASSERT_UNLOCKED(fMutex_Slots);

	fMutex_Slots.Acquire();

	ZAssertStop(ZCONFIG_PhaseTree_Debug,
		iSlot->fState == eStateForked || iSlot->fState == eStateDirty);

	iSlot->fState = eStateDirty;

	fMutex_Slots.Release();
	}

void ZBlockStore_PhaseTree::ReleaseSlotNumber(uint32 iSlotNumber)
	{
	ASSERT_LOCKED(fMutex_Slots);

	// This is only called by Block_PruneTreeRecurse, Extent_RecursiveDelete
	// and Extent_RecursiveDeletePartial, for which the slot in question
	// cannot be loaded, because the ancestor block slot is locked.

	ZAssertStopf(ZCONFIG_PhaseTree_Debug,
		not sContains(fSlots_Loaded, iSlotNumber), "Slot: %d", iSlotNumber);

	// However, the slot could be cached.
	map<uint32, Slot*>::iterator iterCached = fSlots_Cached.find(iSlotNumber);
	if (iterCached != fSlots_Cached.end())
		{
		Slot* theSlot = (*iterCached).second;
		ZAssertStop(ZCONFIG_PhaseTree_Debug, theSlot->fSlotNumber == iSlotNumber);

		fSlots_Cached.erase(iterCached);

		if (theSlot->fCached_Prev)
			theSlot->fCached_Prev->fCached_Next = theSlot->fCached_Next;
		if (theSlot->fCached_Next)
			theSlot->fCached_Next->fCached_Prev = theSlot->fCached_Prev;

		if (fCached_Head == theSlot)
			fCached_Head = theSlot->fCached_Next;
		if (fCached_Tail == theSlot)
			fCached_Tail = theSlot->fCached_Prev;

		theSlot->fCached_Prev = nullptr;
		theSlot->fCached_Next = nullptr;
		delete theSlot;
		}

	if (sQErase(fSlots_Forked, iSlotNumber))
		fSlots_ReleasedForked.insert(iSlotNumber);
	else
		fSlots_ReleasedClean.insert(iSlotNumber);
	}

void ZBlockStore_PhaseTree::ReleaseAndUnuseSlot(Slot* iSlot)
	{
	ASSERT_LOCKED(iSlot->fMutex);
	ASSERT_UNLOCKED(fMutex_Slots);

	fMutex_Slots.Acquire();

	ZAssertStop(ZCONFIG_PhaseTree_Debug,
		iSlot->fState == eStateClean
		|| iSlot->fState == eStateForked
		|| iSlot->fState == eStateDirty);

	if (iSlot->fState == eStateClean)
		{
		iSlot->fState = eStateReleasedClean;
		fSlots_ReleasedClean.insert(iSlot->fSlotNumber);
		}
	else
		{
		iSlot->fState = eStateReleasedForked;
		fSlots_ReleasedForked.insert(iSlot->fSlotNumber);
		}

	sEraseMust(ZCONFIG_PhaseTree_Debug, fSlots_Loaded, iSlot->fSlotNumber);

	if (--iSlot->fUseCount == 0)
		{
		fMutex_Slots.Release();
		delete iSlot;
		}
	else
		{
		// Something is still referring to this slot. The only thing that
		// holds on to slots is our stream entities, who cache the block
		// root. Setting the slotNumber to 0 is an indication that they're
		// referencing a stale slot.
		iSlot->fSlotNumber = 0;
		fMutex_Slots.Release();
		iSlot->fMutex.Release();
		}
	}

void ZBlockStore_PhaseTree::WriteStart()
	{
	fMutex_Flush.Acquire();
	while (fCount_WaitingFlushes || fFlushInFirstStages)
		fCondition_Flush.Wait(fMutex_Flush);
	++fCount_Writes;
	fMutex_Flush.Release();
	}

void ZBlockStore_PhaseTree::WriteFinish()
	{
	fMutex_Flush.Acquire();
	ZAssertStop(ZCONFIG_PhaseTree_Debug, !fFlushInFirstStages);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, fCount_Writes > 0);
	--fCount_Writes;
	fCondition_Flush.Broadcast();
	fMutex_Flush.Release();
	}

void ZBlockStore_PhaseTree::WriteStart(BlockID iBlockID, Slot*& ioBlockSlot)
	{
	this->WriteStart();
	fMutex_Slots.Acquire();
	if (ioBlockSlot->fState != eStateForked && ioBlockSlot->fState != eStateDirty)
		{
		// The slot's no longer forked, so release it and grab a forked one.
		fMutex_Slots.Release();
		this->UnuseUnlockedSlot(ioBlockSlot);
		ioBlockSlot = this->FindBlockSlotForWrite(iBlockID);
		}
	else
		{
		// If we reogranize the code so that loaded dirty slots are written
		// back to disk without it happening because they're flushed (thus
		// becoming marked as clean) or because their ref count hits zero,
		// then we'll likely need to dirty ioBlockSlot slot here.
		fMutex_Slots.Release();
		ioBlockSlot->fMutex.Acquire();
		}
	}

void ZBlockStore_PhaseTree::WriteFinish(Slot* iBlockSlot)
	{
	if (iBlockSlot)
		iBlockSlot->fMutex.Release();

	this->WriteFinish();
	}

void ZBlockStore_PhaseTree::ReadStart(BlockID iBlockID, Slot*& ioBlockSlot)
	{
	fMutex_Slots.Acquire();
	if (ioBlockSlot->fState == eStateReleasedClean || ioBlockSlot->fState == eStateReleasedForked)
		{
		fMutex_Slots.Release();
		ioBlockSlot = this->FindBlockSlot(iBlockID);
		}
	else
		{
		fMutex_Slots.Release();
		ioBlockSlot->fMutex.Acquire();
		}
	}

void ZBlockStore_PhaseTree::ReadFinish(Slot* iBlockSlot)
	{
	if (iBlockSlot)
		iBlockSlot->fMutex.Release();
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Index node manipulation.

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::Index_FindChild(
	Slot* iSlot, bool iIsLeaf, BlockID iBlockID, size_t& oOffset)
	{
	const size_t count = this->Index_GetCount(iSlot);
	uint32* firstKey = this->Index_GetKeysAddress(iSlot);
	const uint32* i = lower_bound(firstKey, firstKey + count, iBlockID, BigLessNative());
	oOffset = i - firstKey;

	if (iIsLeaf)
		{
		if (oOffset >= count || ZByteSwap_BigToHost32(*i) != iBlockID)
			return nullptr;
		}

	uint32 theSlotNumber = this->Index_GetChild(iSlot, iIsLeaf, oOffset);
	return this->UseSlot(theSlotNumber);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::Index_FindChildForked(
	Slot* iSlot, bool iIsLeaf, BlockID iBlockID, size_t& oOffset, uint32& oForkedChildSlotNumber)
	{
	const size_t count = this->Index_GetCount(iSlot);
	uint32* firstKey = this->Index_GetKeysAddress(iSlot);
	const uint32* i = lower_bound(firstKey, firstKey + count, iBlockID, BigLessNative());
	oOffset = i - firstKey;

	if (iIsLeaf)
		{
		if (oOffset >= count || ZByteSwap_BigToHost32(*i) != iBlockID)
			return nullptr;
		}

	uint32 theSlotNumber = this->Index_GetChild(iSlot, iIsLeaf, oOffset);

	return this->UseSlotForked(theSlotNumber, true, oForkedChildSlotNumber);
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::Index_CreateRoot(
	Slot* iLeftChild, size_t iLeftSubTreeSize,
	BlockID iKey, Slot* iRightChild, size_t iRightSubTreeSize)
	{
	Slot* newRoot = this->AllocateSlotDirty();
	WriteField(newRoot, 0, 1); // Count
	WriteField(newRoot, 1, iKey);
	WriteField(newRoot, 1 + fCapacity_IndexInternal, iLeftChild->fSlotNumber);
	WriteField(newRoot, 1 + fCapacity_IndexInternal + 1, iRightChild->fSlotNumber);
	WriteField(newRoot, 2 + 2 * fCapacity_IndexInternal, iLeftSubTreeSize);
	WriteField(newRoot, 2 + 2 * fCapacity_IndexInternal + 1, iRightSubTreeSize);
	return newRoot;
	}

void ZBlockStore_PhaseTree::Index_Split(Slot* iSlot, bool iIsLeaf,
	size_t& oLeftSubTreeSize, BlockID& oMedianKey, Slot*& oNewSibling, size_t& oRightSubTreeSize)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	// Get the key which is to be promoted to our parent, and its offset.
	const size_t medianOffset = count / 2;
	oMedianKey = this->Index_GetKey(iSlot, iIsLeaf, medianOffset);

	/*
	We don't allow repeated keys, so we could take advantage of that
	fact to tend towards full nodes rather than simply guaranteeing no
	node being less than half full (except the root, of course). It would
	require that this method know the height at which we're operating, thus being
	able to determine the optimal fan out and being able to choose which
	key should be the one around which we split. I've done it before in
	an older version of the code, but don't want to complicate this code any further.
	*/

	if (iIsLeaf)
		{
		// How big are the left and right sub trees?
		oLeftSubTreeSize = medianOffset + 1;
		oRightSubTreeSize = count - medianOffset - 1;

		// Allocate the new sibling which will take the data that lies to the right of the key.
		oNewSibling = this->AllocateSlotDirty();
		WriteField(oNewSibling, 0, oRightSubTreeSize);

		// Copy the keys and slot numbers
		for (size_t x = 0; x < oRightSubTreeSize; ++x)
			{
			BlockID aKey = this->Index_GetKey(iSlot, true, medianOffset + 1 + x);
			uint32 aSlotNumber = this->Index_GetChild(iSlot, true, medianOffset + 1 + x);
			this->Index_SetKey(oNewSibling, true, x, aKey);
			this->Index_SetChild(oNewSibling, true, x, aSlotNumber);
			}

		// Note that for leaves we do *not* erase our copy of the median key.
		this->Index_LeafDeleteToEnd(iSlot, medianOffset + 1);
		ZAssertStop(ZCONFIG_PhaseTree_Debug, this->Index_GetCount(iSlot) == oLeftSubTreeSize);
		}
	else
		{
		// How big is the sub tree to the left of the key?
		oLeftSubTreeSize = 0;
		for (size_t x = 0; x < medianOffset + 1; ++x)
			oLeftSubTreeSize += this->Index_GetSubTreeSize(iSlot, x);
		// And the right?
		oRightSubTreeSize = 0;
		for (size_t x = medianOffset + 1; x <= count; ++x)
			oRightSubTreeSize += this->Index_GetSubTreeSize(iSlot, x);

		// Allocate the new sibling which will take the data that lies to the right of the key.
		const size_t sibCount = count - medianOffset - 1;
		oNewSibling = this->AllocateSlotDirty();
		WriteField(oNewSibling, 0, sibCount);

		// Copy the keys
		for (size_t x = 0; x < sibCount; ++x)
			{
			BlockID aKey = this->Index_GetKey(iSlot, false, medianOffset + 1 + x);
			this->Index_SetKey(oNewSibling, false, x, aKey);
			}

		// Copy the child slot number and sub tree sizes
		for (size_t x = 0; x <= sibCount; ++x)
			{
			uint32 aSlotNumber = this->Index_GetChild(iSlot, false, medianOffset + 1 + x);
			uint32 aSubTreeSize = this->Index_GetSubTreeSize(iSlot, medianOffset + 1 + x);
			this->Index_SetChild(oNewSibling, false, x, aSlotNumber);
			this->Index_SetSubTreeSize(oNewSibling, x, aSubTreeSize);
			}

		this->Index_InternalDeleteToEnd(iSlot, medianOffset);
		}

	VALIDATESLOT(iSlot);
	VALIDATESLOT(oNewSibling);
	}

void ZBlockStore_PhaseTree::Index_Merge(Slot* iSlot, bool iSlotsAreLeaves,
	size_t iOffsetA, size_t iOffsetB, Slot* iSlotA, Slot* iSlotB)
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffsetA != iOffsetB);

	size_t countA = this->Index_GetCount(iSlotA);
	size_t countB = this->Index_GetCount(iSlotB);
	if (iSlotsAreLeaves)
		{
		if (iOffsetA < iOffsetB)
			{
			// Put iSlotB's contents on iSlotA's right.
			this->Index_LeafMakeSpaceOnRight(iSlotA, countB);
			for (size_t x = 0; x < countB; ++x)
				{
				BlockID aKey = this->Index_GetKey(iSlotB, true, x);
				uint32 aSlotNumber = this->Index_GetChild(iSlotB, true, x);
				this->Index_SetKey(iSlotA, true, countA + x, aKey);
				this->Index_SetChild(iSlotA, true, countA + x, aSlotNumber);
				}

			// We're going to be deleting the key, child slot number and sub tree size in iSlot
			// at iOffsetB, but we need the key at iOffsetB to remain and to lose the key at
			// iOffset A, so copy it now.
			if (this->Index_GetCount(iSlot) > 1)
				{
				uint32 movedKey = this->Index_GetKey(iSlot, true, iOffsetB);
				this->Index_SetKey(iSlot, true, iOffsetA, movedKey);
				}
			}
		else
			{
			// Put iSlotB's contents on iSlotA's left.
			this->Index_LeafMakeSpaceOnLeft(iSlotA, countB);
			for (size_t x = 0; x < countB; ++x)
				{
				BlockID aKey = this->Index_GetKey(iSlotB, true, x);
				uint32 aSlotNumber = this->Index_GetChild(iSlotB, true, x);
				this->Index_SetKey(iSlotA, true, x, aKey);
				this->Index_SetChild(iSlotA, true, x, aSlotNumber);
				}
			}
		this->Index_SetSubTreeSize(iSlot, iOffsetA, countA + countB);

		this->Index_InternalDeleteOneTowardsRight(iSlot, iOffsetB);
		}
	else
		{
		if (iOffsetA < iOffsetB)
			{
			// Put iSlot's key followed by B's contents on A's right.

			this->Index_InternalMakeSpaceOnRight(iSlotA, countB + 1);

			// Set the key fields.
			BlockID separatingKey = this->Index_GetKey(iSlot, false, iOffsetA);
			this->Index_SetKey(iSlotA, false, countA, separatingKey);
			for (size_t x = 0; x < countB; ++x)
				{
				BlockID aKey = this->Index_GetKey(iSlotB, false, x);
				this->Index_SetKey(iSlotA, false, countA + 1 + x, aKey);
				}
			// And the child slots/sub tree sizes.
			for (size_t x = 0; x <= countB; ++x)
				{
				uint32 aSlotNumber = this->Index_GetChild(iSlotB, false, x);
				size_t aSubTreeSize = this->Index_GetSubTreeSize(iSlotB, x);
				this->Index_SetChild(iSlotA, false, countA + 1 + x, aSlotNumber);
				this->Index_SetSubTreeSize(iSlotA, countA + 1 + x, aSubTreeSize);
				}

			// We're going to be deleting the key, child slot number and sub tree size in iSlot at
			// iOffsetB, but we need the key at iOffsetB to remain and to lose the key
			// at iOffset A, so copy it now.
			if (this->Index_GetCount(iSlot) > 1)
				{
				uint32 movedKey = this->Index_GetKey(iSlot, false, iOffsetB);
				this->Index_SetKey(iSlot, false, iOffsetA, movedKey);
				}

			}
		else
			{
			// Put B's contents followed by iSlot's key on A's left.

			this->Index_InternalMakeSpaceOnLeft(iSlotA, countB + 1);

			// Set the key fields.
			BlockID separatingKey = this->Index_GetKey(iSlot, false, iOffsetB);
			this->Index_SetKey(iSlotA, false, countB, separatingKey);
			for (size_t x = 0; x < countB; ++x)
				{
				BlockID aKey = this->Index_GetKey(iSlotB, false, x);
				this->Index_SetKey(iSlotA, false, x, aKey);
				}
			// And the child slots/sub tree sizes.
			for (size_t x = 0; x <= countB; ++x)
				{
				uint32 aSlotNumber = this->Index_GetChild(iSlotB, false, x);
				size_t aSubTreeSize = this->Index_GetSubTreeSize(iSlotB, x);
				this->Index_SetChild(iSlotA, false, x, aSlotNumber);
				this->Index_SetSubTreeSize(iSlotA, x, aSubTreeSize);
				}
			}
		size_t subTreeSizeA = this->Index_GetSubTreeSize(iSlot, iOffsetA);
		size_t subTreeSizeB = this->Index_GetSubTreeSize(iSlot, iOffsetB);
		this->Index_SetSubTreeSize(iSlot, iOffsetA, subTreeSizeA + subTreeSizeB);
		this->Index_InternalDeleteOneTowardsRight(iSlot, iOffsetB);
		}
	this->ReleaseAndUnuseSlot(iSlotB);
	}

void ZBlockStore_PhaseTree::Index_Rotate(Slot* iSlot, bool iSlotsAreLeaves,
	size_t iOffsetA, size_t iOffsetB, Slot* iSlotA, Slot* iSlotB)
	{
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffsetA != iOffsetB);

	size_t countA = this->Index_GetCount(iSlotA);
	size_t countB = this->Index_GetCount(iSlotB);

	BlockID keyToParent;
	size_t subTreeSizeTransferred;
	if (iSlotsAreLeaves)
		{
		if (iOffsetA < iOffsetB)
			{
			// Rotate B's leftmost key to parent and to A's right.
			keyToParent = this->Index_GetKey(iSlotB, true, 0);
			uint32 aSlotNumber = this->Index_GetChild(iSlotB, true, 0);
			this->Index_LeafDeleteLeftmost(iSlotB);

			this->Index_LeafMakeSpaceOnRight(iSlotA, 1);
			this->Index_SetKey(iSlotA, true, countA, keyToParent);
			this->Index_SetChild(iSlotA, true, countA, aSlotNumber);
			}
		else
			{
			// Rotate B's rightmost key to A's left, and copy its second to right
			// key to the parent.
			keyToParent = this->Index_GetKey(iSlotB, true, countB - 2);
			BlockID aKey = this->Index_GetKey(iSlotB, true, countB - 1);
			uint32 aSlotNumber = this->Index_GetChild(iSlotB, true, countB - 1);
			this->Index_LeafDeleteRightmost(iSlotB);

			this->Index_LeafMakeSpaceOnLeft(iSlotA, 1);
			this->Index_SetKey(iSlotA, true, 0, aKey);
			this->Index_SetChild(iSlotA, true, 0, aSlotNumber);
			}

		ZAssertStop(ZCONFIG_PhaseTree_Debug,
			this->Index_GetSubTreeSize(iSlot, iOffsetA) == countA);

		ZAssertStop(ZCONFIG_PhaseTree_Debug,
			this->Index_GetSubTreeSize(iSlot, iOffsetB) == countB);

		subTreeSizeTransferred = 1;
		}
	else
		{
		if (iOffsetA < iOffsetB)
			{
			// Rotate parent key to A's right and B's leftmost key to parent.
			BlockID keyFromParent = this->Index_GetKey(iSlot, false, iOffsetA);
			keyToParent = this->Index_GetKey(iSlotB, false, 0);

			uint32 aSlotNumber = this->Index_GetChild(iSlotB, false, 0);
			subTreeSizeTransferred = this->Index_GetSubTreeSize(iSlotB, 0);
			this->Index_InternalDeleteLeftmost(iSlotB);

			this->Index_InternalMakeSpaceOnRight(iSlotA, 1);
			this->Index_SetKey(iSlotA, false, countA, keyFromParent);
			this->Index_SetChild(iSlotA, false, countA + 1, aSlotNumber);
			this->Index_SetSubTreeSize(iSlotA, countA + 1, subTreeSizeTransferred);
			}
		else
			{
			// Put B's rightmost entries on A's left
			BlockID keyFromParent = this->Index_GetKey(iSlot, false, iOffsetB);
			keyToParent = this->Index_GetKey(iSlotB, false, countB - 1);

			uint32 aSlotNumber = this->Index_GetChild(iSlotB, false, countB);
			subTreeSizeTransferred = this->Index_GetSubTreeSize(iSlotB, countB);
			this->Index_InternalDeleteRightmost(iSlotB);

			this->Index_InternalMakeSpaceOnLeft(iSlotA, 1);
			this->Index_SetKey(iSlotA, false, 0, keyFromParent);
			this->Index_SetChild(iSlotA, false, 0, aSlotNumber);
			this->Index_SetSubTreeSize(iSlotA, 0, subTreeSizeTransferred);
			}
		}

	if (iOffsetA < iOffsetB)
		this->Index_SetKey(iSlot, false, iOffsetA, keyToParent);
	else
		this->Index_SetKey(iSlot, false, iOffsetB, keyToParent);

	this->Index_SetSubTreeSize(iSlot, iOffsetA,
		this->Index_GetSubTreeSize(iSlot, iOffsetA) + subTreeSizeTransferred);

	this->Index_SetSubTreeSize(iSlot, iOffsetB,
		this->Index_GetSubTreeSize(iSlot, iOffsetB) - subTreeSizeTransferred);
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Index node accessors.

void ZBlockStore_PhaseTree::Index_Initialize(Slot* iSlot)
	{
	WriteField(iSlot, 0, 0);
	}

void ZBlockStore_PhaseTree::Index_SetKey(Slot* iSlot, bool iIsLeaf, size_t iOffset, BlockID iKey)
	{
	if (iIsLeaf)
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
	else
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
	WriteField(iSlot, 1 + iOffset, iKey);
	}

void ZBlockStore_PhaseTree::Index_SetChild(Slot* iSlot, bool iIsLeaf,
	size_t iOffset, uint32 iSlotNumber)
	{
	if (iIsLeaf)
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < ReadField(iSlot, 0));
		WriteField(iSlot, 1 + fCapacity_IndexLeaf + iOffset, iSlotNumber);
		}
	else
		{
		ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset <= ReadField(iSlot, 0));
		WriteField(iSlot, 1 + fCapacity_IndexInternal + iOffset, iSlotNumber);
		}
	}

void ZBlockStore_PhaseTree::Index_SetSubTreeSize(Slot* iSlot, size_t iOffset, uint32 iSubTreeSize)
	{
	VALIDATESLOT(iSlot);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset <= ReadField(iSlot, 0));
	WriteField(iSlot, 2 + 2 * fCapacity_IndexInternal + iOffset, iSubTreeSize);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_InternalMakeSpaceOnLeft(Slot* iSlot, size_t iCount)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + iCount <= fCapacity_IndexInternal);

	// Move the keys
	Move(iSlot, 1, 1 + iCount, count);
	Fill(iSlot, 1, iCount);

	// Move the slots
	Move(iSlot, 1 + fCapacity_IndexInternal, 1 + fCapacity_IndexInternal + iCount, count + 1);
	Fill(iSlot, 1 + fCapacity_IndexInternal, iCount);

	// And the subtree sizes
	Move(iSlot, 2 + 2 * fCapacity_IndexInternal, 2 + 2 * fCapacity_IndexInternal + iCount,
		count + 1);

	Fill(iSlot, 2 + 2 * fCapacity_IndexInternal, iCount);

	WriteField(iSlot, 0, count + iCount);
	}

void ZBlockStore_PhaseTree::Index_InternalMakeSpaceOnRight(Slot* iSlot, size_t iCount)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + iCount <= fCapacity_IndexInternal);
	WriteField(iSlot, 0, count + iCount);
	}

void ZBlockStore_PhaseTree::Index_InternalMakeOneSpaceBefore(Slot* iSlot, size_t iOffset)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + 1 <= fCapacity_IndexInternal);

	// Move the keys
	Move(iSlot, 1 + iOffset, 1 + iOffset + 1, count - iOffset);
	Fill(iSlot, 1 + iOffset, 1);

	// Move the slots
	Move(iSlot, 1 + fCapacity_IndexInternal + iOffset, 1 + fCapacity_IndexInternal + iOffset + 1,
		count + 1 - iOffset);

	Fill(iSlot, 1 + fCapacity_IndexInternal + iOffset, 1);

	// And the subtree sizes
	Move(iSlot, 2 + 2 * fCapacity_IndexInternal + iOffset,
		2 + 2 * fCapacity_IndexInternal + iOffset + 1, count + 1 - iOffset);

	Fill(iSlot, 2 + 2 * fCapacity_IndexInternal + iOffset, 1);

	WriteField(iSlot, 0, count + 1);
	}

void ZBlockStore_PhaseTree::Index_InternalDeleteLeftmost(Slot* iSlot)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count > 0);

	// Remove the keys.
	Move(iSlot, 1 + 1, 1, count - 1);
	Fill(iSlot, 1 + count - 1, 1);

	// Remove the slots.
	Move(iSlot, 1 + fCapacity_IndexInternal + 1, 1 + fCapacity_IndexInternal, count);
	Fill(iSlot, 1 + fCapacity_IndexInternal + count, 1);

	// Remove the sub tree sizes.
	Move(iSlot, 2 + 2 * fCapacity_IndexInternal + 1, 2 + 2 * fCapacity_IndexInternal, count);
	Fill(iSlot, 2 + 2 * fCapacity_IndexInternal + count, 1);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_InternalDeleteRightmost(Slot* iSlot)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count > 0);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_InternalDeleteOneTowardsRight(Slot* iSlot, size_t iOffset)
	{
	VALIDATESLOT(iSlot);
	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count > 0 && iOffset <= count);

	// Remove the keys.
	Move(iSlot, 1 + iOffset + 1, 1 + iOffset, count - iOffset);
	Fill(iSlot, 1 + count - 1, 1);

	// Remove the slots.
	Move(iSlot, 1 + fCapacity_IndexInternal + iOffset + 1,
		1 + fCapacity_IndexInternal + iOffset, count + 1 - iOffset);

	Fill(iSlot, 1 + fCapacity_IndexInternal + count, 1);

	// Remove the sub tree sizes.
	Move(iSlot, 2 + 2 * fCapacity_IndexInternal + iOffset + 1,
		2 + 2 * fCapacity_IndexInternal + iOffset, count + 1 - iOffset);

	Fill(iSlot, 2 + 2 * fCapacity_IndexInternal + count, 1);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_InternalDeleteToEnd(Slot* iSlot, size_t iOffset)
	{
	VALIDATESLOT(iSlot);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < fCapacity_IndexInternal);
	WriteField(iSlot, 0, iOffset);
	}

void ZBlockStore_PhaseTree::Index_LeafMakeSpaceOnLeft(Slot* iSlot, size_t iCount)
	{
	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + iCount <= fCapacity_IndexLeaf);

	// Move the keys
	Move(iSlot, 1, 1 + iCount, count);
	Fill(iSlot, 1, iCount);

	// Move the slots
	Move(iSlot, 1 + fCapacity_IndexLeaf, 1 + fCapacity_IndexLeaf + iCount, count);
	Fill(iSlot, 1 + fCapacity_IndexLeaf, iCount);

	WriteField(iSlot, 0, count + iCount);
	}

void ZBlockStore_PhaseTree::Index_LeafMakeSpaceOnRight(Slot* iSlot, size_t iCount)
	{
	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);
	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + iCount <= fCapacity_IndexLeaf);
	WriteField(iSlot, 0, count + iCount);
	}

void ZBlockStore_PhaseTree::Index_LeafMakeOneSpaceBefore(Slot* iSlot, size_t iOffset)
	{
//	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count + 1 <= fCapacity_IndexLeaf);

	// Move the keys
	Move(iSlot, 1 + iOffset, 1 + iOffset + 1, count - iOffset);
	Fill(iSlot, 1 + iOffset, 1);

	// Move the slots
	Move(iSlot, 1 + fCapacity_IndexLeaf + iOffset,
		1 + fCapacity_IndexLeaf + iOffset + 1, count - iOffset);

	Fill(iSlot, 1 + fCapacity_IndexLeaf + iOffset, 1);

	WriteField(iSlot, 0, count + 1);
//	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_LeafDeleteLeftmost(Slot* iSlot)
	{
	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count > 0);

	// Remove the keys.
	Move(iSlot, 1 + 1, 1, count - 1);
	Fill(iSlot, 1 + count - 1, 1);

	// Remove the slots.
	Move(iSlot, 1 + fCapacity_IndexLeaf + 1, 1 + fCapacity_IndexLeaf, count - 1);
	Fill(iSlot, 1 + fCapacity_IndexLeaf + count - 1, 1);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_LeafDeleteRightmost(Slot* iSlot)
	{
	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, count > 0);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_LeafDeleteOne(Slot* iSlot, size_t iOffset)
	{
	VALIDATESLOT(iSlot);

	const size_t count = ReadField(iSlot, 0);

	ZAssertStop(ZCONFIG_PhaseTree_Debug, iOffset < count);

	// Remove the keys.
	Move(iSlot, 1 + iOffset + 1, 1 + iOffset, count - iOffset - 1);
	Fill(iSlot, 1 + count - 1, 1);

	// Remove the slots.
	Move(iSlot, 1 + fCapacity_IndexLeaf + iOffset + 1,
		1 + fCapacity_IndexLeaf + iOffset, count - iOffset - 1);

	Fill(iSlot, 1 + fCapacity_IndexLeaf + count - 1, 1);

	WriteField(iSlot, 0, count - 1);
	VALIDATESLOT(iSlot);
	}

void ZBlockStore_PhaseTree::Index_LeafDeleteToEnd(Slot* iSlot, size_t iOffset)
	{
	WriteField(iSlot, 0, iOffset);
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Block node manipulation.

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::Block_FindDataSlot(
	Slot* iSlot, size_t iPosition)
	{
	// Figure out how high the tree is.
	size_t blockSize = ReadField(iSlot, 0);

	size_t bytesInBranch = fCapacity_Data;
	size_t countInLastOfLevel = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	while (countInLastOfLevel > fCapacity_BlockRoot_Slots)
		{
		countInLastOfLevel = (countInLastOfLevel + fCapacity_Extent - 1) / fCapacity_Extent;
		bytesInBranch *= fCapacity_Extent;
		}

	size_t offsetInLevel = iPosition / bytesInBranch;
	iPosition = iPosition % bytesInBranch;
	bytesInBranch /= fCapacity_Extent;
	Slot* currentSlot = this->UseSlot(ReadField(iSlot, 1 + offsetInLevel));

	while (bytesInBranch >= fCapacity_Data)
		{
		size_t offsetInLevel = iPosition / bytesInBranch;
		iPosition = iPosition % bytesInBranch;
		bytesInBranch /= fCapacity_Extent;

		Slot* childSlot = this->UseSlot(ReadField(currentSlot, offsetInLevel));

		this->UnuseSlot(currentSlot);
		currentSlot = childSlot;
		}

	return currentSlot;
	}

ZBlockStore_PhaseTree::Slot* ZBlockStore_PhaseTree::Block_FindDataSlotForWrite(
	Slot* iSlot, size_t iPosition, bool iNeedOriginalContent)
	{
	size_t blockSize = ReadField(iSlot, 0);

	size_t bytesInBranch = fCapacity_Data;
	size_t countInLastOfLevel = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	while (countInLastOfLevel > fCapacity_BlockRoot_Slots)
		{
		countInLastOfLevel = (countInLastOfLevel + fCapacity_Extent - 1) / fCapacity_Extent;
		bytesInBranch *= fCapacity_Extent;
		}

	size_t offsetInLevel = iPosition / bytesInBranch;
	iPosition = iPosition % bytesInBranch;
	bytesInBranch /= fCapacity_Extent;

	if (bytesInBranch < fCapacity_Data)
		{
		uint32 newSlotNumber;
		Slot* dataSlot = this->UseSlotDirty(ReadField(iSlot, 1 + offsetInLevel),
			iNeedOriginalContent, newSlotNumber);

		if (newSlotNumber)
			WriteField(iSlot, 1 + offsetInLevel, newSlotNumber);

		return dataSlot;
		}
	else
		{
		uint32 newSlotNumber;
		Slot* currentSlot = this->UseSlotForked(ReadField(iSlot, 1 + offsetInLevel),
			true, newSlotNumber);

		if (newSlotNumber)
			WriteField(iSlot, 1 + offsetInLevel, newSlotNumber);

		while (true)
			{
			size_t offsetInLevel = iPosition / bytesInBranch;
			iPosition = iPosition % bytesInBranch;
			bytesInBranch /= fCapacity_Extent;

			if (bytesInBranch < fCapacity_Data)
				{
				uint32 newSlotNumber;
				Slot* dataSlot = this->UseSlotForked(ReadField(currentSlot, offsetInLevel),
					iNeedOriginalContent, newSlotNumber);

				if (newSlotNumber)
					{
					this->DirtySlot(currentSlot);
					WriteField(currentSlot, offsetInLevel, newSlotNumber);
					}

				this->UnuseSlot(currentSlot);

				this->DirtySlot(dataSlot);
				return dataSlot;
				}
			else
				{
				uint32 newSlotNumber;
				Slot* childSlot = this->UseSlotForked(ReadField(currentSlot, offsetInLevel),
					true, newSlotNumber);

				if (newSlotNumber)
					{
					this->DirtySlot(currentSlot);
					WriteField(currentSlot, offsetInLevel, newSlotNumber);
					}

				this->UnuseSlot(currentSlot);
				currentSlot = childSlot;
				}
			}
		}
	}

void ZBlockStore_PhaseTree::Block_KillTree(Slot* iSlot)
	{
	// Called by SetBlockSizeImp and Delete

	size_t blockSize = ReadField(iSlot, 0);
	if (blockSize <= fCapacity_BlockRoot_Byte)
		return;

	size_t countInLastOfLevel = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	vector<size_t> countInLastOfLevels;
	while (countInLastOfLevel > fCapacity_BlockRoot_Slots)
		{
		size_t temp = ((countInLastOfLevel + fCapacity_Extent - 1) % fCapacity_Extent) + 1;
		countInLastOfLevels.push_back(temp);
		countInLastOfLevel = (countInLastOfLevel + fCapacity_Extent - 1) / fCapacity_Extent;
		}

	countInLastOfLevels.push_back(countInLastOfLevel);

	this->Extent_RecursiveDeletePartial(
		iSlot, 1, countInLastOfLevels, countInLastOfLevels.size() - 1);
	}

void ZBlockStore_PhaseTree::Block_GrowTree(Slot* iSlot, size_t iNewSize)
	{
	// Called by SetBlockSizeImp

	size_t blockSize = ReadField(iSlot, 0);
	size_t dataSlotsAllocated = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	size_t dataSlotsWanted = (iNewSize + fCapacity_Data - 1) / fCapacity_Data;
	ZAssertStop(ZCONFIG_PhaseTree_Debug, dataSlotsAllocated <= dataSlotsWanted);
	size_t additional = dataSlotsWanted - dataSlotsAllocated;
	if (additional == 0)
		return;

	size_t countInLastOfLevel = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	vector<size_t> countInLastOfLevels;
	while (countInLastOfLevel > fCapacity_BlockRoot_Slots)
		{
		size_t temp = ((countInLastOfLevel + fCapacity_Extent - 1) % fCapacity_Extent) + 1;
		countInLastOfLevels.push_back(temp);
		countInLastOfLevel = (countInLastOfLevel + fCapacity_Extent - 1) / fCapacity_Extent;
		}

	countInLastOfLevels.push_back(countInLastOfLevel);

	while (additional)
		{
		this->Block_GrowTreeRecurse(iSlot, 1, fCapacity_BlockRoot_Slots,
			countInLastOfLevels, countInLastOfLevels.size() - 1, additional);

		if (additional && countInLastOfLevels.back() == fCapacity_BlockRoot_Slots)
			{
			// If the root slot is full, create a child with its contents
			Slot* newChild = this->AllocateSlotDirty();
			for (size_t x = 0; x < fCapacity_BlockRoot_Slots; ++x)
				{
				uint32 slotNumber = ReadField(iSlot, 1 + x);
				WriteField(newChild, x, slotNumber);
				}
			WriteField(iSlot, 1, newChild->fSlotNumber);
			Fill(iSlot, 2, fCapacity_BlockRoot_Slots - 1);
			countInLastOfLevels.push_back(1);
			this->UnuseSlot(newChild);
			}
		}

	WriteField(iSlot, 0, iNewSize);
	}

void ZBlockStore_PhaseTree::Block_GrowTreeRecurse(
	Slot* iSlot, size_t iFieldOffset, size_t iMaxChildren,
	vector<size_t>& ioCountInLastOfLevels, int iHeight, size_t& ioAdditional)
	{
	// Called by Block_GrowTree and Block_GrowTreeRecurse

	while (ioAdditional)
		{
		if (iHeight == 0)
			{
			size_t toAllocate = min(ioAdditional, iMaxChildren - ioCountInLastOfLevels[0]);
			if (toAllocate == 0)
				break;
			this->DirtySlot(iSlot);

			fMutex_Slots.Acquire();
			for (size_t x = 0; x < toAllocate; ++x)
				{
				uint32 newSlotNumber = this->AllocateSlotNumber(false);
				WriteField(iSlot, iFieldOffset + ioCountInLastOfLevels[0] + x, newSlotNumber);
				}
			fMutex_Slots.Release();

			ioAdditional -= toAllocate;
			ioCountInLastOfLevels[0] += toAllocate;
			}
		else
			{
			Slot* childSlot;
			if (ioCountInLastOfLevels[iHeight] == 0)
				{
				childSlot = this->AllocateSlotDirty();
				this->DirtySlot(iSlot);
				WriteField(iSlot, iFieldOffset, childSlot->fSlotNumber);
				ioCountInLastOfLevels[iHeight] = 1;
				}
			else
				{
				uint32 slotNumber = ReadField(iSlot,
					iFieldOffset + ioCountInLastOfLevels[iHeight] - 1);

				uint32 newSlotNumber;
				childSlot = this->UseSlotForked(slotNumber, true, newSlotNumber);
				if (newSlotNumber)
					{
					this->DirtySlot(iSlot);
					WriteField(iSlot,
						iFieldOffset + ioCountInLastOfLevels[iHeight] - 1,
						newSlotNumber);
					}
				}
			this->Block_GrowTreeRecurse(childSlot, 0, fCapacity_Extent,
								ioCountInLastOfLevels, iHeight - 1, ioAdditional);
			this->UnuseSlot(childSlot);

			if (ioCountInLastOfLevels[iHeight] == iMaxChildren)
				break;

			if (ioAdditional)
				{
				fMutex_Slots.Acquire();
				uint32 newSlotNumber = this->AllocateSlotNumber(false);
				fMutex_Slots.Release();

				this->DirtySlot(iSlot);
				WriteField(iSlot, iFieldOffset + ioCountInLastOfLevels[iHeight], newSlotNumber);
				ioCountInLastOfLevels[iHeight] += 1;
				for (int x = 0; x < iHeight; ++x)
					{
					ZAssertStop(ZCONFIG_PhaseTree_Debug,
						ioCountInLastOfLevels[x] == fCapacity_Extent);
					ioCountInLastOfLevels[x] = 0;
					}
				}
			}
		}
	}

void ZBlockStore_PhaseTree::Block_PruneTree(Slot* iSlot, size_t iNewSize)
	{
	// Called by SetBlockSizeImp

	size_t blockSize = ReadField(iSlot, 0);
	size_t dataSlotsAllocated = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	size_t dataSlotsWanted = (iNewSize + fCapacity_Data - 1) / fCapacity_Data;
	ZAssertStop(ZCONFIG_PhaseTree_Debug, dataSlotsWanted <= dataSlotsAllocated);
	size_t excess = dataSlotsAllocated - dataSlotsWanted;
	if (excess == 0)
		return;

	size_t countInLastOfLevel = (blockSize + fCapacity_Data - 1) / fCapacity_Data;
	vector<size_t> countInLastOfLevels;
	while (countInLastOfLevel > fCapacity_BlockRoot_Slots)
		{
		size_t temp = ((countInLastOfLevel + fCapacity_Extent - 1) % fCapacity_Extent) + 1;
		countInLastOfLevels.push_back(temp);
		countInLastOfLevel = (countInLastOfLevel + fCapacity_Extent - 1) / fCapacity_Extent;
		}

	countInLastOfLevels.push_back(countInLastOfLevel);

	while (excess)
		{
		size_t height = countInLastOfLevels.size() - 1;
		ZAssertStop(ZCONFIG_PhaseTree_Debug, countInLastOfLevels[height] != 0);
		this->Block_PruneTreeRecurse(iSlot, 1,
							countInLastOfLevels, height, excess);
		if (height
			&& countInLastOfLevels[height] == 1
			&& countInLastOfLevels[height - 1] <= fCapacity_BlockRoot_Slots)
			{
			// If we have one child and it now has few enough children to fit
			// in the root, then migrate those children and release the child.
			Slot* childSlot = this->UseSlot(ReadField(iSlot, 1));
			for (size_t x = 0; x < countInLastOfLevels[height - 1]; ++x)
				{
				uint32 slotNumber = ReadField(childSlot, x);
				WriteField(iSlot, 1 + x, slotNumber);
				}
			this->ReleaseAndUnuseSlot(childSlot);
			countInLastOfLevels.pop_back();
			}
		}

	WriteField(iSlot, 0, iNewSize);
	}

void ZBlockStore_PhaseTree::Block_PruneTreeRecurse(Slot* iSlot, size_t iFieldOffset,
			vector<size_t>& ioCountInLastOfLevels, int iHeight, size_t& ioExcess)
	{
	// Called by Block_PruneTree and self

	while (ioExcess)
		{
		if (ioCountInLastOfLevels[iHeight] == 0)
			break;

		if (iHeight == 0)
			{
			size_t toRelease = min(ioExcess, ioCountInLastOfLevels[0]);
			this->DirtySlot(iSlot);

			fMutex_Slots.Acquire();
			for (size_t x = 0; x < toRelease; ++x)
				{
				uint32 theSlotNumber = ReadField(iSlot,
					iFieldOffset + ioCountInLastOfLevels[0] - x - 1);

				this->ReleaseSlotNumber(theSlotNumber);
				}
			fMutex_Slots.Release();

			ioExcess -= toRelease;
			ioCountInLastOfLevels[0] -= toRelease;
			}
		else
			{
			uint32 slotNumber = ReadField(iSlot, iFieldOffset + ioCountInLastOfLevels[iHeight] - 1);
			uint32 newSlotNumber;
			Slot* childSlot = this->UseSlotForked(slotNumber, true, newSlotNumber);
			if (newSlotNumber)
				{
				this->DirtySlot(iSlot);
				WriteField(iSlot, iFieldOffset + ioCountInLastOfLevels[iHeight] - 1, newSlotNumber);
				}
			this->Block_PruneTreeRecurse(childSlot, 0,
								ioCountInLastOfLevels, iHeight - 1, ioExcess);

			if (ioCountInLastOfLevels[iHeight - 1] == 0)
				{
				this->ReleaseAndUnuseSlot(childSlot);
				ioCountInLastOfLevels[iHeight] -= 1;
				if (ioCountInLastOfLevels[iHeight] != 0)
					{
					for (int x = 0; x < iHeight; ++x)
						{
						ZAssertStop(ZCONFIG_PhaseTree_Debug, ioCountInLastOfLevels[x] == 0);
						ioCountInLastOfLevels[x] = fCapacity_Extent;
						}
					}
				}
			else
				{
				this->UnuseSlot(childSlot);
				}
			}
		}
	}

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree. Extent node manipulation.

void ZBlockStore_PhaseTree::Extent_RecursiveDelete(Slot* iSlot, int iHeight)
	{
	// Called by Extent_RecursiveDeletePartial and self

	if (iHeight)
		{
		for (size_t x = 0; x < fCapacity_Extent; ++x)
			{
			Slot* childSlot = this->UseSlot(ReadField(iSlot, x));
			this->Extent_RecursiveDelete(childSlot, iHeight - 1);
			this->ReleaseAndUnuseSlot(childSlot);
			}
		}
	else
		{
		fMutex_Slots.Acquire();
		for (size_t x = 0; x < fCapacity_Extent; ++x)
			{
			uint32 theSlotNumber = ReadField(iSlot, x);
			this->ReleaseSlotNumber(theSlotNumber);
			}
		fMutex_Slots.Release();
		}
	}

void ZBlockStore_PhaseTree::Extent_RecursiveDeletePartial(
	Slot* iSlot, size_t iFieldOffset, const vector<size_t>& iCountInLastOfLevels, int iHeight)
	{
	// Called by Block_KillTree, and self

	if (iHeight)
		{
		for (size_t x = 0; x < iCountInLastOfLevels[iHeight] - 1; ++x)
			{
			Slot* childSlot = this->UseSlot(ReadField(iSlot, iFieldOffset + x));
			this->Extent_RecursiveDelete(childSlot, iHeight - 1);
			this->ReleaseAndUnuseSlot(childSlot);
			}
		Slot* childSlot = this->UseSlot(ReadField(iSlot,
			iFieldOffset + iCountInLastOfLevels[iHeight] - 1));
		this->Extent_RecursiveDeletePartial(childSlot, 0, iCountInLastOfLevels, iHeight - 1);
		this->ReleaseAndUnuseSlot(childSlot);
		}
	else
		{
		fMutex_Slots.Acquire();
		for (size_t x = 0; x < iCountInLastOfLevels[0]; ++x)
			{
			uint32 theSlotNumber = ReadField(iSlot, iFieldOffset + x);
			this->ReleaseSlotNumber(theSlotNumber);
			}
		fMutex_Slots.Release();
		}
	}

} // namespace ZooLib
