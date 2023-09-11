// Copyright (c) 2003-2020 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_JNI_JavaStream_Channer_Bin__
#define __ZooLib_JNI_JavaStream_Channer_Bin__ 1
#include "zconfig.h"

#include "zoolib/Channer_Bin.h"
#include "zoolib/JNI/JNI.h"

namespace ZooLib {
namespace JNI {

// =================================================================================================
#pragma mark - JavaInputStream_ChannerR_Bin

jobject sJavaInputStream(JNIEnv* evnv, ZP<ChannerR_Bin> iChanner);
jobject sJavaInputStream(ZP<ChannerR_Bin> iChanner);

} // namespace JNI
} // namespace ZooLib

#endif // __ZooLib_JNI_JavaStream_Channer_Bin__
