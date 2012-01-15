/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZWebDAV.h"

#include "zoolib/ZHTTP.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMIME.h"
#include "zoolib/ZML.h"
#include "zoolib/ZStream_Buffered.h"
#include "zoolib/ZStream_POSIX.h"
#include "zoolib/ZStream_String.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimW_ML.h"
#include "zoolib/ZStrimmer.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZString.h"
#include "zoolib/ZTuple.h"
#include "zoolib/ZUtil_Time.h"
#include "zoolib/ZUtil_Strim_Tuple.h"

namespace ZooLib {

using std::pair;
using std::runtime_error;
using std::string;
using std::vector;

#define kDebug_WebDAV 1

// =================================================================================================
// MARK: - DAVIter

namespace { // anonymous

class DAVIter
	{
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(DAVIter, operator_bool_generator_type, operator_bool_type);
public:
	DAVIter(const ZNode& iNode, int iDepth);
	~DAVIter();

	operator operator_bool_type() const;
	DAVIter& Advance();
	ZNode Current();

private:
	ZNode iRoot;
	ZNodeIter* fNodeIter;
	ZNodeTreeIter* fNodeTreeIter;
	};

DAVIter::DAVIter(const ZNode& iNode, int iDepth)
:	iRoot(iNode),
	fNodeIter(nullptr),
	fNodeTreeIter(nullptr)
	{
	if (iDepth == 0)
		{}
	else if (iDepth == 1)
		fNodeIter = new ZNodeIter(iRoot);
	else
		fNodeTreeIter = new ZNodeTreeIter(iRoot);
	}

DAVIter::~DAVIter()
	{
	delete fNodeIter;
	delete fNodeTreeIter;
	}

DAVIter::operator operator_bool_type() const
	{
	if (iRoot || fNodeIter && *fNodeIter || fNodeTreeIter && *fNodeTreeIter)
		return operator_bool_generator_type::translate(true);

	return operator_bool_generator_type::translate(false);
	}

DAVIter& DAVIter::Advance()
	{
	if (iRoot)
		iRoot = ZNode();
	else if (fNodeIter)
		fNodeIter->Advance();
	else if (fNodeTreeIter)
		fNodeTreeIter->Advance();
	return *this;
	}

ZNode DAVIter::Current()
	{
	if (iRoot)
		return iRoot;

	if (fNodeIter)
		return fNodeIter->Current();

	if (fNodeTreeIter)
		return fNodeTreeIter->Current();

	return ZNode();
	}
} // anonymous namespace

// =================================================================================================
// MARK: - Helpers

static ZRef<ZStrimmerR> spMakeStrimmer(const ZTuple& iHeader, ZRef<ZStreamerR> iStreamerR)
	{
	return new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(iStreamerR);
	}

static ZRef<ZStrimmerR> spMakeStrimmer(const ZTuple& iHeader, const ZStreamR& iStreamR)
	{
	return spMakeStrimmer(iHeader, new ZStreamerR_Stream(iStreamR));
	}

static string spAsString_WebDAV(ZTime iTime)
	{
	return ZUtil_Time::sAsStringUTC(iTime, "%a, %d %b %Y %H:%M:%S %Z");	
	}

static string spGetPathFromURL(const string& iURL)
	{
	size_t pos = iURL.find("//");
	if (pos != string::npos)
		{
		size_t pos2 = iURL.find("/", pos + 2);
		if (pos2 != string::npos)
			return iURL.substr(pos2, string::npos);
		}
	return string();
	}

/*
spReadTV and spWriteAsXML transform between the XML representation:
<D:propfind>
	<D:prop>
		<R:fred/>
		<R:bill/>
		<R:frank>Some text</R:frank>
	</D:prop>
</D:propfind>

and this tuple representation:
{
"D:propfind" = 
	{
	"D:prop" = 
		{
		"R:fred" = null;
		"R:bill" = null;
		"R:frank" = "Some text";
		}
	}
}

It's possible to transform arbitrary XML into a tuple representation,
and of course to go the other way, but the affordances of the two
structures are slightly different and the point here is to use the
simple ZTuple API to work with the data that's streamed as XML. See
<http://www.xml.com/lpt/a/2006/05/31/converting-between-xml-and-json.html>
for a discussion of the issues.
*/

static ZTValue spReadTV(ZML::StrimU& ml, const string& iName)
	{
	// We're sitting just after the begin tag. If there is any
	// text, suck it into a string and move on.
	if (ml.Current() == ZML::eToken_Text)
		{
		const string allText = ml.ReadAll8();
		ml.Advance();
		if (ml.Current() == ZML::eToken_TagEnd)
			{
			// We've hit an end tag, so the begin/end tags were wrapping a text value.
			if (iName != ml.Name())
				throw runtime_error("Tags don't match. Wanted: " + iName + ", Got: " + ml.Name());
			ml.Advance();
			return allText;
			}
		// The text following the begin tag was spurious -- it's not representable
		// in the simplified tuple we're building, so we'll just ignore it.
		}

	// Accumulate all child tags into a tuple to be returned.
	ZTuple result;
	for (;;)
		{
		if (ml.Current() == ZML::eToken_TagBegin)
			{
			// We have a nested begin tag. Grab its name and recurse.
			string name = ml.Name();
			ml.Advance();
			ZTValue theTV = spReadTV(ml, name);
			result.Set(name, theTV);
			}
		else if (ml.Current() == ZML::eToken_TagEmpty)
			{
			// We have a nested empty tag -- we represent them as null values.
			string name = ml.Name();
			ml.Advance();
			result.Set(name, ZTValue());
			}
		else if (ml.Current() == ZML::eToken_Text)
			{
			// We've got some text, but it's interspersed with other
			// tags and thus doesn't fit our model -- ignore it.
			ml.Advance();
			}
		else if (ml.Current() == ZML::eToken_TagEnd)
			{
			if (iName != ml.Name())
				throw runtime_error("Tags don't match. Wanted: " + iName + ", Got: " + ml.Name());
			// We've hit the end tag for the tuple, so we
			// consume it and break out of the loop.
			ml.Advance();
			break;
			}
		else
			{
			// We've hit the end of the reader, so we also exit the loop.
			break;
			}
		}
	return result;
	}

static ZTuple spReadTuple(ZML::StrimU& ml)
	{
	// We assume we're sitting just prior to a document element --
	// a top level begin tag. We're going to turn the
	// tag into a tuple.
	sSkipText(ml);
	ZTuple result;
	if (ml.Current() == ZML::eToken_TagBegin)
		{
		string name = ml.Name();
		ml.Advance();
		result = spReadTV(ml, name).GetTuple();
		}
	return result;
	}

static ZTuple spReadTuple(const ZStrimR& iStrimR)
	{
	try
		{
		ZStrimU_Unreader strimU(iStrimR);
		ZML::StrimU ml(strimU);
		return spReadTuple(ml);
		}
	catch (...)
		{}
	return ZTuple();
	}

static void spWriteAsXML(const ZStrimW_ML& s, const string& iName, const ZTValue& iTV);
static void spWriteAsXML(const ZStrimW_ML& s, const ZTuple& iTuple)
	{
	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		spWriteAsXML(s, iTuple.NameOf(i).AsString(), iTuple.Get(i));
	}

static void spWriteAsXML(const ZStrimW_ML& s, const string& iName, const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Vector:
			{
			const vector<ZTValue>& v = iTV.GetSeq().GetVector();
			for (vector<ZTValue>::const_iterator i = v.begin(); i != v.end(); ++i)
				spWriteAsXML(s, iName, *i);
			break;
			}
		case eZType_Tuple:
			{
			s.Begin(iName);
				spWriteAsXML(s, iTV.GetTuple());
			s.End(iName);
			break;
			}
		case eZType_String:
			{
			s.Begin(iName);
			bool old = s.Indent(false);
				s << iTV.GetString();
			s.End(iName);
			s.Indent(old);
			break;
			}
		case eZType_Null:
			{
			s.Empty(iName);
			break;
			}
		}
	}

