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
		kFontRomanScript, kFontNoLanguage, &theATSUFontID))
//	if (noErr != ::ATSUFindFontFromName(fontName.data(), fontName.size() * 2,
//		kFontFullName, kFontUnicodePlatform,
//		kFontRomanScript, kFontNoLanguageCode, &atsuFont))
		{
		return nil;
		}

	// Three parallel arrays for setting up attributes.
	ATSUAttributeTag theTags[] =
		{
		kATSUFontTag,
		kATSUSizeTag,
//		kATSUVerticalCharacterTag,
		kATSUQDBoldfaceTag,
		kATSUQDItalicTag,
		kATSUQDUnderlineTag,
		kATSUQDCondensedTag,
		kATSUQDExtendedTag
		};

	ByteCount theSizes[countof(theTags)] =
		{
		sizeof(ATSUFontID),
		sizeof(Fixed),
//		sizeof(ATSUVerticalCharacterType),
		sizeof(Boolean),
		sizeof(Boolean),
		sizeof(Boolean),
		sizeof(Boolean),
		sizeof(Boolean)
		};

	ATSUAttributeValuePtr theValues[countof(theTags)] =
		{ 0, 0, 0, 0, 0, 0, 0 };

//	ATSUVerticalCharacterType atsuOrientation = kATSUStronglyHorizontal;

	Boolean trueV = true;
	Boolean falseV = false;

	// set the values array to point to our locals
	theValues[0] = &theATSUFontID;
	theValues[1] = &atsuSize;
//	theValues[2] = &atsuOrientation;
	theValues[2] = (theStyle & ZDCFont::bold ? &trueV : &falseV);
	theValues[3] = (theStyle & ZDCFont::italic ? &trueV : &falseV);
	theValues[4] = (theStyle & ZDCFont::underline ? &trueV : &falseV);
	theValues[5] = (theStyle & ZDCFont::condense ? &trueV : &falseV);
	theValues[6] = (theStyle & ZDCFont::extend ? &trueV : &falseV);

	// create a style
	ATSUStyle localStyle = nil;
	if (noErr != ::ATSUCreateStyle(&localStyle))
		return nil;

	// set the style attributes
	if (noErr != ::ATSUSetAttributes(localStyle, countof(theTags), theTags, theSizes, theValues))
		{
		::ATSUDisposeStyle(localStyle);
		return nil;
		}

	return localStyle;
	}

ATSUTextLayout ZUtil_ATSUI::sCreateLayout(
	const UTF16* iText, UniCharCount iTextLength, ATSUStyle iStyle)
	{
	ZAssertCompile(sizeof(UTF16) == sizeof(UniChar));
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
			ATSUAttributeTag theTags[] =
				{
				kATSULineFontFallbacksTag
				};
			ByteCount theSizes[countof(theTags)] =
				{
				sizeof(ATSUFontFallbacks)
				};
			ATSUAttributeValuePtr theValues[countof(theTags)] =
				{
				&theFontFallbacks
				};

			::ATSUSetLayoutControls(theLayout, 1, theTags, theSizes, theValues);
			::ATSUSetTransientFontMatching(theLayout, true);
			}
		::ATSUDisposeFontFallbacks(theFontFallbacks);
		}

	return theLayout;
	}

#endif // ZCONFIG_API_Enabled(Util_ATSUI)
