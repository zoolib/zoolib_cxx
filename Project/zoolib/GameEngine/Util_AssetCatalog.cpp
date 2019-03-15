#include "zoolib/GameEngine/Util.h" // For sPixmap_PNG
#include "zoolib/GameEngine/Util_AssetCatalog.h"
#include "zoolib/GameEngine/Util_Assets.h"

#include "zoolib/Log.h"
#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Compound.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"
//#include "zoolib/ZYad_FS.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace GameEngine {

using namespace Util_string;

using std::map;
using std::pair;

typedef FileSpec YadSpec;

// =================================================================================================
// MARK: - anonymous

namespace { // anonymous

ZDCPixmap spPixmap_PNG(const YadSpec& iYadSpec)
	{
	if (ZDCPixmap thePixmap = sPixmap_PNG(iYadSpec.OpenR()))
		return thePixmap;

	if (ZLOGF(w, eInfo))
		w << "Failed to load pixmap: " << iYadSpec.AsString();

	return null;
	}

ZRef<Callable<ZDCPixmap(const YadSpec&)> > spCallable_Pixmap_PNG = sCallable(spPixmap_PNG);

bool spPopulateBin(const ZRef<AssetCatalog>& iAC,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iProcessed, const FileSpec& iSheets)
	{
	if (ZRef<ChannerR_Bin> channer = iProcessed.OpenR()) // Could use sOpenR_Buffered
		{
		for (FileIter iter = iSheets; iter; iter.Advance())
			{
			string theFileName = iter.CurrentName();
			if (ZQ<string8> theNameQ = sQWithoutSuffix(".png", theFileName))
				{
				ZRef<AssetCatalog::Callable_TextureMaker> theCallable =
					sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, iter.Current()));
				iAC->InstallSheet(*theNameQ, theCallable);
				}
			}
		iAC->Set_Processed(sReadBin(*channer).Get<Map_Any>());
		return true;
		}
	return false;
	}

// std::map is used for oFiles to keep them sorted by name.
bool spReadAnim(const FileSpec& iParentAsFS,
	map<string8,YadSpec>& oFiles, Map_Any& oMap)
	{
	for (FileIter iter = iParentAsFS; iter; iter.Advance())
		{
		const string8 theName = iter.CurrentName();

		if (ZQ<string8> theQ = sQWithoutSuffix(".png", theName))
			oFiles[theName] = iter.Current();
		}

	if (not oFiles.empty())
		{
		if (ZRef<ChannerR_Bin> theChanner = iParentAsFS.Child("meta.txt").OpenR())
			oMap = sGet(sQReadMap_Any(*theChanner, "meta.txt"));
		return true;
		}
	return false;
	}

void spInstall_Anim(
	const ZRef<AssetCatalog>& iAC,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iFS,
	Map_Any& ioMap)
	{
	map<string8,YadSpec> theFiles;
	Map_Any theMap;
	if (not spReadAnim(iFS, theFiles, theMap))
		return;

	const string8 prefix = iFS.Name();

	if (ZLOGF(w, eDebug))
		w << "Installing anim: " << prefix;

	Map_Any& entry = sMut<Map_Any>(ioMap[prefix]);
	entry = theMap;

	Seq_Any& theFrames = sMut<Seq_Any>(entry["Frames"]);

	int frameNumber = 0;
	foreacha (entry, theFiles)
		{
		const string8 theSheetName = prefix + sStringf("_%03d", frameNumber++);

		ZRef<AssetCatalog::Callable_TextureMaker> theCallable =
			sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, entry.second));

		iAC->InstallSheet(theSheetName, theCallable);
		Map_Any theFrame;
		theFrame["SheetName"] = theSheetName;
		theFrames.Append(theFrame);
		}
	}

void spInstall_Art(
	const ZRef<AssetCatalog>& iAC,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
	const FileSpec& iFS,
	Map_Any& ioMap)
	{
	if (iFS.IsDir())
		{
		for (FileIter iter = iFS; iter; iter.Advance())
			spInstall_Art(iAC, iTFP, iter.Current(), ioMap);
		}
	else
		{
		const string8 theName = iFS.Name();

		if (ZQ<string8> theQ = sQWithoutSuffix(".png", theName))
			{
			const string8 woSuffix = *theQ;
			const string8 theMeta = woSuffix + ".txt";

			Map_Any entry;
			if (ZRef<ChannerR_Bin> theChanner = iFS.Sibling(theMeta).OpenR())
				entry = sGet(sQReadMap_Any(*theChanner, theMeta));

			Seq_Any& theFrames = sMut<Seq_Any>(entry["Frames"]);

			const string8 theSheetName = woSuffix;
			ZRef<AssetCatalog::Callable_TextureMaker> theCallable =
				sCallable_Apply(iTFP, sBindR(spCallable_Pixmap_PNG, iFS));
			iAC->InstallSheet(theSheetName, theCallable);
			Map_Any theFrame;
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
// MARK: -

void sPopulate(
	const ZRef<AssetCatalog>& iAC,
	const FileSpec& iRoot,
	const ZRef<Callable_TextureFromPixmap>& iTFP,
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
		Map_Any theMap;

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
