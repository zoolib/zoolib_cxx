#ifndef __ZooLib_GameEngine_Sound_h__
#define __ZooLib_GameEngine_Sound_h__ 1
#include "zconfig.h"

#include "zoolib/Counted.h"
#include "zoolib/UnicodeString8.h"

#include "zoolib/GameEngine/Types.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Sound

class Sound
:	public ZCounted
	{
protected:
	Sound();

public:
	virtual ~Sound();
	
	virtual bool IsFinished() = 0;

	bool fDetached;
	Gain fGain;
	string8 fScope;
	};

// =================================================================================================
#pragma mark - SoundMeister

class SoundMeister
:	public ZCounted
	{
public:
	SoundMeister();
	virtual ~SoundMeister();

	virtual void SetSounds(const ZP<Sound>* iSounds, size_t iCount) = 0;

	virtual ZP<Sound> MakeSound(const string8& iName) = 0;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Sound_h__
