/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/Apple/PullPush_CF.h"

#include "zoolib/Apple/Chan_UTF_CFString.h"
#include "zoolib/Apple/Data_CF.h"
#include "zoolib/Apple/Util_CF.h"
#include "zoolib/Apple/Util_CF_Any.h"
#include "zoolib/Apple/Val_CF.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/Util_Chan.h"

namespace ZooLib {

using std::string;

using namespace PullPush;
using namespace Util_CF;

// =================================================================================================
#pragma mark -
#pragma mark

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	const ChanW_Any& theChanW = *((const ChanW_Any*)iRefcon);
	sPush(sName(sAsUTF8(static_cast<CFStringRef>(iKey))), theChanW);
	sPull_CF_Push(static_cast<CFTypeRef>(iValue), theChanW);
	}

bool sPull_CF_Push(CFTypeRef iCFTypeRef, const ChanW_Any& iChanW)
	{
	if (not iCFTypeRef)
		{
		sPush(Any(), iChanW);
		return true;
		}

	const CFTypeID theTypeID = ::CFGetTypeID(iCFTypeRef);

	if (theTypeID == ::CFStringGetTypeID())
		{
		sPull_UTF_Push(ChanR_UTF_CFString((CFStringRef)iCFTypeRef), iChanW);
		return true;
		}

	if (theTypeID == ::CFDataGetTypeID())
		{
		PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
		sPush(sGetClear(thePullPushPair.second), iChanW);

		if (size_t theLength = ::CFDataGetLength((CFDataRef)iCFTypeRef))
			sWriteFully(*thePullPushPair.first, ::CFDataGetBytePtr((CFDataRef)iCFTypeRef), theLength);

		sDisconnectWrite(*thePullPushPair.first);
		return true;
		}

	if (theTypeID == ::CFArrayGetTypeID())
		{
		CFArrayRef theCFArrayRef = (CFArrayRef)iCFTypeRef;
		sPush(PullPush::kStartSeq, iChanW);
		for (size_t xx = 0, theCount = ::CFArrayGetCount(theCFArrayRef); xx < theCount; ++xx)
			{
			if (not sPull_CF_Push(::CFArrayGetValueAtIndex(theCFArrayRef, xx), iChanW))
				return false;
			}
		sPush(PullPush::kEnd, iChanW);
		return true;
		}

	if (theTypeID == ::CFDictionaryGetTypeID())
		{
		sPush(PullPush::kStartMap, iChanW);
		::CFDictionaryApplyFunction((CFDictionaryRef)iCFTypeRef, spGatherContents, (void*)&iChanW);
		sPush(PullPush::kEnd, iChanW);
		return true;
		}

	if (ZQ<Any> theQ = sQSimpleAsAny(theTypeID, iCFTypeRef))
		{
		sPush(*theQ, iChanW);
		return true;
		}

	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark

static bool spPull_Push_CF(const Any& iAny, const ChanR_Any& iChanR, ZRef<CFTypeRef>& oCFTypeRef)
	{
	if (const string* theString = sPGet<string>(iAny))
		{
		oCFTypeRef = sString(*theString);
		return true;
		}

	if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iAny))
		{
		ZRef<CFMutableStringRef> theStringRef = sStringMutable();
		sCopyAll(*theChanner, ChanW_UTF_CFString(theStringRef));
		oCFTypeRef = theStringRef;
		return true;
		}

	if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iAny))
		{
		oCFTypeRef = sReadAll_T<Data_CF>(*theChanner);
		return true;
		}

	if (sPGet<PullPush::StartMap>(iAny))
		{
		Map_CF theMap;
		for (;;)
			{
			ZQ<Any> theNameOrEndQ = sQRead(iChanR);
			if (not theNameOrEndQ)
				return false;
			if (sPGet<PullPush::End>(*theNameOrEndQ))
				break;
			const Name* theNameStar = sPGet<Name>(*theNameOrEndQ);
			if (not theNameStar)
				return false;
			ZRef<CFTypeRef> theCFTypeRef;
			if (not sPull_Push_CF(iChanR, theCFTypeRef))
				return false;
			theMap.Set(*theNameStar, theCFTypeRef);
			}
		oCFTypeRef = theMap;
		return true;
		}

	if (sPGet<PullPush::StartSeq>(iAny))
		{
		Seq_CF theSeq;
		for (;;)
			{
			ZQ<Any> theQ = sQRead(iChanR);
			if (not theQ)
				return false;
			if (sPGet<PullPush::End>(*theQ))
				break;
			ZRef<CFTypeRef> theCFTypeRef;
			if (not spPull_Push_CF(*theQ, iChanR, theCFTypeRef))
				return false;
			theSeq.Append(theCFTypeRef);
			}
		oCFTypeRef = theSeq;
		return true;
		}

	oCFTypeRef = sAsCFType(iAny);

	return true;
	}

bool sPull_Push_CF(const ChanR_Any& iChanR, ZRef<CFTypeRef>& oCFTypeRef)
	{
	ZQ<Any> theQ = sQRead(iChanR);
	if (not theQ)
		return false;
	return spPull_Push_CF(*theQ, iChanR, oCFTypeRef);
	}

ZRef<CFTypeRef> sPull_CF(const ChanR_Any& iChanR)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		ZRef<CFTypeRef> theCFTypeRef;
		if (spPull_Push_CF(*theQ, iChanR, theCFTypeRef))
			return theCFTypeRef;
		}
	return null;
	}

} // namespace ZooLib
