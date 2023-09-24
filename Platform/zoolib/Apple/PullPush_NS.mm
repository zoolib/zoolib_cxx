// Copyright (c) 2023 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/PullPush_NS.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Apple/Chan_UTF_NSString.h"
#include "zoolib/Apple/Data_NS.h"
#include "zoolib/Apple/Util_NS.h"
#include "zoolib/Apple/Util_NS_ZZ.h"
#include "zoolib/Apple/Val_NS.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/UTCDateTime.h"

using std::string;

using namespace ZooLib;

namespace ZooLib {

using namespace PullPush;
using namespace Util_NS;

// =================================================================================================
#pragma mark - 

bool sFromNS_Push_PPT(NSObject* iNSObject, const ChanW_PPT& iChanW)
	{
	if (not iNSObject)
		{
		sPush(PPT(), iChanW);
		return true;
		}
	else
		{
		return [iNSObject pushPPT:iChanW];
		}
	}

// =================================================================================================
#pragma mark - 

static bool sPull_PPT_AsNS(const PPT& iPPT, const ChanR_PPT& iChanR, NSObject*& oNSObject)
	{
	if (const string* theString = sPGet<string>(iPPT))
		{
		oNSObject = sString(*theString);
		return true;
		}

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		{
		NSMutableString* theString = sStringMutable();
		sCopyAll(*theChanner, ChanW_UTF_NSString(theString));
		oNSObject = theString;
		return true;
		}

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		{
		oNSObject = sReadAll_T<Data_NS>(*theChanner);
		return true;
		}

	if (sIsStart_Map(iPPT))
		{
		NSMutableDictionary* theDictionary = sDictionaryMutable();
		for (;;)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else
				{
				NSObject* theNSObject;
				if (not sPull_PPT_AsNS(iChanR, theNSObject))
					return false;

				[theDictionary
					setObject:theNSObject
					forKey:sString(theNameQ->AsString8())];
				}
			}
		oNSObject = theDictionary;
		return true;
		}

	if (sIsStart_Seq(iPPT))
		{
		NSMutableArray* theArray = sArrayMutable();
		for (;;)
			{
			ZQ<PPT> theQ = sQRead(iChanR);
			if (not theQ)
				return false;

			if (sIsEnd(*theQ))
				break;

			NSObject* theNSObject;
			if (not sPull_PPT_AsNS(*theQ, iChanR, theNSObject))
				return false;

			[theArray addObject:theNSObject];
			}
		oNSObject = theArray;
		return true;
		}

	oNSObject = sDAsNSObject(nullptr, iPPT.As<Val_ZZ>());

	if (not oNSObject)
		oNSObject = [NSNull null];

	return true;
	}

bool sPull_PPT_AsNS(const ChanR_PPT& iChanR, NSObject*& oNSObject)
	{
	ZQ<PPT> theQ = sQRead(iChanR);
	if (not theQ)
		return false;
	return sPull_PPT_AsNS(*theQ, iChanR, oNSObject);
	}

NSObject* sAsNS(const ChanR_PPT& iChanR)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		NSObject* theNSObject;
		if (sPull_PPT_AsNS(*theQ, iChanR, theNSObject))
			return theNSObject;
		}
	return [NSNull null];
	}

} // namespace ZooLib

// =================================================================================================
@implementation NSObject (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	// Hmm, log it and do what?
	ZDebugLogf(0, ("NSObject (ZooLib_ZZ_Additions) pushPPT called"));
	return false;
	}
@end

// =================================================================================================
@interface NSNull (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSNull (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	sPush(PPT(), iChanW);
	return true;
	}

@end

// =================================================================================================
@interface NSDictionary (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSDictionary (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	sPush_Start_Map(iChanW);
	for (id theKey, ii = [self keyEnumerator]; (theKey = [ii nextObject]); /*no inc*/)
		{
		if ([theKey isKindOfClass:[NSString class]])
			{
			sPush(sName(sAsUTF8(static_cast<NSString*>(theKey))), iChanW);
			if (not [[self objectForKey:theKey] pushPPT:iChanW])
				return false;
			}
		}
	sPush_End(iChanW);
	return true;
	}

@end

// =================================================================================================
@interface NSArray (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSArray (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	sPush_Start_Seq(iChanW);
	for (id theValue, ii = [self objectEnumerator]; (theValue = [ii nextObject]); /*no inc*/)
		{
		if (not sFromNS_Push_PPT(theValue, iChanW))
			return false;
		}
	sPush_End(iChanW);
	return true;
	}

@end

// =================================================================================================
@interface NSData (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSData (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	if (size_t theLength = [self length])
		sWriteMemFully(*thePullPushPair.first, [self bytes], theLength);

	sDisconnectWrite(*thePullPushPair.first);
	return true;
	}

@end

// =================================================================================================
@interface NSString (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSString (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	sPull_UTF_Push_PPT(ChanR_UTF_NSString(self), iChanW);
	return true;
	}

@end

// =================================================================================================
@interface NSNumber (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSNumber (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	const char* subType = [self objCType];
	switch (subType[0])
		{
		case 'c': sPush([self charValue], iChanW); return true;
		case 'i': sPush([self intValue], iChanW); return true;
		case 's': sPush([self shortValue], iChanW); return true;
		case 'l': sPush([self longValue], iChanW); return true;
		case 'q': sPush([self longLongValue], iChanW); return true;

		case 'C': sPush([self unsignedCharValue], iChanW); return true;
		case 'I': sPush([self unsignedIntValue], iChanW); return true;
		case 'S': sPush([self unsignedShortValue], iChanW); return true;
		case 'L': sPush([self unsignedLongValue], iChanW); return true;
		case 'Q': sPush([self unsignedLongLongValue], iChanW); return true;

		case 'f': sPush([self floatValue], iChanW); return true;
		case 'd': sPush([self doubleValue], iChanW); return true;
		case 'B': sPush([self boolValue], iChanW); return true;
		}
	return false;
	}

@end

// =================================================================================================
@interface NSDate (ZooLib_pushPPT)
-(bool)pushPPT:(const ChanW_PPT&)iChanW;
@end

@implementation NSDate (ZooLib_pushPPT)

-(bool)pushPPT:(const ChanW_PPT&)iChanW
	{
	sPush(ZooLib::UTCDateTime([self timeIntervalSince1970]), iChanW);
	return true;
	}

@end
#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

//@"c" : @"char",
//@"i" : @"int",
//@"s" : @"short",
//@"l" : @"long",
//@"q" : @"long long",
//@"C" : @"unsigned char",
//@"I" : @"unsigned int",
//@"S" : @"unsigned short",
//@"L" : @"unsigned long",
//@"Q" : @"unsigned long long",
//@"f" : @"float",
//@"d" : @"double",
//@"B" : @"bool",
//@"v" : @"void",
//@"*" : @"char *",
//@"r" : @"const char",     /* Used with char * return types */
//@"@" : @"id",
//@"#" : @"Class",
//@":" : @"SEL",
//@"?" : @"*",
//@"{" : @"struct"
