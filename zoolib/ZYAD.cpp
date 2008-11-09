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

#include "zoolib/ZYAD.h"
#include "zoolib/ZMemoryBlock.h"

using std::string;

/*
YAD is Yet Another Data. It provides a suite of facilities for accessing data that looks like
ZooLib ZTuple suite -- CFDictionary, NSDictionary, PList, XMLRPC, JSON, Javascript types etc.

The idea is that there are a Map-like and List-like entities in many APIs, and that abstracting
access to them allows code to be applied to any of them.

*/

// =================================================================================================
#pragma mark -
#pragma mark * ZYADOptions

ZYADOptions::ZYADOptions(bool iDoIndentation)
:	fRawChunkSize(16),
	fRawByteSeparator(" "),
	fRawAsASCII(iDoIndentation),
	fBreakStrings(true),
	fStringLineLength(80),
	fIDsHaveDecimalVersionComment(iDoIndentation),
	fTimesHaveUserLegibleComment(true)
	{
	if (iDoIndentation)
		{
		fEOLString = "\n";
		fIndentString = "  ";
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADParseException

ZYADParseException::ZYADParseException(const string& iWhat)
:	runtime_error(iWhat)
	{}

ZYADParseException::ZYADParseException(const char* iWhat)
:	runtime_error(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD

ZYAD::ZYAD()
	{}

ZYAD::~ZYAD()
	{}

bool ZYAD::GetTValue(ZTValue& oYalue)
	{
	return false;
	}

ZTValue ZYAD::GetTValue()
	{
	ZTValue result;
	if (this->GetTValue(result))
		return result;
	ZUnimplemented();
	return ZTValue();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_ZTValue

ZYAD_ZTValue::ZYAD_ZTValue(const ZTValue& iTV)
:	fTV(iTV)
	{}

ZYAD_ZTValue::ZYAD_ZTValue(ZType iType, const ZStreamR& iStreamR)
:	fTV(iType, iStreamR)
	{}

ZYAD_ZTValue::~ZYAD_ZTValue()
	{}

bool ZYAD_ZTValue::GetTValue(ZTValue& oTV)
	{
	oTV = fTV;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReader

ZYADReader::ZYADReader()
	{}

ZYADReader::ZYADReader(ZRef<ZYADReaderRep> iRep)
:	fRep(iRep)
	{}

ZYADReader::ZYADReader(ZYADReaderRep* iRep)
:	fRep(iRep)
	{}

ZYADReader::~ZYADReader()
	{}

ZYADReader::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

ZType ZYADReader::Type() const
	{
	if (fRep)
		return fRep->Type();
	return eZType_Null;
	}

bool ZYADReader::IsMap() const
	{ return this->Type() == eZType_Tuple; }

bool ZYADReader::IsList() const
	{ return this->Type() == eZType_Vector; }

bool ZYADReader::IsRaw() const
	{ return this->Type() == eZType_Raw; }

bool ZYADReader::IsOther() const
	{
	switch (this->Type())
		{
		case eZType_Tuple:
		case eZType_Vector:
		case eZType_Raw:
			return false;
		}
	return true;
	}

ZMapReader ZYADReader::ReadMap()
	{
	if (fRep)
		return fRep->ReadMap();
	return ZMapReader();
	}

ZListReader ZYADReader::ReadList()
	{
	if (fRep)
		return fRep->ReadList();
	return ZListReader();
	}

ZRef<ZStreamerR> ZYADReader::ReadRaw()
	{
	if (fRep)
		return fRep->ReadRaw();
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReader::ReadYAD()
	{
	if (fRep)
		return fRep->ReadYAD();
	return ZRef<ZYAD>();	
	}

void ZYADReader::Skip()
	{
	if (fRep)
		fRep->Skip();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep

ZYADReaderRep::ZYADReaderRep()
	{}

ZYADReaderRep::~ZYADReaderRep()
	{}

void ZYADReaderRep::Skip()
	{
	switch (this->Type())
		{
		case eZType_Tuple:
			{
			this->ReadMap()->Skip();
			break;
			}
		case eZType_Vector:
			{
			this->ReadList()->Skip();
			break;
			}
		case eZType_Raw:
			{
			this->ReadRaw()->GetStreamR().SkipAll();
			break;
			}
		default:
			{
			this->ReadYAD();
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReader

ZMapReader::ZMapReader()
	{}

ZMapReader::ZMapReader(ZRef<ZMapReaderRep> iRep)
:	fRep(iRep)
	{}

ZMapReader::ZMapReader(ZMapReaderRep* iRep)
:	fRep(iRep)
	{}

ZMapReader::~ZMapReader()
	{}

ZMapReader::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

string ZMapReader::Name() const
	{
	if (fRep)
		return fRep->Name();
	return string();
	}

ZYADReader ZMapReader::Read()
	{
	if (fRep)
		return fRep->Read();
	return ZYADReader();
	}

void ZMapReader::Skip()
	{
	if (fRep)
		return fRep->Skip();
	}

bool ZMapReader::IsSimple(const ZYADOptions& iOptions) const
	{
	if (fRep)
		return fRep->IsSimple(iOptions);
	return true;
	}

bool ZMapReader::CanRandomAccess() const
	{
	if (fRep)
		return fRep->CanRandomAccess();
	return false;
	}

size_t ZMapReader::Count() const
	{
	if (fRep)
		return fRep->Count();
	return 0;
	}
	
ZYADReader ZMapReader::ReadWithName(const std::string& iName) const
	{
	if (fRep)
		return fRep->ReadWithName(iName);
	return ZYADReader();
	}

ZYADReader ZMapReader::ReadAtIndex(size_t iIndex) const
	{
	if (fRep)
		return fRep->ReadAtIndex(iIndex);
	return ZYADReader();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep

ZMapReaderRep::ZMapReaderRep()
	{}

ZMapReaderRep::~ZMapReaderRep()
	{}

void ZMapReaderRep::Skip()
	{
	while (this->HasValue())
		this->Read()->Skip();
	}

bool ZMapReaderRep::IsSimple(const ZYADOptions& iOptions)
	{ return false; }

bool ZMapReaderRep::CanRandomAccess()
	{ return false; }

size_t ZMapReaderRep::Count()
	{ return 0; }

ZRef<ZYADReaderRep> ZMapReaderRep::ReadWithName(const std::string& iName)
	{ return ZRef<ZYADReaderRep>(); }

ZRef<ZYADReaderRep> ZMapReaderRep::ReadAtIndex(size_t iIndex)
	{ return ZRef<ZYADReaderRep>(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZListReader

ZListReader::ZListReader()
	{}

ZListReader::ZListReader(ZRef<ZListReaderRep> iRep)
:	fRep(iRep)
	{}

ZListReader::ZListReader(ZListReaderRep* iRep)
:	fRep(iRep)
	{}

ZListReader::~ZListReader()
	{}

ZListReader::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep && fRep->HasValue()); }

size_t ZListReader::Index() const
	{
	if (fRep)
		return fRep->Index();
	return 0;
	}

ZYADReader ZListReader::Read()
	{
	if (fRep)
		return fRep->Read();
	return ZYADReader();
	}

void ZListReader::Skip()
	{
	if (fRep)
		return fRep->Skip();
	}

bool ZListReader::IsSimple(const ZYADOptions& iOptions) const
	{
	if (fRep)
		return fRep->IsSimple(iOptions);
	return true;
	}

bool ZListReader::CanRandomAccess() const
	{
	if (fRep)
		return fRep->CanRandomAccess();
	return false;
	}

size_t ZListReader::Count() const
	{
	if (fRep)
		return fRep->Count();
	return 0;
	}

ZYADReader ZListReader::ReadWithIndex(size_t iIndex)
	{
	if (fRep)
		return fRep->ReadWithIndex(iIndex);
	return ZYADReader();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep

ZListReaderRep::ZListReaderRep()
	{}

ZListReaderRep::~ZListReaderRep()
	{}

bool ZListReaderRep::IsSimple(const ZYADOptions& iOptions)
	{ return false; }

bool ZListReaderRep::CanRandomAccess()
	{ return false; }

size_t ZListReaderRep::Count()
	{ return 0; }

ZRef<ZYADReaderRep> ZListReaderRep::ReadWithIndex(size_t iIndex)
	{ return ZRef<ZYADReaderRep>(); }

void ZListReaderRep::Skip()
	{
	while (this->HasValue())
		this->Read()->Skip();
	}

