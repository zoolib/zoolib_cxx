/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZAsset.h"

using std::set;
using std::string;
using std::vector;

namespace ZooLib {

/**
\defgroup Asset

The name \e asset was chosen as an alternative to \e resource. Resource is a
term that already has too many disparate meanings on our target platforms,
an alternate designation keeps the distinction clear. That said, assets are
used in the same situations that MacOS/Win32/BeOS resources would be, although
the mechanism is more flexible.

Rather than occupying a flat space, or one segmented by a type code and
name/ID, assets are instead organized as a tree. Each asset in the tree
is named and has a parent. The root asset's name is the empty string, and
its parent is null. Assets may have children, in which case they're known as
directories, or they may be leaf assets and contain actual data. Let's use
an example:

\code
name "MyApplication"
  {
  name "MyDialog"
    {
    name "Title" string "My Dialog Title"
    name "Icon" binary "MyDialogIcon.bmp"
    }
  }
\endcode

The unnamed root asset is a directory with the single child <tt>MyApplication</tt>,
which in turn is a directory with the single child <tt>"MyDialog"</tt>. That
directory asset has two children, <tt>"Title"</tt> and <tt>"Icon"</tt>, the first
being a data asset containing the string <tt>"My Dialog Title"</tt> and the second
being a data asset whose content will be read from the file
<tt>"MyDialogIcon.bmp"</tt> when this source snippet is compiled.

A program references assets by using ZAsset objects. A ZAsset is a smart
pointer to an asset in a tree. ZAsset::GetChild takes a string (or const
char*) as a parameter and returns a ZAsset referencing that child. If no
such child exists a null ZAsset is returned for which
ZAsset::operator_bool_type() will return false, and for which all
other methods are no-ops. Accessing the example asset tree example would
look something like this:

\code
	ZAsset theRootAsset = SomeMagicMethodToObtainTheRoot();
	ZAsset theAppAsset = theRootAsset.GetChild("MyApplication");
	ZAsset theDialogAsset = theAppAsset.GetChild("MyDialog");
	ZAsset theTitleAsset = theDialogAsset.GetChild("Title");
	ZAsset theIconAsset = theDialogAsset.GetChild("Icon");
	ZAsset theAppAssetAgain = theDialogAsset.GetParent();
\endcode

ZAssets have value semantics, just like ZDCRgn, ZDCPixmap, \c std::string etc.
They can be passed by value into methods, stored in instance variables or
locals:

\code
	ZAsset myAsset = theAppAsset;
\endcode

\c myAsset now references the asset named "MyApplication". After executing this:

\code
  theAppAsset = someOtherAsset;
\endcode

\c myAsset \e still references the asset named "MyApplication".

\note Currently there are no mutating operations on ZAssets nor on what
ZAsset references. They're effectively const pointers to nodes in the tree.

Given a ZAsset one can get its name from ZAsset::GetName, and get the
path from the root to that asset from ZAsset::GetPath. Executing this
snippet:

\code
	string theTitlePath = theTitleAsset.GetPath();
\endcode

ZAsset::GetChild inteprets path strings as well as standalone names,
so we could have initialized \c theTitleAsset by doing this:

\code
	ZAsset theTitleAsset = theRootAsset.GetChild("MyApplication|MyDialog|Title");
\endcode

A path which \e begins with the pipe character starts resolving the path at
the root, regardless of which asset we're dealing with, so this would still
work:

\code
  ZAsset theTitleAsset = theIconAsset.GetChild("|MyApplication|MyDialog|Title");
\endcode

The hat character (^) indicates that resolution should bounce up to the
parent. You can use it anywhere in a path and it will work as expected, but
it's really only useful at the beginning of a path:

\code
	ZAsset theTitleAsset = theIconAsset.GetChild("^Title");
\endcode

You're unlikely to use hat in C++ code, because ZAsset::GetParent is
available. However it is useful in asset source code when using the so far
unmentioned third asset type, aliases. An alias occupies a place in the tree
like any other asset, but when methods are called on it, to get one of its
children for example, that request is passed on to the asset the alias
references. For example, augmenting our sample with some button titles:
\verbatim
name "MyApplication"
  {
  name "StandardButtons"
    {
    name "OK" string "OK"
    name "Cancel" string "Cancel"
    }
  name "MyDialog"
    {
    name "Title" string "My Dialog Title"
    name "Icon" binary "MyDialogIcon.bmp"
    name "Cancel" alias "^^StandardButtons|Cancel"
    }
  }
\endverbatim

Any request to <tt>"|MyApplication|MyDialog|Cancel"</tt> will actually be fielded by
<tt>"|MyApplication|StandardButtons|Cancel"</tt>.

The neat thing about assets being organized hierarchically is that different
subsets of an application can be passed an asset that references 'their'
node in the tree, and they're not affected by changes to
the rest of the tree -- there's no need to worry about name clashes or
reorganization elsewhere.


<h3>Using asset data</h3>

So we're able to navigate the tree. Big deal. What we really want is to get
at the data that the tree is organizing. Depending on how you want to use
the data there are two ways, ZAsset::OpenRPos and ZAsset::GetData. But
first I'll explain one hidden detail. A node defined in asset source code that
supposedly contains data, like the \c Title node above, does not in
fact \e directly contain the data. To provide for a limited degree of type
safety the following:
\verbatim
	name "Title" string "My Dialog Title"
\endverbatim

in fact creates a directory node called \c Title with a single child
called \c !string. \e That node is the one that actually contains the data in
question. For binaries the data-containing node is called \c !binary, and for
string tables (which I'm leaving out of this discussion for now) it's called
\c !stringtable. If I've explained things well enough, or you've read the parser's
source you'll recognize that the following snippet is legal, if obscure:
\verbatim
  name "SomeName" string "Some string" binary "somebinary.bin"
\endverbatim

It creates a node called \c SomeName, with two children. One called
\c !string containing the text <tt>"Some string"</tt> and one called \c !binary
containing the data from the file <tt>"somebinary.bin"</tt>.

Back to getting at asset data. I'll start with ZAsset::OpenRPos, as it's a
little easier to understand.

\warning There's a whole suite of \c ZAsset::OpenXXX methods, but as \c ZAssets
are currently immutable only ZAsset::OpenR and ZAsset::OpenRPos will return a
valid streamer.

\c ZAsset::OpenXXX each return a newly created \link Streamer streamer\endlink
of the appropriate type. The streamer that's returned is yours to do with as you wish.
It's independent of any other streamer created by any other code -- its
position will only be changed by calls to it, not by anything done by
anything else. You do \e not have to maintain a reference to the ZAsset
itself. Behind the scenes the streamer holds on to the ZAssetRep, the object that
ZAsset encapsulates, so something like this is legal and safe:

\code
	ZRef<ZStreamerRPos> theStreamer =
		theRootAsset.GetChild("MyApplication|MyDialog|Title|!string").OpenPos();
\endcode

The second way to get at data is to have the ZAsset load it into memory and
to get a pointer to the data and its size by calling
\link ZAsset::GetData ZAsset::GetData(const void** oData, size_t* oSize) \endlink.
The \c oData and \c oSize parameters are optional, so you can get the size
without getting a pointer to the data or vice versa, by passing nil for the
value you don't care about. The data is loaded and owned by the asset, so the
pointer that is returned is guaranteed to remain valid so long as the
ZAsset itself remains allocated. The data for an asset is not actually
pulled in off disk (if that's the underlying mechanism) until someone calls
ZAsset::GetData with a non-nil \c oData parameter. It's very cheap to
walk an asset tree and to query any attribute of its nodes.

Why would you use ZAsset::GetData rather than \c ZAsset::OpenXXX? There are two
advantages. First, multiple calls to ZAsset::GetData will generally return the
\e same pointer. So independent users of the data can share it without any
higher-level management scheme being required. The second advantage is that
it's possible that the entire asset tree has actually been mapped into memory,
and thus there is no marginal cost to use the data directly -- it does not have
to be read into a dynamically allocated memory block as it is already mapped into the
address space and ZAsset::GetData has only to do a little pointer arithmetic.
Memory mapped access is used by default on linux, and on Win32 for asset trees
loaded from external files. So if read only access to the data in its raw
form is desired, and the data is used in its entirety (i.e. not just some subset),
then preferring ZAsset::GetData can be a big win. For data that needs to be processed
into some other form, streamer-based access is probably preferable. It's your call to
make based on circumstances.


<h3>Compilation and inclusion of assets in an application</h3>

We use the filename extension \c .zas for ZooLib Asset Source files, and the
extension \c .zao for ZooLib Asset Object files. Compiling a \c zas file generates
a chunk of binary data containing the content referenced by string and
binary statements and a description of the tree by which that content is accessed.

Compilation of a \c zas file in a command line environment generates a file
containing the \c zao data. That file is appended to the executable file after
it is linked (and optionally stripped), and the \c zao data accessed by the
executable at run time.

When using CodeWarrior we add the \c zas file to the application's project.
When the \c zas file is compiled for a MacOS target it generates a temp file in
the project's directory with the same name but with \c .zao extension. That
file can be incorporated en masse as a single MacOS resource by placing a
line like this in the application's \c .r file:

\code
	read 'ZAO_' (20000, "MyAssets") "MyAssets.zao";
\endcode

When using CW for a Win32 target we do something a little different. The \c zas
file is still added to the project as before, but this time when it is
compiled it generates a file in the project's directory with a \c .rci
extension, indicating that it is rc source to be included by the
application's rc file, thus:

\code
	#include "MyAssets.rci"
\endcode

Why the difference? The data in MacOS resources can be accessed byte-by-byte
using \c ReadPartialResource; we do not have to pull the entire resource into
memory to get at part of it. On Win32 that's not the case -- resources are
loaded or not, nothing in between. So the \c zao data is segmented into
multiple 64K chunks, each placed in its own named/numbered resource,
and a header resource generated referencing them. Access to any offset in
the \c zao data need only read in the 64K chunk containing that offset.

<h3>Runtime access to assets</h3>

This is the magic part mentioned above. The namespace method
ZUtil_Asset::sGetAssetRootFromExecutable
does the right thing on each platform. The name you pass in is the base
or stem name of the \c zas file that was used to generate the zao data,
i.e. the name with the \c .zas extension removed.
On MacOS this is a convention, the actual name is determined by the string
inside the parentheses of the rez file read statement, but on POSIX, BeOS and Win32
it's a requirement.

\warning Do not call sGetAssetRootFromExecutable more than once for each tree name.
ZooLib does not track which trees have been loaded from where, and so each invocation
will load a fresh copy of the tree, which might take up significant RAM or at
least address space. It is your code's responsibility to load a tree once and
then hand the root asset, or children thereof, to entities that require access.

You don't have to incorporate asset trees in your executable, they can be
left as standalone files.
ZUtil_Asset::sGetAssetRootFromFileSpec
returns an asset representing the root of the tree stored in the zao data in the
file referenced by \c iFileSpec.

\warning As with \c sGetAssetRootFromExecutable, ZooLib does not remember that
a tree has already been loaded from a file, and will blindly re-map or at least
re-parse the data in the file.

<h3>Examining an asset tree</h3>
In most cases your code will know what structure an asset tree or subtree has, and
thus will just call ZAsset::GetChild to walk that structure. Sometimes though
the code will need to discover what children an asset has. ZAssetIter implements
the emerging ZooLib iterator standard API, consisting of the methods
<tt>operator_bool_type</tt>, \c Advance and \c Current.

You initialize a ZAssetIter from a ZAsset thus:

\code
	ZAssetIter theIter = myAsset;
\endcode

Initially \c theIter references the first child of \c myAsset. \c theIter.Current()
returns that child, and \c theIter.CurrentName() returns the child's name.
\c theIter.Advance() updates \c theIter to reference the next child. If there
are no more children ZAssetIter::operator_bool_type() returns false. So to
walk over every child of myAsset, do this:

\code
	for (ZAssetIter theIter; theIter; theIter.Advance())
		{
		ZAsset theChild = theIter.Current();
		string theChildName = theIter.CurrentName();
		}
\endcode

For a ZAssetIter initialized from an asset with no children, ZAssetIter::operator_bool_type()
will never return true, and ZAssetIter::Current() and ZAssetIter::CurrentName() will return
an invalid ZAsset and an empty string, respectively.
*/

// =================================================================================================
// MARK: - ZAsset

/**
\class ZAsset
\ingroup Asset
\sa Asset
*/


/// Return the asset's name.
/** If the asset is invalid, return an empty string. */
string ZAsset::GetName() const
	{
	if (fRep)
		return fRep->GetName();
	return string();
	}


/// Return the path from the asset's tree's root to the asset.
/** Return an empty string if the asset is the root of the asset tree or is itself invalid. */
string ZAsset::GetPath() const
	{
	if (fRep)
		return fRep->GetPath();
	return string();
	}


/// Return the asset's parent.
/** Return an invalid asset if the asset is the root of the asset tree or is itself invalid. */
ZAsset ZAsset::GetParent() const
	{
	if (fRep)
		return fRep->GetParentAssetRep();
	return ZAsset();
	}


/// Parses the path \a iName, returning the asset referenced by that path
/**
If the asset is invalid, return an invalid asset.

If the path does not reference an extant asset, return an invalid asset.
*/
ZAsset ZAsset::GetChild(const char* iName) const
	{
	if (fRep)
		return fRep->ResolvePath(iName);
	return ZAsset();
	}


///\overload
ZAsset ZAsset::GetChild(const string& iName) const
	{
	if (fRep)
		return fRep->ResolvePath(iName.c_str());
	return ZAsset();
	}


/// Put a pointer to the asset data into \a oData, and its size into \a oSize.
/** If \a oData is non-nil the asset does what is necessary to get the data
into memory and puts the address of the data in \a oData. Once the data has
been loaded it will remain so until all assets sharing the underlying rep
have been destroyed. If \a oData is nil then the data will not be loaded just
to service this request.
*/
void ZAsset::GetData(const void** oData, size_t* oSize) const
	{
	if (fRep)
		{
		fRep->GetData(oData, oSize);
		}
	else
		{
		if (oData)
			*oData = nullptr;
		if (oSize)
			*oSize = 0;
		}
	}


/// Return a new ZStreamerR referencing the asset's data.
ZRef<ZStreamerR> ZAsset::OpenR() const
	{
	if (fRep)
		return fRep->OpenR();
	return ZRef<ZStreamerR>();
	}


/// Return a new ZStreamerRPos referencing the asset's data.
ZRef<ZStreamerRPos> ZAsset::OpenRPos() const
	{
	if (fRep)
		return fRep->OpenRPos();
	return ZRef<ZStreamerRPos>();
	}


/// Return a new ZStreamerW referencing the asset's data.
/** Note that currently no ZAssetRep/ZAssetTree implementations support
writing to asset data, and so a nil streamer will be returned.
*/
ZRef<ZStreamerW> ZAsset::OpenW() const
	{
	if (fRep)
		return fRep->OpenW();
	return ZRef<ZStreamerW>();
	}


/// Return a new ZStreamerWPos referencing the asset's data.
/** Note that currently no ZAssetRep/ZAssetTree implementations support
writing to asset data, and so a nil streamer will be returned.
*/
ZRef<ZStreamerWPos> ZAsset::OpenWPos() const
	{
	if (fRep)
		return fRep->OpenWPos();
	return ZRef<ZStreamerWPos>();
	}


/// Return a new ZStreamerRWPos referencing the asset's data.
/** Note that currently no ZAssetRep/ZAssetTree implementations support
writing to asset data, and so a nil streamer will be returned.
*/
ZRef<ZStreamerRWPos> ZAsset::OpenRWPos() const
	{
	if (fRep)
		return fRep->OpenRWPos();
	return ZRef<ZStreamerRWPos>();
	}

// =================================================================================================
// MARK: - ZAssetIter

/**
\class ZAssetIter
\ingroup Asset
\sa Asset
*/

/// Instantiate an iterator that references no asset.
ZAssetIter::ZAssetIter()
	{}

/// Instantiate an iterator referencing the same parent and current
/// child as \a iOther. Subsequent changes to iOther will not affect
/// this iterator.
ZAssetIter::ZAssetIter(const ZAssetIter& iOther)
:	fRep(iOther.fRep)
	{}

/// Instantiate an iterator referencing the first child (if any) of \a iAsset.
/** If \a iAsset is invalid then this constructor is equivalent
to the default constructor. */
ZAssetIter::ZAssetIter(const ZAsset& iAsset)
	{
	if (ZRef<ZAssetRep> theAssetRep = iAsset.GetRep())
		fRep = theAssetRep->CreateIterRep();
	}

ZAssetIter::~ZAssetIter()
	{}

/// Reference the same parent and current child as \a iOther.
/// Subsequent changes to iOther will not affect this iterator.
ZAssetIter& ZAssetIter::operator=(const ZAssetIter& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

/// If the iterator has not been advanced past the last child of
/// the asset it's traversing, return a non-zero value.
ZAssetIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

/// Move on to the next child.
ZAssetIter& ZAssetIter::Advance()
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Clone();

		if (fRep)
			fRep->Advance();
		}
	return *this;
	}