static bool spGetStringAt(const ZTuple& iTuple, const string& iName, string& oString)
	{
	if (iTuple.QGetString(iName, oString))
		return true;

	const vector<ZTValue>& theVector = iTuple.Get(iName).GetSeq().GetVector();
	if (!theVector.empty())
		return theVector[0].QGetString(oString);

	return false;
	}

static int spGetDepth(const ZTuple& iTuple)
	{
	string theDepth;
	if (spGetStringAt(iTuple, "depth", theDepth))
		{
		if (theDepth == "0")
			return 0;
		else if (theDepth == "1")
			return 1;
		}
	return 2;
	}

static ZTuple spGetProp(const ZNode& iNode, const string& iPropName)
 	{
 	ZTuple propT;

	if (iPropName == "D:resourcetype")
		{
		if (iNode.CanHaveChildren())
			propT.Set(iPropName, ZTuple().SetNull("D:collection"));
		else
			propT.SetNull(iPropName);
		}
	else if (iPropName == "D:getcontenttype")
		{
		ZTValue theValue;
		if (iNode.GetProp("MIMEType", theValue))
			{
			string theMIMEType;
			if (theValue.QGetString(theMIMEType))
				propT.SetString(iPropName, theMIMEType);
			}
		}
	else if (iPropName == "D:creationdate")
		{
		ZTValue theValue;
		if (iNode.GetProp("TimeCreated", theValue))
			{
			if (ZTime theTime = theValue.GetTime())
				propT.SetString(iPropName, spAsString_WebDAV(theTime));
			}
		}
	else if (iPropName == "D:getlastmodified")
		{
		ZTValue theValue;
		if (iNode.GetProp("TimeModified", theValue))
			{
			if (ZTime theTime = theValue.GetTime())
				propT.SetString(iPropName, spAsString_WebDAV(theTime));
			}
		}
	else if (iPropName == "D:getcontentlength")
		{
		ZTValue theValue;
		if (iNode.GetProp("ContentLength", theValue))
			{
			int64 theLength;
			if (theValue.QGetInt64(theLength))
	 			propT.SetString(iPropName, ZString::sFromUInt64(theLength));
			}
		}

 	return propT;
 	}

