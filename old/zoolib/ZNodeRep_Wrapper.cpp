/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZNodeRep_Wrapper.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - ZNodeIterRep_Wrapper declaration

class ZNodeIterRep_Wrapper : public ZNodeIterRep
	{
public:
	ZNodeIterRep_Wrapper(ZRef<ZNodeRep> iParent,
		const std::string& iName, size_t iDepth, ZRef<ZNodeIterRep> iIterRep);

// From ZNodeIterRep
	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZNodeRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZNodeIterRep> Clone();

private:
	ZRef<ZNodeRep> fParent;
	std::string fName;
	size_t fDepth;
	ZRef<ZNodeIterRep> fIterRep;
	};

// =================================================================================================
// MARK: - ZNodeRep_Wrapper

ZNodeRep_Wrapper::ZNodeRep_Wrapper(ZRef<ZNodeRep> iParent, const std::string& iName,
	size_t iDepth, ZRef<ZNodeRep> iNodeRep)
:	fParent(iParent),
	fName(iName),
	fDepth(iDepth),
	fNodeRep(iNodeRep)
	{}

ZRef<ZNodeIterRep> ZNodeRep_Wrapper::CreateIterRep()
	{ return new ZNodeIterRep_Wrapper(fParent, fName, fDepth, fNodeRep->CreateIterRep()); }

bool ZNodeRep_Wrapper::GetProp(const string& iPropName, ZTValue& oTV, Error* oError)
	{ return fNodeRep->GetProp(iPropName, oTV, oError); }

bool ZNodeRep_Wrapper::CanHaveChildren(Error* oError)
	{ return fNodeRep->CanHaveChildren(oError); }

std::string ZNodeRep_Wrapper::GetName(Error* oError)
	{
	if (fDepth == 0)
		{
		if (oError)
			*oError = ZNode::errorNone;
		return fName;
		}
	return fNodeRep->GetName(oError);
	}

ZTrail ZNodeRep_Wrapper::TrailTo(ZRef<ZNodeRep> iDest, Error* oError)
	{
	if (ZTrail myTrail = ZNode(this).TrailFromRoot())
		{
		if (ZTrail destTrail = ZNode(iDest).TrailFromRoot())
			return ZTrail::sTrailFromTo(myTrail.GetComps(), destTrail.GetComps());
		}
	
	if (oError)
		*oError = ZNode::errorInvalidNode;
	return ZTrail(false);
	}

ZRef<ZNodeRep> ZNodeRep_Wrapper::GetAncestor(size_t iCount, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorNone;

	if (iCount > fDepth)
		{
		if (fParent)
			return fParent->GetAncestor(iCount - fDepth, oError);
		}
	else if (fDepth > iCount)
		{
		if (ZRef<ZNodeRep> theRep = fNodeRep->GetAncestor(iCount, oError))
			return new ZNodeRep_Wrapper(fParent, fName, fDepth - iCount, theRep);
		}
	else
		{
		return this;
		}

	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep_Wrapper::GetDescendant(const std::string* iComps,
	size_t iCount, Error* oError)
	{
	if (ZRef<ZNodeRep> theRep = fNodeRep->GetDescendant(iComps, iCount, oError))
		return new ZNodeRep_Wrapper(fParent, fName, fDepth + iCount, theRep);
	return ZRef<ZNodeRep>();
	}

bool ZNodeRep_Wrapper::Exists(Error* oError)
	{ return fNodeRep->Exists(oError); }

ZRef<ZNodeRep> ZNodeRep_Wrapper::CreateDir(Error* oError)
	{
	if (fNodeRep->CreateDir(oError))
		return this;
	return ZRef<ZNodeRep>();
	}

static ZRef<ZNodeRep> spGetReal(ZRef<ZNodeRep> iNodeRep)
	{
	if (ZRef<ZNodeRep_Wrapper> theWrapper = ZRefDynamicCast<ZNodeRep_Wrapper>(iNodeRep))
		return spGetReal(theWrapper->GetRealNodeRep());
	return iNodeRep;
	}

ZRef<ZNodeRep> ZNodeRep_Wrapper::MoveTo(ZRef<ZNodeRep> iDest, Error* oError)
	{
	if (ZRef<ZNodeRep> result = fNodeRep->MoveTo(spGetReal(iDest), oError))
		return iDest;
	return ZRef<ZNodeRep>();
	}

bool ZNodeRep_Wrapper::Delete(Error* oError)
	{ return fNodeRep->Delete(oError); }

ZRef<ZStreamerR> ZNodeRep_Wrapper::OpenR(bool iPreventWriters, Error* oError)
	{ return fNodeRep->OpenR(iPreventWriters, oError); }

ZRef<ZStreamerRPos> ZNodeRep_Wrapper::OpenRPos(bool iPreventWriters, Error* oError)
	{ return fNodeRep->OpenRPos(iPreventWriters, oError); }

ZRef<ZStreamerW> ZNodeRep_Wrapper::OpenW(bool iPreventWriters, Error* oError)
	{ return fNodeRep->OpenW(iPreventWriters, oError); }

ZRef<ZStreamerWPos> ZNodeRep_Wrapper::OpenWPos(bool iPreventWriters, Error* oError)
	{ return fNodeRep->OpenWPos(iPreventWriters, oError); }

ZRef<ZStreamerRWPos> ZNodeRep_Wrapper::OpenRWPos(bool iPreventWriters, Error* oError)
	{ return fNodeRep->OpenRWPos(iPreventWriters, oError); }

ZRef<ZStreamerWPos> ZNodeRep_Wrapper::CreateWPos(bool iOpenExisting,
	bool iPreventWriters, Error* oError)
	{ return fNodeRep->CreateWPos(iOpenExisting, iPreventWriters, oError); }

ZRef<ZStreamerRWPos> ZNodeRep_Wrapper::CreateRWPos(bool iOpenExisting,
	bool iPreventWriters, Error* oError)
	{ return fNodeRep->CreateRWPos(iOpenExisting, iPreventWriters, oError); }

ZRef<ZNodeRep> ZNodeRep_Wrapper::GetRealNodeRep()
	{ return fNodeRep; }

// =================================================================================================
// MARK: - ZNodeIterRep_Wrapper definition

ZNodeIterRep_Wrapper::ZNodeIterRep_Wrapper(ZRef<ZNodeRep> iParent,
	const std::string& iName, size_t iDepth, ZRef<ZNodeIterRep> iIterRep)
:	fParent(iParent),
	fName(iName),
	fDepth(iDepth),
	fIterRep(iIterRep)
	{}

bool ZNodeIterRep_Wrapper::HasValue()
	{ return fIterRep->HasValue(); }

void ZNodeIterRep_Wrapper::Advance()
	{ fIterRep->Advance(); }

ZRef<ZNodeRep> ZNodeIterRep_Wrapper::Current()
	{
	if (ZRef<ZNodeRep> theRep = fIterRep->Current())
		return new ZNodeRep_Wrapper(fParent, fName, fDepth, theRep);
	return ZRef<ZNodeRep>();
	}

std::string ZNodeIterRep_Wrapper::CurrentName()
	{ return fIterRep->CurrentName(); }

ZRef<ZNodeIterRep> ZNodeIterRep_Wrapper::Clone()
	{ return new ZNodeIterRep_Wrapper(fParent, fName, fDepth, fIterRep->Clone()); }

} // namespace ZooLib