/// Return the currently-referenced child asset.
ZAsset ZAssetIter::Current() const
	{
	if (fRep)
		return ZAsset(fRep->Current());
	return ZAsset();
	}

/// Return the name of currently-referenced child asset.
string ZAssetIter::CurrentName() const
	{
	if (fRep)
		return fRep->CurrentName();
	return string();
	}

// =================================================================================================
// MARK: - ZAssetRep

ZRef<ZAssetRep> ZAssetRep::ResolvePath(const char* iPath)
	{
	if (iPath == nullptr || iPath[0] == 0)
		{
		// An empty path means us.
		return this;
		}

	if (iPath[0] == '|')
		{
		// A | prefix means we want our root.
		if (ZRef<ZAssetRep> parent = this->GetParentAssetRep())
			{
			// We have a parent, tell it to resolve the whole path.
			return parent->ResolvePath(iPath);
			}
		// We have no parent, so strip off the prefix and call
		// ourselves, probably invoking an overridden version.
		return this->ResolvePath(iPath + 1);
		}

	if (iPath[0] == '^')
		{
		// A ^ prefix means we want our parent to resolve the path that follows.
		if (ZRef<ZAssetRep> parent = this->GetParentAssetRep())
			return parent->ResolvePath(iPath + 1);
		}

	// We've got some path left, but we don't have children, so the resolution fails.
	return ZRef<ZAssetRep>();
	}

