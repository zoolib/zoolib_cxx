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
#include "zoolib/ZTuple.h"

#include <string>

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
#pragma mark * ZYad

class ZYad : public virtual ZRefCountedWithFinalization
	{
protected:
	ZYad();

public:
	virtual bool GetTValue(ZTValue& oTV) = 0;
	ZTValue GetTValue();
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

	virtual ZRef<ZYad> ReadYad() = 0;
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
public:
// Implementation of required ZYadR method
	virtual ZRef<ZYad> ReadYad();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR

class ZYadListR : public virtual ZYadR
	{
public:
// From ZYadR
	virtual ZRef<ZYad> ReadYad();

	virtual size_t GetPosition() = 0;
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
	virtual size_t GetSize() = 0;
	virtual void SetPosition(size_t iPosition) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR

class ZYadMapR : public virtual ZYadR
	{
public:
// Implementation of required ZYadR method
	virtual ZRef<ZYad> ReadYad();

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
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapR

class ZYadListMapR
:	public virtual ZYadMapR,
	public virtual ZYadListR
	{
public:
// Disambiguating override from ZYadMapR and ZYadListR
	virtual ZRef<ZYad> ReadYad();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListMapRPos

class ZYadListMapRPos
:	public ZYadListMapR,
	public ZYadMapRPos,
	public ZYadListRPos
	{
public:
// Disambiguating override from ZYadMapRPos and ZYadListRPos
	virtual bool IsSimple(const ZYadOptions& iOptions);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_TValue

class ZYad_TValue : public ZYad
	{
public:
	ZYad_TValue(const ZTValue& iTV);
	ZYad_TValue(ZType iType, const ZStreamR& iStreamR);
	virtual ~ZYad_TValue();

// From ZYad
	virtual bool GetTValue(ZTValue& oTV);

private:
	const ZTValue fTV;
	};

#endif // __ZYad__
