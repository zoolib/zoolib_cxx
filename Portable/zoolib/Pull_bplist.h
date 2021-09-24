// Copyright (c) 2021 Andrew Green and Mark/Space, Inc. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_bplist_h__
#define __ZooLib_PullPush_bplist_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/PullPush.h"
#include "zoolib/TagVal.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - UID

typedef TagVal<std::vector<byte>, struct Tag_UID> bplist_UID;

// =================================================================================================
#pragma mark - 

class Start_Seq_Array : public PullPush::Start_Seq
	{
protected:
	Start_Seq_Array() {}

public:
	static const PPT sPPT;
	static bool sIs(const PPT& iPPT);
	};

class Start_Seq_SetOrdered : public PullPush::Start_Seq
	{
protected:
	Start_Seq_SetOrdered() {}

public:
	static const PPT sPPT;
	static bool sIs(const PPT& iPPT);
	};

class Start_Seq_SetUnordered : public PullPush::Start_Seq
	{
protected:
	Start_Seq_SetUnordered() {}

public:
	static const PPT sPPT;
	static bool sIs(const PPT& iPPT);
	};

// =================================================================================================
#pragma mark -

void sPull_bplist_Push_PPT(const ChanRPos_Bin& iChanRPos, const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

ZP<ChannerR_PPT> sChannerR_PPT_bplist(const ZP<Channer<ChanRPos_Bin>>& iChanner);

} // namespace ZooLib

#endif // __ZooLib_PullPush_bplist_h__