void ZAssetRep::GetData(const void** oData, size_t* oSize)
	{
	if (oData)
		*oData = nullptr;
	if (oSize)
		*oSize = 0;
	}

ZRef<ZStreamerR> ZAssetRep::OpenR()
	{ return this->OpenRPos(); }

ZRef<ZStreamerRPos> ZAssetRep::OpenRPos()
	{ return this->OpenRWPos(); }

ZRef<ZStreamerW> ZAssetRep::OpenW()
	{ return this->OpenWPos(); }

ZRef<ZStreamerWPos> ZAssetRep::OpenWPos()
	{ return this->OpenRWPos(); }

ZRef<ZStreamerRWPos> ZAssetRep::OpenRWPos()
	{ return ZRef<ZStreamerRWPos>(); }

ZRef<ZAssetIterRep> ZAssetRep::CreateIterRep()
	{ return ZRef<ZAssetIterRep>(); }

// =================================================================================================
// MARK: - ZAssetIterRep

ZAssetIterRep::ZAssetIterRep()
	{}

ZAssetIterRep::~ZAssetIterRep()
	{}

// =================================================================================================
// MARK: - ZAssetRep_Overlay

ZAssetRep_Overlay::ZAssetRep_Overlay(const ZRef<ZAssetRep>& iParent, const vector<ZRef<ZAssetRep> >& iOverlays)
:	fParent(iParent),
	fOverlays(iOverlays)
	{
	// We must not be created with an empty list
	ZAssertStop(1, !fOverlays.empty());
	}

