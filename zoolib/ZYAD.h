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

#ifndef __ZYAD__
#define __ZYAD__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZTuple.h"

#include <string>

class ZYADReader;
class ZMapReader;
class ZListReader;

class ZYADReaderRep;
class ZMapReaderRep;
class ZListReaderRep;

// =================================================================================================
#pragma mark -
#pragma mark * ZYADOptions

struct ZYADOptions
	{
	ZYADOptions(bool iDoIndentation = false);

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
#pragma mark * ZYADParseException

class ZYADParseException : public std::runtime_error
	{
public:
	ZYADParseException(const std::string& iWhat);
	ZYADParseException(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD

class ZYAD : public ZRefCountedWithFinalization
	{
protected:
	ZYAD();

public:
	virtual ~ZYAD();

	virtual bool GetTValue(ZTValue& oYalue);
	ZTValue GetTValue();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZTValue

class ZYAD_ZTValue : public ZYAD
	{
public:
	ZYAD_ZTValue(const ZTValue& iTV);
	ZYAD_ZTValue(ZType iType, const ZStreamR& iStreamR);
	virtual ~ZYAD_ZTValue();

	virtual bool GetTValue(ZTValue& oYalue);

private:
	const ZTValue fTV;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReader

class ZYADReader
	{
public:
	ZYADReader();
	ZYADReader(ZRef<ZYADReaderRep> iRep);
	ZYADReader(ZYADReaderRep* iRep);
	~ZYADReader();

	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZYADReader, operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const;

	ZType Type() const;

	bool IsMap() const;
	bool IsList() const;
	bool IsRaw() const;
	bool IsOther() const;

	ZMapReader ReadMap();
	ZListReader ReadList();
	ZRef<ZStreamerR> ReadRaw();
	ZRef<ZYAD> ReadYAD();

	void Skip();

private:
	ZRef<ZYADReaderRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep

class ZYADReaderRep : public ZRefCountedWithFinalization
	{
protected:
	ZYADReaderRep();

public:
	virtual ~ZYADReaderRep();

	virtual bool HasValue() = 0;
	virtual ZType Type() = 0;

	virtual ZRef<ZMapReaderRep> ReadMap() = 0;
	virtual ZRef<ZListReaderRep> ReadList() = 0;
	virtual ZRef<ZStreamerR> ReadRaw() = 0;
	virtual ZRef<ZYAD> ReadYAD() = 0;

	virtual void Skip();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReader

class ZMapReader
	{
public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZMapReader, operator_bool_generator_type, operator_bool_type);

	ZMapReader();
	ZMapReader(ZRef<ZMapReaderRep> iRep);
	ZMapReader(ZMapReaderRep* iRep);
	~ZMapReader();

	operator operator_bool_type() const;

	std::string Name() const;
	ZYADReader Read();
	void Skip();

	bool IsSimple(const ZYADOptions& iOptions) const;
	bool CanRandomAccess() const;
	size_t Count() const;
	ZYADReader ReadWithName(const std::string& iName) const;
	ZYADReader ReadAtIndex(size_t iIndex) const;

private:
	ZRef<ZMapReaderRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep

class ZMapReaderRep : public ZRefCountedWithFinalization
	{
protected:
	ZMapReaderRep();

public:
	virtual ~ZMapReaderRep();

	virtual bool HasValue() = 0;
	virtual std::string Name() = 0;
	virtual ZRef<ZYADReaderRep> Read() = 0;

	virtual void Skip();

	virtual bool IsSimple(const ZYADOptions& iOptions);
	virtual bool CanRandomAccess();
	virtual size_t Count();
	virtual ZRef<ZYADReaderRep> ReadWithName(const std::string& iName);
	virtual ZRef<ZYADReaderRep> ReadAtIndex(size_t iIndex);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReader

class ZListReader
	{
public:
	ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZListReader, operator_bool_generator_type, operator_bool_type);

	ZListReader();
	ZListReader(ZRef<ZListReaderRep> iRep);
	ZListReader(ZListReaderRep* iRep);
	~ZListReader();

	operator operator_bool_type() const;

	size_t Index() const;
	ZYADReader Read();
	void Skip();

	bool IsSimple(const ZYADOptions& iOptions) const;
	bool CanRandomAccess() const;
	size_t Count() const;
	ZYADReader ReadWithIndex(size_t iIndex);

private:
	ZRef<ZListReaderRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep

class ZListReaderRep : public ZRefCountedWithFinalization
	{
protected:
	ZListReaderRep();

public:
	virtual ~ZListReaderRep();

	virtual bool HasValue() = 0;
	virtual size_t Index() = 0;
	virtual ZRef<ZYADReaderRep> Read() = 0;

	virtual void Skip();

	virtual bool IsSimple(const ZYADOptions& iOptions);
	virtual bool CanRandomAccess();
	virtual size_t Count();
	virtual ZRef<ZYADReaderRep> ReadWithIndex(size_t iIndex);
	};

#endif // __ZYAD__
