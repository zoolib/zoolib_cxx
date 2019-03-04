#include "zoolib/GameEngine/Sound.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Sound

Sound::Sound()
:	fDetached(false)
	{}

Sound::~Sound()
	{}

// =================================================================================================
// MARK: - SoundMeister

static ZRef<SoundMeister> spSoundMeister;

ZRef<SoundMeister> SoundMeister::sGet()
	{ return spSoundMeister; }

SoundMeister::SoundMeister()
	{ spSoundMeister = this; }

SoundMeister::~SoundMeister()
	{}

} // namespace GameEngine
} // namespace ZooLib