ZAssetRep_Overlay::ZAssetRep_Overlay(
	const ZRef<ZAssetRep>& iParent, const ZRef<ZAssetRep>& iOverlay)
:	fParent(iParent)
	{
	ZAssertStop(1, iOverlay);

	fOverlays.push_back(iOverlay);
	}

ZAssetRep_Overlay::ZAssetRep_Overlay(const vector<ZRef<ZAssetRep> >& iOverlays)
:	fOverlays(iOverlays)
	{
	// We must not be created with an empty list
	ZAssertStop(1, !fOverlays.empty());
	}

ZAssetRep_Overlay::~ZAssetRep_Overlay()
	{}

string ZAssetRep_Overlay::GetName()
	{ return fOverlays.front()->GetName(); }

string ZAssetRep_Overlay::GetPath()
	{
	if (ZRef<ZAssetRep> myParent = fParent)
		return myParent->GetPath() + "|" + fOverlays.front()->GetName();
	return fOverlays.front()->GetName();
	}

ZRef<ZAssetRep> ZAssetRep_Overlay::GetParentAssetRep()
	{ return fParent; }

ZRef<ZAssetRep> ZAssetRep_Overlay::ResolvePath(const char* iPath)
	{
	// It's not possible to have no entries.
	ZAssertStop(1, !fOverlays.empty());

	// Handle bouncing up the chain, or empty paths.
	if (ZRef<ZAssetRep> theRep = ZAssetRep::ResolvePath(iPath))
		return theRep;

	// Special case when we're just wrapping a single asset.
	if (fOverlays.size() == 1)
		{
		if (ZRef<ZAssetRep> theChild = fOverlays.front()->ResolvePath(iPath))
			return new ZAssetRep_Overlay(this, theChild);
		}

	vector<ZRef<ZAssetRep> > newOverlays;
	newOverlays.reserve(fOverlays.size());
	for (vector<ZRef<ZAssetRep> >::iterator i = fOverlays.begin(); i != fOverlays.end(); ++i)
		{
		if (ZRef<ZAssetRep> theChild = (*i)->ResolvePath(iPath))
			newOverlays.push_back(theChild);
		}

	if (newOverlays.empty())
		return ZRef<ZAssetRep>();

	return new ZAssetRep_Overlay(this, newOverlays);
	}

