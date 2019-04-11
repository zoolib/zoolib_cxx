#include "zoolib/GameEngine/Sound.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Sound

Sound::Sound()
:	fDetached(false)
	{}

Sound::~Sound()
	{}

// =================================================================================================
#pragma mark - SoundMeister

static ZRef<SoundMeister> spSoundMeister;

ZRef<SoundMeister> SoundMeister::sGet()
	{ return spSoundMeister; }

SoundMeister::SoundMeister()
	{ spSoundMeister = this; }

SoundMeister::~SoundMeister()
	{}

} // namespace GameEngine
} // namespace ZooLib
