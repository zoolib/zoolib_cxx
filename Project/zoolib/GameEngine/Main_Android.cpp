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

#pragma clang diagnostic ignored "-Wmissing-prototypes"

#define JNIDEF(aa) extern "C" JNIEXPORT aa JNICALL

JNIDEF(jlong)
Java_org_zoolib_ViewModel_1Game_nspInit(JNIEnv *env, jclass iClass, jstring iAPKPath)
    {
	JNI::Env theEnv(env);

	ZP<Callable_Void> theHaptic;

    FileSpec resourceFS = sFileSpec_Zip(JNI::sAsString(iAPKPath)).Child("assets");

    ZP<GameEngine::Game> theGame = GameEngine::sMakeGame(resourceFS, false, null, theHaptic);
    theGame->Retain();
	return reinterpret_cast<jlong>(theGame.Get());
    }

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npPauseGameLoop(JNIEnv *env, jobject ob,
	jlong iNative)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative))
		theGame->Pause();
    }

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npResumeGameLoop(JNIEnv *env, jobject ob,
	jlong iNative)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative))
		theGame->Resume();
    }

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npTearDown(JNIEnv *env, jobject ob,
	jlong iNative)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative))
		theGame->Release();
    }

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npSurfaceAndContextIsFresh(JNIEnv *env, jobject ob,
	jlong iNative)
    {
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	if (ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative))
		theGame->ExternalPurgeHasOccurred();
    }

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npOnTouch(
    JNIEnv *env, jobject ob,
    jlong iNative,
    jint pointerId, jint action,
    jfloat x, jfloat y, jfloat p)
    {
	JNI::Env theEnv(env);

// 	if (ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative))

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

JNIDEF(void)
Java_org_zoolib_ViewModel_1Game_npDraw(
    JNIEnv *env, jobject ob,
    jlong iNative,
    jint width, jint height)
    {
	JNI::Env theEnv(env);

	ZP<GameEngine::Game> theGame = reinterpret_cast<GameEngine::Game*>(iNative);

    const double loopStart = Time::sSystem();

    const GPoint backingSize = sGPoint(width, height);

    theGame->Draw(loopStart, backingSize, null);

    TouchSet theTouchesDown;
    TouchSet theTouchesMove;
    TouchSet theTouchesUp;

    spUpdateTouches(backingSize, theGame->GetGameSize(),
        spTouchesAll,
        theTouchesDown, theTouchesMove, theTouchesUp);

    theGame->UpdateTouches(&theTouchesDown, &theTouchesMove, &theTouchesUp);
    }

#endif // defined(__ANDROID__)