static void spGetProperties(
	const ZNode& iNode, const vector<string>& iPropNames, ZTuple& oGoodT, ZTuple& oBadT)
	{
 	for (vector<string>::const_iterator i = iPropNames.begin(); i != iPropNames.end(); ++i)
 		{
 		if (ZTuple propT = spGetProp(iNode, *i))
			oGoodT.Mutable("D:prop").MutableSeq().Append(propT);
 		else
 			oBadT.Mutable("D:prop").MutableSeq().Append(ZTuple().SetNull(*i));
 		}
	}

static ZTrail spTrailFromTo(const ZNode& iFrom, const ZNode& iTo)
	{
	if (ZTrail fromTrail = iFrom.TrailFromRoot())
		{
		if (ZTrail toTrail = iTo.TrailFromRoot())
			{
			ZTrail result = ZTrail::sTrailFromTo(fromTrail.GetComps(), toTrail.GetComps());
			if (ZLOG(s, eDebug, "spTrailFromTo"))
				{
				s << "From: " << fromTrail.AsString()
					<< "\nTo: " << toTrail.AsString()
					<< "\nResult: " << result.AsString();
				}
			return result;
			}
		}
	return ZTrail(false);
	}

static string spMakeHREF(const ZTrail& iPrefix, const ZNode& iRoot, const ZNode& iNode)
	{
	ZTrail fromTo = spTrailFromTo(iRoot, iNode);
	ZTrail combined = iPrefix + fromTo;
	string result = ZHTTP::sEncodeTrail(combined);
	if (ZLOG(s, eDebug, "spMakeHREF"))
		{
		s << "iPrefix: " << iPrefix.AsString()
			<< "\nfromTo: " << fromTo.AsString()
			<< "\ncombined: " << combined.AsString()
			<< "\nresult: " << result;
		}
	return result;
	}

static bool spDelete(const ZTrail& iPrefix, const ZNode& iRoot, const ZNode& iNode, ZTuple& ioT)
	{
	bool allOkay = true;
	// Delete any descendants of iNode.
	for (ZNodeIter i = iNode; i; i.Advance())
		{
		if (!spDelete(iPrefix, iRoot, i.Current(), ioT))
			allOkay = false;
		}

	if (allOkay)
		{
		if (iNode.Delete())
			return true;

		ZTuple responseT;
		responseT.SetString("D:href", spMakeHREF(iPrefix, iRoot, iNode));
		responseT.SetString("D:status", "HTTP/1.1 404");
		ioT.Mutable("D:response").MutableSeq().Append(responseT);
		}
	return false;
	}

