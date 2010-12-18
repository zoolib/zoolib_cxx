/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTB.h"
#include "zoolib/ZStream.h"

using std::min;
using std::vector;

namespace ZooLib {

#define kDebug_TB 0

// =================================================================================================
/**
\defgroup Tuplebase
\sa Tuplestore

A tuplebase is an array of 2^64 tuples. It's always fully populated so that
every index of the array contains a tuple. When first created a tuplebase contains
2^64 empty tuples, tuples with no properties, which is why you don't need
2^64 * x bytes of storage to create a tuplebase -- empty tuples don't need
to actually be stored. The 64 bit index of each tuple
is unique and permanent and is more usually called its ID. Individual
tuples are accessed by ID, and more usefully sets of tuples can be accessed
by using iterators that return tuples matching criteria.

All accesses to a tuplebase, even simple reads, are made in the context of
a transaction. When the transaction is committed
failure can be reported. The power of this is that your code can treat
the tuplebase as always being in a consistent state without any need
to explicitly synchronize access.
The same applies when your transaction needs to
write to the tuplebase -- you can write code of arbitrary
complexity, and when you commit either all of the work is made
permanent and visible to the world or none of it is.

A tuplebase is represented in your code by a ZTB object. ZTB is a
smart pointer with value semantics, it can thus be passed by
value or by reference and can be assigned from and to. Under the hood
there is an instance of ZTBRep, of which there are several concrete
subclasses that use a blockstore or RAM to store tuples, or use
a network connection to a remote tuplebase. These differences do
not affect application-level code.

\note In many of these examples I'll assume that a reference to
a ZTxn instance has been passed in a parameter called
\c iTxn, and that a ZTB reference has been passed in a parameter
called \c iTB.

<h3>Access via ID</h3>
The simplest and least interesting way to access tuples in a tuplebase
is by using the ZTB::Get and ZTB::Set, treating it as a very
large array. We retrieve the tuple stored at ID 27 thus:

\code
ZTuple theTuple = iTB.Get(iTxn, 27);
\endcode
\c theTuple is just a regular tuple, it is independent of the tuplebase
from which it came (although the copy-on-write representation sharing
that ZTuple uses means there's often no actual cost). We can make
changes to theTuple, assigning to it or from it. When the time comes
to write our changes we do this:

\code
iTB.Set(iTxn, 27, theTuple);
\endcode

<h3>No create or delete</h3>
In the preceding example we presumed the existence of a tuple with ID 27.
A problem? No. A tuplebase is \em always fully populated. You can
always read an arbitrary ID, just as you can always write to an
arbitrary ID. Consequentially there is no concept of 'creating' or
'deleting' a tuple in a tuplestore. However we do need to arbitrate access
to the ID Space. We want to be able to write tuples into slots that are
guaranteed never to have been used previously.
That's why we use 64 bit IDs -- they're large enough to be considered
inexhaustible. ZTB::AllocateID returns a 64 bit ID that has never been
returned previously, nor ever will again. For convenience ZTB::Add
both allocates an ID and stores the passed-in tuple under that ID.

So we've got a humungous array of tuples. This in itself can be useful,
perhaps as a way to store tuple-ized object trees. More interesting
is in treating it as a database, but to see how to do so we need
some more building blocks.

<h3>Specifications</h3>

ZTBSpec provides a way to specify tuples. An instance of ZTBSpec
is initialized with criteria or by being the combination of
other specifications. Then ZTBSpec::Matches can be used to determine
if a particular tuple matches that specification.

For example, the spec that will match all tuples whose property named
\c "Prop" has the string value \c "Value" can be constructed thus, using the static
pseudo-constructor ZTBSpec::sEquals:
\code
ZTBSpec equalsSpec = ZTBSpec::sEquals("Prop", "Value");
\endcode
or by explicitly passing the relationship:
\code
ZTBSpec equalsSpec("Prop", ZTBSpec::eRel_Equal, "Value");
\endcode
The property name parameter is of course always a string, and should
considered to be UTF-8 although at this stage it's really just a
bunch of bytes. The value parameter can be any ZTValue. So
to specify tuples whose property \c "OtherProp" is an int32 less than 100:
\code
ZTBSpec lessSpec = ZTBSpec::sLess("OtherProp", int32(100));
\endcode

ZTBSpec instances can be combined with the \c & and \c | operators,
so this expression would match all tuples satsifying \em either of the
prior specifications:
\code
ZTBSpec eitherSpec = equalsSpec | lessSpec;
\endcode
and to specify tuples that match \em both:
\code
ZTBSpec bothSpec = equalsSpec & lessSpec;
\endcode

<h3>Iterators</h3>
ZTBSpec gives us what we need to \em describe tuples that we're interested
in. ZTBIter encapsulates the notion of applying such a specification to
a tuplebase and iterating through all the tuples that match. Of course we
also need to have a ZTxn instance to specify the context in which
the access will be performed.

The ZTBIter constructor thus takes a ZTxn, a ZTB and a ZTBQuery. The
ZTBQuery can itself be constructed from a ZTBSpec, and its additional
capabilities will be covered a little later.

To iterate through all the tuples matching \c bothSpec from above:
\code
for (ZTBIter theIter(iTxn, iTB, bothSpec); theIter; theIter.Advance())
	{
	ZTuple aTuple = theIter.Get();
	// Do something with aTuple.
	}
\endcode
Here ZTBIter::operator_bool_type() is being checked before each iteration
occurs. It continues to return true until we run off the end of the result
set. ZTBIter::Advance updates the iterator to reference the next matching
tuple, or changes its state so that ZTBIter::operator_bool_type() will return
false, indicating that there are no more tuples to examine.

ZTBIter has value semantics, so it can be assigned to or from another iterator,
can be kept in instance variables, passed as a parameter to functions etc. Under
the hood, copy-on-write makes it virtually zero cost to pass around instances of
ZTBIter. However once the transaction with which it was initialized has been
committed or aborted the iterator will become invalid, and currently it becomes
unsafe to use (except to destroy).

<h3>More complex queries</h3>
Being able to access that subset of the tuples in a tuplebase that match
a specification is very useful. However real-world use often requires that
property values from found tuples be used as the criteria for another level
of search, somewhat like an SQL join and select. It's straightforward
to structure application code as a loop that walks an iterator, using properties
from visited tuples as the criteria for an iterator walked by a nested loop.
For the following examples we'll assume we have a tuplebase with the following
tuples, each is preceded by its ID:
\code
 1: { Kind = "Organization"; "Name" = "SomeCompany" }
 2: { Kind = "Organization"; "Name" = "OtherCompany" }

10: { Kind = "Person"; "Name" = "Fred"; Organization = ID(1); }
11: { Kind = "Person"; "Name" = "Bill"; Organization = ID(1); }
12: { Kind = "Person"; "Name" = "Jack"; Organization = ID(2); }
13: { Kind = "Person"; "Name" = "Jill"; Organization = ID(2); }
14: { Kind = "Person"; "Name" = "John"; Organization = ID(3); }

15: { Kind = "Equipment"; "Model" = "Fujitsu"; Organization = ID(2); }
\endcode
To examine each tuple that has an extant organization (IDs 10 through
15, excepting 14):
\code
ZTBSpec orgSpec = ZTBSpec::sEquals("Kind", "Organization");
for (ZTBIter orgIter(iTxn, iTB, orgSpec); orgIter; orgIter.Advance())
	{
	uint64 orgID = orgIter.GetID();
	ZTBSpec entitySpec = ZTBSpec::sEquals("Organization", orgID);
	for (ZTBIter entityIter(iTxn, iTB, entitySpec); entityIter; entityIter.Advance())
		{
		ZTuple entityTuple = entityIter.Get();
		// Do something with the tuple.
		}
	}
\endcode

By walking the outer iterator and using returned values to initialize
the inner iterator's query we're effectively using C++ as our query
language. The preceding example is a bit verbose, and could have been
simplified somewhat as follows:

\code
for (ZTBIter orgIter(iTxn, iTB, ZTBSpec::sEquals("Kind", "Organization"));
	orgIter; orgIter.Advance())
	{
	for (ZTBIter entityIter(iTxn, iTB, ZTBSpec::sEquals("Organization", orgIter.GetID()));
		entityIter; entityIter.Advance())
		{
		ZTuple entityTuple = entityIter.Get();
		// Do something with the tuple.
		}
	}
\endcode

But the real problem is with the line <tt>// Do something with the tuple.</tt>
Given that C++ does not support closures (functors and function pointers
notwithstanding), how could we parameterize that line? Well, we can't.
But we \em can represent the nested searches thus:

\code
ZTBQuery orgQuery = ZTBSpec::sEquals("Kind", "Organization"));
ZTBQuery entityQuery("Organization", orgQuery);
for (ZTBIter iter(iTxn, iTB, entityQuery); iter; iter.Advance())
	{
	ZTuple entityTuple = entityIter.Get();
	// Do something with the tuple.
	}
\endcode

Here \c orgQuery represents those tuples whose property \c "Kind" has
the value \c "Organization". And \c entityQuery represents those
tuples whose property \c "Organization" is of type \c ID and matches
any of the IDs of tuples from \c orgQuery. The code is not really any
shorter, but it does have two points at which we can parameterize
things. We can take \c entityQuery and return it as the result of
a function, store it in an instance variable or pass it to a function. It's
an abstract representation of the nested loops from earlier, and can be applied
against any ZTxn/ZTB pair. Or we can take the initialized ZTBIter object
and pass it off, return it or store it. Let's turn our example into
a factory function, that returns a ZTBQuery:
\code
ZTBQuery QueryFactory()
	{
	ZTBQuery orgQuery = ZTBSpec::sEquals("Kind", "Organization");
	return ZTBQuery("Organization", orgQuery);
	}
\endcode

This returns the ZTBQuery that represents "entities that have an extant organization".
To further restrict the results to include only people
(i.e. tuples whose \c "Kind" property has the value \c "Person"):

\code
ZTBQuery theQuery = QueryFactory();
theQuery &= ZTBSpec::sEquals("Kind", "Person");
\endcode

run against our example tuples this drops ID 15, because its \c "Kind"
property has the value \c "Equipment".

The other advantage to the use of a complex ZTBQuery over manual
iteration is that it is possible for the ZTBQuery to be shipped over the
wire to a remote server for execution close to the tuplebase, thus
removing the latency that would be incurred on each construction of
a ZTBIter by nested loops. And because the whole of the query is
available to the tuplebase it can be examined in its entirety and the
work needed to generate results can be optimized. The disadvantage is
that code using a complex query sees only the tuples that would be returned
by the innermost loop of a manual iteration. If the higher level tuples
are needed then a hybrid approach can be used.

The tuplebase may be configured to maintain indices of the values of tuples,
in which case walking an iterator can be very efficient. If no suitable
index exists then the iterator will still work, but it may require that every
non-empty tuple be visited. Configuring indices on a tuplebase is a system
administration job, and updating the suite of indices is something that
should be informed by viewing log information associated with a tuplebase,
or by knowledge of the actual usage patterns of a tuplebase by code known to
be executing against it.

<h3>Kinds of queries</h3>

We've already seen that a ZTBQuery can be initialized from a ZTBSpec, and
a ZTBIter initialized from such a ZTBQuery will return all the tuples
that match the specification. The other simple instantiations of a ZTBQuery
take an ID or a list of IDs in a \c vector, \c set or pointer and count. A
ZTBIter initialized from one of the following queries would simply return
the tuples with the specified IDs.

\code
ZTBQuery theQuery(27); // Single ID

vector<uint64> theVector;
ZTBQuery theQuery(theVector);

set<uint64> theSet;
ZTBQuery theQuery(theSet);

uint64 theIDs = [ 1, 7, 11, 13, 27 ];
ZTBQuery theQuery(theIDs, 5);
\endcode

More complex queries can be formed by intersecting a ZTBSpec with a ZTBQuery thus:
\code
ZTBQuery theQuery;
ZTBSpec theSpec;
ZTBQuery intersectedQuery = theQuery & theSpec;
\endcode
This represents those tuples that would be returned by \c theQuery which
also satisfy the specification \c theSpec. It might be that \c theQuery
is already simply a search for tuples matching certain criteria, in which
case the criteria represented by \c theSpec are added in as a further
constraint. Or it might be that \c theQuery is highly complex
and cannot simply have a specification applied to it, in which case
\c theSpec will be used to filter the results that are returned. In any
case it's not the application's concern, the underlying mechanisms will
take care of doing the most efficient job possible based on the details
both of the query and of what data actually exists in the tuplebase and
how any indices that may exist can be used.

Similarly one can union a pair of queries thus:
\code
ZTBQuery queryA;
ZTBQuery queryB;
ZTBQuery unionedQuery = queryA | queryB;
\endcode
which represents those tuples that would be returned by \c queryA \em plus
those that would be returned by \c queryB. The tuplebase
implementation will take care of actually collapsing both queries into
a single physical search if that is possible.

Our earlier examples also showed how one can use the result set of
one subquery to provide values to be fed into another:
\code
ZTBQuery theQuery("Organization", orgQuery);
\endcode
the syntax is intended to read <em>tuples whose property named
<tt>"Organization"</tt> are of type ID and match the ID of tuples returned
by <tt>orgQuery</tt></em>. Compare it to the similar ZTBSpec constructor syntax
\code
ZTBSpec theSpec("Organization", uint64(27));
\endcode

The opposite order:
\code
ZTBQuery theQuery(entityQuery, "Organization")
\endcode
reads as <em>tuples whose ID matches the property named <tt>"Organization"</tt>
from tuples returned by <tt>entityQuery</tt></em>.
If in our sample tuplebase \c entityQuery represents the \c "Person" and \c "Equipment"
tuples (IDs 10 through 15), then \c theQuery represents the organizations of those entities.
*/

// =================================================================================================

/**
\defgroup TuplebaseInternals Tuplebase Internals
To use a tuplebase you do not need to know any of this stuff, but it's here in case you're
curious.
*/

// =================================================================================================
#pragma mark -
#pragma mark * static helper functions

struct Callback_AllocateID_t
	{
	ZSem fSem;
	uint64 fID;
	};

static void spCallback_AllocateID(void* iRefcon, uint64 iBaseID, size_t iCount)
	{
	ZAssert(iCount == 1);
	static_cast<Callback_AllocateID_t*>(iRefcon)->fID = iBaseID;
	static_cast<Callback_AllocateID_t*>(iRefcon)->fSem.Vacate();
	}

struct Callback_GetTuple_t
	{
	ZSem fSem;
	ZTuple fTuple;
	};

static void spCallback_GetTuple(
	void* iRefcon, size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	ZAssert(iCount == 1);
	static_cast<Callback_GetTuple_t*>(iRefcon)->fTuple = iTuples[0];
	static_cast<Callback_GetTuple_t*>(iRefcon)->fSem.Vacate();
	}

static ZTuple spGetTuple(ZTBRepTransaction* iTBRepTransaction, uint64 iID)
	{
	Callback_GetTuple_t theStruct;
	iTBRepTransaction->GetTuples(1, &iID, spCallback_GetTuple, &theStruct);
	theStruct.fSem.Procure();

	return theStruct.fTuple;
	}

struct Callback_Count_t
	{
	ZSem fSem;
	size_t fResult;
	};

static void spCallback_Count(void* iRefcon, size_t iResult)
	{
	static_cast<Callback_Count_t*>(iRefcon)->fResult = iResult;
	static_cast<Callback_Count_t*>(iRefcon)->fSem.Vacate();
	}

static size_t spCount(ZTBRepTransaction* iTBRepTransaction, const ZTBQuery& iQuery)
	{
	Callback_Count_t theStruct;
	iTBRepTransaction->Count(iQuery, spCallback_Count, &theStruct);
	theStruct.fSem.Procure();

	return theStruct.fResult;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTB

/**
\class ZTB
\ingroup Tuplebase
*/

ZTB::ZTB(ZRef<ZTBRep> iTBRep)
:	fTBRep(iTBRep)
	{}

ZTB::ZTB(const ZTB& iOther)
:	fTBRep(iOther.fTBRep)
	{}

ZTB& ZTB::operator=(const ZTB& iOther)
	{
	fTBRep = iOther.fTBRep;
	return *this;
	}

ZTB::~ZTB()
	{}

uint64 ZTB::AllocateID()
	{
	Callback_AllocateID_t theStruct;
	fTBRep->AllocateIDs(1, spCallback_AllocateID, &theStruct);
	theStruct.fSem.Procure();
	return theStruct.fID;
	}

uint64 ZTB::Add(const ZTxn& iTxn, const ZTuple& iTuple)
	{
	Callback_AllocateID_t theStruct;
	fTBRep->AllocateIDs(1, spCallback_AllocateID, &theStruct);
	theStruct.fSem.Procure();

	ZTBRepTransaction* theTBRepTransaction = fTBRep->FindOrCreateTransaction(iTxn);
	theTBRepTransaction->SetTuple(theStruct.fID, iTuple);

	return theStruct.fID;
	}

void ZTB::Set(const ZTxn& iTxn, uint64 iID, const ZTuple& iTuple)
	{
	ZTBRepTransaction* theTBRepTransaction = fTBRep->FindOrCreateTransaction(iTxn);
	theTBRepTransaction->SetTuple(iID, iTuple);
	}

void ZTB::Erase(const ZTxn& iTxn, uint64 iID)
	{
	ZTBRepTransaction* theTBRepTransaction = fTBRep->FindOrCreateTransaction(iTxn);
	theTBRepTransaction->SetTuple(iID, ZTuple());
	}

ZTuple ZTB::Get(const ZTxn& iTxn, uint64 iID)
	{
	ZTBRepTransaction* theTBRepTransaction = fTBRep->FindOrCreateTransaction(iTxn);
	return spGetTuple(theTBRepTransaction, iID);
	}

size_t ZTB::Count(const ZTxn& iTxn, const ZTBQuery& iQuery)
	{
	ZTBRepTransaction* theTBRepTransaction = fTBRep->FindOrCreateTransaction(iTxn);
	return spCount(theTBRepTransaction, iQuery);
	}

ZRef<ZTBRep> ZTB::GetTBRep() const
	{ return fTBRep; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBTxn

/**
\class ZTBTxn
\ingroup Tuplebase
*/

ZTBTxn::ZTBTxn(const ZTBTxn& iOther)
:	fTBRepTransaction(iOther.fTBRepTransaction)
	{}

ZTBTxn::ZTBTxn(const ZTxn& iTxn, ZTB iTB)
	{
	fTBRepTransaction = iTB.GetTBRep()->FindOrCreateTransaction(iTxn);
	}

ZTBTxn::~ZTBTxn()
	{}

uint64 ZTBTxn::AllocateID() const
	{
	Callback_AllocateID_t theStruct;
	fTBRepTransaction->GetTBRep()->AllocateIDs(1, spCallback_AllocateID, &theStruct);
	theStruct.fSem.Procure();

	return theStruct.fID;
	}

uint64 ZTBTxn::Add(const ZTuple& iTuple) const
	{
	Callback_AllocateID_t theStruct;
	fTBRepTransaction->GetTBRep()->AllocateIDs(1, spCallback_AllocateID, &theStruct);
	theStruct.fSem.Procure();

	fTBRepTransaction->SetTuple(theStruct.fID, iTuple);

	return theStruct.fID;
	}

void ZTBTxn::Set(uint64 iID, const ZTuple& iTuple) const
	{
	fTBRepTransaction->SetTuple(iID, iTuple);
	}

void ZTBTxn::Erase(uint64 iID) const
	{
	fTBRepTransaction->SetTuple(iID, ZTuple());
	}

ZTuple ZTBTxn::Get(uint64 iID) const
	{
	return spGetTuple(fTBRepTransaction, iID);
	}

size_t ZTBTxn::Count(const ZTBQuery& iQuery) const
	{
	return spCount(fTBRepTransaction, iQuery);
	}

ZTBRepTransaction* ZTBTxn::GetTBRepTransaction() const
	{ return fTBRepTransaction; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBIterRep

class ZTBIterRep : public ZCountedWithoutFinalize
	{
public:
	ZTBIterRep(ZTBRepTransaction* iTransaction, const ZTBQuery& iQuery);
	virtual ~ZTBIterRep();

	ZTBRepTransaction* fTransaction;
	vector<uint64> fIDs;
	};

struct Callback_Search_Struct
	{
	ZTBIterRep* fRep;
	ZSem fSem;
	};

static void spCallback_Search(void* iRefcon, vector<uint64>& ioResults)
	{
	Callback_Search_Struct* theStruct = static_cast<Callback_Search_Struct*>(iRefcon);
	theStruct->fRep->fIDs.swap(ioResults);
	theStruct->fSem.Vacate();
	}

ZTBIterRep::ZTBIterRep(ZTBRepTransaction* iTransaction, const ZTBQuery& iQuery)
:	fTransaction(iTransaction)
	{
	Callback_Search_Struct theStruct;
	theStruct.fRep = this;
	fTransaction->Search(iQuery, spCallback_Search, &theStruct);
	theStruct.fSem.Procure();
	}

ZTBIterRep::~ZTBIterRep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTBIter

/**
\class ZTBIter
\ingroup Tuplebase
*/

/// Instantiate a null iterator that references no tuple.
ZTBIter::ZTBIter()
	{}

/**
Instantiate an iterator referencing the same set of tuples
as \a iOther. Subsequent changes to \a iOther will not affect
this iterator.
*/
ZTBIter::ZTBIter(const ZTBIter& iOther)
:	fRep(iOther.fRep),
	fIndex(iOther.fIndex)
	{}

/// Instantiate an iterator referencing tuples from \a iTB.
ZTBIter::ZTBIter(const ZTxn& iTxn, ZTB iTB, const ZTBQuery& iQuery)
:	fIndex(0)
	{
	if (ZRef<ZTBRep> theTBRep = iTB.GetTBRep())
		fRep = new ZTBIterRep(theTBRep->FindOrCreateTransaction(iTxn), iQuery);
	}

/// Instantiate an iterator referencing tuples from \a iTBTxn.
ZTBIter::ZTBIter(const ZTBTxn& iTBTxn, const ZTBQuery& iQuery)
:	fIndex(0)
	{
	fRep = new ZTBIterRep(iTBTxn.GetTBRepTransaction(), iQuery);
	}

ZTBIter::~ZTBIter()
	{}

/**
Reference the same set of tuples as \a iOther. Subsequent changes
to \a iOther will not affect this iterator.
*/
ZTBIter& ZTBIter::operator=(const ZTBIter& iOther)
	{
	fRep = iOther.fRep;
	fIndex = iOther.fIndex;
	return *this;
	}

ZTBIter::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fIndex < fRep->fIDs.size()); }

/**
Update the iterator so that Get and GetID will return the next tuple from the result set.
*/
void ZTBIter::Advance()
	{
	if (fRep && fIndex < fRep->fIDs.size())
		++fIndex;
	}

size_t ZTBIter::Advance(size_t iCount)
	{
	if (!fRep || fIndex >= fRep->fIDs.size())
		return 0;

	size_t actualCount = min(iCount, fRep->fIDs.size() - fIndex);
	fIndex += actualCount;
	return actualCount;
	}

size_t ZTBIter::AdvanceAll()
	{
	if (!fRep || fIndex >= fRep->fIDs.size())
		return 0;

	size_t actualCount = fRep->fIDs.size() - fIndex;
	fIndex = fRep->fIDs.size();
	return actualCount;
	}

ZTuple ZTBIter::Get() const
	{
	if (fRep && fIndex < fRep->fIDs.size())
		return spGetTuple(fRep->fTransaction, fRep->fIDs[fIndex]);
	return ZTuple();
	}

uint64 ZTBIter::GetID() const
	{
	if (fRep && fIndex < fRep->fIDs.size())
		return fRep->fIDs[fIndex];
	return 0;
	}

void ZTBIter::Get(uint64* oID, ZTuple* oTuple) const
	{
	if (fRep && fIndex < fRep->fIDs.size())
		{
		if (oID)
			*oID = fRep->fIDs[fIndex];
		if (oTuple)
			*oTuple = spGetTuple(fRep->fTransaction, fRep->fIDs[fIndex]);
		}
	else
		{
		if (oID)
			*oID = 0;
		if (oTuple)
			oTuple->Clear();
		}
	}

void ZTBIter::Set(const ZTuple& iTuple)
	{
	if (fRep && fIndex < fRep->fIDs.size())
		fRep->fTransaction->SetTuple(fRep->fIDs[fIndex], iTuple);
	}

void ZTBIter::Erase()
	{
	if (fRep && fIndex < fRep->fIDs.size())
		fRep->fTransaction->SetTuple(fRep->fIDs[fIndex], ZTuple());
	}

size_t ZTBIter::EraseAll()
	{
	if (!fRep || fIndex >= fRep->fIDs.size())
		return 0;

	size_t count = fRep->fIDs.size() - fIndex;
	ZTuple emptyTuple;
	while (fIndex < fRep->fIDs.size())
		{
		fRep->fTransaction->SetTuple(fRep->fIDs[fIndex], emptyTuple);
		++fIndex;
		}
	return count;
	}

} // namespace ZooLib

