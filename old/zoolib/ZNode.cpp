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

#include "zoolib/ZNode.h"

namespace ZooLib {

using std::set;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - ZNode

ZNode::ZNode()
	{}

ZNode::ZNode(const ZNode& iNode)
:	fNodeRep(iNode.fNodeRep),
	fComps(iNode.fComps)
	{}

ZNode::ZNode(const ZRef<ZNodeRep>& iNodeRep)
:	fNodeRep(iNodeRep)
	{}

ZNode::ZNode(const ZRef<ZNodeRep>& iNodeRep, const string& iComp)
:	fNodeRep(iNodeRep),
	fComps(1, iComp)
	{}

ZNode::ZNode(ZNodeRep* iNodeRep)
:	fNodeRep(iNodeRep)
	{}

ZNode::~ZNode()
	{}

ZNode& ZNode::operator=(const ZNode& iOther)
	{
	fNodeRep = iOther.fNodeRep;
	fComps = iOther.fComps;
	return *this;
	}
	
ZNode& ZNode::operator=(const ZRef<ZNodeRep>& iNodeRep)
	{
	fNodeRep = iNodeRep;
	fComps.clear();
	return *this;
	}

ZNode::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fNodeRep && true); }

bool ZNode::GetProp(const string& iPropName, ZTValue& oTV, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->GetProp(iPropName, oTV, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return false;
	}

bool ZNode::CanHaveChildren(Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->CanHaveChildren(oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return false;
	}

string ZNode::Name(Error* oError) const
	{
	if (fNodeRep)
		{
		if (fComps.empty())
			return fNodeRep->GetName(oError);
		if (oError)
			*oError = errorNone;
		return fComps.back();
		}

	if (oError)
		*oError = errorInvalidNode;

	return string();
	}

ZNode ZNode::Ancestor(size_t iCount, Error* oError) const
	{
	if (fNodeRep)
		{
		if (iCount >= fComps.size())
			{
			ZRef<ZNodeRep> newNodeRep = fNodeRep->GetAncestor(iCount - fComps.size(), oError);
			return ZNode(newNodeRep);
			}
		if (oError)
			*oError = errorNone;
		return ZNode(fNodeRep, fComps.begin(), fComps.end() - iCount);
		}

	if (oError)
		*oError = errorInvalidNode;
	return ZNode();
	}

ZNode ZNode::Parent(Error* oError) const
	{ return this->Ancestor(1, oError); }

ZNode ZNode::Child(const string& iName, Error* oError) const
	{
	if (oError)
		*oError = errorNone;

	if (iName.empty())
		return *this;
	else
		return ZNode(fNodeRep, fComps.begin(), fComps.end(), iName);
	}

ZNode ZNode::Sibling(const string& iName, Error* oError) const
	{
	if (ZNode parent = this->Parent(oError))
		return parent.Child(iName, oError);

	return ZNode();
	}

ZNode ZNode::Trail(const ZTrail& iTrail, Error* oError) const
	{
	if (fNodeRep)
		{
		vector<string> originalComps = fComps;
		originalComps.insert(originalComps.end(), iTrail.GetComps().begin(), iTrail.GetComps().end());
		vector<string> realComps;
		size_t bounces = ZTrail::sNormalize_ReturnLeadingBounces(originalComps, realComps);

		if (ZRef<ZNodeRep> ancestorNodeRep = fNodeRep->GetAncestor(bounces, oError))
			{
			if (realComps.size())
				return ZNode(ancestorNodeRep, realComps.begin(), realComps.end());
			return ZNode(ancestorNodeRep);
			}
		}

	if (oError)
		*oError = errorInvalidNode;
	return ZNode();
	}

ZNode ZNode::Descendant(const std::string* iComps, size_t iCount, Error* oError) const
	{		
	if (fNodeRep)
		{
		if (oError)
			*oError = errorNone;
		if (iCount == 0)
			return *this;

		vector<string> newComps = fComps;
		newComps.insert(newComps.end(), iComps, iComps + iCount);

		return ZNode(fNodeRep, newComps.begin(), newComps.end());
		}

	if (oError)
		*oError = errorInvalidNode;
	return ZNode();
	}

ZTrail ZNode::TrailTo(const ZNode& iDest, Error* oError) const
	{
	if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
		{
		if (ZRef<ZNodeRep> destRealRep = iDest.pPhysicalNodeRep())
			return realRep->TrailTo(destRealRep, oError);
		}

	if (oError)
		*oError = errorInvalidNode;

	return ZTrail(false);
	}

ZTrail ZNode::TrailFrom(const ZNode& iSource, Error* oError) const
	{
	if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
		{
		if (ZRef<ZNodeRep> sourceRealRep = iSource.pPhysicalNodeRep())
			return sourceRealRep->TrailTo(realRep, oError);
		}

	if (oError)
		*oError = errorInvalidNode;

	return ZTrail(false);
	}

ZTrail ZNode::TrailFromRoot(Error* oError) const
	{
	if (ZNode theNode = fNodeRep)
		{
		ZTrail result;
		while (theNode)
			{
			string theName = theNode.Name();
			if (!theName.empty())
				result.PrependComp(theName);
			theNode = theNode.Parent();
			}
		if (oError)
			*oError = errorNone;
		return result;
		}

	if (oError)
		*oError = errorInvalidNode;

	return ZTrail(false);
	}

bool ZNode::Exists(Error* oError) const
	{
	if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
		return realRep->Exists(oError);

	if (oError)
		*oError = errorInvalidNode;

	return false;
	}

ZNode ZNode::CreateDir(Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->CreateDir(oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZNode();
	}

ZNode ZNode::MoveTo(const ZNode& iDest, Error* oError) const
	{
	if (fNodeRep && iDest.fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			{
			if (ZRef<ZNodeRep> destRealRep = iDest.pPhysicalNodeRep())
				return realRep->MoveTo(destRealRep, oError);
			}
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}

	return ZNode();
	}
#if 0
ZNode ZNode::MoveFrom(const ZNode& iSource, Error* oError) const
	{
	if (fNodeRep && iSource.fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			{
			if (ZRef<ZNodeRep> sourceRealRep = iSource.pPhysicalNodeRep())
				return realRep->MoveFrom(sourceRealRep, oError);
			}
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}

	return ZNode();
	}
#endif
bool ZNode::Delete(Error* oError) const
	{
	if (!fNodeRep)
		{
		if (oError)
			*oError = errorInvalidNode;
		return false;
		}

	if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
		return realRep->Delete(oError);

	if (oError)
		*oError = errorDoesntExist;

	return false;
	}

ZRef<ZStreamerR> ZNode::OpenR(bool iPreventWriters, Error* oError) const
	{
	return this->OpenRPos(iPreventWriters, oError);
	}

ZRef<ZStreamerRPos> ZNode::OpenRPos(bool iPreventWriters, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->OpenRPos(iPreventWriters, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZRef<ZStreamerRPos>();
	}

ZRef<ZStreamerW> ZNode::OpenW(bool iPreventWriters, Error* oError) const
	{
	return this->OpenWPos(iPreventWriters, oError);
	}

ZRef<ZStreamerWPos> ZNode::OpenWPos(bool iPreventWriters, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->OpenWPos(iPreventWriters, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZNode::OpenRWPos(bool iPreventWriters, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->OpenRWPos(iPreventWriters, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZStreamerWPos> ZNode::CreateWPos(bool iOpenExisting, bool iPreventWriters, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->CreateWPos(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZNode::CreateRWPos(bool iOpenExisting, bool iPreventWriters, Error* oError) const
	{
	if (fNodeRep)
		{
		if (ZRef<ZNodeRep> realRep = this->pPhysicalNodeRep())
			return realRep->CreateRWPos(iOpenExisting, iPreventWriters, oError);
		if (oError)
			*oError = errorDoesntExist;
		}
	else
		{
		if (oError)
			*oError = errorInvalidNode;
		}
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZNodeRep> ZNode::GetNodeRep() const
	{ return this->pPhysicalNodeRep(); }

ZRef<ZNodeRep> ZNode::pPhysicalNodeRep() const
	{
	if (!fNodeRep || fComps.empty())
		return fNodeRep;

	return fNodeRep->GetDescendant(&fComps[0], fComps.size(), nullptr);
	}

// =================================================================================================
// MARK: - ZNodeIter

ZNodeIter::ZNodeIter()
	{}

ZNodeIter::ZNodeIter(const ZNodeIter& iOther)
:	fRep(iOther.fRep)
	{}

ZNodeIter::ZNodeIter(const ZNode& iRoot)
	{
	if (ZRef<ZNodeRep> theRep = iRoot.GetNodeRep())
		fRep = theRep->CreateIterRep();
	}

ZNodeIter::~ZNodeIter()
	{}

ZNodeIter& ZNodeIter::operator=(const ZNodeIter& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZNodeIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

ZNodeIter& ZNodeIter::Advance()
	{
	if (fRep)
		{
		if (fRep->GetRefCount() > 1)
			fRep = fRep->Clone();

		if (fRep)
			fRep->Advance();
		}
	return *this;
	}

ZNode ZNodeIter::Current() const
	{
	if (fRep)
		return ZNode(fRep->Current());
	return ZNode();
	}

string ZNodeIter::CurrentName() const
	{
	if (fRep)
		return fRep->CurrentName();
	return string();
	}

// =================================================================================================
// MARK: - ZNodeTreeIter

ZNodeTreeIter::ZNodeTreeIter()
	{}

ZNodeTreeIter::ZNodeTreeIter(const ZNodeTreeIter& iOther)
:	fCurrent(iOther.fCurrent),
	fStack(iOther.fStack)
	{}

ZNodeTreeIter::ZNodeTreeIter(const ZNode& iCurrent)
:	fCurrent(iCurrent)
	{}

ZNodeTreeIter::~ZNodeTreeIter()
	{}

ZNodeTreeIter& ZNodeTreeIter::operator=(const ZNodeTreeIter& iOther)
	{
	fCurrent = iOther.fCurrent;
	fStack = iOther.fStack;
	return *this;
	}

ZNodeTreeIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fCurrent && true); }

ZNodeTreeIter& ZNodeTreeIter::Advance()
	{
	if (ZNodeIter iter = fCurrent)
		{
		// fCurrent is something with children, so descend into it.
		fStack.push_back(iter);
		fCurrent = fStack.back().Current();
		}
	else
		{
		// fCurrent is something with no children.
		fCurrent = ZNode();
		while (!fStack.empty() && !fCurrent)
			{
			if (fStack.back().Advance())
				fCurrent = fStack.back().Current();
			else
				fStack.pop_back();
			}	
		}
	return *this;
	}

ZNode ZNodeTreeIter::Current() const
	{ return fCurrent; }

std::string ZNodeTreeIter::CurrentName() const
	{ return fCurrent.Name(); }

// =================================================================================================
// MARK: - ZNodeRep

ZNodeRep::ZNodeRep()
	{}

ZNodeRep::~ZNodeRep()
	{}

ZRef<ZNodeIterRep> ZNodeRep::CreateIterRep()
	{ return ZRef<ZNodeIterRep>(); }

bool ZNodeRep::GetProp(const string& iPropName, ZTValue& oTV, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorNone;
	return false;
	}

bool ZNodeRep::CanHaveChildren(Error* oError)
	{
	if (oError)
		*oError = ZNode::errorNone;
	return false;
	}

ZRef<ZNodeRep> ZNodeRep::CreateDir(Error* oError)
	{
	if (oError)
		*oError = ZNode::errorWrongTypeForOperation;
	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep::MoveTo(ZRef<ZNodeRep> iDest, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorGeneric;
	return ZRef<ZNodeRep>();
	}

#if 0//##
ZRef<ZNodeRep> ZNodeRep::MoveFrom(ZRef<ZNodeRep> iSource, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorGeneric;
	return ZRef<ZNodeRep>();
	}
#endif

bool ZNodeRep::Delete(Error* oError)
	{
	if (oError)
		*oError = ZNode::errorGeneric;
	return false;
	}

ZRef<ZStreamerR> ZNodeRep::OpenR(bool iPreventWriters, Error* oError)
	{ return this->OpenRPos(iPreventWriters, oError); }

ZRef<ZStreamerRPos> ZNodeRep::OpenRPos(bool iPreventWriters, Error* oError)
	{ return this->OpenRWPos(iPreventWriters, oError); }

ZRef<ZStreamerRWPos> ZNodeRep::OpenRWPos(bool iPreventWriters, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorWrongTypeForOperation;
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZStreamerW> ZNodeRep::OpenW(bool iPreventWriters, Error* oError)
	{ return this->OpenWPos(iPreventWriters, oError); }

ZRef<ZStreamerWPos> ZNodeRep::OpenWPos(bool iPreventWriters, Error* oError)
	{ return this->OpenRWPos(iPreventWriters, oError); }

ZRef<ZStreamerWPos> ZNodeRep::CreateWPos(bool iOpenExisting, bool iPreventWriters, Error* oError)
	{ return this->CreateRWPos(iOpenExisting, iPreventWriters, oError); }

ZRef<ZStreamerRWPos> ZNodeRep::CreateRWPos(bool iOpenExisting, bool iPreventWriters, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorWrongTypeForOperation;
	return ZRef<ZStreamerRWPos>();
	}

// =================================================================================================
// MARK: - ZNodeIterRep

ZNodeIterRep::ZNodeIterRep()
	{}

ZNodeIterRep::~ZNodeIterRep()
	{}

// =================================================================================================
// MARK: - ZNodeIterRep_Std

ZNodeIterRep_Std::ZNodeIterRep_Std(ZRef<RealRep> iRealRep, size_t iIndex)
:	fRealRep(iRealRep),
	fIndex(iIndex)
	{}

ZNodeIterRep_Std::~ZNodeIterRep_Std()
	{}

bool ZNodeIterRep_Std::HasValue()
	{ return fRealRep->HasValue(fIndex); }

void ZNodeIterRep_Std::Advance()
	{ ++fIndex; }

ZRef<ZNodeRep> ZNodeIterRep_Std::Current()
	{ return fRealRep->GetNodeRep(fIndex); }

string ZNodeIterRep_Std::CurrentName()
	{ return fRealRep->GetName(fIndex); }

ZRef<ZNodeIterRep> ZNodeIterRep_Std::Clone()
	{ return new ZNodeIterRep_Std(fRealRep, fIndex); }

} // namespace ZooLib