static ZTrail spStripPrefix(const ZTrail& iPrefix, const ZTrail& iTrail)
	{
	size_t start = 0;
	while (start < iPrefix.Count()
		&& start < iTrail.Count()
		&& iPrefix.At(start) == iTrail.At(start))
		{
		++start;
		}

	return iTrail.SubTrail(start, iTrail.Count());
	}

// =================================================================================================
// MARK: - ZWebDAV

bool ZWebDAV::sHandle_DELETE(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR&, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZNode theNode = iRoot.Trail(iTrail);

	ZHTTP::Response r;
	r.Set("date", spAsString_WebDAV(ZTime::sNow()));

	if (theNode.Delete())
		{
		// theNode, and any descendants, was deleted succesfully.
		r.SetResult(204);
		r.Set("Content-Length", 0);
		r.Send(iStreamW);
		}
	else
		{
		// theNode could not be deleted, try doing an explicit depth first,
		// post-order scan of the tree. Failure of a child causes failure
		// of all ancestors, but not necessarily of any sibling.
		ZTuple responsesT;
		if (spDelete(iPrefix, iRoot, theNode, responsesT))
			{
			// The whole tree was deleted succesfully.
			r.SetResult(204);
			r.Set("Content-Length", 0);
			r.Send(iStreamW);
			}
		else
			{
			// Some part of the tree had problems, recorded in responsesT.
			r.SetResult(207);
			r.Set("Content-Type", "text/xml; charset=\"utf-8\"");
			r.Set("Transfer-Encoding", "chunked");
			r.Send(iStreamW);

			ZHTTP::StreamW_Chunked chunkedStream(iStreamW);
			ZStrimW_StreamUTF8 theStrimW(iStreamW);
			ZStrimW_ML s(false, theStrimW);
			s.PI("xml")
				.Attr("version", "1.0")
				.Attr("encoding", "utf-8");

			s.Begin("D:multistatus");
				s.Attr("xmlns:D", "DAV:");
				spWriteAsXML(s, responsesT);
			s.End("D:multistatus");
			}
		}
	return true;
	}

bool ZWebDAV::sHandle_GET(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR&, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZNode theNode = iRoot.Trail(iTrail);

	ZHTTP::Response r;
	r.Set("date", spAsString_WebDAV(ZTime::sNow()));

	if (iHeader.Has("range"))
		{
		if (ZLOG(s, eInfo, "ZWebDAV"))
			s << "GET with range:\n" << iHeader;
		}

	if (theNode.Exists())
		{
		r.SetResult(200);
		if (theNode.CanHaveChildren())
			{
			r.Set("Content-Type", "text/html; charset=\"utf-8\"");
			r.Set("Transfer-Encoding", "chunked");
			r.Send(iStreamW);

			ZHTTP::StreamW_Chunked chunkedStream(iStreamW);
			ZStrimW_StreamUTF8 theStrimW(chunkedStream);
			ZStrimW_ML s(false, theStrimW);
			s.Begin("html");
				s.Begin("title");
					s << theNode.Name();
				s.End("title");
				s.Begin("body");
					for (ZNodeIter i = theNode; i; i.Advance())
						{
						s.Begin("p");
							s.Begin("a");
								if (i.Current().CanHaveChildren())
									{
									s.Attr("href",
										ZHTTP::sEncodeComponent(i.Current().Name()) + "/");
									s << i.Current().Name() << "/";
									}
								else
									{
									s.Attr("href", ZHTTP::sEncodeComponent(i.Current().Name()));
									s << i.Current().Name();
									}
							s.End("a");
						s.End("p");
						}
				s.End("body");
			
			s.End("html");
			}
		else if (ZRef<ZStreamerRPos> theStreamer = theNode.OpenRPos())
			{
			const ZStreamRPos& theStreamRPos = theStreamer->GetStreamRPos();
			uint64 sentSize = theStreamRPos.GetSize();

			if (ZTValue rangeParam = iHeader.Get("range"))
				{
				vector<pair<size_t, size_t> > ranges;
				if (!ZHTTP::sOrganizeRanges(sentSize, rangeParam, ranges))
					{
					iStreamW.WriteString("HTTP/1.1 406 Unsatisfiable range\r\n\r\n");
					return false;
					}
				r.SetResult(206, "Partial Content");

				r.Set("Content-Range",
					ZString::sFormat("bytes %d-%d/%d",
					ranges.front().first, ranges.front().second - 1, sentSize));

				theStreamRPos.SetPosition(ranges.front().first);
				sentSize = ranges.front().second - ranges.front().first;
				}
			else
				{
				r.SetResult(200);
				}
			
			string theMIMEType = "application/octet-stream";
			ZTValue theMIMEValue;
			if (theNode.GetProp("MIMEType", theMIMEValue))
				{
				string asString;
				if (theMIMEValue.QGetString(asString))
					theMIMEType = asString;
				}
			r.Set("Content-Type", theMIMEType);

			ZTValue theValue;
			if (theNode.GetProp("lastModified", theValue))
				{
				if (ZTime theTime = theValue.GetTime())
					r.Set("Last-Modified", spAsString_WebDAV(theTime));
				}

			r.Set("Content-Transfer-Encoding", "binary");
			r.Set("Content-Length", ZString::sFromUInt64(sentSize));

			r.Send(iStreamW);

			iStreamW.CopyFrom(theStreamRPos, sentSize);
			}
		}
	else
		{
		r.SetResult(404);
		r.Send(iStreamW);
		r.Set("Content-Length", 0);
		}
	return true;
	}

