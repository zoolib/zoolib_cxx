// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_JNI_PullPush_JNI__
#define __ZooLib_JNI_PullPush_JNI__ 1
#include "zconfig.h"

#include "zoolib/PullPush.h"
#include "zoolib/Val_ZZ.h" // used by sAs_JNI
#include "zoolib/JNI/JNI.h"

namespace ZooLib {

typedef TagVal<std::string, struct Tag_UnhandledJNI> UnhandledJNI;

// =================================================================================================
#pragma mark -

void sFromJNI_Push_PPT(jobject iObject,
	const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark -

jobject sPull_PPT_AsJNI(const ChanR_PPT& iChanR);

jobject sAsJNI(const Val_ZZ& iVal);

// =================================================================================================
#pragma mark - sChannerR_PPT

ZP<ChannerR_PPT> sChannerR_PPT(jobject iJObject);

} // namespace ZooLib

#endif // __ZooLib_JNI_PullPush_JNI__
