#if defined(__ANDROID__)

#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/Game_Render.h"
#include "zoolib/GameEngine/Types.h"
#include "zoolib/GameEngine/Util.h"

#include "zoolib/JNI/JNI.h"

// =================================================================================================
// MARK: -

using std::map;
using std::vector;

using namespace ZooLib;
using namespace ZooLib::GameEngine;

const double kUpdateRate = 30.0;

static GPoint spWinSize = sPoint<GPoint>(480, 320);

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

typedef map<int64,ZRef<Touch> > TouchMap;

} // anonymous namespace

// =================================================================================================

static jclass spClass_Activity;

static bool spCreateContext(jint majorVersion, jint minorVersion)
	{
	JNIEnv* env = JNI::Env::sGet();
	jmethodID mid = env->GetStaticMethodID(spClass_Activity, "createGLContext", "(II)Z");

	return env->CallStaticBooleanMethod(spClass_Activity, mid, majorVersion, minorVersion);
	}

static void spSwapWindow()
	{
	JNIEnv* env = JNI::Env::sGet();
	jmethodID mid = env->GetStaticMethodID(spClass_Activity, "flipBuffers", "()V");
	env->CallStaticVoidMethod(spClass_Activity, mid);
	}

static ZRef<Callable_Void> spCallable_SwapWindow = sCallable(spSwapWindow);

static void spSetTitle(const char *title)
	{
	JNIEnv* env = JNI::Env::sGet();
	if (jmethodID mid = env->GetStaticMethodID(spClass_Activity, "setActivityTitle", "(Ljava/lang/String;)V"))
		env->CallStaticVoidMethod(spClass_Activity, mid, env->NewStringUTF(title));
	}

// =================================================================================================
// MARK: - Library load and unload

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
// MARK: - SDLActivity native methods

void sRun(const string8& iAPK);

extern "C"
void Java_org_zoolib_SDLActivity_sRun
	(JNIEnv* env, jclass cls,
	jstring iAPK)
	{
	JNI::Env theEnv(env);

	spClass_Activity = (jclass)env->NewGlobalRef(cls);

	string8 theString;
	if (const jchar* theJString = env->GetStringChars(iAPK, nullptr))
		{
		if (jsize theLength = env->GetStringLength(iAPK))
			theString = Unicode::sAsUTF8((const UTF16*)theJString, theLength);
		env->ReleaseStringChars(iAPK, theJString);
		}

	spCreateContext(2, 0);

	sRun(theString);

	exit(0);
	}

bool spKeepRunning = true;

extern "C"
void Java_org_zoolib_SDLActivity_sQuit
	(JNIEnv* env, jclass cls)
	{
	ZLOGTRACE(eDebug);
	spKeepRunning = false;
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnPause
	(JNIEnv* env, jclass jcls,
	jint keycode)
	{
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnResume
	(JNIEnv* env, jclass jcls,
	jint keycode)
	{
	JNI::Env theEnv(env);
	ZLOGTRACE(eDebug);
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnResize
	(JNIEnv* env, jclass jcls,
	jint width, jint height, jint format)
	{
	JNI::Env theEnv(env);
	if (ZLOGF(w, eErr))
		w << "width: " << width << ", height:" << height << ", format:" << format;

	X(spWinSize) = width;
	Y(spWinSize) = height;
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnKeyDown
	(JNIEnv* env, jclass jcls,
	jint keycode)
	{
	JNI::Env theEnv(env);
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnKeyUp
	(JNIEnv* env, jclass jcls,
	jint keycode)
	{
	JNI::Env theEnv(env);
	}

extern "C"
void Java_org_zoolib_SDLActivity_sOnTouch
	(JNIEnv* env, jclass jcls,
	jint pointerId, jint action, jfloat x, jfloat y, jfloat p)
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

extern "C"
void Java_org_zoolib_SDLActivity_sOnAccel
	(JNIEnv* env, jclass jcls,
	jfloat x, jfloat y, jfloat z)
	{
	JNI::Env theEnv(env);
	}

// =================================================================================================

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
		ZRef<Touch> theTouch;
		if (ZQ<ZRef<Touch> > theQ = sQGet(ioAll, theTI.fPointerId))
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

static TouchMap spTouchesAll;

bool ZooLib_RunOnce(const ZRef<Game>& iGame)
	{
	const GPoint backingSize = spWinSize;

	const double loopStart = Time::sSystem();

	iGame->Draw(loopStart, backingSize, spCallable_SwapWindow);

	TouchSet theTouchesDown;
	TouchSet theTouchesMove;
	TouchSet theTouchesUp;

	spUpdateTouches(backingSize, iGame->GetGameSize(),
		spTouchesAll,
		theTouchesDown, theTouchesMove, theTouchesUp);

	iGame->UpdateTouches(&theTouchesDown, &theTouchesMove, &theTouchesUp);

	return spKeepRunning;
	}

#endif // defined(__ANDROID__)
