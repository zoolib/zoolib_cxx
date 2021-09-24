// Copyright (c) 2018-2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib__PullPush_ZZ_h__
#define __ZooLib__PullPush_ZZ_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Promise.h" // For Delivery
#include "zoolib/PullPush.h"
#include "zoolib/Val_ZZ.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - 

typedef
	Callable<bool(const Val_ZZ& iVal, const ChanW_PPT& iChanW)> Callable_ZZ_WriteFilter;

void sFromZZ_Push_PPT(const Val_ZZ& iVal,
	const ZP<Callable_ZZ_WriteFilter>& iWriteFilter,
	const ChanW_PPT& iChanW);

void sFromZZ_Push_PPT(const Val_ZZ& iVal,
	const ChanW_PPT& iChanW);

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

ZP<ChannerR_PPT> sChannerR_PPT(const Val_ZZ& iVal,
	const ZP<Callable_ZZ_WriteFilter>& iWriteFilter);

ZP<ChannerR_PPT> sChannerR_PPT(const Val_ZZ& iVal);

// =================================================================================================
#pragma mark -

typedef
	Callable<bool(const PPT& iPPT, const ChanR_PPT& iChanR, Val_ZZ& oVal)> Callable_ZZ_ReadFilter;

void sPull_PPT_AsZZ(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter,
	Val_ZZ& oVal);

bool sPull_PPT_AsZZ(const ChanR_PPT& iChanR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter,
	Val_ZZ& oVal);

bool sPull_PPT_AsZZ(const ChanR_PPT& iChanR,
	Val_ZZ& oVal);

Val_ZZ sAsZZ(const PPT& iPPT, const ChanR_PPT& iChanR);
ZQ<Val_ZZ> sQAsZZ(const ChanR_PPT& iChanR);
Val_ZZ sAsZZ(const ChanR_PPT& iChanR);

// =================================================================================================
#pragma mark -

ZP<Delivery<Val_ZZ>> sStartAsync_AsZZ(const ZP<ChannerR_PPT>& iChannerR);

ZP<Delivery<Val_ZZ>> sStartAsync_AsZZ(const ZP<ChannerR_PPT>& iChannerR,
	const ZP<Callable_ZZ_ReadFilter>& iReadFilter);

} // namespace ZooLib

#endif // __ZooLib__PullPush_ZZ_h__
