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

#include "zoolib/ZUtil_ATSUI.h"

#if ZCONFIG_API_Enabled(Util_ATSUI)

#include "zoolib/ZDebug.h" // For ZAssertCompile

namespace ZooLib {
namespace ZUtil_ATSUI {

ZAssertCompile(sizeof(UTF16) == sizeof(UniChar));

using std::string;

// =================================================================================================
// MARK: - ZUtil_ATSUI

ATSUStyle sAsATSUStyle(const ZDCFont& iFont, float iFontSize)
	{
	ZDCFont::Style theStyle = iFont.GetStyle();

	Fixed atsuSize = FixRatio(iFont.GetSize(), 1);
	if (iFontSize > 0)
		atsuSize = iFontSize * 65536.0;

	ATSUFontID theATSUFontID;
	string fontName = iFont.GetName();
	if (noErr != ::ATSUFindFontFromName((char*)fontName.data(), fontName.size(),
		kFontFullName, kFontMacintoshPlatform,
		kFontRomanScript, kFontNoLanguageCode, &theATSUFontID))
		{
		return nullptr;
		}

	const Boolean trueV = true;
	Attributes theAttributes;
	theAttributes.Add_T(kATSUFontTag, theATSUFontID);
	theAttributes.Add_T(kATSUSizeTag, atsuSize);

	if (theStyle & ZDCFont::bold)
		theAttributes.Add_T(kATSUQDBoldfaceTag, trueV);

	if (theStyle & ZDCFont::italic)
		theAttributes.Add_T(kATSUQDItalicTag, trueV);

	if (theStyle & ZDCFont::underline)
		theAttributes.Add_T(kATSUQDUnderlineTag, trueV);

	if (theStyle & ZDCFont::condense)
		theAttributes.Add_T(kATSUQDCondensedTag, trueV);

	if (theStyle & ZDCFont::extend)
		theAttributes.Add_T(kATSUQDExtendedTag, trueV);

	ATSUStyle localStyle;
	if (noErr != ::ATSUCreateStyle(&localStyle))
		return nullptr;

	if (!theAttributes.Apply(localStyle))
		{
		::ATSUDisposeStyle(localStyle);
		return nullptr;
		}

	return localStyle;
	}

static ATSUFontFallbacks spCreate_ATSUFontFallbacks()
	{
	ATSUFontFallbacks result = nullptr;
	if (noErr == ::ATSUCreateFontFallbacks(&result))
		{
		if (noErr == ::ATSUSetObjFontFallbacks(
			result, 0, nullptr, kATSUDefaultFontFallbacks))
			{
			return result;
			}
		}
	return nullptr;
	}

static ATSUFontFallbacks spATSUFontFallbacks = spCreate_ATSUFontFallbacks();

ATSUTextLayout sCreateLayout(
	const UTF16* iText, UniCharCount iTextLength, ATSUStyle iStyle, bool iUseFallbacks)
	{
	if (not iStyle)
		return nullptr;

	if (not iTextLength)
		return nullptr;

	UniCharCount runLengths = kATSUToTextEnd;
	ATSUTextLayout theLayout = nullptr;
	::ATSUCreateTextLayoutWithTextPtr((ConstUniCharArrayPtr)iText,
		kATSUFromTextBeginning, kATSUToTextEnd,
		iTextLength,
		1,
		&runLengths,
		&iStyle,
		&theLayout);

	if (iUseFallbacks && spATSUFontFallbacks)
		{
		Attributes theAttributes;
		theAttributes.Add_T(kATSULineFontFallbacksTag, spATSUFontFallbacks);
		theAttributes.Apply(theLayout);

		::ATSUSetTransientFontMatching(theLayout, true);
		}

	return theLayout;
	}

// =================================================================================================
// MARK: - Attributes

Attributes::Attributes()
	{}

Attributes::~Attributes()
	{}

void Attributes::Add(
	const ATSUAttributeTag& iTag, const void* iValue, size_t iSize)
	{
	fTags.push_back(iTag);
	fSizes.push_back(iSize);
	fValues.push_back(iValue);
	}

bool Attributes::Apply(ATSUTextLayout iLayout)
	{
	return noErr == ::ATSUSetLayoutControls(iLayout,
		fTags.size(), &fTags[0], &fSizes[0], const_cast<void**>(&fValues[0]));
	}

bool Attributes::Apply(ATSUStyle iStyle)
	{
	return noErr == ::ATSUSetAttributes(iStyle,
		fTags.size(), &fTags[0], &fSizes[0], const_cast<void**>(&fValues[0]));
	}

} // namespace ZUtil_ATSUI
} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Util_ATSUI)
