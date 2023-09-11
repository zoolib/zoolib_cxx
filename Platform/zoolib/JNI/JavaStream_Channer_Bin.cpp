// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/JNI/JavaStream_Channer_Bin.h"
#include "zoolib/ChanR_Bin.h"

namespace ZooLib {
namespace JNI {

// =================================================================================================
#pragma mark -

namespace { // anonymous

struct LoadHandler_JavaStream : public JNI::LoadHandler
	{
	void OnLoad(JNIEnv* iEnv);
	} spLoadHandler_JavaStream;

jclass jclass_Input;
	jmethodID jmethodID_Input_Init;

void LoadHandler_JavaStream::OnLoad(JNIEnv* env)
	{
	jclass_Input = (jclass) env->NewGlobalRef(env->FindClass("org/zoolib/InputStream_Channer"));
		jmethodID_Input_Init = env->GetMethodID(jclass_Input, "<init>", "(J)V");
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - JavaInputStream_ChannerR_Bin

jobject sJavaInputStream(ZP<ChannerR_Bin> iChanner)
	{ return sJavaInputStream(EnvTV::sGet(), iChanner); }

jobject sJavaInputStream(JNIEnv* env, ZP<ChannerR_Bin> iChanner)
	{
	if (not iChanner)
		return nullptr;
	iChanner->Retain();
	return env->NewObject(jclass_Input, jmethodID_Input_Init, reinterpret_cast<jlong>(iChanner.Get()));
	}

} // namespace JNI
} // namespace ZooLib

// =================================================================================================
#pragma mark -

using namespace ZooLib;
using namespace JNI;

JNIFUNC(jint, Java_org_zoolib_InputStream_1Channer_npRead(
	JNIEnv *env, jobject ob,
	jlong channer, jbyteArray buffer, jint off, jint len))
	{
	EnvTV theEnvTV(env);
	if (!channer)
		return -1;
	jint count = (jint)sReadMem(*reinterpret_cast<ChannerR_Bin*>(channer), sPtr(sPaC_Array(buffer)) + off, len);
	if (count == 0)
		return -1;
	return count;
	}

JNIFUNC(void, Java_org_zoolib_InputStream_1Channer_npClose(
	JNIEnv *env, jobject ob,
	jlong channer))
	{
	EnvTV theEnvTV(env);
	if (channer)
		reinterpret_cast<ChannerR_Bin*>(channer)->Release();
	}
