// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush_XMLPList.h"

#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/ParseException.h"
#include "zoolib/Pull_ML.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_Time.h"

namespace ZooLib {

using namespace PullPush;
using namespace Pull_ML;
using std::string;

static void spPull_Base64_Push_PPT(const ZooLib::ChanR_UTF& iChanR, const ChanW_PPT& iChanW)
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	ChanR_Bin_ASCIIStrim theStreamR_ASCIIStrim(iChanR);
	ChanR_Bin_Base64Decode theStreamR_Base64Decode(theStreamR_ASCIIStrim);
	sECopyAll(theStreamR_Base64Decode, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	}

// =================================================================================================
#pragma mark - sPull_XMLPList_Push_PPT (reading from ChanR_PPT)

static void spHandleSimple(const Name& iTagName, const ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW)
	{
	if (iTagName == "data")
		{
		spPull_Base64_Push_PPT(iChanRU, iChanW);
		}
	else if (iTagName == "integer")
		{
		int64 theInt64;
		if (not Util_Chan::sTryRead_SignedDecimalInteger(iChanRU, theInt64))
			sThrow_ParseException("Expected valid integer");
		sPush(theInt64, iChanW);
		}
	else if (iTagName == "real")
		{
		int64 theInt64;
		double theDouble;
		bool isDouble;
		if (not Util_Chan::sTryRead_SignedGenericNumber(iChanRU, theInt64, theDouble, isDouble))
			sThrow_ParseException("Expected valid real");

		if (isDouble)
			sPush(theDouble, iChanW);
		else
			sPush(theInt64, iChanW);
		}
	else
		{
		sThrow_ParseException("Unhandled begin tag '" + string(iTagName) + "'");
		}
	}

static void spPull_XMLPList_Push_PPT(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_PPT& iChanW)
	{
	if (ZP<TagEmpty> theP = TagEmpty::sAs(iPPT))
		{
		const string theName = theP->GetName();
		if (theName == "dict")
			{
			sPush_Start_Map(iChanW);
			sPush_End(iChanW);
			}
		else if (theName == "array")
			{
			sPush_Start_Seq(iChanW);
			sPush_End(iChanW);
			}
		else if (theName == "string")
			{
			sPush(string(), iChanW);
			}
		else if (theName == "data")
			{
			sPush(Data_ZZ(), iChanW);
			}
		else if (theName == "true")
			{
			sPush(true, iChanW);
			}
		else if (theName == "false")
			{
			sPush(false, iChanW);
			}
		else if (theName == "nil")
			{
			sPush(null, iChanW);
			}
		else
			{
			sThrow_ParseException("Unhandled empty tag " + theName);
			}
		}
	else if (ZP<TagBegin> theP = TagBegin::sAs(iPPT))
		{
		const string theName = theP->GetName();
		if (theName == "plist")
			{
			sPull_XMLPList_Push_PPT(iChanR, iChanW);
			sESkipText_ReadEnd(iChanR, "plist");
			}
		else if (theName == "dict")
			{
			sPush_Start_Map(iChanW);
			for (;;)
				{
				const PPT thePPT = sESkipText_Read(iChanR);
				if (ZP<TagEnd> theTagEnd = TagEnd::sAs(thePPT))
					{
					if (theTagEnd->GetName() != Name("dict"))
						sThrow_ParseException("Read end tag '" + string(theTagEnd->GetName()) + "', should have been 'dict'");
					sPush_End(iChanW);
					break;
					}
				else
					{
					ZP<TagBegin> theTagBegin = TagBegin::sAs(thePPT);
					if (not theTagBegin)
						{
						sThrow_ParseException("Expected begin tag ('key')");
						}
					else if (theTagBegin->GetName() != Name("key"))
						{
						sThrow_ParseException("Expected begin tag 'key'");
						}
					else if (NotQ<PPT> theQ = sQRead(iChanR))
						{
						sThrow_ExhaustedR();
						}
					else
						{
						if (NotQ<string> theStringQ = sQAsString(*theQ))
							sThrow_ParseException("Expected text in 'key' tag");
						else
							sPush(Name(*theStringQ), iChanW);
						sESkipText_ReadEnd(iChanR, "key");
						}
					}
				sPull_XMLPList_Push_PPT(iChanR, iChanW);
				}
			}
		else if (theName == "array")
			{
			sPush_Start_Seq(iChanW);
			for (;;)
				{
				const PPT thePPT = sESkipText_Read(iChanR);
				if (ZP<TagEnd> theTagEnd = TagEnd::sAs(thePPT))
					{
					if (theTagEnd->GetName() != Name("array"))
						sThrow_ParseException("Read end tag '" + string(theTagEnd->GetName()) + "', should have been 'array'");
					sPush_End(iChanW);
					break;
					}
				spPull_XMLPList_Push_PPT(thePPT, iChanR, iChanW);
				}
			}
		else
			{
			// Scope for thePPT and asChanner, so we don't hold it while blocking
			// on sESkipText_ReadEnd.
				{
				const PPT thePPT = sERead(iChanR);

				const string* asStringP = sPGet<string>(thePPT);
				ZP<ChannerR_UTF> asChanner = sGet<ZP<ChannerR_UTF>>(thePPT);

				if (not asStringP && not asChanner)
					sThrow_ParseException("Expected text");

				if (theName == "string")
					{
					sPush(thePPT, iChanW);
					}
				else if (theName == "date")
					{
					const string asString = asStringP ? *asStringP : sReadAllUTF8(*asChanner);
					UTCDateTime theUTCDateTime = Util_Time::sFromString_ISO8601(asString);
					sPush(theUTCDateTime, iChanW);
					}
				else if (asStringP)
					spHandleSimple(theName, ChanRU_UTF_string8(*asStringP), iChanW);
				else
					spHandleSimple(theName, ChanRU_XX_Unreader<UTF32>(*asChanner), iChanW);
				}

			sESkipText_ReadEnd(iChanR, theName);
			}
		}
	else
		{
		sThrow_ParseException("Unhandled PPT, type: " + Util_Debug::sPrettyName(iPPT.Type()));
		}
	}

void sPull_XMLPList_Push_PPT(const ChanR_PPT& iChanR, const ChanW_PPT& iChanW)
	{
	const PPT thePPT = sESkipText_Read(iChanR);
	spPull_XMLPList_Push_PPT(thePPT, iChanR, iChanW);
	}

// =================================================================================================
#pragma mark - sPull_XMLPList_Push_PPT (reading from ChanRU_UTF_ML)

static void sESkipText_ReadEnd(ChanRU_UTF_ML& r, const string& iTagName)
	{
	sSkipText(r);
	sTryRead_End(r, iTagName) || sThrow_ParseException("Expected end tag '" + iTagName + "'");
	}

void sPull_XMLPList_Push_PPT(ChanRU_UTF_ML& iChanRU, const ChanW_PPT& iChanW)
	{
	sSkipText(iChanRU);
	if (iChanRU.Current() == ML::eToken_TagEmpty)
		{
		string theName = iChanRU.Name();
		if (theName == "dict")
			{
			iChanRU.Advance();
			sPush_Start_Map(iChanW);
			sPush_End(iChanW);
			}
		else if (theName == "array")
			{
			iChanRU.Advance();
			sPush_Start_Seq(iChanW);
			sPush_End(iChanW);
			}
		else if (theName == "string")
			{
			iChanRU.Advance();
			sPush(string(), iChanW);
			}
		else if (theName == "data")
			{
			iChanRU.Advance();
			sPush(Data_ZZ(), iChanW);
			}
		else if (iChanRU.Name() == "true")
			{
			iChanRU.Advance();
			sPush(true, iChanW);
			}
		else if (iChanRU.Name() == "false")
			{
			iChanRU.Advance();
			sPush(false, iChanW);
			}
		else if (iChanRU.Name() == "nil")
			{
			iChanRU.Advance();
			sPush(null, iChanW);
			}
		else
			{
			sThrow_ParseException("Unhandled empty tag " + theName);
			}
		}
	else if (iChanRU.Current() == ML::eToken_TagBegin)
		{
		if (iChanRU.Name() == "plist")
			{
			iChanRU.Advance();
			sPull_XMLPList_Push_PPT(iChanRU, iChanW);
			sESkipText_ReadEnd(iChanRU, "plist");
			}
		else if (iChanRU.Name() == "dict")
			{
			iChanRU.Advance();
			sPush_Start_Map(iChanW);
			for (;;)
				{
				sSkipText(iChanRU);
				if (sTryRead_End(iChanRU, "dict"))
					{
					sPush_End(iChanW);
					break;
					}

				if (not sTryRead_Begin(iChanRU, "key"))
					sThrow_ParseException("Expected <key>");

				Name theName = sReadAllUTF8(iChanRU);

				sPush(theName, iChanW);

				sESkipText_ReadEnd(iChanRU, "key");

				sPull_XMLPList_Push_PPT(iChanRU, iChanW);
				}
			}
		else if (iChanRU.Name() == "array")
			{
			iChanRU.Advance();
			sPush_Start_Seq(iChanW);
			for (;;)
				{
				sSkipText(iChanRU);
				if (sTryRead_End(iChanRU, "array"))
					{
					sPush_End(iChanW);
					break;
					}

				sPull_XMLPList_Push_PPT(iChanRU, iChanW);
				}
			}
		else if (iChanRU.Name() == "string")
			{
			iChanRU.Advance();
			sPull_UTF_Push_PPT(iChanRU, iChanW);
			sESkipText_ReadEnd(iChanRU, "string");
			}
		else if (iChanRU.Name() == "data")
			{
			iChanRU.Advance();
			spPull_Base64_Push_PPT(iChanRU, iChanW);
			sESkipText_ReadEnd(iChanRU, "data");
			}
		else if (iChanRU.Name() == "integer")
			{
			iChanRU.Advance();
			int64 theInt64;
			if (not Util_Chan::sTryRead_SignedDecimalInteger(iChanRU, theInt64))
				sThrow_ParseException("Expected valid integer");
			sPush(theInt64, iChanW);
			sESkipText_ReadEnd(iChanRU, "integer");
			}
		else if (iChanRU.Name() == "real")
			{
			iChanRU.Advance();
			int64 theInt64;
			double theDouble;
			bool isDouble;
			if (not Util_Chan::sTryRead_SignedGenericNumber(iChanRU, theInt64, theDouble, isDouble))
				sThrow_ParseException("Expected valid real");

			if (isDouble)
				sPush(theDouble, iChanW);
			else
				sPush(theInt64, iChanW);
			sESkipText_ReadEnd(iChanRU, "real");
			}
		else if (iChanRU.Name() == "date")
			{
			iChanRU.Advance();
			UTCDateTime theUTCDateTime = Util_Time::sFromString_ISO8601(sReadAllUTF8(iChanRU));
			sPush(theUTCDateTime, iChanW);
			sESkipText_ReadEnd(iChanRU, "date");
			}
		else
			{
			sThrow_ParseException("Unhandled begin tag '" + iChanRU.Name() + "'");
			}
		}
	}

// =================================================================================================
#pragma mark -

static void spPull_PPT_Push_XMLPList(const PPT& iPPT, const ChanR_PPT& iChanR, const ChanW_UTF_ML& iChanW)
	{
	const ChanW_UTF_ML& s = iChanW;

	if (false)
		{}

	else if (const string* theString = sPGet<string>(iPPT))
		{
		s.Begin("string");
			sEWrite(s, *theString);
		s.End("string");
		}

	else if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		{
		s.Begin("string");
			sECopyAll(*theChanner, s);
		s.End("string");
		}

	else if (const Data_ZZ* theData = sPGet<Data_ZZ>(iPPT))
		{
		s.Begin("data");
			sEWriteMem(ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)),
				theData->GetPtr(), theData->GetSize());
		s.End("data");
		}

	else if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		{
		s.Begin("data");
			sECopyAll(*theChanner, ChanW_Bin_Base64Encode(ChanW_Bin_ASCIIStrim(s)));
		s.End("data");
		}

	else if (sIsStart_Map(iPPT))
		{
		s.Begin("dict");
			for (;;)
				{
				if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
					{
					break;
					}
				else
					{
					s.Begin("key");
						sEWrite(s, *theNameQ);
					s.End("key");

					if (NotQ<PPT> theNotQ = sQRead(iChanR))
						{
						sThrow_ParseException("Require value after Name from ChanR_PPT");
						}
					else
						{
						spPull_PPT_Push_XMLPList(*theNotQ, iChanR, iChanW);
						}
					}
				}
		s.End("dict");
		}

	else if (sIsStart_Seq(iPPT))
		{
		s.Begin("array");
			for (;;)
				{
				if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
					break;
				else
					spPull_PPT_Push_XMLPList(*theNotQ, iChanR, iChanW);
				}
		s.End("array");
		}

	else if (iPPT.IsNull())
		{
		s.Empty("nil");
		}
	else if (const bool* asBool = iPPT.PGet<bool>())
		{
		if (*asBool)
			s.Empty("true");
		else
			s.Empty("false");
		}
	else if (ZQ<int64> asIntQ = sQCoerceInt(iPPT))
		{
		s.Begin("integer");
			sEWritef(s, "%lld", (long long)*asIntQ);
		s.End("integer");
		}
	else if (ZQ<double> asDoubleQ = sQCoerceRat(iPPT))
		{
		s.Begin("real");
			Util_Chan::sWrite_Exact(s, *asDoubleQ);
		s.End("real");
		}
	else if (const UTCDateTime* theValue = iPPT.PGet<UTCDateTime>())
		{
		s.Begin("date");
			s << Util_Time::sAsString_ISO8601(*theValue, true);
		s.End("date");
		}
	else
		{
		s.Begin("nil");
			s.Raw() << "<!--!! Unhandled: */" << Util_Debug::sPrettyName(iPPT.Type()) << " !!-->";
		s.End("nil");
		}
	}

