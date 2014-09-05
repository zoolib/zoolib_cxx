/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZStrimmerFromStrim.h"
#include "zoolib/ZStrim_Indirect.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStrimmerFromStrimR

class ZStrimmerFromStrimR::Strimmer : public ZStrimmerR, public ZStrimR_Indirect
	{
public:
	Strimmer(const ZStrimR& iStrim) : ZStrimR_Indirect(&iStrim) {}
	virtual const ZStrimR& GetStrimR() { return *this; }
	};

ZStrimmerFromStrimR::ZStrimmerFromStrimR(const ZStrimR& iStrim)
:	fStrimmer(new Strimmer(iStrim))
	{}

ZStrimmerFromStrimR::~ZStrimmerFromStrimR()
	{ fStrimmer->Set(nullptr); }

ZStrimmerFromStrimR::operator ZRef<ZStrimmerR>()
	{ return fStrimmer; }

// =================================================================================================
// MARK: - ZStrimmerFromStrimU

class ZStrimmerFromStrimU::Strimmer : public ZStrimmerU, public ZStrimU_Indirect
	{
public:
	Strimmer(const ZStrimU& iStrim) : ZStrimU_Indirect(&iStrim) {}
	virtual const ZStrimU& GetStrimU() { return *this; }
	};

ZStrimmerFromStrimU::ZStrimmerFromStrimU(const ZStrimU& iStrim)
:	fStrimmer(new Strimmer(iStrim))
	{}

ZStrimmerFromStrimU::~ZStrimmerFromStrimU()
	{ fStrimmer->Set(nullptr); }

ZStrimmerFromStrimU::operator ZRef<ZStrimmerU>()
	{ return fStrimmer; }

// =================================================================================================
// MARK: - ZStrimmerFromStrimW

class ZStrimmerFromStrimW::Strimmer : public ZStrimmerW, public ZStrimW_Indirect
	{
public:
	Strimmer(const ZStrimW& iStrim) : ZStrimW_Indirect(&iStrim) {}
	virtual const ZStrimW& GetStrimW() { return *this; }
	};

ZStrimmerFromStrimW::ZStrimmerFromStrimW(const ZStrimW& iStrim)
:	fStrimmer(new Strimmer(iStrim))
	{}

ZStrimmerFromStrimW::~ZStrimmerFromStrimW()
	{ fStrimmer->Set(nullptr); }

ZStrimmerFromStrimW::operator ZRef<ZStrimmerW>()
	{ return fStrimmer; }

} // namespace ZooLib
