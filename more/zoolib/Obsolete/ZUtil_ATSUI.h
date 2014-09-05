/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZUtil_ATSUI_h__
#define __ZUtil_ATSUI_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Util_ATSUI
// ?? Is this the right condition ??
	#define ZCONFIG_API_Avail__Util_ATSUI ZCONFIG_SPI_Enabled(Carbon)
#endif

#ifndef ZCONFIG_API_Desired__Util_ATSUI
	#define ZCONFIG_API_Desired__Util_ATSUI 1
#endif

#if ZCONFIG_API_Enabled(Util_ATSUI)

#include "zoolib/ZDCFont.h"

#include <vector>

#include ZMACINCLUDE3(ApplicationServices,QD,QD.h)

namespace ZooLib {
namespace ZUtil_ATSUI {

// =================================================================================================
// MARK: - ZUtil_ATSUI

ATSUStyle sAsATSUStyle(const ZDCFont& iFont, float iFontSize);

ATSUTextLayout sCreateLayout(
	const UTF16* iText, UniCharCount iTextLength, ATSUStyle iStyle, bool iUseFallbacks);

class Attributes
	{
public:
	Attributes();
	~Attributes();

	void Add(const ATSUAttributeTag& iTag, const void* iValue, size_t iSize);

	template <typename T>
	void Add_T(const ATSUAttributeTag& iTag, const T& iValue)
		{ this->Add(iTag, &iValue, sizeof(iValue)); }

	bool Apply(ATSUTextLayout iLayout);
	bool Apply(ATSUStyle iStyle);

private:
	std::vector<ATSUAttributeTag> fTags;
	std::vector<ByteCount> fSizes;
	std::vector<const void*> fValues;
	};

} // namespace ZUtil_ATSUI
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Util_ATSUI)

#endif // __ZUtil_ATSUI_h__
