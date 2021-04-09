/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_JNI_JNI_h__
#define __ZooLib_JNI_JNI_h__ 1
#include "zconfig.h"

#include "zoolib/ThreadVal.h"

#include <jni.h>

#include <string>

namespace ZooLib {
namespace JNI {

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

class PushPopLocalFrame
	{
public:
	PushPopLocalFrame();
	PushPopLocalFrame(JNIEnv* iEnv);
	~PushPopLocalFrame();

private:
	JNIEnv* fEnv;
	};

// =================================================================================================
#pragma mark - JNI::sAsString

std::string sAsString(jstring s);

std::string sAsString(JNIEnv *env, jstring s);

jstring sAsJString(const std::string& iString);

jstring sAsJString(JNIEnv *env, const std::string& iString);

// =================================================================================================
#pragma mark - sMakeBoolean, sMakeInteger, sMakeLong, sMakeFloat

jobject sMakeBoolean(JNIEnv* env, bool iBool);

jobject sMakeInteger(JNIEnv* env, int iInt);

jobject sMakeLong(JNIEnv* env, jlong iLong);

jobject sMakeFloat(JNIEnv* env, float iFloat);

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
ZMACRO_ArrayAccessors(jbyte, Byte)
ZMACRO_ArrayAccessors(jchar, Char)
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

// This accessor makes it reasonable to do PaC_Array<xx> aa(env->NewxxxArray());
template <class Array_p>
Array_p sArray(const PaC_Array<Array_p>& iPaC_Array) { return iPaC_Array.GetArray(); }

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

// No sArray accessor for PaC_ConstArray, emphasizing that the array will have come from
// elsewhere, preloaded with read-only data, and if you need the Java array you
// can get it from the same place that the PaC_ConstArray ctor did.

} // namespace JNI
} // namespace ZooLib

#endif // __ZooLib_JNI_JNI_h__
