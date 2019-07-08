// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/StdIO.h"

#include "zoolib/Chan_Bin_FILE.h"
#include "zoolib/Chan_UTF_Chan_Bin.h"

namespace ZooLib {

static ChanR_Bin_FILE spChanR_Bin_In(stdin);
static ChanW_Bin_FILE spChanW_Bin_Out(stdout);
static ChanW_Bin_FILE spChanW_Bin_Err(stderr);

static ChanR_UTF_Chan_Bin_UTF8 spChanR_UTF_In(spChanR_Bin_In);
static ChanW_UTF_Chan_Bin_UTF8 spChanW_UTF_Out(spChanW_Bin_Out);
static ChanW_UTF_Chan_Bin_UTF8 spChanW_UTF_Err(spChanW_Bin_Err);

namespace StdIO {

const ChanR_Bin& sChan_Bin_In = spChanR_Bin_In;
const ChanW_Bin& sChan_Bin_Out = spChanW_Bin_Out;
const ChanW_Bin& sChan_Bin_Err = spChanW_Bin_Err;

const ChanR_UTF& sChan_UTF_In = spChanR_UTF_In;
const ChanW_UTF& sChan_UTF_Out = spChanW_UTF_Out;
const ChanW_UTF& sChan_UTF_Err = spChanW_UTF_Err;

} // namespace StdIO

} // namespace ZooLib
