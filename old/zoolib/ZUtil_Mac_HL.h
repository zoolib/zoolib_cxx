/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZUtil_Mac_HL_h__
#define __ZUtil_Mac_HL_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include ZMACINCLUDE3(Carbon,HIToolbox,Dialogs.h) // For DialogPtr
#include ZMACINCLUDE3(Carbon,HIToolbox,Events.h) // For EventRecord

#include "zoolib/ZDCPixmapCombo.h"

#include <string>

namespace ZooLib {

class ZDCPattern;

namespace ZUtil_Mac_HL {

// =================================================================================================

bool sInteractWithUser();
void sPreDialog();
void sPostDialog();

pascal Boolean sModalFilter(DialogPtr theDialog, EventRecord* inOutEventRecord, short* inOutItemHit);

std::string sGetVersionString();

// =================================================================================================

void sPixmapsFromCIconHandle(CIconHandle inHandle, ZDCPixmap* outColorPixmap, ZDCPixmap* outMonoPixmap, ZDCPixmap* outMaskPixmap);
CIconHandle sCIconHandleFromPixmapCombo(const ZDCPixmapCombo& inPixmapCombo);
CIconHandle sCIconHandleFromPixmaps(const ZDCPixmap& inColorPixmap, const ZDCPixmap& inMonoPixmap, const ZDCPixmap& inMaskPixmap);

IconFamilyHandle sNewIconFamily();
void sDisposeIconFamily(IconFamilyHandle inIconFamilyHandle);

enum IconSize { eHuge, eLarge, eSmall, eMini };
enum IconKind { eData1, eData4, eData8, eData32, eMask1, eMask8 };

void sAddPixmapToIconFamily(const ZDCPixmap& inPixmap, IconKind inAsIconKind, IconFamilyHandle inIconFamilyHandle);
ZDCPixmap sPixmapFromIconFamily(IconFamilyHandle inIconFamilyHandle, IconSize inIconSize, IconKind inIconKind);

ZDCPixmap sPixmapFromIconSuite(IconSuiteRef inIconSuite, IconSize inIconSize, IconKind inIconKind);

ZDCPixmap sPixmapFromIconDataHandle(Handle inIconDataHandle, IconSize inIconSize, IconKind inIconKind);

IconRef sIconRefFromPixmaps(const ZDCPixmap& inColorPixmap, const ZDCPixmap& inMonoPixmap, const ZDCPixmap& inMaskPixmap);

// =================================================================================================

} // namespace ZUtil_Mac_HL

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================

#if ZCONFIG_SPI_Enabled(QuickDraw)

namespace ZooLib {

class ZStreamR;

namespace ZUtil_Mac_HL {

ZDCPixmap sPixmapFromStreamPICT(const ZStreamR& inStream);

} // namespace ZUtil_Mac_HL

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(QuickDraw)

// =================================================================================================

#endif // __ZUtil_Mac_HL_h__