bool ZWebDAV::sHandle_LOCK(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZHTTP::Response r;
	r.SetResult(200);
	r.Set("Date", spAsString_WebDAV(ZTime::sNow()));
	r.Set("Content-Type", "text/xml; charset=\"utf-8\"");
	r.Set("Transfer-Encoding", "chunked");
	r.Send(iStreamW);

	ZHTTP::StreamW_Chunked chunkedStream(iStreamW);
	ZStrimW_StreamUTF8 theStrimW(chunkedStream);
	ZStrimW_ML s(false, theStrimW);
	s.PI("xml");
		s.Attr("version", "1.0");
		s.Attr("encoding", "utf-8");

	s.Begin("D:prop");
		s.Attr("xmlns:D", "DAV:");
		s.Begin("D:lockdiscovery");
			s.Begin("D:activelock");
				s.Begin("D:locktype");
					s.Empty("D:write");
				s.End("D:locktype");

				s.Begin("D:lockscope");
					s.Empty("D:exclusive");
				s.End("D:lockscope");

				s.Begin("D:depth")
					.Write("Infinity")
				.End("D:depth");


//				s.Begin("D:depth");
//					s << "Infinity";
//				s.End("D:depth");

				s.Begin("D:owner");
					s.Begin("D:href");
						s << "default-owner";
					s.End("D:href");
				s.End("D:owner");
						
				s.Begin("D:timeout");
					s << "Second-604800";
				s.End("D:timeout");

				s.Begin("D:locktoken");
					s.Begin("D:href");
						s << "opaquelocktoken:this_is_a_bogus_lock_token";
					s.End("D:href");
				s.End("D:locktoken");
			s.End("D:activelock");
		s.End("D:lockdiscovery");
	s.End("D:prop");
	return true;
	}

bool ZWebDAV::sHandle_MKCOL(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR&, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZNode theNode = iRoot.Trail(iTrail);

	ZHTTP::Response r;
	r.Set("date", spAsString_WebDAV(ZTime::sNow()));
	r.Set("Content-Length", 0);

	if (theNode.Exists())
		{
		r.SetResult(405, "That node already exists");
		}
	else
		{			
		ZNode::Error err;
		if (ZNode newNode = theNode.CreateDir(&err))
			{
			r.SetResult(201, "Created");
			}
		else
			{
			r.SetResult(403);
			}
		}
	r.Send(iStreamW);
	return true;
	}

