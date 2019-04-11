#ifndef __ZooLib_GameEngine_Sound_CoreAudio_h__
#define __ZooLib_GameEngine_Sound_CoreAudio_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/Sound.h"

#if not defined(__ANDROID__) 

#include "zoolib/File.h"

#include <map>
#include <set>
#include <vector>

#include <AudioUnit/AudioUnit.h>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - SoundMeister_CoreAudio

class SoundMeister_CoreAudio
:	public SoundMeister
	{
public:
	SoundMeister_CoreAudio(const FileSpec& iFS);
	virtual ~SoundMeister_CoreAudio();

	virtual void SetSounds(const ZRef<Sound>* iSounds, size_t iCount);

	virtual ZRef<Sound> MakeSound(const string8& iName);

// Our protocol
	void SetPaused(bool iPaused);

private:
	void pCallback(void* ioDest, size_t iCount);

	static OSStatus spCallback(
		void* inRefCon,
		AudioUnitRenderActionFlags * ioActionFlags,
		const AudioTimeStamp* inTimeStamp,
		UInt32 inBusNumber,
		UInt32 inNumberFrames,
		AudioBufferList* ioData);

	AudioStreamBasicDescription fASBD;
	AudioUnit fAudioUnit;

	ZMtx fMtx;
	const FileSpec fFS;

	class PSound;
	std::set<ZRef<PSound> > fPSounds;
	std::vector<char> fBuffer;

	bool fPaused;

	std::map<string8,Gain> fScopes;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // not defined(__ANDROID__) 

#endif // __ZooLib_GameEngine_Sound_CoreAudio_h__
