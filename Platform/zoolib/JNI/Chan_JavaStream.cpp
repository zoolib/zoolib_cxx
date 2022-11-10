// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/JNI/Chan_JavaStream.h"

#include "zoolib/Compat_algorithm.h" // For sMin

namespace ZooLib {

using namespace JNI;

// =================================================================================================
#pragma mark -

namespace {  // anonymous

struct LoadHandler_JavaStream : public JNI::LoadHandler
	{
	void OnLoad(JNIEnv* iEnv);
	} spLoadHandler_JavaStream;

jclass jclass_InputStream;
	jmethodID jmethodID_InputStream_available;
	jmethodID jmethodID_InputStream_read;

jclass jclass_OutputStream;
	jmethodID jmethodID_OutputStream_flush;
	jmethodID jmethodID_OutputStream_write;

void LoadHandler_JavaStream::OnLoad(JNIEnv* env)
	{
	jclass_InputStream = (jclass) env->NewGlobalRef(env->FindClass("java/io/InputStream"));
		jmethodID_InputStream_available = env->GetMethodID(jclass_InputStream, "available", "()I");
		jmethodID_InputStream_read = env->GetMethodID(jclass_InputStream, "read", "([BII)I");

	jclass_OutputStream = (jclass) env->NewGlobalRef(env->FindClass("java/io/OutputStream"));
		jmethodID_OutputStream_flush = env->GetMethodID(jclass_OutputStream, "flush", "()V");
		jmethodID_OutputStream_write = env->GetMethodID(jclass_OutputStream, "write", "([BII)V");

	ZEnsure(not env->ExceptionCheck());
	}

} // anonymous namespace

static constexpr size_t kBufferSize = 4096;

// =================================================================================================
#pragma mark - ChanR_Bin_JavaInputStream

ChanR_Bin_JavaInputStream::ChanR_Bin_JavaInputStream(jobject iInputStream)
	{
	JNIEnv* env = JNI::EnvTV::sGet();
	fInputStream = env->NewGlobalRef(iInputStream);
	fBuffer = (jbyteArray)env->NewGlobalRef(env->NewByteArray(kBufferSize));
	}

ChanR_Bin_JavaInputStream::~ChanR_Bin_JavaInputStream()
	{
	EnsureAttachedToCurrentThread eatct(sJavaVM());

	JNIEnv* env = JNI::EnvTV::sGet();

	env->DeleteGlobalRef(fInputStream);
	env->DeleteGlobalRef(fBuffer);
	}

size_t ChanR_Bin_JavaInputStream::Read(byte* oDest, size_t iCount)
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	jint countRead = env->CallIntMethod(fInputStream, jmethodID_InputStream_read,
		fBuffer, 0, sMin(iCount, kBufferSize));

	if (countRead <= 0)
		return 0;

	env->GetByteArrayRegion(fBuffer, 0, countRead, (jbyte*)oDest);
	return countRead;
	}

size_t ChanR_Bin_JavaInputStream::Readable()
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	return env->CallIntMethod(fInputStream, jmethodID_InputStream_available);
	}

// =================================================================================================
#pragma mark - ChanW_Bin_JavaOutputStream

ChanW_Bin_JavaOutputStream::ChanW_Bin_JavaOutputStream(jobject iOutputStream)
	{
	JNIEnv* env = JNI::EnvTV::sGet();
	fOutputStream = env->NewGlobalRef(iOutputStream);
	fBuffer = (jbyteArray)env->NewGlobalRef(env->NewByteArray(kBufferSize));
	}

ChanW_Bin_JavaOutputStream::~ChanW_Bin_JavaOutputStream()
	{
	EnsureAttachedToCurrentThread eatct(sJavaVM());

	JNIEnv* env = JNI::EnvTV::sGet();

	env->DeleteGlobalRef(fOutputStream);
	env->DeleteGlobalRef(fBuffer);
	}

size_t ChanW_Bin_JavaOutputStream::Write(const byte* iSource, size_t iCount)
	{
	if (size_t theCount = sMin(iCount, kBufferSize))
		{
		JNIEnv* env = JNI::EnvTV::sGet();

		env->SetByteArrayRegion(fBuffer, 0, theCount, (const jbyte*)iSource);

		env->CallVoidMethod(fOutputStream, jmethodID_OutputStream_write,
			fBuffer, 0, theCount);

		if (not env->ExceptionCheck())
			return theCount;
		env->ExceptionClear();
		}
	return 0;
	}

void ChanW_Bin_JavaOutputStream::Flush()
	{
	JNIEnv* env = JNI::EnvTV::sGet();

	env->CallVoidMethod(fOutputStream, jmethodID_OutputStream_flush);
	}

} // namespace ZooLib
