/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZYad__
#define __ZYad__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadOptions

struct ZYadOptions
	{
	ZYadOptions(bool iDoIndentation = false);

	std::string fEOLString;
	std::string fIndentString;

	int fRawChunkSize;
	std::string fRawByteSeparator;
	bool fRawAsASCII;

	bool fBreakStrings;
	int fStringLineLength;

	bool fIDsHaveDecimalVersionComment;

	bool fTimesHaveUserLegibleComment;

	bool DoIndentation() const { return !fIndentString.empty(); }
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException

class ZYadParseException : public std::runtime_error
	{
public:
	ZYadParseException(const std::string& iWhat);
	ZYadParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadR

class ZYadR : public virtual ZRefCountedWithFinalization
	{
protected:
	ZYadR();

public:
	virtual bool HasChild() = 0;
	virtual ZRef<ZYadR> NextChild() = 0;

	virtual void Skip();
	virtual void SkipAll();

	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR

class ZYadPrimR : public virtual ZYadR
	{
public:
	virtual bool HasChild();
	virtual ZRef<ZYadR> NextChild();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR

class ZYadRawR
:	public virtual ZYadPrimR,
	public virtual ZStreamerR
	{
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR

class ZYadListR : public virtual ZYadR
	{
public:
	virtual uint64 GetPosition() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListRPos

class ZYadListRPos : public virtual ZYadListR
	{
public:
// Implementation of optional ZYadR methods
	virtual bool IsSimple(const ZYadOptions& iOptions);
	virtual void Skip();
	virtual void SkipAll();

// Our protocol
	virtual uint64 GetSize() = 0;
	virtual void SetPosition(uint64 iPosition) = 0;
	virtual ZRef<ZYadListRPos> ListClone() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR

class ZYadMapR : public virtual ZYadR
	{
public:
	virtual std::string Name() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapRPos

class ZYadMapRPos : public virtual ZYadMapR
	{
public:
// Implementation of optional ZYadR method
	virtual bool IsSimple(const ZYadOptions& iOptions);

// Our protocol
	virtual void SetPosition(const std::string& iName) = 0;
	virtual ZRef<ZYadMapRPos> MapClone() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapR

class ZYadListMapR
:	public virtual ZYadListR,
	public virtual ZYadMapR
	{
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos

class ZYadListMapRPos
:	public virtual ZYadListMapR,
	public virtual ZYadListRPos,
	public virtual ZYadMapRPos
	{
public:
// Disambiguating overrides from ZYadMapRPos and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);
	virtual void SetPosition(uint64 iPosition);
	virtual void SetPosition(const std::string& iName);
	
// Default implementation calling through to ListMapClone
	virtual ZRef<ZYadListRPos> ListClone();
	virtual ZRef<ZYadMapRPos> MapClone();

// Our protocol
	virtual ZRef<ZYadListMapRPos> ListMapClone() = 0;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZYad__
