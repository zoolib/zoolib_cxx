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

#ifndef __ZBlockStore_PhaseTree_h__
#define __ZBlockStore_PhaseTree_h__ 1
#include "zconfig.h"

#include "zoolib/ZBlockStore.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZThread.h"

#include <set>
#include <map>

#ifndef ZCONFIG_PhaseTree_Debug
	#define ZCONFIG_PhaseTree_Debug 3
#endif

namespace ZooLib {

// =================================================================================================
// MARK: - ZBlockStore_PhaseTree

class ZBlockStore_PhaseTree : public ZBlockStore
	{
public:
	/// Create constructor
	ZBlockStore_PhaseTree(ZRef<ZFileRW> iFile, size_t iSlotSize, size_t iUserHeaderSize);

	/// Open constructor
	ZBlockStore_PhaseTree(ZRef<ZFileRW> iFile, size_t iUserHeaderSize);

	/// Read-only open constructor
	ZBlockStore_PhaseTree(ZRef<ZFileR> iFile, size_t iUserHeaderSize);

	virtual ~ZBlockStore_PhaseTree();

// From ZBlockStore.
	virtual void Flush();

	virtual ZRef<ZStreamerRWPos> Create(BlockID& oBlockID);
	virtual ZRef<ZStreamerRPos> OpenRPos(BlockID iBlockID);
	virtual ZRef<ZStreamerRWPos> OpenRWPos(BlockID iBlockID);
	virtual bool Delete(BlockID iBlockID);
	virtual size_t EfficientSize();

// Our extended public protocol.
	ZRef<ZStreamerRWPos> CreateWithSpecificID(BlockID iDesiredBlockID, BlockID& oActualBlockID);
	BlockID GetNextBlockID(BlockID iBlockID);

	void Dump();
	void DumpMeta();

	void ValidateTree();

protected:
	class Slot; friend class Slot;
	class StreamRPos; friend class StreamRPos;
	class StreamRWPos; friend class StreamRWPos;
	class StreamerRPos;
	class StreamerRWPos;

	void DumpOne(Slot* iSlot, int iHeight, int iIndent);
	void DumpBlock(Slot* iSlot, int iIndent);

	size_t ValidateOne(Slot* iSlot, int iHeight, uint32 iMinKeyMinusOne, uint32 iMaxKey);

	enum EState
		{ eStateClean, eStateForked, eStateDirty, eStateReleasedClean, eStateReleasedForked };

	// Methods called by stream or our block store API.
	void StreamDisposing(Slot* iBlockSlot);

	Slot* FindBlockSlot(BlockID iBlockID);
	Slot* FindBlockSlotForWrite(BlockID iBlockID);

	void ReadFromBlock(BlockID iBlockID, Slot*& ioBlockSlot,
		uint64 iPosition, void* oDest, size_t iCount, size_t& oCountRead);

	void WriteToBlock(BlockID iBlockID, Slot*& ioBlockSlot,
		uint64 iPosition, const void* iSource, size_t iCount, size_t& oCountWritten);

	Slot* CreateBlockImp(BlockID iBlockID, BlockID& oBlockID);

	bool DeleteBlockRecurse(
		Slot* iParentSlot, int iHeight, BlockID iBlockID, Slot*& oBlockSlot, Slot** oNewRoot);

	BlockID GetNextBlockIDRecurse(Slot* iParentSlot, int iHeight, BlockID iBlockID);

	size_t GetBlockSize(BlockID iBlockID, Slot*& ioBlockSlot);
	void SetBlockSize(BlockID iBlockID, Slot*& ioBlockSlot, size_t iSize);
	void SetBlockSizeIfLessThan(BlockID iBlockID, Slot*& ioBlockSlot, size_t iSize);
	void SetBlockSizeImp(Slot* iSlot, size_t iSize);

	// Slot management.
	uint32 AllocateSlotNumber(bool iForImmediateUse);
	Slot* AllocateSlotDirty();

	Slot* UseRootSlot();

	Slot* UseRootSlotReal(bool iDirtied);
	Slot* UseRootSlotForked();
	Slot* UseRootSlotDirty();

