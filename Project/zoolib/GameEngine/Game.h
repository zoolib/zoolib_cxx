#ifndef __ZooLib_GameEngine_Game_h__
#define __ZooLib_GameEngine_Game_h__ 1
#include "zconfig.h"

#include "zoolib/File.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/Cog.h"
#include "zoolib/GameEngine/Sound.h"
#include "zoolib/GameEngine/Touch.h"
#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

void sRotateGameRate();

void sSetGameRate(Rat iRate);
Rat sGetGameRate();

// =================================================================================================
// MARK: -

class Game
:	public ZCounted
	{
public:
	Game(const FileSpec& iFS,
		const ZRef<Callable_TextureFromPixmap>& iCallable_TextureFromPixmap,
		bool iPreferProcessedArt, bool iPreferSmallArt,
		const ZRef<SoundMeister>& iSoundMeister);

	virtual ~Game();

// From ZCounted
	virtual void Initialize();

// Our protocol
	void Purge();

	ZRef<NookScope> GetNookScope();

	void UpdateTouches(const TouchSet* iDown, const TouchSet* iMove, const TouchSet* iUp);

	void Draw(
		double iNewTimestamp,
		GPoint iBackingSize,
		GPoint iGameSize,
		bool iUseShader,
		const ZRef<Callable_Void>& iCallable_FlipBuffers);

	void RunOnce(
		GPoint iBackingSize,
		GPoint iGameSize);

private:
	void pUpdateTouches();

	ZRef<Rendered> pCrank(double iInterval);

	ZRef<Rendered> fRendered;
	ZRef<Rendered> fRendered_Prior;

	ZMtx fMtx_Game;
	ZCnd fCnd_Game;

	double fTimestamp_LatestDrawn;
	double fTimestamp_ToDraw;

	ZRef<NookScope> fNookScope;

	ZRef<AssetCatalog> fAssetCatalog;

	ZRef<SoundMeister> fSoundMeister;

	std::vector<ZRef<TouchListener> > fTLs;
	std::map<ZRef<Touch>,ZRef<TouchListener> > fExclusiveTouches;

	TouchSet fTouchesActive;
	TouchSet fTouchesUp;

	TouchSet fPendingTouchesDown;
	TouchSet fPendingTouchesMove;
	TouchSet fPendingTouchesUp;

	uint64 fNextEra;
	double fAccumulated;
	Cog fCog;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Game_h__
