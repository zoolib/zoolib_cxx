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

#include "ZDebug.h" // For ZAssertCompile

ZAssertCompile(sizeof(UTF16) == sizeof(UniChar));

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_ATSUI

ATSUStyle ZUtil_ATSUI::sAsATSUStyle(const ZDCFont& iFont, float iFontSize)
	{
	ZDCFont::Style theStyle = iFont.GetStyle();

	Fixed atsuSize = FixRatio(iFont.GetSize(), 1);
	if (iFontSize > 0)
		atsuSize = iFontSize * 65536.0;

    ATSUFontID theATSUFontID;
	// find the font ID
	string fontName = iFont.GetName();
	if (noErr != ::ATSUFindFontFromName((char*)fontName.data(), fontName.size(),
		kFontFullName, kFontMacintoshPlatform,
		kFontRomanScript, kFontNoLanguageCode, &theATSUFontID))
		{
		return nil;
		}

	// Three parallel arrays for setting up attributes.
	const Boolean trueV = true;
	const Boolean falseV = false;
	Attributes theAttributes;
	theAttributes.Add_T(kATSUFontTag, theATSUFontID);
	theAttributes.Add_T(kATSUSizeTag, atsuSize);
	theAttributes.Add_T(kATSUQDBoldfaceTag, theStyle & ZDCFont::bold ? trueV : falseV);
	theAttributes.Add_T(kATSUQDItalicTag, theStyle & ZDCFont::italic ? trueV : falseV);
	theAttributes.Add_T(kATSUQDUnderlineTag, theStyle & ZDCFont::underline ? trueV : falseV);
	theAttributes.Add_T(kATSUQDCondensedTag, theStyle & ZDCFont::condense ? trueV : falseV);
	theAttributes.Add_T(kATSUQDExtendedTag, theStyle & ZDCFont::extend ? trueV : falseV);

	// create a style
	ATSUStyle localStyle;
	if (noErr != ::ATSUCreateStyle(&localStyle))
		return nil;

	if (!theAttributes.Apply(localStyle))
		{
		::ATSUDisposeStyle(localStyle);
		return nil;
		}
		
	return localStyle;
	}

ATSUTextLayout ZUtil_ATSUI::sCreateLayout(
	const UTF16* iText, UniCharCount iTextLength, ATSUStyle iStyle)
	{
	if (!iStyle)
		return nil;

  	if (!iTextLength)
  		return nil;

  	ATSUTextLayout theLayout = nil;
	::ATSUCreateTextLayoutWithTextPtr((ConstUniCharArrayPtr)iText, 0, iTextLength, iTextLength,
		1, &iTextLength, &iStyle, &theLayout);

	ATSUFontFallbacks theFontFallbacks = nil;
	if (noErr == ::ATSUCreateFontFallbacks(&theFontFallbacks))
		{
		if (noErr == ::ATSUSetObjFontFallbacks(
			theFontFallbacks, 0, nil, kATSUDefaultFontFallbacks))
			{
			Attributes theAttributes;
			theAttributes.Add_T(kATSULineFontFallbacksTag, theFontFallbacks);
			theAttributes.Apply(theLayout);

			::ATSUSetTransientFontMatching(theLayout, true);
			}
		}

	return theLayout;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_ATSUI::Attributes

ZUtil_ATSUI::Attributes::Attributes()
	{}

ZUtil_ATSUI::Attributes::~Attributes()
	{}

void ZUtil_ATSUI::Attributes::Add(
	const ATSUAttributeTag& iTag, size_t iSize, const void* iValue)
	{
	fTags.push_back(iTag);
	fSizes.push_back(iSize);
	fValues.push_back(iValue);
	}

bool ZUtil_ATSUI::Attributes::Apply(ATSUTextLayout iLayout)
	{
	return noErr == ::ATSUSetLayoutControls(iLayout,
		fTags.size(), &fTags[0], &fSizes[0], const_cast<void**>(&fValues[0]));
	}

bool ZUtil_ATSUI::Attributes::Apply(ATSUStyle iStyle)
	{
	return noErr == ::ATSUSetAttributes(iStyle,
		fTags.size(), &fTags[0], &fSizes[0], const_cast<void**>(&fValues[0]));
	}


#endif // ZCONFIG_API_Enabled(Util_ATSUI)
