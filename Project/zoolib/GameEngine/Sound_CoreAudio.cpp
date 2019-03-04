#include "zoolib/GameEngine/Sound_CoreAudio.h"

#if not defined(__ANDROID__) 

#include "zoolib/Log.h"
#include "zoolib/Memory.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZThread_pthread_more.h"

#include <AudioToolbox/AudioToolbox.h>

#include <vector>

namespace ZooLib {
namespace GameEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
// MARK: - SoundMeister_CoreAudio::PSound

class SoundMeister_CoreAudio::PSound
:	public Sound
	{
public:
	PSound(ExtAudioFileRef iEAFR)
	:	fEAFR(iEAFR)
	,	fOK(true && iEAFR)
		{}

	virtual ~PSound()
		{
		if (fEAFR)
			::ExtAudioFileDispose(fEAFR);
		}

// From Sound
	virtual bool IsFinished()
		{ return not fOK; }
	
// Our protocol
	bool Callback(void* ioDest, size_t iCount)
		{
		if (fOK)
			{
			AudioBufferList theABL;
			theABL.mNumberBuffers = 1;
			theABL.mBuffers[0].mNumberChannels = 2;
			theABL.mBuffers[0].mDataByteSize = iCount;
			theABL.mBuffers[0].mData = ioDest;

			const size_t countToRead = iCount / 4;
			UInt32 countRead = countToRead;
			if (noErr == ::ExtAudioFileRead(fEAFR, &countRead, &theABL))
				{
				if (countToRead > countRead)
					{
					sMemZero(((char*)ioDest) + 4 * countRead, 4 * (countToRead - countRead));
					fOK = false;
					}
				return true;
				}
			}

		fOK = false;
		return false;
		}

	ExtAudioFileRef fEAFR;
	bool fOK;
	};

// =================================================================================================
// MARK: - SoundMeister_CoreAudio

SoundMeister_CoreAudio::SoundMeister_CoreAudio(const FileSpec& iFS)
:	fAudioUnit(nullptr)
,	fFS(iFS)
,	fPaused(false)
	{
	#if 1 && not ZMACRO_IOS_Device
		fScopes["Music"] = Gain(1.0/25);
	#endif
	
	sMemZero_T(fASBD);
	fASBD.mFormatID = kAudioFormatLinearPCM;
	fASBD.mFormatFlags |= kLinearPCMFormatFlagIsPacked;
	fASBD.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
	fASBD.mSampleRate = 44100;
	fASBD.mChannelsPerFrame = 2;
	fASBD.mBitsPerChannel = 16;
	fASBD.mFramesPerPacket = 1;
	fASBD.mBytesPerFrame = fASBD.mBitsPerChannel * fASBD.mChannelsPerFrame / 8;
	fASBD.mBytesPerPacket = fASBD.mBytesPerFrame * fASBD.mFramesPerPacket;

	#if ZMACRO_IOS
	#else
	AudioDeviceID theAudioDeviceID = 0;
	::AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
		sMutablePtr(UInt32(sizeof theAudioDeviceID)), &theAudioDeviceID);
	#endif

	AudioComponentDescription desc = {};
	desc.componentType = kAudioUnitType_Output;

	#if ZMACRO_IOS
		desc.componentSubType = kAudioUnitSubType_RemoteIO;
	#else
		desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	#endif

	desc.componentManufacturer = kAudioUnitManufacturer_Apple;

	// AudioComponentFindNext etc are only available on 10.6+. Set up function refs
	// on older OSes?
	if (AudioComponent theComp = ::AudioComponentFindNext(0, &desc))
		{
		if (0 == ::AudioComponentInstanceNew(theComp, &fAudioUnit))
			{
			#if ZMACRO_IOS
			#else
			if (noErr == ::AudioUnitSetProperty(fAudioUnit,
				kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0,
				&theAudioDeviceID, sizeof(theAudioDeviceID)))
			#endif
				{
				if (noErr == ::AudioUnitSetProperty(fAudioUnit,
					kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0,
					&fASBD, sizeof(fASBD)))
					{
					AURenderCallbackStruct theAURCS = {};
					theAURCS.inputProc = spCallback;
					theAURCS.inputProcRefCon = this;
					if (noErr == ::AudioUnitSetProperty(fAudioUnit,
						kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0,
						&theAURCS, sizeof(theAURCS)))
						{
						if (noErr == ::AudioUnitInitialize(fAudioUnit))
							{
							if (noErr == ::AudioOutputUnitStart(fAudioUnit))
								{
								}
							}
						}
					}
				}
			}
		}
	}

