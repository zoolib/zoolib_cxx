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

#ifndef __ZStrimmer_h__
#define __ZStrimmer_h__ 1
#include "zconfig.h"

#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZStrim.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerR

/** A refcounted entity that provides access to a ZStrimR.
\ingroup Strimmer
*/

class ZStrimmerR : public virtual ZCounted
	{
public:
	virtual const ZStrimR& GetStrimR() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerU

/** A refcounted entity that provides access to a ZStrimU.
\ingroup Strimmer
*/

class ZStrimmerU : public virtual ZStrimmerR
	{
public:
// From ZStrimmerR
	virtual const ZStrimR& GetStrimR();

// Our protocol
	virtual const ZStrimU& GetStrimU() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerW

/** A refcounted entity that provides access to a ZStrimW.
\ingroup Strimmer
*/

class ZStrimmerW : public virtual ZCounted
	{
public:
	virtual const ZStrimW& GetStrimW() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerR_T

template <class Strim_p>
class ZStrimmerR_T : public virtual ZStrimmerR
	{
public:
	ZStrimmerR_T() {}

	virtual ~ZStrimmerR_T() {}

	template <class P>
	ZStrimmerR_T(P& iParam) : fStrim(iParam) {}

	template <class P>
	ZStrimmerR_T(const P& iParam) : fStrim(iParam) {}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	Strim_p fStrim;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerR_FT

template <class Strim_p>
class ZStrimmerR_FT : public virtual ZStrimmerR
	{
protected:
	ZStrimmerR_FT() {}

public:
	virtual ~ZStrimmerR_FT() {}

	template <class P>
	ZStrimmerR_FT(P& iParam, ZRef<ZStrimmerR> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimR())
		{}

	template <class P>
	ZStrimmerR_FT(const P& iParam, ZRef<ZStrimmerR> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimR())
		{}

	ZStrimmerR_FT(ZRef<ZStrimmerR> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iStrimmer->GetStrimR())
		{}

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	ZRef<ZStrimmerR> fStrimmerReal;
	Strim_p fStrim;
	};

template <class Strim_p, class Strimmer_p>
ZRef<ZStrimmerR> sStrimmerR_FT(const ZRef<Strimmer_p>& iStrimmer)
	{
	if (iStrimmer)
		return new ZStrimmerR_FT<Strim_p>(iStrimmer);
	return null;
	}

template <class Strim_p, class Param_p, class Strimmer_p>
ZRef<ZStrimmerR> sStrimmerR_FT(const Param_p& iParam, const ZRef<Strimmer_p>& iStrimmer)
	{
	if (iStrimmer)
		return new ZStrimmerR_FT<Strim_p>(iParam, iStrimmer);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerU_T

template <class Strim_p>
class ZStrimmerU_T : public virtual ZStrimmerU
	{
public:
	ZStrimmerU_T() {}

	virtual ~ZStrimmerU_T() {}

	template <class P>
	ZStrimmerU_T(P& iParam) : fStrim(iParam) {}

	template <class P>
	ZStrimmerU_T(const P& iParam) : fStrim(iParam) {}

// From ZStrimmerU
	virtual const ZStrimU& GetStrimU() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	Strim_p fStrim;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerU_FT

template <class Strim_p>
class ZStrimmerU_FT : public virtual ZStrimmerU
	{
protected:
	ZStrimmerU_FT() {}

public:
	virtual ~ZStrimmerU_FT() {}

	template <class P>
	ZStrimmerU_FT(const P& iParam, ZRef<ZStrimmerR> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimR())
		{}

	ZStrimmerU_FT(ZRef<ZStrimmerR> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iStrimmer->GetStrimR())
		{}

	template <class P>
	ZStrimmerU_FT(const P& iParam, ZRef<ZStrimmerU> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimU())
		{}

	ZStrimmerU_FT(ZRef<ZStrimmerU> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iStrimmer->GetStrimU())
		{}

// From ZStrimmerR via ZStrimmerU
	virtual const ZStrimR& GetStrimR() { return fStrim; }

// From ZStrimmerU
	virtual const ZStrimU& GetStrimU() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	ZRef<ZStrimmerR> fStrimmerReal;
	Strim_p fStrim;
	};

template <class Strim_p, class Strimmer_p>
ZRef<ZStrimmerU> sStrimmerU_FT(const ZRef<Strimmer_p>& iStrimmer)
	{
	if (iStrimmer)
		return new ZStrimmerU_FT<Strim_p>(iStrimmer);
	return null;
	}

template <class Strim_p, class Param_p, class Strimmer_p>
ZRef<ZStrimmerU> sStrimmerU_FT(const Param_p& iParam, const ZRef<Strimmer_p>& iStrimmer)
	{
	if (iStrimmer)
		return new ZStrimmerU_FT<Strim_p>(iParam, iStrimmer);
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerW_T

template <class Strim_p>
class ZStrimmerW_T : public virtual ZStrimmerW
	{
public:
	ZStrimmerW_T() {}

	virtual ~ZStrimmerW_T() {}

	template <class P>
	ZStrimmerW_T(P& iParam) : fStrim(iParam) {}

	template <class P>
	ZStrimmerW_T(const P& iParam) : fStrim(iParam) {}

// From ZStrimmerW
	virtual const ZStrimW& GetStrimW() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	Strim_p fStrim;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerW_FT

template <class Strim_p>
class ZStrimmerW_FT : public virtual ZStrimmerW
	{
protected:
	ZStrimmerW_FT() {}

public:
	virtual ~ZStrimmerW_FT() {}

	template <class P>
	ZStrimmerW_FT(P& iParam, ZRef<ZStrimmerW> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimW())
		{}

	template <class P>
	ZStrimmerW_FT(const P& iParam, ZRef<ZStrimmerW> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iParam, iStrimmer->GetStrimW())
		{}

	ZStrimmerW_FT(ZRef<ZStrimmerW> iStrimmer)
	:	fStrimmerReal(iStrimmer),
		fStrim(iStrimmer->GetStrimW())
		{}

// From ZStrimmerW
	virtual const ZStrimW& GetStrimW() { return fStrim; }

// Our protocol
	Strim_p& GetStrim() { return fStrim; }

protected:
	ZRef<ZStrimmerW> fStrimmerReal;
	Strim_p fStrim;
	};

} // namespace ZooLib

#endif // __ZStrimmer_h__
