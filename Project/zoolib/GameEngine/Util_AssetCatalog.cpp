// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Util.h" // For sPixmap_PNG
#include "zoolib/GameEngine/Util_AssetCatalog.h"
#include "zoolib/GameEngine/Util_Assets.h"
#include "zoolib/GameEngine/Util_TextData.h" // For sQReadMap_ZZ

#include "zoolib/Log.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Compound.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using namespace Util_string;

using std::map;
using std::pair;

// =================================================================================================
#pragma mark - anonymous

namespace { // anonymous

Pixmap spPixmap_PNG(const FileSpec& iFileSpec)
	{
	if (Pixmap thePixmap = sPixmap_PNG(iFileSpec.OpenR()))
		return thePixmap;

	if (ZLOGF(w, eInfo))
		w << "Failed to load pixmap: " << iFileSpec.AsString();

	return null;
	}

ZP<Callable<Pixmap(const FileSpec&)> > spCallable_Pixmap_PNG = sCallable(spPixmap_PNG);

bool spPopulateBin(const ZP<AssetCatalog>& iAC,
	const ZP<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iProcessed, const FileSpec& iSheets)
	{
	if (ZP<ChannerR_Bin> channer = iProcessed.OpenR()) // Could use sOpenR_Buffered
		{
		for (FileIter iter = iSheets; iter; iter.Advance())
			{
			string theFileName = iter.CurrentName();
			if (ZQ<string8> theNameQ = sQWithoutSuffix(theFileName, ".png"))
				{
				ZP<AssetCatalog::Callable_TextureMaker> theCallable =
					sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, iter.Current()));
				iAC->InstallSheet(*theNameQ, theCallable);
				}
			}
		iAC->Set_Processed(sReadBin(*channer).Get<Map_ZZ>());
		return true;
		}
	return false;
	}

// std::map is used for oFiles to keep them sorted by name.
bool spReadAnim(const FileSpec& iParentAsFS,
	map<string8,FileSpec>& oFiles, Map_ZZ& oMap)
	{
	for (FileIter iter = iParentAsFS; iter; iter.Advance())
		{
		const string8 theName = iter.CurrentName();

		if (ZQ<string8> theQ = sQWithoutSuffix(theName, ".png"))
			oFiles[theName] = iter.Current();
		}

	if (not oFiles.empty())
		{
		if (ZP<ChannerR_Bin> theChanner = iParentAsFS.Child("meta.txt").OpenR())
			oMap = sGet(sQReadMap_ZZ(*theChanner, "meta.txt"));
		return true;
		}
	return false;
	}

void spInstall_Anim(
	const ZP<AssetCatalog>& iAC,
	const ZP<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iFS,
	Map_ZZ& ioMap)
	{
	map<string8,FileSpec> theFiles;
	Map_ZZ theMap;
	if (not spReadAnim(iFS, theFiles, theMap))
		return;

	const string8 prefix = iFS.Name();

	if (ZLOGF(w, eDebug))
		w << "Installing anim: " << prefix;

	Map_ZZ& entry = sMut<Map_ZZ>(ioMap[prefix]);
	entry = theMap;

	Seq_ZZ& theFrames = sMut<Seq_ZZ>(entry["Frames"]);

	int frameNumber = 0;
	foreacha (entry, theFiles)
		{
		const string8 theSheetName = prefix + sStringf("_%03d", frameNumber++);

		ZP<AssetCatalog::Callable_TextureMaker> theCallable =
			sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, entry.second));

		iAC->InstallSheet(theSheetName, theCallable);
		Map_ZZ theFrame;
		theFrame["SheetName"] = theSheetName;
		theFrames.Append(theFrame);
		}
	}

void spInstall_Art(
	const ZP<AssetCatalog>& iAC,
	const ZP<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iFS,
	Map_ZZ& ioMap)
	{
	if (iFS.IsDir())
		{
		for (FileIter iter = iFS; iter; iter.Advance())
			spInstall_Art(iAC, iTFP, iter.Current(), ioMap);
		}
	else
		{
		const string8 theName = iFS.Name();

		if (ZQ<string8> theQ = sQWithoutSuffix(theName, ".png"))
			{
			const string8 woSuffix = *theQ;
			const string8 theMeta = woSuffix + ".txt";

			Map_ZZ entry;
			if (ZP<ChannerR_Bin> theChanner = iFS.Sibling(theMeta).OpenR())
				entry = sGet(sQReadMap_ZZ(*theChanner, theMeta));

			Seq_ZZ& theFrames = sMut<Seq_ZZ>(entry["Frames"]);

			const string8 theSheetName = woSuffix;
			ZP<AssetCatalog::Callable_TextureMaker> theCallable =
				sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, iFS));
			iAC->InstallSheet(theSheetName, theCallable);
			Map_ZZ theFrame;
			theFrame["SheetName"] = theSheetName;
			theFrames.Append(theFrame);

			ioMap[woSuffix] = entry;
			if (ZLOGF(w, eDebug))
				w << "Installed art: " << theSheetName;
			}
		}
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -

void sPopulate(
	const ZP<AssetCatalog>& iAC,
	const FileSpec& iRoot,
	const ZP<Callable_TextureFromPixmap>& iTFP,
	bool iPreferProcessedArt, bool iPreferSmallArt)
	{
	const double startTime = Time::sSystem();

	bool gotArt = false;

	if (not gotArt && iPreferProcessedArt && iPreferSmallArt)
		gotArt = spPopulateBin(iAC, iTFP, iRoot.Child("processed2.bin"), iRoot.Child("sheet_2"));

	if (not gotArt && iPreferProcessedArt)
		gotArt = spPopulateBin(iAC, iTFP, iRoot.Child("processed1.bin"), iRoot.Child("sheet_1"));

	if (not gotArt && iPreferProcessedArt)
		gotArt = spPopulateBin(iAC, iTFP, iRoot.Child("processed2.bin"), iRoot.Child("sheet_2"));

	if (not gotArt)
		{
		Map_ZZ theMap;

		const FileSpec theFS_Anim = iRoot.Child("anim");
		if (not theFS_Anim.IsDir())
			{
			if (ZLOGF(w, eNotice))
				w << "missing 'anim'";			
			}
		else for (FileIter iter = theFS_Anim; iter; iter.Advance())
			{
			spInstall_Anim(iAC, iTFP, iter.Current(), theMap);
			}

		const FileSpec theFS_Art = iRoot.Child("art");
		if (not theFS_Art.IsDir())
			{
			if (ZLOGF(w, eNotice))
				w << "missing 'art'";
			}
		else for (FileIter iter = theFS_Art; iter; iter.Advance())
			{
			spInstall_Art(iAC, iTFP, iter.Current(), theMap);
			}

		iAC->Set_Processed(theMap);
		}

	if (ZLOGF(w, eNotice))
		sEWritef(w, "read stuff: %7.3fms", (Time::sSystem() - startTime) * 1000);
	}

} // namespace GameEngine
} // namespace ZooLib