SoundMeister_CoreAudio::~SoundMeister_CoreAudio()
	{
	if (fAudioUnit)
		{
		::AudioOutputUnitStop(fAudioUnit);
		::AudioUnitUninitialize(fAudioUnit);
		::AudioComponentInstanceDispose(fAudioUnit);
		}
	}

void SoundMeister_CoreAudio::SetSounds(const ZRef<Sound>* iSounds, size_t iCount)
	{
	set<ZRef<PSound> > newPSounds;

	while (iCount--)
		{
		if (ZRef<PSound> thePSound = (*iSounds++).DynamicCast<PSound>())
			newPSounds.insert(thePSound);
		}

	ZAcqMtx acq(fMtx);

	swap(fPSounds, newPSounds);
	}

ZRef<Sound> SoundMeister_CoreAudio::MakeSound(const string8& iName)
	{
	const FileSpec theFS = fFS.Child(iName);
	if (theFS.IsFile())
		{
		const string8 asString = theFS.AsString();
		if (ZRef<CFURLRef> theURL = sAdopt& ::CFURLCreateFromFileSystemRepresentation(0,
			(const UInt8*)(&asString[0]), asString.length(), false))
			{
			ExtAudioFileRef theEAFR;
			if (noErr == ::ExtAudioFileOpenURL(theURL, &theEAFR))
				{
				OSErr err;

				AudioStreamBasicDescription theASBD = {};

				err = ::ExtAudioFileGetProperty(theEAFR,
					kExtAudioFileProperty_FileDataFormat,
					sMutablePtr(UInt32(sizeof theASBD)), &theASBD);

				err = ::ExtAudioFileSetProperty(theEAFR,
					kExtAudioFileProperty_ClientDataFormat,
					sizeof(fASBD), &fASBD);

				return new PSound(theEAFR);
				}
			}
		}
	return null;
	}

void SoundMeister_CoreAudio::SetPaused(bool iPaused)
	{
	ZAcqMtx acq(fMtx);
	fPaused = iPaused;
	}

void SoundMeister_CoreAudio::pCallback(void* ioDest, size_t iCount)
	{
	ZAcqMtx acq(fMtx);
	vector<ZRef<PSound> > vec(fPSounds.begin(), fPSounds.end());

	ZRelMtx rel(fMtx);

	sMemZero(ioDest, iCount);

	if (fPaused)
		return;

	if (fBuffer.size() < iCount)
		fBuffer.resize(iCount);

	void* buf = &fBuffer[0];
	const size_t frameCount = iCount / 4;
	foreachv (ZRef<PSound> thePSound, vec)
		{
		if (thePSound && thePSound->Callback(buf, iCount))
			{
			const int16* src = (const int16*)buf;
			int16* dst = (int16*)ioDest;
			Gain theGain = thePSound->fGain;
			
			if (ZQ<Gain> theQ = Util_STL::sQGet(fScopes, thePSound->fScope))
				theGain *= *theQ;
			
			for (size_t xx = 0; xx < frameCount; ++xx)
				{
				dst[0] = sMinMax<float>(-0x8000, theGain.fL * src[0] + dst[0], 0x7FFF);
				dst[1] = sMinMax<float>(-0x8000, theGain.fR * src[1] + dst[1], 0x7FFF);
				src += 2;
				dst += 2;
				}
			}
		}
	}

OSStatus SoundMeister_CoreAudio::spCallback(
	void* inRefCon,
	AudioUnitRenderActionFlags * ioActionFlags,
	const AudioTimeStamp* inTimeStamp,
	UInt32 inBusNumber,
	UInt32 inNumberFrames,
	AudioBufferList* ioData)
	{
	#if ZCONFIG_Debug
		ZThread::sSetName("SoundMeister_CoreAudio");
	#endif

	AudioBuffer& theAB = ioData->mBuffers[0];
	
	static_cast<SoundMeister_CoreAudio*>(inRefCon)->pCallback(theAB.mData, theAB.mDataByteSize);

	return 0;
	}

} // namespace GameEngine
} // namespace ZooLib

#endif // not defined(__ANDROID__) 