	Slot* UseSlot(uint32 iSlotNumber);

	Slot* UseSlotReal(
		bool iDirtied, uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber);
	Slot* UseSlotForked(uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber);
	Slot* UseSlotDirty(uint32 iSlotNumber, bool iReadOriginalContent, uint32& oForkedSlotNumber);

	void UnuseSlot(Slot* iSlot, bool iPurge = true);
	void UnuseUnlockedSlot(Slot* iSlot);

	void DirtySlot(Slot* ioSlot);

	void ReleaseSlotNumber(uint32 iSlotNumber);
	void ReleaseAndUnuseSlot(Slot* iSlot);

	void WriteStart();
	void WriteFinish();

	void WriteStart(BlockID iBlockID, Slot*& ioBlockSlot);
	void WriteFinish(Slot* iBlockSlot);

	void ReadStart(BlockID iBlockID, Slot*& ioBlockSlot);
	void ReadFinish(Slot* iBlockSlot);

	// Index node manipulation.
	Slot* Index_FindChild(Slot* iSlot, bool iIsLeaf, BlockID iBlockID, size_t& oChildOffset);

	Slot* Index_FindChildForked(Slot* iSlot, bool iIsLeaf, BlockID iBlockID,
		size_t& oChildOffset, uint32& oForkedChildSlotNumber);

	Slot* Index_CreateRoot(Slot* iLeftChild, size_t iLeftSubTreeSize,
		BlockID iKey, Slot* iRightChild, size_t iRightSubTreeSize);

	void Index_Split(Slot* iSlot, bool iIsLeaf,
		size_t& oLeftSubTreeSize, BlockID& oKey, Slot*& oNewSibling, size_t& oRightSubTreeSize);

	void Index_Merge(Slot* iSlot, bool iSlotsAreLeaves,
		size_t iOffsetA, size_t iOffsetB, Slot* iSlotA, Slot* iSlotB);

	void Index_Rotate(Slot* iSlot, bool iSlotsAreLeaves,
		size_t iOffsetA, size_t iOffsetB, Slot* iSlotA, Slot* iSlotB);

	// Index node accessors
	void Index_Initialize(Slot* iSlot);

	bool Index_IsFull(Slot* iSlot, bool iIsLeaf);

	size_t Index_GetCount(Slot* iSlot);

	BlockID Index_GetKey(Slot* iSlot, bool iIsLeaf, size_t iOffset);
	uint32* Index_GetKeysAddress(Slot* iSlot);
	void Index_SetKey(Slot* iSlot, bool iIsLeaf, size_t iOffset, BlockID iKey);

	uint32 Index_GetChild(Slot* iSlot, bool iIsLeaf, size_t iOffset);
	void Index_SetChild(Slot* iSlot, bool iIsLeaf, size_t iOffset, uint32 iSlotNumber);

	uint32 Index_GetSubTreeSize(Slot* iSlot, size_t iOffset);
	void Index_SetSubTreeSize(Slot* iSlot, size_t iOffset, uint32 iSubTreeSize);

	void Index_InternalMakeSpaceOnLeft(Slot* iSlot, size_t iCount);
	void Index_InternalMakeSpaceOnRight(Slot* iSlot, size_t iCount);
	void Index_InternalMakeOneSpaceBefore(Slot* iSlot, size_t iOffset);
	void Index_InternalDeleteLeftmost(Slot* iSlot);
	void Index_InternalDeleteRightmost(Slot* iSlot);
	void Index_InternalDeleteOneTowardsRight(Slot* iSlot, size_t iOffset);
	void Index_InternalDeleteToEnd(Slot* iSlot, size_t iOffset);

	void Index_LeafMakeSpaceOnLeft(Slot* iSlot, size_t iCount);
	void Index_LeafMakeSpaceOnRight(Slot* iSlot, size_t iCount);
	void Index_LeafMakeOneSpaceBefore(Slot* iSlot, size_t iOffset);
	void Index_LeafDeleteLeftmost(Slot* iSlot);
	void Index_LeafDeleteRightmost(Slot* iSlot);
	void Index_LeafDeleteOne(Slot* iSlot, size_t iOffset);
	void Index_LeafDeleteToEnd(Slot* iSlot, size_t iOffset);

