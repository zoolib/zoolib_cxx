/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZUtil_TS.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZStream_Buffered.h"
#include "zoolib/ZTextCoder_Unicode.h"
#include "zoolib/ZStrimU_Std.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZUtil_Strim_Tuple.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

using std::map;
using std::pair;
using std::runtime_error;
using std::string;

namespace ZooLib {

namespace ZUtil_TS {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS exceptions

Ex_Base::Ex_Base(const char* what)
:	runtime_error(what)
	{}

Ex_Base::Ex_Base(const std::string& what)
:	runtime_error(what)
	{}

Ex_MagicTextMissing::Ex_MagicTextMissing()
:	Ex_Base("Magic Text Missing")
	{}

Ex_IDInvalid::Ex_IDInvalid()
:	Ex_Base("ID Invalid")
	{}

Ex_IDOutOfSequence::Ex_IDOutOfSequence()
:	Ex_Base("ID Out Of Sequence")
	{}

Ex_IDOutOfBounds::Ex_IDOutOfBounds()
:	Ex_Base("ID Out Of Bounds")
	{}

Ex_IDDuplicate::Ex_IDDuplicate()
:	Ex_Base("ID Duplicate")
	{}

Ex_OversizedStream::Ex_OversizedStream()
:	Ex_Base("Oversized Stream")
	{}

Ex_MalformedText::Ex_MalformedText(const char* what)
:	Ex_Base(what)
	{}

Ex_MalformedText::Ex_MalformedText(const std::string& what)
:	Ex_Base(what)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Source_Map

Source_Map::Source_Map(const map<uint64, ZTuple>& iMap)
:	fIter(iMap.begin()),
	fEnd(iMap.end())
	{}

bool Source_Map::Get(uint64& oID, ZTuple& oTuple)
	{
	if (fIter == fEnd)
		return false;

	oID = (*fIter).first;
	oTuple = (*fIter).second;
	++fIter;
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Sink_Map

Sink_Map::Sink_Map(map<uint64, ZTuple>& oMap)
:	fMap(oMap)
	{}

bool Sink_Map::Set(uint64 iID, const ZTuple& iTuple)
	{
	pair<map<uint64, ZTuple>::iterator, bool> result =
		fMap.insert(map<uint64, ZTuple>::value_type(iID, iTuple));
	return result.second;
	}

void Sink_Map::Clear()
	{ fMap.clear(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS

static const char spMagicText[] = "ZTS_RAM 1.0 CRLF\r\nCR\rLF\n";

void sToStream(uint64 iNextUnusedID, Source& iSource, const ZStreamWPos& iStreamWPos)
	{
	iStreamWPos.Truncate();

	iStreamWPos.Write(spMagicText, sizeof(spMagicText));

	iStreamWPos.WriteUInt64(iNextUnusedID);
	uint64 positionOfCount = iStreamWPos.GetPosition();
	iStreamWPos.WriteUInt32(0); // Dummy

	size_t theCount = 0;
	{ // Start of scope for theSWB
	ZStreamW_Buffered theSWB(64 * 1024, iStreamWPos);
	for (;;)
		{
		uint64 anID;
		ZTuple aTuple;
		if (not iSource.Get(anID, aTuple))
			break;
		theSWB.WriteUInt64(anID);
		aTuple.ToStream(theSWB);
		++theCount;
		}
	// Although theSWB's destructor will flush data to iStreamW, it will
	// absorb any exception that occurs because it is otherwise a potential
	// source of a double-exception. So we do it explicitly and if there
	// is a problem we will safely unwind.
	theSWB.Flush();
	} // End of scope for theSWB

	iStreamWPos.SetPosition(positionOfCount);
	iStreamWPos.WriteUInt32(theCount);
	iStreamWPos.SetPosition(iStreamWPos.GetSize());
	}

void sFromStream(Sink& iSink, uint64& oNextUnusedID, const ZStreamR& iStreamR)
	{
	ZStreamR_Buffered theSRB(64 * 1024, iStreamR);

	char dummy[sizeof(spMagicText)];
	size_t countRead;
	theSRB.Read(dummy, sizeof(spMagicText), &countRead);
	if (countRead != sizeof(spMagicText) || 0 != memcmp(dummy, spMagicText, sizeof(spMagicText)))
		throw Ex_MagicTextMissing();

	oNextUnusedID = theSRB.ReadUInt64();
	size_t count = theSRB.ReadUInt32();
	uint64 priorID = 0;
	while (count--)
		{
//		uint64 offset = theSRB.GetPosition();//##
		uint64 theID = theSRB.ReadUInt64();
		ZTuple theTuple(theSRB);

		if (theID == 0)
			throw Ex_IDInvalid();

		if (theID >= oNextUnusedID)
			throw Ex_IDOutOfBounds();

//		if (theID <= priorID)
//			throw Ex_IDOutOfSequence();

		if (not iSink.Set(theID, theTuple))
			throw Ex_IDDuplicate();

		priorID = theID;
		}

	if (theSRB.CountReadable() > 0)
		throw Ex_OversizedStream();
	}

void sToStrim(uint64 iNextUnusedID, Source& iSource, const ZStrimW& iStrimW)
	{
	iStrimW.Writef("// Version 1.0\n// Next unused ID: \n0x%llX /*%lld*/\n",
		iNextUnusedID, iNextUnusedID);

	ZYadOptions theOptions;
	theOptions.fEOLString = "\n\t";
	theOptions.fIndentString = "  ";
	theOptions.fRawChunkSize = 16;
	theOptions.fRawAsASCII = true;
	theOptions.fIDsHaveDecimalVersionComment = true;

	for (;;)
		{
		uint64 anID;
		ZTuple aTuple;
		if (not iSource.Get(anID, aTuple))
			break;
		iStrimW.Writef("0x%llX /*%lld*/:\t", anID, anID);
		ZYad_ZooLibStrim::sToStrim(0, theOptions, sYadR(aTuple), iStrimW);
		iStrimW.Write("\n");
		}
	}

void sFromStrim(Sink& iSink, uint64& oNextUnusedID, const ZStrimU& iStrimU)
	{
	using namespace ZUtil_Strim;

	string openingComment;
	sCopy_WSAndCPlusPlusComments(iStrimU, ZStrimW_String8(openingComment));

	if (openingComment == "/* Next unused ID: */ ")
		{
		// The opening comment is what we used when IDs were unadorned hex.
		sSkip_WSAndCPlusPlusComments(iStrimU);

		int64 signedID;
		if (!sTryRead_HexInteger(iStrimU, signedID))
			throw Ex_MalformedText("Expected next unused ID");
		oNextUnusedID = signedID;

		uint64 priorID = 0;
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_HexInteger(iStrimU, signedID))
				break;
			uint64 theID = signedID;

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_CP(iStrimU, ':'))
				throw Ex_MalformedText("Expected ':' after tuple ID");

			sSkip_WSAndCPlusPlusComments(iStrimU);

			ZQ<ZMap_Any> theTupleQ;
			ZTValue theTValue;
			if (ZUtil_Strim_Tuple::sFromStrim(iStrimU, theTValue))
				theTupleQ = theTValue.QGet<ZMap_Any>();
			if (not theTupleQ)
				throw Ex_MalformedText("Expected tuple after 'tuple ID:'");

			if (theID == 0)
				throw Ex_IDInvalid();

			if (theID >= oNextUnusedID)
				throw Ex_IDOutOfBounds();

//			if (theID <= priorID)
//				throw Ex_IDOutOfSequence();

			if (not iSink.Set(theID, *theTupleQ))
				throw Ex_IDDuplicate();

			priorID = theID;
			}
		}
	else
		{
		// There is some other opening comment, or none at all. In which
		// case we can do our more generic parsing, where IDs are generic
		// integers. If they're hex they'll be prefixed with 0x, otherwise
		// they're decimal, or some other format that may eventually be
		// supported by ZUtil_Strim::sTryRead_SignedGenericInteger

		sSkip_WSAndCPlusPlusComments(iStrimU);

		int64 signedID;
		if (!sTryRead_SignedGenericInteger(iStrimU, signedID))
			throw Ex_MalformedText("Expected next unused ID");
		oNextUnusedID = signedID;

		uint64 priorID = 0;
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_SignedGenericInteger(iStrimU, signedID))
				break;
			uint64 theID = signedID;

			sSkip_WSAndCPlusPlusComments(iStrimU);

			if (!sTryRead_CP(iStrimU, ':'))
				throw Ex_MalformedText("Expected ':' after tuple ID");

			sSkip_WSAndCPlusPlusComments(iStrimU);

			ZQ<ZMap_Any> theTupleQ;
			ZTValue theTValue;
			if (ZUtil_Strim_Tuple::sFromStrim(iStrimU, theTValue))
				theTupleQ = theTValue.QGet<ZMap_Any>();
			if (not theTupleQ)
				throw Ex_MalformedText("Expected tuple after 'tuple ID:'");


			if (theID == 0)
				throw Ex_IDInvalid();

			if (theID >= oNextUnusedID)
				throw Ex_IDOutOfBounds();

//			if (theID <= priorID)
//				throw Ex_IDOutOfSequence();

			if (not iSink.Set(theID, *theTupleQ))
				throw Ex_IDDuplicate();

			priorID = theID;
			}
		}
	}

