// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_StdIO_h__
#define __ZooLib_StdIO_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {
namespace StdIO {

extern const ChanR_Bin& sChan_Bin_In;
extern const ChanW_Bin& sChan_Bin_Out;
extern const ChanW_Bin& sChan_Bin_Err;

extern const ChanR_UTF& sChan_UTF_In;
extern const ChanW_UTF& sChan_UTF_Out;
extern const ChanW_UTF& sChan_UTF_Err;

} // namespace ZStdIO
} // namespace ZooLib

#endif // __ZooLib_StdIO_h__
