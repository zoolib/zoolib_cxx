// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_PullPush_bplist_h__
#define __ZooLib_PullPush_bplist_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
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

void sPull_bplist_Push_PPT(ChanRPos_Bin& iChanRPos, const ChanW_PPT& iChanW);

bool sPull_PPT_Push_bplist(const ChanR_PPT& iChanR, const ChanW_Bin& iChanW);

//void sWriteXMLPListPreamble(const ChanW_UTF_ML& s);
//void sWriteXMLPListPostamble(const ChanW_UTF_ML& s);

} // namespace ZooLib

#endif // __ZooLib_PullPush_bplist_h__