void sRead(const ZStreamRPos& iStreamRPos, uint64& oNextUnusedID, Sink& iSink)
	{
	try
		{
		// First try reading it as binary
		iStreamRPos.SetPosition(0);
		sFromStream(iSink, oNextUnusedID, iStreamRPos);
		}
	catch (Ex_MagicTextMissing& /*ex*/)
		{
		// Reset our stream, and toss any data that might have been put into oTuples.
		iStreamRPos.SetPosition(0);
		iSink.Clear();

		// We can't inline the buffer stream and strim, because we need
		// access to theStrimU in the catch clauses below.

		ZStreamR_Buffered theSRB(64 * 1024, iStreamRPos);
		ZStrimU_Std theStrimU(new ZTextDecoder_Unicode_AutoDetect, theSRB);
		try
			{
			sFromStrim(iSink, oNextUnusedID, theStrimU);
			}
		catch (ZUtil_Strim::ParseException& ex)
			{
			string newMessage = string("ParseException '") + ex.what()
					+ "' at line " + sStringf("%zu", theStrimU.GetLine());
			throw ZUtil_Strim::ParseException(newMessage);
			}
		catch (Ex_MalformedText& ex)
			{
			string newMessage = string("Ex_MalformedText '") + ex.what()
					+ "' at line " + sStringf("%zu", theStrimU.GetLine());
			throw Ex_MalformedText(newMessage);
			}
		}
	}

void sWriteMagicText(const ZStreamW& iStreamW)
	{
	iStreamW.Write(spMagicText, sizeof(spMagicText));
	}

} // namespace ZUtil_TS

} // namespace ZooLib
