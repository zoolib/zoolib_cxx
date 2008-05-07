/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStrim_Tee__
#define __ZStrim_Tee__ 1
#include "zconfig.h"

#include "ZStrim.h"
#include "ZStrimmer.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Tee

/// A read filter stream that echoes any data read from it to a write stream.

class ZStrimR_Tee : public ZStrimR
	{
public:
	ZStrimR_Tee(const ZStrimR& iSource, const ZStrimW& iSink);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount);

private:
	const ZStrimR& fSource;
	const ZStrimW& fSink;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerR_Tee

/// A read filter strimmer encapsulating a ZStrimR_Tee.

class ZStrimmerR_Tee : public ZStrimmerR
	{
public:
	ZStrimmerR_Tee(ZRef<ZStrimmerR> iStrimmerSource, ZRef<ZStrimmerW> iStrimmerSink);
	virtual ~ZStrimmerR_Tee();

// From ZStrimmerR
	virtual const ZStrimR& GetStrimR();

protected:
	ZRef<ZStrimmerR> fStrimmerSource;
	ZRef<ZStrimmerW> fStrimmerSink;
	ZStrimR_Tee fStrim;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_Tee

/// A write filter strim that replicates writes made against it.

class ZStrimW_Tee : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_Tee(const ZStrimW& iSink1, const ZStrimW& iSink2);

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStrimW& fSink1;
	const ZStrimW& fSink2;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimmerW_Tee

/// A write filter strimmer encapsulating a ZStrimW_Tee.

class ZStrimmerW_Tee : public ZStrimmerW
	{
public:
	ZStrimmerW_Tee(ZRef<ZStrimmerW> iStrimmerSink1, ZRef<ZStrimmerW> iStrimmerSink2);
	virtual ~ZStrimmerW_Tee();

// From ZStrimmerW
	virtual const ZStrimW& GetStrimW();

protected:
	ZRef<ZStrimmerW> fStrimmerSink1;
	ZRef<ZStrimmerW> fStrimmerSink2;
	ZStrimW_Tee fStrim;
	};

#endif // __ZStrim_Tee__
