// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UITVC_WithSections_More__
#define __ZooLib_UITVC_WithSections_More__ 1
#include "zconfig.h"

#include "zoolib/UIKit/UITVController_WithSections.h"

#if ZCONFIG_SPI_Enabled(iPhone)

// =================================================================================================
#pragma mark - ZooLib::UIKit

namespace ZooLib {
namespace UIKit {

ZP<Section> sMakeSection(ZP<SectionBody> iBody, bool iHideWhenEmpty);
ZP<Section> sMakeSection(ZP<SectionBody> iBody);

ZP<SectionBody_SingleRow> sMakeSingleRow(ZP<SectionBody_Multi> iParent,
	ZP<SectionBody::Callable_RowSelected> iCallable_RowSelected = null,
	ZP<SectionBody::Callable_ButtonTapped> iCallable_ButtonTapped = null);

ZP<SectionBody_SingleRow> sMakeSingleRow(ZP<SectionBody_Multi> iParent,
	id iDelegate, SEL iSEL);

ZP<UILabel> sMakeUILabel();

} // namespace UIKit
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __ZooLib_UITVC_WithSections_More__
