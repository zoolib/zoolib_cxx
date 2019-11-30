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

ZQ<GPoint> sQGPoint(const ZQ<Val_ZZ>& iVal_AnyQ);
ZQ<GPoint> sQGPoint(const Val_ZZ& iVal_Any);

ZQ<GRect> sQGRect(const ZQ<Val>& iValQ);
ZQ<GRect> sQGRect(const Val& iVal);

ZQ<GRect> sQGRect(const ZQ<Val_ZZ>& iVal_AnyQ);
ZQ<GRect> sQGRect(const Val_ZZ& iVal_Any);

ZQ<Map_ZZ> sQReadMap_Any(const ChanR_Bin& iChanR, const string8* iName = nullptr);
ZQ<Map_ZZ> sQReadMap_Any(const ChanR_Bin& iChanR, const string8& iName);
ZQ<Map_ZZ> sQReadMap_Any(const FileSpec& iFS);

Map_ZZ sReadTextData(const FileSpec& iFS);

ZP<ChannerW_Bin> sCreateW_Truncate(const FileSpec& iFS);
ZP<ChannerR_Bin> sOpenR_Buffered(const FileSpec& iFS);

void sWriteBin(const Val_ZZ& iVal, const ChanW_Bin& w);
Val_ZZ sReadBin(const ChanR_Bin& iChanR);

void sDump(const ChanW_UTF& w, const Val& iVal);
void sDump(const Val& iVal);

void sDump(const ChanW_UTF& w, const Val_ZZ& iVal);
void sDump(const Val_ZZ& iVal);

uint64 sNextID();

Pixmap sPixmap_PNG(const ZP<ChannerR_Bin>& iChannerR);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_h__
