// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_GameEngine_Util_h__
#define __ZooLib_GameEngine_Util_h__ 1

#include "zoolib/GameEngine/Types.h"

#include "zoolib/File.h" // For FileSpec

#include "zoolib/Pixels/Pixmap.h"

namespace ZooLib {
namespace GameEngine {

using Pixels::Pixmap;

// =================================================================================================
#pragma mark -

ZQ<GPoint> sQGPoint(const ZQ<Val>& iValQ);
ZQ<GPoint> sQGPoint(const Val& iVal);

ZQ<GPoint> sQGPoint(const ZQ<Val_Any>& iVal_AnyQ);
ZQ<GPoint> sQGPoint(const Val_Any& iVal_Any);

ZQ<GRect> sQGRect(const ZQ<Val>& iValQ);
ZQ<GRect> sQGRect(const Val& iVal);

ZQ<GRect> sQGRect(const ZQ<Val_Any>& iVal_AnyQ);
ZQ<GRect> sQGRect(const Val_Any& iVal_Any);

ZQ<Map_Any> sQReadMap_Any(const ChanR_Bin& iChanR, const string8* iName = nullptr);
ZQ<Map_Any> sQReadMap_Any(const ChanR_Bin& iChanR, const string8& iName);
ZQ<Map_Any> sQReadMap_Any(const FileSpec& iFS);

Map_Any sReadTextData(const FileSpec& iFS);

ZP<ChannerW_Bin> sCreateW_Truncate(const FileSpec& iFS);
ZP<ChannerR_Bin> sOpenR_Buffered(const FileSpec& iFS);

void sWriteBin(const Val_Any& iVal, const ChanW_Bin& w);
Val_Any sReadBin(const ChanR_Bin& iChanR);

void sDump(const ChanW_UTF& w, const Val& iVal);
void sDump(const Val& iVal);

void sDump(const ChanW_UTF& w, const Any& iAny);
void sDump(const Any& iAny);

uint64 sNextID();

Pixmap sPixmap_PNG(const ZP<ChannerR_Bin>& iChannerR);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_h__