	// Block node manipulation.
	Slot* Block_FindDataSlot(Slot* iBlockSlot, size_t iPosition);
	Slot* Block_FindDataSlotForWrite(Slot* iBlockSlot, size_t iPosition, bool iNeedOriginalContent);
	void Block_KillTree(Slot* iSlot);
	void Block_GrowTree(Slot* iSlot, size_t iNewSize);
	void Block_GrowTreeRecurse(Slot* iSlot, size_t iFieldOffset, size_t iMaxChildren,
				std::vector<size_t>& ioCountInLastOfLevels, int iHeight, size_t& ioAdditional);
	void Block_PruneTree(Slot* iSlot, size_t iNewSize);
	void Block_PruneTreeRecurse(Slot* iSlot, size_t iFieldOffset,
				std::vector<size_t>& ioCountInLastOfLevels, int iHeight, size_t& ioExcess);

	// Extent node manipulation.
	void Extent_RecursiveDelete(Slot* iSlot, int iHeight);
	void Extent_RecursiveDeletePartial(Slot* iSlot, size_t iFieldOffset,
		const std::vector<size_t>& iCountInLastOfLevels, int iHeight);

	// Low level field access. Suitable for inlining?
	uint32 ReadField(Slot* iSlot, size_t iOffset);
	uint32* FieldAddress(Slot* iSlot, size_t iOffset);
	void WriteField(Slot* iSlot, size_t iOffset, uint32 iValue);
	void Move(Slot* iSlot, size_t iBegin, size_t iEnd, size_t iCount);
	void Fill(Slot* iSlot, size_t iBegin, size_t iCount);

	// These variables are read-only after initialization.
	const size_t fUserHeaderSize;
	size_t fSlotSize;
	size_t fCapacity_BlockRoot_Byte;
	size_t fCapacity_BlockRoot_Slots;
	size_t fCapacity_IndexInternal;
	size_t fCapacity_IndexLeaf;
	size_t fCapacity_Extent;
	size_t fCapacity_Data;

	ThreadSafe_t fStreamsInstantiated;

	ZMtx fMutex_Flush;
	ZCnd fCondition_Flush;
		int fCount_Writes;
		int fCount_WaitingFlushes;
		bool fFlushInProgress;
		bool fFlushInFirstStages;

	// Mutexes are listed in the order in which they must be acquired. Following
	// each mutex are the instance variables protected by that mutex.

	ZMtx fMutex_MetaRoot;
		uint32 fRootSlotNumber;
		uint32 fHeight;

	// Note that each slot also has a mutex, their mutexes conceptually belong here,
	// after fMutex_MetaRoot and before fMutex_Slots. There is an additional restriction
	// on slot mutexes, they must be acquired in order descending the tree -- we're free
	// to release a parent's mutex after acquiring a child's, but we cannot go back up
	// the tree, the code is organized such that it's never necessary to backtrack.

	ZMtx fMutex_Slots;
		std::set<uint32> fSlots_Forked;
		std::set<uint32> fSlots_ReleasedClean;
		std::set<uint32> fSlots_ReleasedForked;

		std::map<uint32, Slot*> fSlots_Loaded;

		std::map<uint32, Slot*> fSlots_Cached;
		Slot* fCached_Head;
		Slot* fCached_Tail;

		uint32 fHighWater;
		uint32 fAllocatedSpace;

		int fUseRequests;
		int fUseFailures;
		int fUseCached;
		// fMutex_Slots also protects the fields fUseCount and fState in every Slot.

	ZRef<ZFileRW> fFileRW;
	ZRef<ZFileR> fFileR;

	void ValidateSlot(Slot* iSlot, const char* iFunctionName, int iLineNumber);
	};

} // namespace ZooLib

#endif // __ZBlockStore_PhaseTree_h__
