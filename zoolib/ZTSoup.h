/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTSoup__
#define __ZTSoup__ 1

#include "zoolib/ZDList.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZTSWatcher.h"

class ZTCrouton;
class ZTSieve;

#ifndef ZCONFIG_Debug_TSoup_Debug
#	define ZCONFIG_Debug_TSoup_Debug 1
#endif

// =================================================================================================
#pragma mark -
#pragma mark * ZTSoup

class ZTSoup : public ZRefCounted
	{
public:
	enum { kDebug = ZCONFIG_Debug_TSoup_Debug };

	enum ELoaded
		{
		eLoaded_Local, // By a local set before any update/sync has happened
		eLoaded_Update,
		eLoaded_Registration // Already registered by another sieve/crouton
		};

	enum EChanged
		{
		eChanged_Local,
		eChanged_Remote,
		eChanged_Loaded
		};

	static std::string sAsString(ELoaded iLoaded);
	static std::string sAsString(EChanged iChanged);

	typedef void (*Callback_UpdateNeeded_t)(void* iRefcon);
	typedef void (*Callback_SyncNeeded_t)(void* iRefcon);

	ZTSoup(ZRef<ZTSWatcher> iTSWatcher);
	virtual ~ZTSoup();

	void SetCallback_Update(
		Callback_UpdateNeeded_t iCallback_UpdateNeeded, void* iRefcon_UpdateNeeded);

	void SetCallback_Sync(
		Callback_SyncNeeded_t iCallback_SyncNeeded, void* iRefcon_SyncNeeded);

	bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	uint64 AllocateID();

	void Set(uint64 iID, const ZTuple& iTuple);

	uint64 Add(const ZTuple& iTuple);

	void Register(ZRef<ZTSieve> iTSieve, const ZTBQuery& iTBQuery);
	void Register(ZRef<ZTSieve> iTSieve, const ZTBQuery& iTBQuery, bool iPrefetch);
	void Register(ZRef<ZTCrouton> iTCrouton, uint64 iID);
	void Register(ZRef<ZTCrouton> iTCrouton);

	bool Sync();
	void Update();
	void Purge();

// These methods *can* be overridden, but it's by no means essential.
	virtual void Updated(
		const std::set<ZRef<ZTSieve> >& iSievesLoaded,
		const std::set<ZRef<ZTSieve> >& iSievesChanged,
		const std::set<ZRef<ZTCrouton> >& iCroutonsLoaded,
		const std::set<ZRef<ZTCrouton> >& iCroutonsChanged);

	virtual void Loaded(const std::set<ZRef<ZTSieve> >& iTSieves, ELoaded iLoaded);
	virtual void Changed(const std::set<ZRef<ZTSieve> >& iTSieves, EChanged iChanged);

	virtual void Loaded(const std::set<ZRef<ZTCrouton> >& iTCroutons, ELoaded iLoaded);
	virtual void Changed(const std::set<ZRef<ZTCrouton> >& iTCroutons, EChanged iChanged);

	virtual void Loaded(const ZRef<ZTSieve>& iTSieve, ELoaded iLoaded);
	virtual void Changed(const ZRef<ZTSieve>& iTSieve, EChanged iChanged);

	virtual void Loaded(const ZRef<ZTCrouton>& iTCrouton, ELoaded iLoaded);
	virtual void Changed(const ZRef<ZTCrouton>& iTCrouton, EChanged iChanged);

private:
	static void pCallback_TSWatcher(void* iRefcon);

	friend class ZTSieve;
	friend class ZTCrouton;

	class DLink_PSieve_Update;
	class DLink_PSieve_Sync;
	class DLink_PSieve_Changed;
	class PSieve;

	class DLink_PCrouton_Update;
	class DLink_PCrouton_Sync;
	class DLink_PCrouton_Changed;
	class DLink_PCrouton_Syncing;
	class DLink_PCrouton_Pending;
	class PCrouton;

	void pDisposingTSieve(ZTSieve* iTSieve);
	void pDisposingTCrouton(ZTCrouton* iTCrouton);

	bool pSieveIsLoaded(ZTSieve* iTSieve);

	const std::vector<uint64>& pSieveGet(ZTSieve* iTSieve);
	void pSieveGet(ZTSieve* iTSieve, std::vector<uint64>& oCurrent);
	void pSieveGet(ZTSieve* iTSieve, std::set<uint64>& oCurrent);

	void pSieveGetAdded(ZTSieve* iTSieve, std::set<uint64>& oAdded);
	void pSieveGetRemoved(ZTSieve* iTSieve, std::set<uint64>& oRemoved);

	void pCheckSieveDiffs(PSieve* iPSieve);

	bool pCroutonIsLoaded(ZTCrouton* iTCrouton);

	ZTuple pCroutonGet(ZTCrouton* iTCrouton);

	PCrouton* pGetPCrouton(uint64 iID);

	void pSetCroutonFromTCrouton(ZTCrouton* iTCrouton, const ZTuple& iTuple);
	void pSet(ZooLib::ZMutexLocker& iLocker_Structure, PCrouton* iPCrouton, const ZTuple& iTuple);

	void pTriggerUpdate();
	void pTriggerSync();

	ZRef<ZTSWatcher> fTSWatcher;

	ZooLib::ZMutex fMutex_CallSync;
	ZooLib::ZMutex fMutex_CallUpdate;
	ZooLib::ZMutex fMutex_Structure;
	ZooLib::ZMutex fMutex_TSWatcher;

	bool fCalled_UpdateNeeded;
	Callback_UpdateNeeded_t fCallback_UpdateNeeded;
	void* fRefcon_UpdateNeeded;
	
	bool fCalled_SyncNeeded;
	Callback_SyncNeeded_t fCallback_SyncNeeded;
	void* fRefcon_SyncNeeded;

	std::map<ZTBQuery, PSieve> fTBQuery_To_PSieve;

	ZooLib::DListHead<DLink_PSieve_Update> fPSieves_Update;
	ZooLib::DListHead<DLink_PSieve_Sync> fPSieves_Sync;
	ZooLib::DListHead<DLink_PSieve_Changed> fPSieves_Changed;

	std::map<uint64, PCrouton> fID_To_PCrouton;

	ZooLib::DListHead<DLink_PCrouton_Update> fPCroutons_Update;
	ZooLib::DListHead<DLink_PCrouton_Sync> fPCroutons_Sync;
	ZooLib::DListHead<DLink_PCrouton_Changed> fPCroutons_Changed;
	ZooLib::DListHead<DLink_PCrouton_Syncing> fPCroutons_Syncing;
	ZooLib::DListHead<DLink_PCrouton_Pending> fPCroutons_Pending;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSieve

class DLink_ZTSieve_Using : public ZooLib::DListLink<ZTSieve, DLink_ZTSieve_Using>
	{};

class ZTSieve : public ZRefCounted, public DLink_ZTSieve_Using
	{
public:
	ZTSieve();
	virtual ~ZTSieve();

	ZRef<ZTSoup> GetTSoup();
	ZTBQuery GetTBQuery();

	bool IsLoaded();

	const std::vector<uint64>& Get();
	void Get(std::vector<uint64>& oResults);
	void Get(std::set<uint64>& oResults);

	void GetAdded(std::set<uint64>& oAdded);
	void GetRemoved(std::set<uint64>& oRemoved);

	virtual void Loaded(ZTSoup::ELoaded iLoaded);
	virtual void Changed(ZTSoup::EChanged iChanged);

private:
	friend class ZTSoup;

	ZTSoup::PSieve* fPSieve;
	bool fJustRegistered;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTCrouton

class DLink_ZTCrouton_Using : public ZooLib::DListLink<ZTCrouton, DLink_ZTCrouton_Using>
	{};

class ZTCrouton : public ZRefCounted, public DLink_ZTCrouton_Using
	{
public:
	ZTCrouton();
	virtual ~ZTCrouton();

	ZRef<ZTSoup> GetTSoup();
	uint64 GetID();

	bool IsLoaded();

	ZTuple Get();
	void Set(const ZTuple& iTuple);

	virtual void Loaded(ZTSoup::ELoaded iLoaded);
	virtual void Changed(ZTSoup::EChanged iChanged);

private:
	friend class ZTSoup;

	ZTSoup::PCrouton* fPCrouton;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTCrouton_Bowl

class ZTBowl;

class ZTCrouton_Bowl : public ZTCrouton
	{
public:
	ZTCrouton_Bowl();
	virtual void Changed(ZTSoup::EChanged iChanged);

	ZRef<ZTBowl> GetBowl() const;

private:
	friend class ZTBowl;
	ZTBowl* fTBowl;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBowl

class ZTBowl : public ZTSieve
	{
public:
	ZTBowl();
	virtual ~ZTBowl();

// From ZTSieve
	virtual void Changed(ZTSoup::EChanged iChanged);

// Our protocol
	virtual ZRef<ZTCrouton_Bowl> MakeCrouton();
	virtual void CroutonChanged(ZRef<ZTCrouton> iCrouton, ZTSoup::EChanged iChanged);

	const std::vector<ZRef<ZTCrouton> >& GetCroutons();

private:
	std::vector<ZRef<ZTCrouton> > fTCroutons;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBowl_T

struct ZTBowlBogusInitializer_t;

template <class Crouton_t, typename Initalizer_t = ZTBowlBogusInitializer_t>
class ZTBowl_T : public ZTBowl
	{
public:
	ZTBowl_T(Initalizer_t iInitializer)
	:	fInitializer(iInitializer)
		{}

// From ZTBowl
	virtual ZRef<ZTCrouton_Bowl> MakeCrouton()
		{ return new Crouton_t(fInitializer); }

private:
	Initalizer_t fInitializer;
	};

// For use with croutons that do not take an initializer.
template <class Crouton_t>
class ZTBowl_T<Crouton_t, ZTBowlBogusInitializer_t> : public ZTBowl
	{
public:
	virtual ZRef<ZTCrouton_Bowl> MakeCrouton()
		{ return new Crouton_t; }
	};

#endif // __ZSoup__
