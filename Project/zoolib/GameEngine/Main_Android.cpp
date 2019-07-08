// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#if defined(__ANDROID__)

#include "zoolib/Log.h"
#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/DebugFlags.h"
#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/RenderGame.h" // For GameEngine::sPixelToGame
#include "zoolib/GameEngine/Types.h"
#include "zoolib/GameEngine/Util.h"

#include "zoolib/JNI/JNI.h"

#include "zoolib/Zip/File_Zip.h"

using std::map;
using std::vector;

using namespace ZooLib;
using namespace GameEngine;

// =================================================================================================
// MARK: -

using std::map;
using std::vector;

using namespace ZooLib;
using namespace ZooLib::GameEngine;

// =================================================================================================
// MARK: -

namespace { // anonymous

ZMtx sMtx;

struct TouchInfo
	{
    int fPointerId;
	int fAction;
	int fX;
	int fY;
	};

vector<TouchInfo> sTouches;

typedef map<int64,ZP<Touch> > TouchMap;

} // anonymous namespace

// =================================================================================================

extern "C"
jint JNI_OnLoad(JavaVM* vm, void* reserved)
	{
	Util_Debug::sInstall();

	Util_Debug::sSetLogPriority(7);

	return JNI_VERSION_1_4;
	}

extern "C"
void JNI_OnUnload(JavaVM* vm, void* reserved)
	{}

// =================================================================================================

static ZP<GameEngine::Game> spGame;

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sInit(JNIEnv *env, jclass type, jstring iAPKPath_)
    {
    const char *iAPKPath = env->GetStringUTFChars(iAPKPath_, 0);

    FileSpec resourceFS = sFileSpec_Zip(iAPKPath).Child("assets");
    spGame = GameEngine::sMakeGame(resourceFS, false);

    env->ReleaseStringUTFChars(iAPKPath_, iAPKPath);
    }

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sPauseGameLoop(JNIEnv *env, jclass type)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	spGame->Pause();
    }

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sResumeGameLoop(JNIEnv *env, jclass type)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	spGame->Resume();
    }

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sTearDown(JNIEnv *env, jclass type)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
    }

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sSurfaceAndContextIsFresh(JNIEnv *env, jclass type)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	spGame->ExternalPurgeHasOccurred();
    }

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sOnTouch(
    JNIEnv *env, jclass type,
    jint pointerId, jint action,
    jfloat x, jfloat y, jfloat p)
    {
	JNI::Env theEnv(env);

	if (ZLOGF(w, eErr))
		w << "pointerId: " << pointerId << ", action: " << action << ", x: " << x << ", y: " << y << ", p: " << p;

	ZAcqMtx axq(sMtx);
	TouchInfo theTI;

	theTI.fPointerId= pointerId;
	theTI.fAction = action;
	theTI.fX = x;
	theTI.fY = y;

	sTouches.push_back(theTI);
    }

static TouchMap spTouchesAll;

static void spUpdateTouches(
	const GPoint& iPixelSize, const GPoint& iGameSize,
	TouchMap& ioAll,
	TouchSet& ioDown,
	TouchSet& ioMove,
	TouchSet& ioUp)
	{
	using namespace Util_STL;

	ZAcqMtx acq(sMtx);
	foreachv (const TouchInfo& theTI, sTouches)
		{
		ZP<Touch> theTouch;
		if (ZQ<ZP<Touch> > theQ = sQGet(ioAll, theTI.fPointerId))
			{
			theTouch = *theQ;
			}
		else
			{
			theTouch = new Touch;
			theTouch->fFingerID = theTI.fPointerId;
			sInsertMust(ioAll, theTI.fPointerId, theTouch);
			}

		theTouch->fPos = sHomogenous(GameEngine::sPixelToGame(
			iPixelSize, iGameSize, sGPoint(theTI.fX, theTI.fY)));

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
				sInsertMust(ioDown, theTouch);
				break;
				}
            case ACTION_UP:
            case ACTION_POINTER_UP:
            case ACTION_CANCEL:
            	{
            	sInsertMust(ioUp, theTouch);
				sEraseMust(ioAll, theTI.fPointerId);
				break;
				}
            case ACTION_MOVE:
            case ACTION_OUTSIDE:
            	{
            	// The same touch may well have multiple moves reported, so we don't sInsertMust here.
            	sQInsert(ioMove, theTouch);
				break;
				}
			}
		}
	sTouches.clear();
	}

extern "C"
JNIEXPORT void JNICALL
Java_org_zoolib_ViewModel_1Game_sDraw(
    JNIEnv *env, jclass type,
    jint width, jint height)
    {
	JNI::Env theEnv(env);

    const double loopStart = Time::sSystem();

    const GPoint backingSize = sGPoint(width, height);

    spGame->Draw(loopStart, backingSize, null);

    TouchSet theTouchesDown;
    TouchSet theTouchesMove;
    TouchSet theTouchesUp;

    spUpdateTouches(backingSize, spGame->GetGameSize(),
        spTouchesAll,
        theTouchesDown, theTouchesMove, theTouchesUp);

    spGame->UpdateTouches(&theTouchesDown, &theTouchesMove, &theTouchesUp);
    }

#endif // defined(__ANDROID__)
