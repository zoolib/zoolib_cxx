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

#ifndef __ZAsset_Std_h__
#define __ZAsset_Std_h__ 1
#include "zconfig.h"

#include "zoolib/ZAsset.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZThreadOld.h"

namespace ZooLib {

class ZAssetIterRep_Std_Directory;
class ZAssetRep_Std_Directory;
class ZAssetTree_Std;

// =================================================================================================
// MARK: - ZAssetRep_Std

class ZAssetRep_Std : public ZAssetRep
	{
public:
	ZAssetRep_Std(ZAssetRep_Std_Directory* iParent, const char* iName, size_t iNameLength);
	virtual ~ZAssetRep_Std();

// From ZCounted via ZAssetRep
	virtual void Finalize();

// From ZAssetRep
	virtual std::string GetName();
	virtual std::string GetPath();
	virtual ZRef<ZAssetRep> GetParentAssetRep();

// Our protocol
	virtual void DoInitialization() = 0;
	virtual void DoFinalization() = 0;

protected:
	struct CompareAgainstRep;
	struct CompareAgainstPath;

	ZRef<ZAssetTree_Std> fAssetTree;
	const char* fName;
	size_t fNameLength;

private:
	ZAssetRep_Std_Directory* fParent;

	friend class ZAssetIterRep_Std_Directory;
	friend class ZAssetRep_Std_Directory;
	friend class ZAssetTree_Std;
	};

// =================================================================================================
// MARK: - ZAssetRep_Std_Directory

class ZAssetRep_Std_Directory : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Directory(
		ZAssetTree_Std* iAssetTree, ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength,
		const ZStreamRPos& iStream,
		const char* iNameTable, const size_t* iNameOffsets);
	virtual ~ZAssetRep_Std_Directory();

// From ZAssetRep via ZAssetRep_Std
	virtual ZRef<ZAssetRep> ResolvePath(const char* iPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	size_t fChildrenCount;
	union
		{
		ZAssetRep_Std* fChild;
		ZAssetRep_Std** fChildren;
		};

	friend class ZAssetIterRep_Std_Directory;
	};

// =================================================================================================
// MARK: - ZAssetRep_Std_Union

class ZAssetRep_Std_Union : public ZAssetRep_Std
	{
public:
	ZAssetRep_Std_Union(
		ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength,
		const ZStreamRPos& iStream,
		const char* iNameTable, const size_t* iNameOffsets);

	virtual ~ZAssetRep_Std_Union();

// From ZAssetRep via ZAssetRep_Std
	virtual ZRef<ZAssetRep> ResolvePath(const char* iPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

// From ZAssetRep_Std
	virtual void DoInitialization();
	virtual void DoFinalization();

protected:
	void pResolvePaths();

	std::vector<const char*> fPaths;

	ZMutex fMutex;
	bool fResolved;
	std::vector<ZRef<ZAssetRep> > fResolvedReps;
	};

// =================================================================================================
// MARK: - ZAssetIterRep_Std_Directory

class ZAssetIterRep_Std_Directory : public ZAssetIterRep
	{
public:
	ZAssetIterRep_Std_Directory(ZRef<ZAssetRep_Std_Directory> iAssetRep, size_t iIndex);

	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZAssetRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZAssetIterRep> Clone();

private:
	ZRef<ZAssetRep_Std_Directory> fAssetRep;
	size_t fCurrent;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std

class ZAssetTree_Std : public ZAssetTree
	{
protected:
	ZAssetTree_Std();
	virtual ~ZAssetTree_Std();

// Called by sub classes and ZAssetRep_Std_Directory during load up
	ZAssetRep_Std* LoadAssetRep(
		ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength,
		const ZStreamRPos& iStream,
		const char* iNameTable, const size_t* iNameOffsets);

// Implemented by subclasses
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength, const ZStreamRPos& iStream) = 0;

// Called by reps
	ZRef<ZAssetRep_Std> UseRep(ZAssetRep_Std* iRep);
	void RepBeingFinalized(ZAssetRep_Std* iRep);

