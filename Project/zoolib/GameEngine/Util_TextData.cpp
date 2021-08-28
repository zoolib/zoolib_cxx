// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Util_TextData.h"

#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"
#include "zoolib/ChanRU_UTF_Std.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For ThreadVal_NameUniquifier
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_JSON.h"
#include "zoolib/Util_File.h"
#include "zoolib/Util_string.h"
#include "zoolib/Util_ZZ_JSON.h"

namespace ZooLib {
namespace GameEngine {

using namespace Util_string;

// =================================================================================================
#pragma mark -

static ZQ<Map_ZZ> spQReadMap(const ChanRU_UTF& iChanRU)
	{
	Util_Chan_JSON::PullTextOptions_JSON theOptions;
	theOptions.fAllowUnquotedPropertyNames = true;
	theOptions.fAllowEquals = true;
	theOptions.fAllowSemiColons = true;
	theOptions.fAllowTerminators = true;
	theOptions.fLooseSeparators = true;
	if (ZQ<Val_ZZ> theQ = Util_ZZ_JSON::sQRead(iChanRU, theOptions))
		return theQ->QGet<Map_ZZ>();

	return null;
	}

ZQ<Map_ZZ> sQReadMap_ZZ(const ChanR_Bin& iChanR, const string8* iName)
	{
	ThreadVal_NameUniquifier theNU;

	ChanR_XX_Buffered<ChanR_Bin> theChanR_Buffered(iChanR, 4096);
	ChanR_UTF_Chan_Bin_UTF8 theChanR_UTF(theChanR_Buffered);
	ChanRU_UTF_Std theChanRU_UTF(theChanR_UTF);

	try { return spQReadMap(theChanRU_UTF); }
	catch (std::exception& ex)
		{
		if (ZLOGF(w, eErr))
			{
			w << "Caught exception: " << ex.what();
			if (iName)
				w << " in file: " << *iName;
			w << ", line: " << theChanRU_UTF.GetLine() + 1;
			w << ", char: " << theChanRU_UTF.GetColumn() + 1;
			}
		}
	return null;
	}

ZQ<Map_ZZ> sQReadMap_ZZ(const ChanR_Bin& iChanR, const string8& iName)
	{ return sQReadMap_ZZ(iChanR, &iName); }

ZQ<Map_ZZ> sQReadMap_ZZ(const FileSpec& iFS)
	{
	if (ZP<ChannerR_Bin> channerR = iFS.OpenR())
		return sQReadMap_ZZ(*channerR, iFS.Name());
	return null;
	}

Map_ZZ sReadTextData(const FileSpec& iFS)
	{
	Map_ZZ theMap;
	for (FileIter iter = Util_File::sSorted(iFS); iter; iter.Advance())
		{
		const string theName = iter.CurrentName();
		if (false)
			{}
		else if (theName.empty() || ispunct(theName.at(0)))
			{}
		else
			{
			ZQ<string8> theQ = sQWithoutSuffix(theName, ".txt");

			if (not theQ)
				theQ = sQWithoutSuffix(theName, ".vals");

			if (theQ)
				{
				if (ZP<ChannerR_Bin> channerR = iter.Current().OpenR())
					{
					// To handle multiple maps in a single file, sQReadMap_ZZ needs to be
					// refactored so we use the *same* buffer between invocations
					if (ZQ<Map_ZZ> theQ = sQReadMap_ZZ(*channerR, theName))
						theMap = sAugmented(theMap, *theQ);
					}
				}
			}
		}
	return theMap;
	}

} // namespace GameEngine
} // namespace ZooLib