bool ZWebDAV::sHandle_MOVE(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR&, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZHTTP::Response r;
	r.Set("date", spAsString_WebDAV(ZTime::sNow()));
	r.Set("Content-Length", 0);


	ZNode theNode = iRoot.Trail(iTrail);
	if (!theNode.Exists())
		{
		r.SetResult(404);
		}
	else
		{
		string thePath = spGetPathFromURL(ZHTTP::sGetString0(iHeader.Get("destination")));
		ZTrail theTrail = spStripPrefix(iPrefix, ZHTTP::sDecodeTrail(thePath));
		ZNode theDestNode = iRoot.Trail(theTrail);

		if (ZLOG(s, eInfo, "ZWebDAV::sHandle_MOVE"))
			{
			s << "thePath: " << thePath << "\n";
			s << "theTrail: " << theTrail.AsString() << "\n";
			}

		bool doneIt = false;
		if (!doneIt)
			doneIt = theNode.MoveTo(theDestNode);

//		if (!doneIt)
//			doneIt = theDestNode.MoveFrom(theNode);

		if (!doneIt)
			{
			if (ZRef<ZStreamerR> sourceStreamer = theNode.OpenR(false))
				{
				if (ZLOG(s, eInfo, "ZWebDAV::sHandle_MOVE"))
					s << "Got a source streamer, ";
				const ZStreamR& source = sourceStreamer->GetStreamR();
				if (ZRef<ZStreamerWPos> destStreamer = theDestNode.CreateWPos(true, false))
					{
					if (ZLOG(s, eInfo, "ZWebDAV::sHandle_MOVE"))
						s << "and a dest streamer";
					const ZStreamWPos& dest = destStreamer->GetStreamWPos();
					dest.Truncate();
					dest.CopyAllFrom(source);
					theNode.Delete();
					doneIt = true;
					}
				}
			}

		if (doneIt)
			r.SetResult(204);
		else
			r.SetResult(400);
		}

	r.Send(iStreamW);
	return true;
	}

bool ZWebDAV::sHandle_OPTIONS(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR&, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
//	r.Set("Allow", "OPTIONS,GET,HEAD,POST,DELETE,TRACE,PROPFIND,PROPPATCH,COPY,MOVE,LOCK,UNLOCK");

	ZHTTP::Response r;
	r.SetResult(200);
	r.Set("Allow", "OPTIONS,GET,PUT,DELETE,PROPFIND,MKCOL");
	r.Set("DAV", "1,2");
	r.Set("Content-Length", 0);

	r.Send(iStreamW);
	return true;
	}

static void spHandle_PROPFIND_Some(
	const ZTrail& iPrefix, const ZNode& iRoot, const ZNode& iNode,
	int iDepth, const vector<string>& iPropNames, ZTuple& ioTuple)
	{
	for (DAVIter i = DAVIter(iNode, iDepth); i; i.Advance())
		{
		ZNode theNode = i.Current();
		if (theNode.Exists())
			{
			ZTuple goodT, badT;
			spGetProperties(theNode, iPropNames, goodT, badT);

			ZTuple responseT;
			responseT.SetString("D:href", spMakeHREF(iPrefix, iRoot, theNode));

			if (goodT)
				{
				goodT.SetString("D:status", "HTTP/1.1 200 OK");
				responseT.Mutable("D:propstat").MutableSeq().Append(goodT);
				}

			if (badT)
				{
				badT.SetString("D:status", "HTTP/1.1 404 Not Found");
				responseT.Mutable("D:propstat").MutableSeq().Append(badT);
				}
			
			ioTuple.Mutable("D:response").MutableSeq().Append(responseT);
			}
		}
	}

static void spHandle_PROPFIND_All(
	const ZTrail& iPrefix, const ZNode& iRoot, const ZNode& iNode, int iDepth, ZTuple& ioTuple)
	{
	vector<string> thePropNames;
	thePropNames.push_back("D:resourcetype");
	thePropNames.push_back("D:getcontenttype");
	thePropNames.push_back("D:creationdate");
	thePropNames.push_back("D:getlastmodified");
	thePropNames.push_back("D:getcontentlength");

	for (DAVIter i = DAVIter(iNode, iDepth); i; i.Advance())
		{
		ZNode theNode = i.Current();
		if (theNode.Exists())
			{
			ZTuple goodT, badT;
			spGetProperties(theNode, thePropNames, goodT, badT);

			ZTuple responseT;
			responseT.SetString("D:href", spMakeHREF(iPrefix, iRoot, theNode));

			if (goodT)
				{
				goodT.SetString("D:status", "HTTP/1.1 200 OK");
				responseT.Mutable("D:propstat").MutableSeq().Append(goodT);
				}
			
			ioTuple.Mutable("D:response").MutableSeq().Append(responseT);
			}
		}
	}

