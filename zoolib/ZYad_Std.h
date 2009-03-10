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

#ifndef __ZYad_Std__
#define __ZYad_Std__ 1
#include "zconfig.h"

#include "zoolib/ZYad_ZooLib.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Std

class ZYadParseException_Std : public ZYadParseException
	{
public:
	ZYadParseException_Std(const std::string& iWhat);
	ZYadParseException_Std(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR_Std

class ZYadR_Std : public virtual ZYadR
	{
public:
	// Our protocol
	virtual void Finish() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Std

class ZYadPrimR_Std
:	public ZYadR_TValue,
	public ZYadPrimR,
	public ZYadR_Std
	{
public:
	ZYadPrimR_Std(const ZTValue& iTV);

// From ZYadR_Std
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_Std

class ZYadRawR_Std
:	public ZYadRawRPos_MemoryBlock,
	public ZYadR_Std
	{
public:
	ZYadRawR_Std(const ZMemoryBlock& iMB);

// From ZYadR_Std
	virtual void Finish();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_Std

class ZYadListR_Std
:	public ZYadListR,
	public ZYadR_Std
	{
public:
	ZYadListR_Std();

// From ZYadR_Std
	virtual void Finish();

// From ZYadR via ZYadListR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadListR
	virtual size_t GetPosition();

// Our protocol
	virtual void Imp_Advance(bool iIsFirst, ZRef<ZYadR_Std>& oYadR) = 0;

private:
	void pMoveIfNecessary();

	size_t fPosition;
	bool fStarted;
	bool fFinished;
	ZRef<ZYadR_Std> fValue_Current;
	ZRef<ZYadR_Std> fValue_Prior;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Std

class ZYadMapR_Std
:	public ZYadMapR,
	public ZYadR_Std
	{
public:
	ZYadMapR_Std();

// From ZYadR_Std
	virtual void Finish();

// From ZYadR via ZYadMapR
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();

// From ZYadMapR
	virtual std::string Name();

// Our protocol
	virtual void Imp_Advance(bool iIsFirst, std::string& oName, ZRef<ZYadR_Std>& oYadR) = 0;

private:
	void pMoveIfNecessary();

	std::string fName;
	bool fStarted;
	bool fFinished;
	ZRef<ZYadR_Std> fValue_Current;
	ZRef<ZYadR_Std> fValue_Prior;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad_Std__
