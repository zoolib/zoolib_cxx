// Copyright (c) 2015-2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/JNI/JNI.h"

#include <stdexcept> // for std::runtime_error

namespace ZooLib {
namespace JNI {

// =================================================================================================
#pragma mark - LoadHandler

LoadHandler* LoadHandler::spHead;

LoadHandler::LoadHandler()
	{
	fNext = spHead;
	spHead = this;
	}

LoadHandler::~LoadHandler()
	{}

void LoadHandler::sOnLoad(JavaVM* iJavaVM)
	{
	for (LoadHandler* theHandler = spHead; theHandler; theHandler = theHandler->fNext)
		theHandler->OnLoad(iJavaVM);
	}

void LoadHandler::sOnUnload()
	{
	for (LoadHandler* theHandler = spHead; theHandler; theHandler = theHandler->fNext)
		theHandler->OnUnload();
	}

void LoadHandler::OnLoad(JavaVM* iJavaVM)
	{
	JNIEnv* env;
	const jint result = iJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);

	// Mutst be called on a thread that's already attached, ie from JNI_OnLoad.
	ZAssert(result == JNI_OK);
	this->OnLoad(env);
	}

void LoadHandler::OnLoad(JNIEnv* iEnv)
	{}

void LoadHandler::OnUnload()
	{}

// =================================================================================================
#pragma mark -

namespace { // anonymous

struct LoadHandler_Core : public JNI::LoadHandler
	{
	void OnLoad(JNIEnv* iEnv);
	} spLoadHandler_Core;

jclass jclass_Boolean;
	jmethodID jmethodID_Boolean_Init;

jclass jclass_Character;
	jmethodID jmethodID_Character_Init;

jclass jclass_Byte;
	jmethodID jmethodID_Byte_Init;

jclass jclass_Short;
	jmethodID jmethodID_Short_Init;

jclass jclass_Integer;
	jmethodID jmethodID_Integer_Init;

jclass jclass_Long;
	jmethodID jmethodID_Long_Init;

jclass jclass_Float;
	jmethodID jmethodID_Float_Init;

jclass jclass_Double;
	jmethodID jmethodID_Double_Init;

void LoadHandler_Core::OnLoad(JNIEnv* env)
	{
	jclass_Boolean = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Boolean"));
		jmethodID_Boolean_Init = env->GetMethodID(jclass_Boolean, "<init>", "(Z)V");

	jclass_Character = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Character"));
		jmethodID_Character_Init = env->GetMethodID(jclass_Character, "<init>", "(C)V");

	jclass_Byte = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Byte"));
		jmethodID_Byte_Init = env->GetMethodID(jclass_Byte, "<init>", "(B)V");

	jclass_Short = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Short"));
		jmethodID_Short_Init = env->GetMethodID(jclass_Short, "<init>", "(S)V");

	jclass_Integer = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Integer"));
		jmethodID_Integer_Init = env->GetMethodID(jclass_Integer, "<init>", "(I)V");

	jclass_Long = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Long"));
		jmethodID_Long_Init = env->GetMethodID(jclass_Long, "<init>", "(J)V");

	jclass_Float = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Float"));
		jmethodID_Float_Init = env->GetMethodID(jclass_Float, "<init>", "(F)V");

	jclass_Double = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Double"));
		jmethodID_Double_Init = env->GetMethodID(jclass_Double, "<init>", "(D)V");
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - JNI::EnsureAttachedToCurrentThread

EnsureAttachedToCurrentThread::EnsureAttachedToCurrentThread(JavaVM* iJavaVM)
:	fJavaVM(iJavaVM)
,	fNeedsDetach(false)
	{
	JNIEnv* env;
	const jint result = fJavaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
	switch (result)
		{
		case JNI_OK:
			{
			// Already attached, get env into our thread-local Env.
			break;
			}
		case JNI_EDETACHED:
			{
			// Not attached, we'll need to detach on destruction.
			JavaVMAttachArgs args;
			args.version = JNI_VERSION_1_6;
			args.name = nullptr;
			args.group = nullptr;
			if (JNI_OK != fJavaVM->AttachCurrentThread(&env, &args))
				throw std::runtime_error("EnsureAttachedToCurrentThread failed, couldn't attach");
			fNeedsDetach = true;
			break;
			}
		default:
			{
			throw std::runtime_error("EnsureAttachedToCurrentThread failed, other");
			}
		}

	// If we get to here, then env holds the env for the current thread, whether it was
	// because we created it, or because it already existed. So shove it into our fEnvTV.
	fEnvTV.Set(env);
	}

EnsureAttachedToCurrentThread::~EnsureAttachedToCurrentThread()
	{
	if (fNeedsDetach)
		fJavaVM->DetachCurrentThread();
	}

// =================================================================================================
#pragma mark - PushPopLocalFrame

PushPopLocalFrame::PushPopLocalFrame()
:	fEnv(EnvTV::sGet())
	{
	assert(fEnv);
	fEnv->PushLocalFrame(kLocalFrameSlots);
	}

PushPopLocalFrame::PushPopLocalFrame(JNIEnv* iEnv)
:	fEnv(iEnv)
	{
	assert(fEnv);
	fEnv->PushLocalFrame(kLocalFrameSlots);
	}

PushPopLocalFrame::~PushPopLocalFrame()
	{
	if (fEnv)
		fEnv->PopLocalFrame(nullptr);
	}

jobject PushPopLocalFrame::PopReturn(jobject iVal)
	{
	assert(fEnv);
	jobject result = fEnv->PopLocalFrame(iVal);
	fEnv = nullptr;
	return result;
	}


// =================================================================================================
#pragma mark - sAsString

string8 sAsString8(JNIEnv* env, jstring ss)
	{
	std::string result;
	if (env && ss)
		{
		if (const char* charP = env->GetStringUTFChars(ss, 0))
			{
			result = charP;
			env->ReleaseStringUTFChars(ss, charP);
			}
		}
	return result;
	}

string8 sAsString8(jstring ss)
	{ return sAsString8(EnvTV::sGet(), ss); }

string16 sAsString16(JNIEnv* env, jstring ss)
	{
	if (ss)
		{
		if (jsize theSize = env->GetStringLength(ss))
			{
			string16 result(theSize, u'\0');
			env->GetStringRegion(ss, 0, theSize, (jchar*)result.data());
			return result;
			}
		}
	return string16();
	}

string16 sAsString16(jstring ss)
	{ return sAsString16(EnvTV::sGet(), ss); }

// =================================================================================================
#pragma mark - sMakeInteger, sMakeFloat

jstring sMakeString(JNIEnv* env, const string8& iVal)
	{
	// We actually should be careful here -- JNI wants *modified* UTF8, which uses UTF8-encoded
	// UTF16 surrogates for CPs outside the BMP. And our caller probably isn't making
	// sure that's hhappening.

	return env->NewStringUTF(iVal.data());
	}

jstring sMakeString(const string8& iVal)
	{ return sMakeString(EnvTV::sGet(), iVal); }

jstring sMakeString(JNIEnv* env, const string16& iVal)
	{ return env->NewString((const jchar*)iVal.data(), iVal.length()); }

jstring sMakeString(const string16& iVal)
	{ return sMakeString(EnvTV::sGet(), iVal); }

jobject sMakeBoolean(JNIEnv* env, bool iVal)
	{ return env->NewObject(jclass_Boolean, jmethodID_Boolean_Init, iVal); }

jobject sMakeBoolean(bool iVal)
	{ return sMakeBoolean(EnvTV::sGet(), iVal); }

jobject sMakeCharacter(JNIEnv* env, UTF16 iVal)
	{ return env->NewObject(jclass_Character, jmethodID_Character_Init, iVal); }

jobject sMakeCharacter(UTF16 iVal)
	{ return sMakeCharacter(EnvTV::sGet(), iVal); }

jobject sMakeByte(JNIEnv* env, jbyte iVal)
	{ return env->NewObject(jclass_Byte, jmethodID_Byte_Init, iVal); }

jobject sMakeByte(jbyte iVal)
	{ return sMakeByte(EnvTV::sGet(), iVal); }

jobject sMakeShort(JNIEnv* env, jshort iVal)
	{ return env->NewObject(jclass_Short, jmethodID_Short_Init, iVal); }

jobject sMakeShort(jshort iVal)
	{ return sMakeShort(EnvTV::sGet(), iVal); }

jobject sMakeInteger(JNIEnv* env, jint iVal)
	{ return env->NewObject(jclass_Integer, jmethodID_Integer_Init, iVal); }

jobject sMakeInteger(jint iVal)
	{ return sMakeInteger(EnvTV::sGet(), iVal); }

jobject sMakeLong(JNIEnv* env, jlong iVal)
	{ return env->NewObject(jclass_Long, jmethodID_Long_Init, iVal); }

jobject sMakeLong(jlong iVal)
	{ return sMakeLong(EnvTV::sGet(), iVal); }

jobject sMakeFloat(JNIEnv* env, jfloat iVal)
	{ return env->NewObject(jclass_Float, jmethodID_Float_Init, iVal); }

jobject sMakeFloat(jfloat iVal)
	{ return sMakeFloat(EnvTV::sGet(), iVal); }

jobject sMakeDouble(JNIEnv* env, jdouble iVal)
	{ return env->NewObject(jclass_Double, jmethodID_Double_Init, iVal); }

jobject sMakeDouble(jdouble iVal)
	{ return sMakeDouble(EnvTV::sGet(), iVal); }

} // namespace JNI
} // namespace ZooLib
