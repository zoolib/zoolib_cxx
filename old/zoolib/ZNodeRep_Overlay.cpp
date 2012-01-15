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

#include "zoolib/ZNodeRep_Overlay.h"
#include "zoolib/ZUtil_STL.h"
#include <set>

namespace ZooLib {

using std::set;
using std::string;
using std::vector;

// =================================================================================================
// MARK: - ZNodeIterRep_Overlay declaration

class ZNodeIterRep_Overlay : public ZNodeIterRep
	{
public:
	ZNodeIterRep_Overlay(ZRef<ZNodeRep> iParent, const vector<ZRef<ZNodeRep> >& iRepsUnvisited);
	ZNodeIterRep_Overlay(ZRef<ZNodeRep> iParent, ZRef<ZNodeIterRep> iCurrentIterRep,
		const vector<ZRef<ZNodeRep> >& iRepsUnvisited, const set<string>& iNamesSeen);

// From ZNodeIterRep
	virtual bool HasValue();
	virtual void Advance();
	virtual ZRef<ZNodeRep> Current();
	virtual std::string CurrentName();

	virtual ZRef<ZNodeIterRep> Clone();

private:
	ZRef<ZNodeRep> fParent;
	ZRef<ZNodeIterRep> fCurrentIterRep;
	std::vector<ZRef<ZNodeRep> > fRepsUnvisited;
	std::set<std::string> fNamesSeen;
	};

// =================================================================================================
// MARK: - ZNodeRep_Overlay

ZNodeRep_Overlay::ZNodeRep_Overlay(ZRef<ZNodeRep> iParent,
	const std::string& iName, const vector<ZRef<ZNodeRep> >& iOverlays)
:	fParent(iParent),
	fName(iName),
	fOverlays(iOverlays)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		ZAssertStop(0, *i);
	}

ZRef<ZNodeIterRep> ZNodeRep_Overlay::CreateIterRep()
	{
	return new ZNodeIterRep_Overlay(this, fOverlays);
	}

bool ZNodeRep_Overlay::GetProp(const string& iPropName, ZTValue& oTV, Error* oError)
	{
	return this->pGetNodeRep()->GetProp(iPropName, oTV, oError);
	}

bool ZNodeRep_Overlay::CanHaveChildren(Error* oError)
	{
	return this->pGetNodeRep()->CanHaveChildren(oError);
	}

std::string ZNodeRep_Overlay::GetName(Error* oError)
	{
	return fName;
	}

ZTrail ZNodeRep_Overlay::TrailTo(ZRef<ZNodeRep> iDest, Error* oError)
	{
	ZUnimplemented();
	return ZTrail(false);
	}

ZRef<ZNodeRep> ZNodeRep_Overlay::GetAncestor(size_t iCount, Error* oError)
	{
	if (oError)
		*oError = ZNode::errorNone;

	if (iCount == 0)
		return this;

	if (fParent)
		return fParent->GetAncestor(iCount - 1, oError);

	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep_Overlay::GetDescendant(const std::string* iComps,
	size_t iCount, Error* oError)
	{
	if (iCount == 0)
		return this;

	vector<ZRef<ZNodeRep> > newOverlays;
	newOverlays.reserve(fOverlays.size());
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZNodeRep> theRep = (*i)->GetDescendant(iComps, 1, oError))
			newOverlays.push_back(theRep);
		}

	ZRef<ZNodeRep_Overlay> theRep = new ZNodeRep_Overlay(this, *iComps, newOverlays);
	return theRep->GetDescendant(iComps + 1, iCount - 1, oError);
	}

bool ZNodeRep_Overlay::Exists(Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->Exists(oError))
			return true;
		}
	return false;
	}

ZRef<ZNodeRep> ZNodeRep_Overlay::CreateDir(Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->CreateDir(oError))
			return this;
		}

	return ZRef<ZNodeRep>();
	}

ZRef<ZNodeRep> ZNodeRep_Overlay::MoveTo(ZRef<ZNodeRep> iDest, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->MoveTo(iDest, oError))
			return this;
		}

	if (oError)
		*oError = ZNode::errorGeneric;

	return ZRef<ZNodeRep>();
	}

#if 0//##
ZRef<ZNodeRep> ZNodeRep_Overlay::MoveFrom(ZRef<ZNodeRep> iSource, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->MoveFrom(iSource, oError))
			return this;
		}

	if (oError)
		*oError = ZNode::errorGeneric;

	return ZRef<ZNodeRep>();
	}
#endif //##

bool ZNodeRep_Overlay::Delete(Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->Delete(oError))
			return true;
		}
	return false;
	}

ZRef<ZStreamerR> ZNodeRep_Overlay::OpenR(bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerR> theStreamer = (*i)->OpenR(iPreventWriters, oError))
			return theStreamer;
		}
	return ZRef<ZStreamerR>();
	}

ZRef<ZStreamerRPos> ZNodeRep_Overlay::OpenRPos(bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerRPos> theStreamer = (*i)->OpenRPos(iPreventWriters, oError))
			return theStreamer;
		}
	return ZRef<ZStreamerRPos>();
	}

ZRef<ZStreamerW> ZNodeRep_Overlay::OpenW(bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerW> theStreamer = (*i)->OpenW(iPreventWriters, oError))
			return theStreamer;
		}
	return ZRef<ZStreamerW>();
	}

