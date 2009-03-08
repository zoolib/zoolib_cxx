/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZYad_Bencode__
#define __ZYad_Bencode__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Bencode

class ZYadParseException_Bencode : public ZYadParseException
	{
public:
	ZYadParseException_Bencode(const std::string& iWhat);
	ZYadParseException_Bencode(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_Bencode

class ZYadR_Bencode : public virtual ZYadR
	{
public:
	// Our protocol
	virtual void Finish() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Bencode

class ZYadPrimR_Bencode
:	public ZYadR_TValue,
	public ZYadPrimR,
	public ZYadR_Bencode
	{
public:
	ZYadPrimR_Bencode(const ZTValue& iTV);

// From ZYadR_Bencode
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_Bencode

class ZYadRawR_Bencode
:	public ZYadRawRPos_MemoryBlock,
	public ZYadR_Bencode
	{
public:
	ZYadRawR_Bencode(const ZMemoryBlock& iMB);

// From ZYadR_Bencode
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_Bencode

class ZYadListR_Bencode
:	public ZYadListR,
	public ZYadR_Bencode
	{
public:
	ZYadListR_Bencode(const ZStreamU& iStreamU);

// From ZYadR_Bencode
	virtual void Finish();

// From ZYadR via ZYadListR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR
	virtual size_t GetPosition();

private:
	void pMoveIfNecessary();

	const ZStreamU& fStreamU;
	size_t fPosition;
	bool fFinished;
	ZRef<ZYadR_Bencode> fValue_Current;
	ZRef<ZYadR_Bencode> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Bencode

class ZYadMapR_Bencode
:	public ZYadMapR,
	public ZYadR_Bencode
	{
public:
	ZYadMapR_Bencode(const ZStreamU& iStreamU);

// From ZYadR_Bencode
	virtual void Finish();

// From ZYadR via ZYadMapR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadMapR
	virtual std::string Name();

private:
	void pMoveIfNecessary();

	const ZStreamU& fStreamU;
	std::string fName;
	bool fFinished;
	ZRef<ZYadR_Bencode> fValue_Current;
	ZRef<ZYadR_Bencode> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_Bencode

namespace ZYad_Bencode {

ZRef<ZYadR> sMakeYadR(const ZStreamU& iStreamU);

} // namespace ZYad_Bencode

NAMESPACE_ZOOLIB_END

#endif // __ZYad_Bencode__
