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

ZQ<GPoint> sQGPoint(const ZQ<Val_ZZ>& iValQ);
ZQ<GPoint> sQGPoint(const Val_ZZ& iVal);

ZQ<GRect> sQGRect(const ZQ<Val>& iValQ);
ZQ<GRect> sQGRect(const Val& iVal);

ZQ<GRect> sQGRect(const ZQ<Val_ZZ>& iValQ);
ZQ<GRect> sQGRect(const Val_ZZ& iVal);

ZP<ChannerW_Bin> sCreateW_Clear(const FileSpec& iFS);
ZP<ChannerR_Bin> sOpenR_Buffered(const FileSpec& iFS);

void sWriteBin(const ChanW_Bin& ww, const Val_ZZ& iVal);
Val_ZZ sReadBin(const ChanR_Bin& iChanR);

void sDump(const ChanW_UTF& ww, const Val& iVal);
void sDump(const Val& iVal);

void sDump(const ChanW_UTF& ww, const Val_ZZ& iVal);
void sDump(const Val_ZZ& iVal);

uint64 sNextID();

Pixmap sPixmap_PNG(const ZP<ChannerR_Bin>& iChannerR);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Util_h__