ZRef<ZStreamerWPos> ZNodeRep_Overlay::OpenWPos(bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerWPos> theStreamer = (*i)->OpenWPos(iPreventWriters, oError))
			return theStreamer;
		}
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZNodeRep_Overlay::OpenRWPos(bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerRWPos> theStreamer = (*i)->OpenRWPos(iPreventWriters, oError))
			return theStreamer;
		}
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZStreamerWPos> ZNodeRep_Overlay::CreateWPos(bool iOpenExisting,
	bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerWPos> theStreamer =
			(*i)->CreateWPos(iOpenExisting, iPreventWriters, oError))
			{
			return theStreamer;
			}
		}
	return ZRef<ZStreamerWPos>();
	}

ZRef<ZStreamerRWPos> ZNodeRep_Overlay::CreateRWPos(bool iOpenExisting,
	bool iPreventWriters, Error* oError)
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZStreamerRWPos> theStreamer =
			(*i)->CreateRWPos(iOpenExisting, iPreventWriters, oError))
			{
			return theStreamer;
			}
		}
	return ZRef<ZStreamerRWPos>();
	}

ZRef<ZNodeRep> ZNodeRep_Overlay::pGetNodeRep()
	{
	for (vector<ZRef<ZNodeRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if ((*i)->Exists(nullptr))
			return *i;
		}
	return ZRef<ZNodeRep>();
	}

// =================================================================================================
// MARK: - ZNodeIterRep_Overlay definition

ZNodeIterRep_Overlay::ZNodeIterRep_Overlay(ZRef<ZNodeRep> iParent,
	const vector<ZRef<ZNodeRep> >& iRepsUnvisited)
:	fParent(iParent),
	fRepsUnvisited(iRepsUnvisited)
	{
	for (;;)
		{
		if (fRepsUnvisited.empty())
			break;
		fCurrentIterRep = fRepsUnvisited.front()->CreateIterRep();
		fRepsUnvisited.erase(fRepsUnvisited.begin());
		if (fCurrentIterRep && fCurrentIterRep->HasValue())
			{
			fNamesSeen.insert(fCurrentIterRep->CurrentName());
			break;
			}
		fCurrentIterRep.Clear();
		}
	}

ZNodeIterRep_Overlay::ZNodeIterRep_Overlay(ZRef<ZNodeRep> iParent,
	ZRef<ZNodeIterRep> iCurrentIterRep,
	const vector<ZRef<ZNodeRep> >& iRepsUnvisited,
	const set<string>& iNamesSeen)
:	fParent(iParent),
	fCurrentIterRep(iCurrentIterRep),
	fRepsUnvisited(iRepsUnvisited),
	fNamesSeen(iNamesSeen)
	{}

bool ZNodeIterRep_Overlay::HasValue()
	{
	return fCurrentIterRep && fCurrentIterRep->HasValue();
	}

void ZNodeIterRep_Overlay::Advance()
	{
	if (!fCurrentIterRep)
		return;

	fCurrentIterRep->Advance();

	for (;;)
		{
		if (fCurrentIterRep && fCurrentIterRep->HasValue())
			{
			string currentName = fCurrentIterRep->CurrentName();
			if (!ZUtil_STL::sContains(fNamesSeen, currentName))
				{
				fNamesSeen.insert(currentName);
				break;
				}
			fCurrentIterRep->Advance();
			}
		else if (!fRepsUnvisited.empty())
			{
			fCurrentIterRep = fRepsUnvisited.front()->CreateIterRep();
			fRepsUnvisited.erase(fRepsUnvisited.begin());
			}
		else
			{
			fCurrentIterRep.Clear();
			break;
			}
		}
	}

ZRef<ZNodeRep> ZNodeIterRep_Overlay::Current()
	{
	if (fCurrentIterRep)
		{
		// Although we have an iterator pointing at a node we still need to return
		// a overlay rep that references the parent we were instantiated from,
		// and all children that match the name in any overlays as yet unvisited.
		// We don't need to reference overlays we have visited, as they cannot
		// by definition have currentName as one of their children.
		string currentName = fCurrentIterRep->CurrentName();
		vector<ZRef<ZNodeRep> > newOverlays;
		if (ZRef<ZNodeRep> currentNodeRep = fCurrentIterRep->Current())
			newOverlays.push_back(currentNodeRep);
		for (vector<ZRef<ZNodeRep> >::iterator i = fRepsUnvisited.begin();
			i != fRepsUnvisited.end(); ++i)
			{
			if (ZRef<ZNodeRep> theChild = (*i)->GetDescendant(&currentName, 1, nullptr))
				newOverlays.push_back(theChild);
			}
		if (newOverlays.empty())
			return ZRef<ZNodeRep>();
	
		return new ZNodeRep_Overlay(fParent, currentName, newOverlays);
		}

	return ZRef<ZNodeRep>();
	}

string ZNodeIterRep_Overlay::CurrentName()
	{
	if (fCurrentIterRep)
		return fCurrentIterRep->CurrentName();

	return string();
	}

ZRef<ZNodeIterRep> ZNodeIterRep_Overlay::Clone()
	{
	return new ZNodeIterRep_Overlay(fParent, fCurrentIterRep->Clone(), fRepsUnvisited, fNamesSeen);
	}

} // namespace ZooLib
