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
#pragma mark - JNI::Env

typedef ThreadVal<JNIEnv*,struct Tag_JNIEnv> Env;
typedef Env EnvTV;

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
	Env fEnv;
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
#pragma mark -

template <class Array_p>
struct ArrayAccessors;

#define MACRO_ArrayAccessors(jtype, jname) \
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
	static const jtype_t* sGetConstElements(JNIEnv* env, jArray_t array) \
		{ return env->Get##jname##ArrayElements(array, nullptr); } \
\
	static void sRelease(JNIEnv* env, jArray_t array, jtype_t* ptr) \
		{ env->Release##jname##ArrayElements(array, ptr, JNI_COMMIT); } \
\
	static void sRelease(JNIEnv* env, jArray_t array, const jtype_t* ptr) \
		{ env->Release##jname##ArrayElements(array, sNonConst(ptr), JNI_ABORT); } \
	};

MACRO_ArrayAccessors(jboolean, Boolean)
MACRO_ArrayAccessors(jbyte, Byte)
MACRO_ArrayAccessors(jchar, Char)
MACRO_ArrayAccessors(jshort, Short)
MACRO_ArrayAccessors(jint, Int)
MACRO_ArrayAccessors(jlong, Long)
MACRO_ArrayAccessors(jfloat, Float)
MACRO_ArrayAccessors(jdouble, Double)

template <class Array_p>
class JArray : public PaC<typename ArrayAccessors<Array_p>::jtype_t>
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
	JArray(Array_p iArray)
	:	PaC_t(spGetPaC(EnvTV::sGet(), iArray))
	,	fArray(iArray)
		{}

	~JArray()
		{
		if (fArray)
			AA::sRelease(EnvTV::sGet(), fArray, PaC_t::first);
		}
	};

template <class Array_p>
class JConstArray : public PaC<const typename ArrayAccessors<Array_p>::jtype_t>
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
	JConstArray(Array_p iArray)
	:	PaC_t(spGetPaC(EnvTV::sGet(), iArray))
	,	fArray(iArray)
		{}

	~JConstArray()
		{
		if (fArray)
			AA::sRelease(EnvTV::sGet(), fArray, PaC_t::first);
		}
	};

} // namespace JNI
} // namespace ZooLib

#endif // __ZooLib_JNI_JNI_h__