ZRef<ZAssetIterRep> ZAssetRep_Overlay::CreateIterRep()
	{ return new ZAssetIterRep_Union(this, fOverlays); }

void ZAssetRep_Overlay::GetData(const void** oData, size_t* oSize)
	{ fOverlays.front()->GetData(oData, oSize); }

ZRef<ZStreamerR> ZAssetRep_Overlay::OpenR()
	{ return fOverlays.front()->OpenR(); }

ZRef<ZStreamerRPos> ZAssetRep_Overlay::OpenRPos()
	{ return fOverlays.front()->OpenRPos(); }

ZRef<ZStreamerW> ZAssetRep_Overlay::OpenW()
	{ return fOverlays.front()->OpenW(); }

ZRef<ZStreamerWPos> ZAssetRep_Overlay::OpenWPos()
	{ return fOverlays.front()->OpenWPos(); }

ZRef<ZStreamerRWPos> ZAssetRep_Overlay::OpenRWPos()
	{ return fOverlays.front()->OpenRWPos(); }

// =================================================================================================
// MARK: - ZAssetIterRep_Union

ZAssetIterRep_Union::ZAssetIterRep_Union(
	ZRef<ZAssetRep> iParent, const vector<ZRef<ZAssetRep> >& iRepsUnvisited)
