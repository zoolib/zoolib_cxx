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

#ifndef __ZTB__
#define __ZTB__ 1
#include "zconfig.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/tuplebase/ZTBRep.h"
#include "zoolib/tuplebase/ZTBSpec.h"
#include "zoolib/ZTxn.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTB

/// A smart pointer representing the connection to a tuplebase.

class ZTB
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTB, operator_bool_generator_type, operator_bool_type);
public:
	ZTB() {}
	ZTB(ZRef<ZTBRep> iTBRep);
	ZTB(const ZTB& iOther);
	ZTB& operator=(const ZTB& iOther);
	~ZTB();

	/// Returns true-equivalent if referencing a usable tuplebase.
	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(fTBRep && true); }

	/// Return an ID that's never been returned before, and never will again.
	uint64 AllocateID();

	/// Allocate an ID, and store \a iTuple at that ID.
	uint64 Add(const ZTxn& iTxn, const ZTuple& iTuple);

	/// Store \a iTuple at the ID \a iID.
	void Set(const ZTxn& iTxn, uint64 iID, const ZTuple& iTuple);

	/// Store an empty tuple at the ID \a iID.
	void Erase(const ZTxn& iTxn, uint64 iID);

	/// Return the tuple stored at ID \a iID.
	ZTuple Get(const ZTxn& iTxn, uint64 iID);

	/// Return the number of tuples that would be visited by walking \a iQuery.
	size_t Count(const ZTxn& iTxn, const ZTBQuery& iQuery);

	/// Returns the underlying TBRep.
	ZRef<ZTBRep> GetTBRep() const;

protected:
	ZRef<ZTBRep> fTBRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBTxn

/// Combines references to a ZTxn and ZTB.

class ZTBTxn
	{
private:
	ZTBTxn(); // Not implemented
	ZTBTxn& operator=(const ZTBTxn&); // Not implemented

public:
	ZTBTxn(const ZTBTxn& iOther);
	ZTBTxn(const ZTxn& iTxn, ZTB iTB);
	~ZTBTxn();

	/// Return an ID that's never been returned before, and never will again.
	uint64 AllocateID() const;

	/// Allocate an ID, and store \a iTuple at that ID.
	uint64 Add(const ZTuple& iTuple) const;

	/// Store \a iTuple at the ID \a iID.
	void Set(uint64 iID, const ZTuple& iTuple) const;

	/// Store an empty tuple at the ID \a iID.
	void Erase(uint64 iID) const;

	/// Return the tuple stored at ID \a iID.
	ZTuple Get(uint64 iID) const;

	/// Return the number of tuples that would be visited by walking \a iQuery.
	size_t Count(const ZTBQuery& iQuery) const;

	ZTBRepTransaction* GetTBRepTransaction() const;

protected:
	mutable ZTBRepTransaction* fTBRepTransaction;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTBIter

class ZTBIterRep;

/// Allows one to walk over the set of tuples in a ZTB as described by a ZTBQuery.

class ZTBIter
	{
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZTBIter, operator_bool_generator_type, operator_bool_type);
public:
/** \name Constructors
*/	//@{
	/// Default constructor, an iterator that's empty.
	ZTBIter();

	/// Creates a fresh iterator that independently references the unvisited tuples in \a iOther.
	ZTBIter(const ZTBIter& iOther);

	/** \brief Iterator that accesses all tuples satisfying \a iQuery in
	tuplebase \a iTB, in the context of transaction \a iTxn. */
	ZTBIter(const ZTxn& iTxn, ZTB iTB, const ZTBQuery& iQuery);

	/** \brief Iterator that accesses all tuples satisfying \a iQuery in
	the tuplebase and transaction referenced by \a iTBTxn. */
	ZTBIter(const ZTBTxn& iTBTxn, const ZTBQuery& iQuery);
	//@}
	~ZTBIter();

	/// Update the iterator to reference unvisited tuples in \a iOther.
	ZTBIter& operator=(const ZTBIter& iOther);

	/// Returns true-equivalent if still referencing a tuple.
	operator operator_bool_type() const;

	/// Move on to the next tuple.
	void Advance();

	/// Skip past no more than \a iCount tuples, returning the number that were actually skipped.
	size_t Advance(size_t iCount);

	/// Skip past every tuple, returning the number that were skipped.
	size_t AdvanceAll();

	/// Return the currently referenced tuple.
	ZTuple Get() const;

	/// Return the ID of the currently referenced tuple.
	uint64 GetID() const;

	/// Return the ID and value of the currently referenced tuple.
	void Get(uint64* oID, ZTuple* oTuple) const;

	/// Replace the currently referenced tuple with \a iTuple.
	void Set(const ZTuple& iTuple);

	/// Replace the currently referenced tuple with an empty tuple.
	void Erase();

	/// Replace all remaining tuples with an empty tuple, returning the number of tuples touched.
	size_t EraseAll();

private:
	ZRef<ZTBIterRep> fRep;
	size_t fIndex;
	};

} // namespace ZooLib

#endif // __ZTB__
