/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/Chan_XX_Unreader.h"
#include "zoolib/ChanW_UTF_string.h"
#include "zoolib/Util_Any_JSON.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_JSON.h"

//#include "zoolib/ZStrimmer_Streamer.h"
//#include "zoolib/ZStrim_Stream.h"

// =================================================================================================
// MARK: -

namespace ZooLib {
namespace Util_Any_JSON {

//ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR, const ZRef<ChannerU_UTF>& iChannerU)
//	{
//	if (iSU)
//		{
//		ZYad_JSON::ReadOptions theRO;
//		theRO.fAllowUnquotedPropertyNames = true;
//		theRO.fAllowEquals = true;
//		theRO.fAllowSemiColons = true;
//		theRO.fAllowTerminators = true;
//		theRO.fLooseSeparators = true;
//		theRO.fAllowBinary = true;
//		return ZYad_Any::sQFromYadR(sYadR(iChannerR, iChannerU, theRO));
//		}
//	return null;
//	}

ZQ<Val_Any> sQRead(const ZRef<ChannerR_UTF>& iChannerR)
	{
	ZRef<Channer_FT<Chan_XX_Unreader<UTF32> > > theChanner =
		new Channer_FT<Chan_XX_Unreader<UTF32> >(iChannerR);
	return sQRead(theChanner, theChanner);
	}

ZQ<Val_Any> sQRead(const ZRef<ChannerR_Bin>& iChannerR)
	{
	return sQRead(ZRef<ZStrimmerR>(sStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(iSR)));
	}

void sWrite(const Val_Any& iVal, const ChanW_UTF& iChanW)
	{ ZYad_JSON::sToStrim(sYadR(iVal), iChanW); }

void sWrite(bool iPrettyPrint, const Val_Any& iVal, const ChanW_UTF& iChanW)
	{
	if (iPrettyPrint)
		ZYad_JSON::sToStrim(0, ZYadOptions(true), sYadR(iVal), iChanW);
	else
		ZYad_JSON::sToStrim(sYadR(iVal), iChanW);
	}

string8 sAsJSON(const Val_Any& iVal)
	{
	string8 result;
	sWrite(iVal, ChanW_UTF_string8(&result));
	return result;
	}

const ZVal_Any sFromJSON(const string8& iString)
	{
	ZRef<ZStrimmerU> theStrimmerU = new ZStrimmerU_T<ZStrimU_String>(iString);
	return sQRead(theStrimmerU).Get();
	}

} // namespace Util_Any_JSON

namespace Operators_Any_JSON {

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Val_Any& iVal)
	{
	Util_Any_JSON::sWrite(iVal, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Map_Any& iMap)
	{
	Util_Any_JSON::sWrite(iMap, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Seq_Any& iSeq)
	{
	Util_Any_JSON::sWrite(iSeq, iChanW);
	return iChanW;
	}

const ChanW_UTF& operator<<(const ChanW_UTF& iChanW, const Data_Any& iData)
	{
	Util_Any_JSON::sWrite(iData, iChanW);
	return iChanW;
	}

} // namespace Operators_Any_JSON

} // namespace ZooLib
