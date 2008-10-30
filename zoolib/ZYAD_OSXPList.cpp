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

#include "zoolib/ZMemoryBlock.h"
#include "zoolib/ZStream_AsciiStrim.h"
#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZStrimW_ML.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZYAD_OSXPList.h"
#include "zoolib/ZYAD_ZooLib.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void sBegin(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_Begin(r, iTagName))
		throw ZYAD_OSXPList::ParseException("Expected begin tag '" + iTagName + "'");
	}

static void sEnd(ZML::Reader& r, const string& iTagName)
	{
	sSkipText(r);

	if (!sTryRead_End(r, iTagName))
		throw ZYAD_OSXPList::ParseException("Expected end tag '" + iTagName + "'");
	}

static void sRead_BodyOfDict(ZML::Reader& r, ZTuple& oTuple);
void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector);

static bool sTryRead_Value(ZML::Reader& r, ZTValue& oTV)
	{
	if (r.Current() == ZML::eToken_TagEmpty)
		{
		if (r.Name() == "true")
			{
			oTV.SetBool(true);
			r.Advance();
			return true;
			}

		if (r.Name() == "false")
			{
			oTV.SetBool(false);
			r.Advance();
			return true;
			}

		return false;
		}

	// If there's no open tag, then we're not at the start of a value.
	if (r.Current() != ZML::eToken_TagBegin)
		return false;

	const string tagName = r.Name();
	r.Advance();

	// We've read and advanced past an open tag, in tagName.
	if (tagName == "dict")
		{
		sRead_BodyOfDict(r, oTV.SetMutableTuple());
		}
	else if (tagName == "integer")
		{
		int64 theInteger;
		if (!ZUtil_Strim::sTryRead_SignedDecimalInteger(ZStrimU_Unreader(r.Text()), theInteger))
			throw ZYAD_OSXPList::ParseException("Expected valid integer");

		oTV.SetInt32(theInteger);
		}
	else if (tagName == "real")
		{
		double theDouble;
		if (!ZUtil_Strim::sTryRead_SignedDouble(ZStrimU_Unreader(r.Text()), theDouble))
			throw ZYAD_OSXPList::ParseException("Expected valid real");

		oTV.SetDouble(theDouble);
		}
	else if (tagName == "string")
		{
		string theString;
		ZStrimW_String(theString).CopyAllFrom(r.Text());

		oTV.SetString(theString);
		}
	else if (tagName == "data")
		{
		ZMemoryBlock theMB;
		ZStreamR_Base64Decode(ZStreamR_ASCIIStrim(r.Text()))
			.CopyAllTo(ZStreamRWPos_MemoryBlock(theMB));

		oTV.SetRaw(theMB);
		}
	else if (tagName == "date")
		{
		string theDate;
		ZStrimW_String(theDate).CopyAllFrom(r.Text());

		oTV.SetTime(ZUtil_Time::sFromString_ISO8601(theDate));
		}
	else if (tagName == "array")
		{
		sRead_BodyOfArray(r, oTV.SetMutableVector());
		}
	else
		{
		// Hmm. Ignore tags we don't recognize?
		// throw ZYAD_OSXPList::ParseException("Invalid begin tag '" + tagName + "'");
		}

	sEnd(r, tagName);
	return true;
	}

static void sRead_Value(ZML::Reader& r, ZTValue& oValue)
	{
	sSkipText(r);

	if (!sTryRead_Value(r, oValue))
		throw ZYAD_OSXPList::ParseException("Expected value");
	}