:	fParent(iParent),
	fRepsUnvisited(iRepsUnvisited)
	{
	fCurrentIterRep = fRepsUnvisited.front()->CreateIterRep();
	}

ZAssetIterRep_Union::ZAssetIterRep_Union(
	ZRef<ZAssetRep> iParent, ZRef<ZAssetIterRep> iCurrentIterRep,
	const vector<ZRef<ZAssetRep> >& iRepsUnvisited, const set<string>& iNamesSeen)
:	fParent(iParent),
	fCurrentIterRep(iCurrentIterRep),
	fRepsUnvisited(iRepsUnvisited),
	fNamesSeen(iNamesSeen)
	{
	fCurrentIterRep = fRepsUnvisited.front()->CreateIterRep();
	}

bool ZAssetIterRep_Union::HasValue()
	{
	return fCurrentIterRep && fCurrentIterRep->HasValue();
	}

void ZAssetIterRep_Union::Advance()
	{
	if (!fCurrentIterRep)
		return;

	for (;;)
		{
		fCurrentIterRep->Advance();

		if (fCurrentIterRep->HasValue())
			{
			string currentName = fCurrentIterRep->CurrentName();
			if (fNamesSeen.end() == fNamesSeen.find(currentName))
				{
				fNamesSeen.insert(currentName);
				break;
				}
			}
		else
			{
			if (fRepsUnvisited.empty())
				{
				fCurrentIterRep.Clear();
				break;
				}
			fCurrentIterRep = fRepsUnvisited.front()->CreateIterRep();
			fRepsUnvisited.erase(fRepsUnvisited.begin());
			}
		}
	}