bool ZWebDAV::sHandle_PROPFIND(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	int depth = spGetDepth(iHeader);

	ZRef<ZStrimmerR> theStrimmerR = spMakeStrimmer(iHeader, iStreamR);
	const ZStrimR& theStrimR = theStrimmerR->GetStrimR();

	ZHTTP::Response r;
	r.Set("Date", spAsString_WebDAV(ZTime::sNow()));

	ZNode theNode = iRoot.Trail(iTrail);
	if (!theNode.Exists())
		{
		theStrimR.SkipAll();
		r.Set("Content-Length", 0);
		r.SetResult(404);
		r.Send(iStreamW);
		}
	else
		{		
		ZTuple t = spReadTuple(theStrimR);
		
		ZTuple results;
		if (t.Empty() || t.Has("D:allprop"))
			{
			spHandle_PROPFIND_All(iPrefix, iRoot, theNode, depth, results);
			}
		else
			{
			ZTuple propT = t.GetTuple("D:prop");
			vector<string> thePropNames;
			for (ZTuple::const_iterator i = propT.begin(); i != propT.end(); ++i)
				thePropNames.push_back(propT.NameOf(i).AsString());
			spHandle_PROPFIND_Some(iPrefix, iRoot, theNode, depth, thePropNames, results);
			}

		if (ZLOG(s, eDebug, "ZWebDAV"))
			{
			s << "PropFind Request:" << t << "\n";
			s << "PropFind Results:" << results << "\n";
			}

		r.SetResult(207, "Multi-Status");
		r.Set("Content-Type", "text/xml; charset=\"utf-8\"");
		r.Set("Transfer-Encoding", "chunked");		
		r.Send(iStreamW);

		ZHTTP::StreamW_Chunked chunkedStream(iStreamW);
		ZStrimW_StreamUTF8 theStrimW(chunkedStream);
		ZStrimW_ML s(false, theStrimW);
		s.PI("xml");
			s.Attr("version", "1.0");
			s.Attr("encoding", "utf-8");
		s.Begin("D:multistatus");
			s.Attr("xmlns:D", "DAV:");
			spWriteAsXML(s, results);
		s.End("D:multistatus");
		}
	return true;
	}

bool ZWebDAV::sHandle_PROPPATCH(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
//	int depth = spGetDepth(iHeader);

	ZRef<ZStrimmerR> theStrimmerR = spMakeStrimmer(iHeader, iStreamR);
	const ZStrimR& theStrimR = theStrimmerR->GetStrimR();

	ZHTTP::Response r;
	r.Set("Date", spAsString_WebDAV(ZTime::sNow()));
	
	ZNode theNode = iRoot.Trail(iTrail);

	ZTuple t = spReadTuple(theStrimR);
	if (ZLOG(s, eInfo, "ZWebDAV"))
		s << "PROPPATCH Request:" << t << "\n";

//	if (!theNode.Exists())
		{
		theStrimR.SkipAll();
		r.Set("Content-Length", 0);
		r.SetResult(404);
		r.Send(iStreamW);
		}
	return true;
	}

bool ZWebDAV::sHandle_PUT(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZHTTP::Response r;
	r.Set("date", spAsString_WebDAV(ZTime::sNow()));
	r.Set("Content-Length", 0);

	ZNode theNode = iRoot.Trail(iTrail);

	ZRef<ZStreamerWPos> theStreamerWPos = theNode.CreateWPos(true);
	if (!theStreamerWPos)
		{
		r.SetResult(409);
		}
	else
		{
		theStreamerWPos->GetStreamW().CopyAllFrom(iStreamR);
		theStreamerWPos->GetStreamWPos().Truncate();

		r.SetResult(204);
		}

	r.Send(iStreamW);
	return true;
	}

bool ZWebDAV::sHandle_UNLOCK(
	const ZTrail& iPrefix, ZNode iRoot, const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const ZTuple& iHeader, const ZTrail& iTrail, const ZTuple& iParam)
	{
	ZHTTP::Response r;
	r.SetResult(204);
	r.Set("Content-Length", 0);
	r.Send(iStreamW);

	return true;
	}

} // namespace ZooLib
