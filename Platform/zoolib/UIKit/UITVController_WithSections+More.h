/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZooLib_UITVC_WithSections_More__
#define __ZooLib_UITVC_WithSections_More__ 1
#include "zconfig.h"

#include "zoolib/UIKit/UITVController_WithSections.h"

#if ZCONFIG_SPI_Enabled(iPhone)

// =================================================================================================
#pragma mark -
#pragma mark ZooLib::UIKit

namespace ZooLib {
namespace UIKit {

ZRef<Section> sMakeSection(ZRef<SectionBody> iBody, bool iHideWhenEmpty);
ZRef<Section> sMakeSection(ZRef<SectionBody> iBody);

ZRef<SectionBody_SingleRow> sMakeSingleRow(ZRef<SectionBody_Multi> iParent,
	ZRef<SectionBody::Callable_RowSelected> iCallable_RowSelected = null,
	ZRef<SectionBody::Callable_ButtonTapped> iCallable_ButtonTapped = null);

ZRef<SectionBody_SingleRow> sMakeSingleRow(ZRef<SectionBody_Multi> iParent,
	id iDelegate, SEL iSEL);

ZRef<UILabel> sMakeUILabel();

} // namespace UIKit
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __ZooLib_UITVC_WithSections_More__