bool sPull_PPT_Push_XMLPList(const ChanR_PPT& iChanR, const ChanW_UTF_ML& iChanW)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		spPull_PPT_Push_XMLPList(*theQ, iChanR, iChanW);
		return true;
		}
	return false;
	}

void sWriteXMLPListPreamble(const ChanW_UTF_ML& s)
	{
	s.PI("xml");
		s.Attr("version", "1.0");
		s.Attr("encoding", "UTF-8");

	s.Tag("!DOCTYPE");
		s.Attr("plist");
		s.Attr("PUBLIC");
		s.Attr("\"-//Apple Computer//DTD PLIST 1.0//EN\"");
		s.Attr("\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");

	s.Begin("plist");
		s.Attr("version", "1.0");
	}

void sWriteXMLPListPostamble(const ChanW_UTF_ML& s)
	{
	s.End("plist");
	}

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

static void spChannerR_PPT_XMLPList(ZP<ChannerRU_UTF_ML> iChannerR,
	const ZP<Channer<ChanWCon_PPT>>& iChannerWCon)
	{
	ZThread::sSetName("sChannerR_PPT_XMLPList");
	try
		{
		sPull_XMLPList_Push_PPT(*iChannerR, *iChannerWCon);

		sDisconnectWrite(*iChannerWCon);
		}
	catch (std::exception& ex)
		{
		ZLOGTRACE(eDebug); // In lieu of general error handling
		}
	}

ZP<ChannerR_PPT> sChannerR_PPT_XMLPList(const ZP<ChannerRU_UTF_ML>& iChanner)
	{
	PullPushPair<PPT> thePair = sMakePullPushPair<PPT>();
	sStartOnNewThread(sBindR(sCallable(spChannerR_PPT_XMLPList), iChanner, thePair.first));
	return thePair.second;
	}

} // namespace ZooLib
