// Copyright (c) 2012-2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_JNI_JNI_h__
#define __ZooLib_JNI_JNI_h__ 1
#include "zconfig.h"

#include "zoolib/ThreadVal.h"
#include "zoolib/UnicodeString.h"

#include <jni.h>

#include <string>

namespace ZooLib {
namespace JNI {

// =================================================================================================
#pragma mark - LoadHandler

class LoadHandler
	{
public:
	// To be called as part of your system's JNI_onLoad.
	static void sOnLoad(JavaVM* iJavaVM);
	static void sOnUnload();

	LoadHandler();
	~LoadHandler();

	virtual void OnLoad(JavaVM* iJavaVM);
	virtual void OnLoad(JNIEnv* iEnv);
	virtual void OnUnload();

private:
	static LoadHandler* spHead;
	LoadHandler* fNext;
	};

// =================================================================================================
#pragma mark - JNIDEF

#define JNIDEF(ReturnType) extern "C" JNIEXPORT ReturnType JNICALL

// Declare and define in one go.
#define JNIFUNC(ReturnType, remainder) \
	extern "C" JNIEXPORT ReturnType JNICALL remainder; \
	extern "C" JNIEXPORT ReturnType JNICALL remainder \

// =================================================================================================
#pragma mark - JNI::EnvTV

typedef ThreadVal<JNIEnv*,struct Tag_JNIEnv> EnvTV;

// =================================================================================================
#pragma mark - JNI::EnsureAttachedToCurrentThread

class EnsureAttachedToCurrentThread
	{
public:
	EnsureAttachedToCurrentThread(JavaVM* iJavaVM);
	~EnsureAttachedToCurrentThread();

private:
	JavaVM* fJavaVM;
	bool fNeedsDetach;
	EnvTV fEnvTV;
	};

// =================================================================================================
#pragma mark - JNI::PushPopLocalFrame

constexpr size_t kLocalFrameSlots = 16;

class PushPopLocalFrame
	{
public:
	PushPopLocalFrame(JNIEnv* iEnv, size_t iSlots = kLocalFrameSlots);
	PushPopLocalFrame(size_t iSlots = kLocalFrameSlots);

	~PushPopLocalFrame();

	jobject PopReturn(jobject);

private:
	JNIEnv* fEnv;
	};

// =================================================================================================
#pragma mark - sMakeBoolean, sMakeInteger, etc

jstring sMakeString(JNIEnv* env, const string8& iVal);
jstring sMakeString(const string8& iVal);

jstring sMakeString(JNIEnv* env, const string16& iVal);
jstring sMakeString(const string16& iVal);

jobject sMakeBoolean(JNIEnv* env, bool iVal);
jobject sMakeBoolean(bool iVal);

jobject sMakeCharacter(JNIEnv* env, UTF16 iVal);
jobject sMakeCharacter(UTF16 iVal);

jobject sMakeByte(JNIEnv* env, jbyte iVal);
jobject sMakeByte(jbyte iVal);

jobject sMakeShort(JNIEnv* env, jshort iVal);
jobject sMakeShort(jshort iVal);

jobject sMakeInteger(JNIEnv* env, jint iVal);
jobject sMakeInteger(jint iVal);

jobject sMakeLong(JNIEnv* env, jlong iVal);
jobject sMakeLong(jlong iVal);

jobject sMakeFloat(JNIEnv* env, jfloat iVal);
jobject sMakeFloat(jfloat iVal);

jobject sMakeDouble(JNIEnv* env, jdouble iVal);
jobject sMakeDouble(jdouble iVal);

// =================================================================================================
#pragma mark - sAsStringXX

string8 sAsString8(JNIEnv* env, jstring ss);
string8 sAsString8(jstring ss);

string16 sAsString16(JNIEnv* env, jstring ss);
string16 sAsString16(jstring ss);

// =================================================================================================
#pragma mark -

template <class Array_p>
struct ArrayAccessors;

#define ZMACRO_ArrayAccessors(jtype, jname) \
\
template <> \
struct ArrayAccessors<jtype##Array> \
	{ \
	typedef jtype jtype_t; \
	typedef jtype##Array jArray_t; \
\
	static jtype_t* sGetElements(JNIEnv* env, jArray_t array) \
		{ return env->Get##jname##ArrayElements(array, nullptr); } \
\
	static void sRelease(JNIEnv* env, jArray_t array, jtype_t* ptr) \
		{ env->Release##jname##ArrayElements(array, ptr, 0); } \
\
	static void sRelease(JNIEnv* env, jArray_t array, const jtype_t* ptr) \
		{ env->Release##jname##ArrayElements(array, sNonConst(ptr), JNI_ABORT); } \
	};

ZMACRO_ArrayAccessors(jboolean, Boolean)
ZMACRO_ArrayAccessors(jchar, Char)

ZMACRO_ArrayAccessors(jbyte, Byte)
ZMACRO_ArrayAccessors(jshort, Short)
ZMACRO_ArrayAccessors(jint, Int)
ZMACRO_ArrayAccessors(jlong, Long)
ZMACRO_ArrayAccessors(jfloat, Float)
ZMACRO_ArrayAccessors(jdouble, Double)

#undef ZMACRO_ArrayAccessors

// =================================================================================================
#pragma mark - PaC_Array

template <class Array_p>
class PaC_Array : public PaC<typename ArrayAccessors<Array_p>::jtype_t>
	{
	typedef ArrayAccessors<Array_p> AA;
	typedef PaC<typename AA::jtype_t> PaC_t;

	Array_p fArray;

	static PaC_t spGetPaC(JNIEnv* env, Array_p iArray)
		{
		if (iArray)
			return PaC_t(AA::sGetElements(env, iArray), env->GetArrayLength(iArray));
		return PaC_t(nullptr, 0);
		}

public:
	Array_p GetArray() const { return fArray; }

	PaC_Array(Array_p iArray)
	:	PaC_t(spGetPaC(EnvTV::sGet(), iArray))
	,	fArray(iArray)
		{}

	~PaC_Array()
		{
		if (fArray)
			AA::sRelease(EnvTV::sGet(), fArray, PaC_t::first);
		}
	};

template <class Array_p>
PaC_Array<Array_p> sPaC_Array(Array_p iArray)
	{ return PaC_Array<Array_p>(iArray); }

// =================================================================================================
#pragma mark - PaC_ConstArray

template <class Array_p>
class PaC_ConstArray : public PaC<const typename ArrayAccessors<Array_p>::jtype_t>
	{
	typedef ArrayAccessors<Array_p> AA;
	typedef PaC<const typename AA::jtype_t> PaC_t;

	Array_p fArray;

	static PaC_t spGetPaC(JNIEnv* env, Array_p iArray)
		{
		if (iArray)
			return PaC_t(sConst(AA::sGetElements(env, iArray)), env->GetArrayLength(iArray));
		return PaC_t(nullptr, 0);
		}

public:
	PaC_ConstArray(Array_p iArray)
	:	PaC_t(spGetPaC(EnvTV::sGet(), iArray))
	,	fArray(iArray)
		{}

	~PaC_ConstArray()
		{
		if (fArray)
			AA::sRelease(EnvTV::sGet(), fArray, PaC_t::first);
		}
	};

template <class Array_p>
PaC_ConstArray<Array_p> sPaC_ConstArray(Array_p iArray)
	{ return PaC_ConstArray<Array_p>(iArray); }

} // namespace JNI
} // namespace ZooLib

#endif // __ZooLib_JNI_JNI_h__
