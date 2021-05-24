// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#if defined(__ANDROID__)

#include "zoolib/File_Archive.h"
#include "zoolib/Log.h"
#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/RenderGame.h" // For GameEngine::sPixelToGame
#include "zoolib/GameEngine/Types.h"
#include "zoolib/GameEngine/Util.h"

#include "zoolib/JNI/JNI.h"

#include "zoolib/Zip/Archive_Zip.h"

// =================================================================================================
#pragma mark -

using std::map;
using std::string;
using std::vector;

using namespace ZooLib;
using namespace ZooLib::GameEngine;
using namespace ZooLib::Util_STL;

// =================================================================================================

class GameIntermediary
:	public Counted
	{
	ZMtx fMtx;

	struct TouchInfo
		{
		int fPointerId;
		int fAction;
		int fX;
		int fY;
		};

	vector<TouchInfo> fTouches;

	typedef map<int64,ZP<Touch> > TouchMap;

	TouchMap fTouchesAll;

public:
	ZP<GameEngine::Game> fGame;

	GameIntermediary(const string& iAPK)
		{
		ZP<SoundMeister> theSoundMeister;
		ZP<Callable_Void> theHaptic;

		FileSpec resourceFS = sFileSpec_Archive(sArchive_Zip(iAPK)).Child("assets");

		fGame = GameEngine::sMakeGame(resourceFS, false, theSoundMeister, theHaptic);
		}

	virtual ~GameIntermediary()
		{}

	void OnTouch(jint pointerId, jint action, jfloat x, jfloat y, jfloat p)
		{
		if (ZLOGF(w, eErr))
			w << "pointerId: " << pointerId << ", action: " << action << ", x: " << x << ", y: " << y << ", p: " << p;

		ZAcqMtx axq(fMtx);
		TouchInfo theTI;

		theTI.fPointerId= pointerId;
		theTI.fAction = action;
		theTI.fX = x;
		theTI.fY = y;

		fTouches.push_back(theTI);
		}

	void UpdateTouches(const GPoint& iPixelSize)
		{
		const GPoint theGameSize = fGame->GetGameSize();

		TouchSet theTouchesDown;
		TouchSet theTouchesMove;
		TouchSet theTouchesUp;

		ZAcqMtx acq(fMtx);
		foreachv (const TouchInfo& theTI, fTouches)
			{
			ZP<Touch> theTouch;
			if (ZQ<ZP<Touch> > theQ = sQGet(fTouchesAll, theTI.fPointerId))
				{
				theTouch = *theQ;
				}
			else
				{
				theTouch = new Touch;
				theTouch->fFingerID = theTI.fPointerId;
				sInsertMust(fTouchesAll, theTI.fPointerId, theTouch);
				}

			theTouch->fPos = sHomogenous(GameEngine::sPixelToGame(
				iPixelSize, theGameSize, sGPoint(theTI.fX, theTI.fY)));

			enum
				{
				ACTION_DOWN = 0,
				ACTION_UP = 1,
				ACTION_MOVE = 2,
				ACTION_CANCEL = 3,
				ACTION_OUTSIDE = 4,
				ACTION_POINTER_DOWN = 5,
				ACTION_POINTER_UP = 6,
				};

			switch (theTI.fAction)
				{
				case ACTION_DOWN:
				case ACTION_POINTER_DOWN:
					{
					sInsertMust(theTouchesDown, theTouch);
					break;
					}
				case ACTION_UP:
				case ACTION_POINTER_UP:
				case ACTION_CANCEL:
					{
					sInsertMust(theTouchesUp, theTouch);
					sEraseMust(fTouchesAll, theTI.fPointerId);
					break;
					}
				case ACTION_MOVE:
				case ACTION_OUTSIDE:
					{
					// The same touch may well have multiple moves reported, so we don't sInsertMust here.
					sQInsert(theTouchesMove, theTouch);
					break;
					}
				}
			}
		fTouches.clear();

		fGame->UpdateTouches(&theTouchesDown, &theTouchesMove, &theTouchesUp);
		}
	};

// =================================================================================================
#pragma mark -

extern void sTickleAndroid()
	{}

JNIFUNC(jlong,
Java_org_zoolib_ViewModel_1Game_nspInit(JNIEnv *env, jclass iClass, jstring iAPKPath))
	{
	JNI::EnvTV theEnv(env);
	ZP<GameIntermediary> theGI = new GameIntermediary(JNI::sAsString8(iAPKPath));
	theGI.Retain();
	return reinterpret_cast<jlong>(theGI.Get());
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npTearDown(JNIEnv *env, jobject ob,
	jlong iNative))
	{
	JNI::EnvTV theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		theGI->Release();
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npPauseGameLoop(JNIEnv *env, jobject ob,
	jlong iNative))
	{
	JNI::EnvTV theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		theGI->fGame->Pause();
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npResumeGameLoop(JNIEnv *env, jobject ob,
	jlong iNative))
	{
	JNI::EnvTV theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		theGI->fGame->Resume();
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npSurfaceAndContextIsFresh(JNIEnv *env, jobject ob,
	jlong iNative))
	{
	JNI::EnvTV theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		theGI->fGame->ExternalPurgeHasOccurred();
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npOnTouch(
	JNIEnv *env, jobject ob,
	jlong iNative,
	jint pointerId, jint action,
	jfloat x, jfloat y, jfloat p))
	{
	JNI::EnvTV theEnv(env);

	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		theGI->OnTouch(pointerId, action, x, y, p);
	}

JNIFUNC(void,
Java_org_zoolib_ViewModel_1Game_npDraw(
	JNIEnv *env, jobject ob,
	jlong iNative,
	jint width, jint height))
	{
	JNI::EnvTV theEnv(env);

	if (ZP<GameIntermediary> theGI = reinterpret_cast<GameIntermediary*>(iNative))
		{
		const double loopStart = Time::sSystem();

		const GPoint backingSize = sGPoint(width, height);

		theGI->fGame->Draw(loopStart, backingSize, null);
		theGI->UpdateTouches(backingSize);
		}
	}

#endif // defined(__ANDROID__)