static void sRead_BodyOfDict(ZML::Reader& r, ZTuple& oTuple)
	{
	ZRef<ZTupleRep> theRep = new ZTupleRep;
	ZTuple::PropList& theProperties = theRep->fProperties;

	// We've consumed a <dict> tag, and are pointing at any text following it.
	for (;;)
		{
		sSkipText(r);

		if (!sTryRead_Begin(r, "key"))
			break;

		string propertyName;
		ZStrimW_String(propertyName).CopyAllFrom(r.Text());

		sEnd(r, "key");

		ZTValue theTV;
		sRead_Value(r, theTV);

		theProperties.push_back(ZTuple::NameTV(ZTName(propertyName), theTV));
		}

	oTuple = ZTuple(theRep);
	}

void sRead_BodyOfArray(ZML::Reader& r, vector<ZTValue>& oVector)
	{
	for (;;)
		{
		sSkipText(r);

		ZTValue theTV;
		if (!sTryRead_Value(r, theTV))
			break;
		oVector.push_back(theTV);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_OSXPList

ZYAD_OSXPList::ZYAD_OSXPList(ZML::Reader& iR)
	{
	sRead_Value(iR, fTV);
	}

ZYAD_OSXPList::~ZYAD_OSXPList()
	{}

bool ZYAD_OSXPList::GetTValue(ZTValue& oTV)
	{
	oTV = fTV;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYAD_OSXPList::ParseException

ZYAD_OSXPList::ParseException::ParseException(const string& iWhat)
:	ZYAD::ParseException(iWhat)
	{}

ZYAD_OSXPList::ParseException::ParseException(const char* iWhat)
:	ZYAD::ParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_OSXPList declaration

class ZMapReaderRep_OSXPList : public ZMapReaderRep
	{
public:
	ZMapReaderRep_OSXPList(ZML::Reader& iReader);

	virtual bool HasValue();
	virtual std::string Name();
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	void pReadNameIfNecessary();

	ZML::Reader& fR;
	bool fFinished;
	bool fNeedsReadName;
	string fName;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_OSXPList declaration

class ZListReaderRep_OSXPList : public ZListReaderRep
	{
public:
	ZListReaderRep_OSXPList(ZML::Reader& iReader);

	virtual bool HasValue();
	virtual size_t Index() const;
	virtual ZRef<ZYADReaderRep> Read();
	virtual void Skip();

private:
	void pMoveIfNecessary();

	ZML::Reader& fR;
	bool fFinished;
	bool fNeedsMove;
	size_t fIndex;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_OSXPlist

class ZStreamerR_OSXPlist
:	public ZStreamerR
	{
public:
	ZStreamerR_OSXPlist(ZML::Reader& iR);
	virtual ~ZStreamerR_OSXPlist();
	
// From ZStreamerR
	const ZStreamR& GetStreamR();

	ZML::Reader& fR;
	ZStreamR_ASCIIStrim fStreamR_ASCIIStrim;
	ZStreamR_Base64Decode fStreamR_Base64Decode;
	};

ZStreamerR_OSXPlist::ZStreamerR_OSXPlist(ZML::Reader& iR)
:	fR(iR),
	fStreamR_ASCIIStrim(fR.Text()),
	fStreamR_Base64Decode(fStreamR_ASCIIStrim)
	{
	sBegin(fR, "data");
	}

ZStreamerR_OSXPlist::~ZStreamerR_OSXPlist()
	{
	sEnd(fR, "data");
	}

const ZStreamR& ZStreamerR_OSXPlist::GetStreamR()
	{ return fStreamR_Base64Decode; }

// =================================================================================================
#pragma mark -
#pragma mark * ZYADReaderRep_OSXPList definition

void ZYADReaderRep_OSXPList::pMove()
	{
	sSkipText(fR);
	}

ZYADReaderRep_OSXPList::ZYADReaderRep_OSXPList(ZML::Reader& iReader)
:	fR(iReader)
	{
	this->pMove();	
	}

bool ZYADReaderRep_OSXPList::HasValue()
	{
	return fR.Current() == ZML::eToken_TagEmpty || fR.Current() == ZML::eToken_TagBegin;
	}

ZType ZYADReaderRep_OSXPList::Type()
	{
	if (fR.Current() == ZML::eToken_TagEmpty)
		{
		if (fR.Name() == "true")
			return eZType_Bool;
		if (fR.Name() == "false")
			return eZType_Bool;
		}
	else if (fR.Current() == ZML::eToken_TagBegin)
		{
		if (fR.Name() == "dict")
			return eZType_Tuple;
		else if (fR.Name() == "array")
			return eZType_Vector;
		else if (fR.Name() == "integer")
			return eZType_Int64;
		else if (fR.Name() == "real")
			return eZType_Double;
		else if (fR.Name() == "string")
			return eZType_String;
		else if (fR.Name() == "data")
			return eZType_Raw;
		else if (fR.Name() == "date")
			return eZType_Time;
		}
	return eZType_Null;
	}

ZRef<ZMapReaderRep> ZYADReaderRep_OSXPList::ReadMap()
	{
	try
		{
		return new ZMapReaderRep_OSXPList(fR);
		}
	catch (...)
		{}
	return ZRef<ZMapReaderRep>();
	}

ZRef<ZListReaderRep> ZYADReaderRep_OSXPList::ReadList()
	{
	try
		{
		return new ZListReaderRep_OSXPList(fR);
		}
	catch (...)
		{}
	return ZRef<ZListReaderRep>();
	}

ZRef<ZStreamerR> ZYADReaderRep_OSXPList::ReadRaw()
	{
	try
		{
		return new ZStreamerR_OSXPlist(fR);
		}
	catch (...)
		{}
	return ZRef<ZStreamerR>();
	}

ZRef<ZYAD> ZYADReaderRep_OSXPList::ReadYAD()
	{
	try
		{
		return new ZYAD_OSXPList(fR);
		}
	catch (...)
		{}
	return ZRef<ZYAD>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMapReaderRep_OSXPList definition

void ZMapReaderRep_OSXPList::pReadNameIfNecessary()
	{
	if (fFinished)
		return;

	if (!fNeedsReadName)
		return;

	fNeedsReadName = false;

	sSkipText(fR);

	if (fR.Current() == ZML::eToken_TagEnd && fR.Name() == "dict")
		{
		fFinished = true;
		fR.Advance();
		return;
		}

	sBegin(fR, "key");
	fName.clear();
	ZStrimW_String(fName).CopyAllFrom(fR.Text());
	sEnd(fR, "key");
	sSkipText(fR);
	}

ZMapReaderRep_OSXPList::ZMapReaderRep_OSXPList(ZML::Reader& iR)
:	fR(iR),
	fFinished(false),
	fNeedsReadName(true)
	{
	sBegin(fR, "dict");
	this->pReadNameIfNecessary();
	}

bool ZMapReaderRep_OSXPList::HasValue()
	{
	this->pReadNameIfNecessary();

	return !fFinished;
	}

string ZMapReaderRep_OSXPList::Name()
	{
	this->pReadNameIfNecessary();

	if (!fFinished)
		return fName;
	return string();
	}

ZRef<ZYADReaderRep> ZMapReaderRep_OSXPList::Read()
	{
	this->pReadNameIfNecessary();

	if (!fFinished)
		{
		fNeedsReadName = true;
		return new ZYADReaderRep_OSXPList(fR);
		}
	return ZRef<ZYADReaderRep>();
	}

void ZMapReaderRep_OSXPList::Skip()
	{
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZListReaderRep_OSXPList definition

void ZListReaderRep_OSXPList::pMoveIfNecessary()
	{
	if (fFinished)
		return;

	if (!fNeedsMove)
		return;

	fNeedsMove = false;

	sSkipText(fR);

	if (fR.Current() == ZML::eToken_TagEnd && fR.Name() == "array")
		{
		fFinished = true;
		fR.Advance();
		}
	}

ZListReaderRep_OSXPList::ZListReaderRep_OSXPList(ZML::Reader& iR)
:	fR(iR),
	fFinished(false),
	fNeedsMove(true),
	fIndex(0)
	{
	sBegin(fR, "array");
	this->pMoveIfNecessary();
	}

bool ZListReaderRep_OSXPList::HasValue()
	{
	this->pMoveIfNecessary();

	return !fFinished;
	}

size_t ZListReaderRep_OSXPList::Index() const
	{
	return fIndex;
	}

ZRef<ZYADReaderRep> ZListReaderRep_OSXPList::Read()
	{
	this->pMoveIfNecessary();

	if (!fFinished)
		{		
		++fIndex;
		fNeedsMove = true;
		return new ZYADReaderRep_OSXPList(fR);
		}
	return ZRef<ZYADReaderRep>();
	}

void ZListReaderRep_OSXPList::Skip()
	{
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYADUtil_OSXPList

static void sToStrimW_ML(const ZStrimW_ML& s, ZMapReader iMapReader)
	{
	s.Begin("dict");
	while (iMapReader)
		{
		s.Begin("key");
			s << iMapReader.Name();
		s.End("key");
		ZYADUtil_OSXPList::sToStrimW_ML(s, iMapReader.Read());
		}
	s.End("dict");
	}

static void sToStrimW_ML(const ZStrimW_ML& s, ZListReader iListReader)
	{
	s.Begin("array");
	while (iListReader)
		ZYADUtil_OSXPList::sToStrimW_ML(s, iListReader.Read());
	s.End("array");
	}

static void sToStrimW_ML(const ZStrimW_ML& s, const ZStreamR& iStreamR)
	{
	s.Begin("data");
		iStreamR.CopyAllTo(ZStreamW_Base64Encode(ZStreamW_ASCIIStrim(s)));
	s.End("data");	
	}

static void sToStrimW_ML(const ZStrimW_ML& s, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Tuple:
			{
			sToStrimW_ML(s, new ZMapReaderRep_ZooLib(iTV.GetTuple()));
			break;
			}
		case eZType_Vector:
			{
			sToStrimW_ML(s, new ZListReaderRep_ZooLib(iTV.GetVector()));
			break;
			}
		case eZType_String:
			{
			s.Begin("string");
				s.Write(iTV.GetString());
			s.End();
			break;
			}
		case eZType_Int32:
			{
			s.Begin("integer");
				s.Writef("%d", iTV.GetInt32());
			s.End("integer");
			break;
			}
		case eZType_Double:
			{
			s.Begin("real");
				s.Writef("%.17g", iTV.GetDouble());
			s.End("real");
			break;
			}
		case eZType_Bool:
			{
			if (iTV.GetBool())
				s.Empty("true");
			else
				s.Empty("false");
			break;
			}
		case eZType_Raw:
			{
			sToStrimW_ML(s, ZStreamRPos_MemoryBlock(iTV.GetRaw()));
			}
		case eZType_Time:
			{
			s.Begin("date");
				s << ZUtil_Time::sAsString_ISO8601(iTV.GetTime(), true);
			s.End("date");
			}
		}
	}

static void sToStrimW_ML(const ZStrimW_ML& s, ZRef<ZYAD> iYAD)
	{
	sToStrimW_ML(s, iYAD->GetTValue());
	}

void ZYADUtil_OSXPList::sToStrimW_ML(const ZStrimW_ML& s, ZYADReader iYADReader)
	{
	if (iYADReader)
		{
		if (iYADReader.IsMap())
			{
			sToStrimW_ML(s, iYADReader.ReadMap());
			}
		else if (iYADReader.IsList())
			{
			sToStrimW_ML(s, iYADReader.ReadList());
			}
		else if (iYADReader.IsRaw())
			{
			sToStrimW_ML(s, iYADReader.ReadRaw()->GetStreamR());
			}
		else
			{
			sToStrimW_ML(s, iYADReader.ReadYAD());
			}
		}
	}