ZRef<ZAssetRep> ZAssetIterRep_Union::Current()
	{
	if (fCurrentIterRep)
		{
		// Although we have an iterator pointing at an asset we still need to return
		// a overlay rep that references the parent we were instantiated from,
		// and all children that match the name in any overlays as yet unvisited.
		// We don't need to reference overlays we have visited, as they cannot
		// by definition have currentName as one of their children.
		string currentName = fCurrentIterRep->CurrentName();
		const char* currentNameCString = currentName.c_str();
		vector<ZRef<ZAssetRep> > newOverlays;
		newOverlays.push_back(fCurrentIterRep->Current());
		for (vector<ZRef<ZAssetRep> >::iterator i = fRepsUnvisited.begin();
			i != fRepsUnvisited.end(); ++i)
			{
			if (ZRef<ZAssetRep> theChild = (*i)->ResolvePath(currentNameCString))
				newOverlays.push_back(theChild);
			}
		if (newOverlays.empty())
			return ZRef<ZAssetRep>();
	
		return new ZAssetRep_Overlay(fParent, newOverlays);
		}

	return ZRef<ZAssetRep>();
	}

string ZAssetIterRep_Union::CurrentName()
	{
	if (fCurrentIterRep)
		return fCurrentIterRep->CurrentName();

	return string();
	}

ZRef<ZAssetIterRep> ZAssetIterRep_Union::Clone()
	{
	return new ZAssetIterRep_Union(fParent, fCurrentIterRep->Clone(), fRepsUnvisited, fNamesSeen);
	}

} // namespace ZooLib
