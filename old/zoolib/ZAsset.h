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

#ifndef __ZAsset_h__
#define __ZAsset_h__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZStreamer.h"

#include <set>
#include <string>
#include <vector>

namespace ZooLib {

class ZAsset;
class ZAssetIterRep;
class ZAssetRep;
class ZAssetTree;

// =================================================================================================
// MARK: - ZAsset

/// Represents an asset in an asset tree.

class ZAsset
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZAsset, operator_bool_generator_type, operator_bool_type);
public:
	ZAsset();
	ZAsset(const ZAsset& iOther);
	ZAsset(const ZRef<ZAssetRep>& iRep);
	~ZAsset();
	ZAsset& operator=(const ZAsset& iOther);
	ZAsset& operator=(const ZRef<ZAssetRep>& iRep);

	bool operator==(const ZAsset& iOther) const;
	bool operator<(const ZAsset& iOther) const;

	operator operator_bool_type() const;

	std::string GetName() const;
	std::string GetPath() const;

	ZAsset GetParent() const;

	ZAsset GetChild(const char* iName) const;
	ZAsset GetChild(const std::string& iName) const;

	void GetData(const void** oData, size_t* oSize) const;

	ZRef<ZStreamerR> OpenR() const;
	ZRef<ZStreamerRPos> OpenRPos() const;
	ZRef<ZStreamerW> OpenW() const;
	ZRef<ZStreamerWPos> OpenWPos() const;
	ZRef<ZStreamerRWPos> OpenRWPos() const;
	
	ZRef<ZAssetRep> GetRep() const;

protected:
	ZRef<ZAssetRep> fRep;
	};

// =================================================================================================
// MARK: - ZAssetIter

/// An iterator that provides access to the children of an asset.

class ZAssetIter
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZAssetIter, operator_bool_generator_type, operator_bool_type);
public:
	ZAssetIter();
	ZAssetIter(const ZAssetIter& iOther);
	ZAssetIter(const ZAsset& iAsset);
	~ZAssetIter();
	ZAssetIter& operator=(const ZAssetIter& iOther);

	operator operator_bool_type() const;
	ZAssetIter& Advance();
	ZAsset Current() const;
	std::string CurrentName() const;

private:
	ZRef<ZAssetIterRep> fRep;
	};

// =================================================================================================
// MARK: - ZAssetTree

/// Represents an entire asset tree.

class ZAssetTree : public ZCounted, NonCopyable
	{
protected:
	ZAssetTree() {}
	virtual ~ZAssetTree() {}

public:
	virtual ZRef<ZAssetRep> GetRoot() = 0;

	static bool sCheckAccessEnabled() { return ZCONFIG_Debug > 2; }
	};

// =================================================================================================
// MARK: - ZAssetRep

class ZAssetRep : public ZCounted, NonCopyable
	{
protected:
	ZAssetRep() {}
	virtual ~ZAssetRep() {}

public:
	static bool sCheckAccessEnabled() { return ZCONFIG_Debug > 2; }

	virtual std::string GetName() = 0;
	virtual std::string GetPath() = 0;
	virtual ZRef<ZAssetRep> GetParentAssetRep() = 0;

	virtual ZRef<ZAssetRep> ResolvePath(const char* iPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

	virtual void GetData(const void** oData, size_t* oSize);

	virtual ZRef<ZStreamerR> OpenR();
	virtual ZRef<ZStreamerRPos> OpenRPos();
	virtual ZRef<ZStreamerW> OpenW();
	virtual ZRef<ZStreamerWPos> OpenWPos();
	virtual ZRef<ZStreamerRWPos> OpenRWPos();
	};

// =================================================================================================
// MARK: - ZAssetIterRep

class ZAssetIterRep : public ZCounted
	{
protected:
	ZAssetIterRep();

public:
	virtual ~ZAssetIterRep();

	virtual bool HasValue() = 0;
	virtual void Advance() = 0;
	virtual ZRef<ZAssetRep> Current() = 0;
	virtual std::string CurrentName() = 0;

	virtual ZRef<ZAssetIterRep> Clone() = 0;
	};

// =================================================================================================
// MARK: - ZAssetRep_Overlay

class ZAssetRep_Overlay : public ZAssetRep
	{
public:
	ZAssetRep_Overlay(
		const ZRef<ZAssetRep>& iParent, const std::vector<ZRef<ZAssetRep> >& iOverlays);
	ZAssetRep_Overlay(const ZRef<ZAssetRep>& iParent, const ZRef<ZAssetRep>& iOverlay);
	ZAssetRep_Overlay(const std::vector<ZRef<ZAssetRep> >& iOverlays);

	virtual ~ZAssetRep_Overlay();

	virtual std::string GetName();
	virtual std::string GetPath();
	virtual ZRef<ZAssetRep> GetParentAssetRep();

	virtual ZRef<ZAssetRep> ResolvePath(const char* iPath);

	virtual ZRef<ZAssetIterRep> CreateIterRep();

	virtual void GetData(const void** oData, size_t* oSize);

	virtual ZRef<ZStreamerR> OpenR();
	virtual ZRef<ZStreamerRPos> OpenRPos();
	virtual ZRef<ZStreamerW> OpenW();
	virtual ZRef<ZStreamerWPos> OpenWPos();
	virtual ZRef<ZStreamerRWPos> OpenRWPos();

protected:
	ZRef<ZAssetRep> fParent;
	std::vector<ZRef<ZAssetRep> > fOverlays;
	};

// =================================================================================================
// MARK: - ZAssetIterRep_Union

class ZAssetIterRep_Union : public ZAssetIterRep
	{
public:
	ZAssetIterRep_Union(
		ZRef<ZAssetRep> iParent, const std::vector<ZRef<ZAssetRep> >& iRepsUnvisited);

	ZAssetIterRep_Union(
		ZRef<ZAssetRep> iParent,
		ZRef<ZAssetIterRep> iCurrentIterRep,
		const std::vector<ZRef<ZAssetRep> >& iRepsUnvisited,
		const std::set<std::string>& iNamesSeen);

	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZAssetRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZAssetIterRep> Clone();

private:
	ZRef<ZAssetRep> fParent;
	ZRef<ZAssetIterRep> fCurrentIterRep;
	std::vector<ZRef<ZAssetRep> > fRepsUnvisited;
	std::set<std::string> fNamesSeen;
	};

// =================================================================================================
// MARK: - ZAsset Inlines

inline ZAsset::ZAsset()
	{}

inline ZAsset::ZAsset(const ZAsset& iOther)
:	fRep(iOther.fRep)
	{}

inline ZAsset::ZAsset(const ZRef<ZAssetRep>& iRep)
:	fRep(iRep)
	{}

inline ZAsset::~ZAsset()
	{}

inline ZAsset& ZAsset::operator=(const ZAsset& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

inline ZAsset& ZAsset::operator=(const ZRef<ZAssetRep>& iRep)
	{
	fRep = iRep;
	return *this;
	}

inline bool ZAsset::operator==(const ZAsset& iOther) const
	{ return fRep == iOther.fRep; }

inline bool ZAsset::operator<(const ZAsset& iOther) const
	{ return fRep < iOther.fRep; }

inline ZAsset::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && true); }

inline ZRef<ZAssetRep> ZAsset::GetRep() const
	{ return fRep; }

} // namespace ZooLib

#endif // __ZAsset_h__