	ZMutex fMutex;

	friend class ZAssetIterRep_Std_Directory;
	friend class ZAssetRep_Std;
	friend class ZAssetRep_Std_Directory;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std_Stream

class ZAssetRep_Std_Data_Stream;

class ZAssetTree_Std_Stream : public ZAssetTree_Std
	{
protected:
	ZAssetTree_Std_Stream();

public:
	ZAssetTree_Std_Stream(const ZStreamRPos& iStream, size_t iOffset, size_t iSize);
	virtual ~ZAssetTree_Std_Stream();

protected:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength, const ZStreamRPos& iStream);

// Our protocol
	void LoadUp(const ZStreamRPos* iStream, size_t iOffset, size_t iSize);
	void ShutDown();

// Called by ZAssetRep_Std_Data_Stream
	ZRef<ZStreamerRPos> OpenRPos(size_t iOffset, size_t iSize);

// Called by ZAssetRep_Std_Data_Stream and StreamerRPos
	void ReadData(size_t iOffset, size_t iSize, void* oData);

	const ZStreamRPos* fStream;
	char* fNameTable;
	size_t fDataOffset;
	ZAssetRep_Std* fRoot;

	class StreamerRPos;
	friend class StreamerRPos;
	friend class ZAssetRep_Std_Data_Stream;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std_Streamer

class ZAssetTree_Std_Streamer : public ZAssetTree_Std_Stream
	{
public:
	ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> iStreamer);
	ZAssetTree_Std_Streamer(ZRef<ZStreamerRPos> iStreamer, size_t iOffset, size_t iSize);
	virtual ~ZAssetTree_Std_Streamer();

protected:
	ZRef<ZStreamerRPos> fStreamer;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std_File

class ZAssetRep_Std_Data_File;

class ZAssetTree_Std_File : public ZAssetTree_Std
	{
public:
	ZAssetTree_Std_File(ZRef<ZFileR> iFile, uint64 iOffset, size_t iSize);
	virtual ~ZAssetTree_Std_File();

protected:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength, const ZStreamRPos& iStream);

// Called by ZAssetRep_Std_Data_File
	ZRef<ZStreamerRPos> OpenRPos(uint64 iOffset, size_t iSize);

// Called by ZAssetRep_Std_Data_File and StreamerStream
	void ReadData(uint64 iOffset, size_t iSize, void* oData);

	ZRef<ZFileR> fFile;
	char* fNameTable;
	uint64 fDataOffset;
	ZAssetRep_Std* fRoot;

	class StreamerRPos;
	friend class StreamerRPos;
	friend class ZAssetRep_Std_Data_File;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std_Memory

class ZAssetRep_Std_Data_Memory;

class ZAssetTree_Std_Memory : public ZAssetTree_Std
	{
protected:
	ZAssetTree_Std_Memory();

	ZAssetTree_Std_Memory(const void* iAddress, size_t iSize);
	virtual ~ZAssetTree_Std_Memory();

public:
// From ZAssetTree via ZAssetTree_Std
	virtual ZRef<ZAssetRep> GetRoot();

protected:
// From ZAssetTree_Std
	virtual ZAssetRep_Std* LoadAssetRep_Data(ZAssetRep_Std_Directory* iParent,
		const char* iName, size_t iNameLength, const ZStreamRPos& iStream);

// Our protocol
	void LoadUp(const void* iAddress, size_t iSize);
	void ShutDown();

	const char* fAddress;
	ZAssetRep_Std* fRoot;
	};

// =================================================================================================
// MARK: - ZAssetTree_Std_Memory_StaticData

class ZAssetTree_Std_Memory_StaticData : public ZAssetTree_Std_Memory
	{
public:
	ZAssetTree_Std_Memory_StaticData(const void* iAddress, size_t iSize);
	virtual ~ZAssetTree_Std_Memory_StaticData();
	};

} // namespace ZooLib

#endif // __ZAsset_Std_h__
