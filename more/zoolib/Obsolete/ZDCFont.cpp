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

#include "zoolib/ZMacFixup.h"

#include "zoolib/ZDCFont.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZDebug.h"
//#include "zoolib/ZStream.h"
#include "zoolib/ZUtil_string.h"
#include "zoolib/ZUnicode.h"

#if ZCONFIG_SPI_Enabled(QuickDraw)
	#include ZMACINCLUDE3(ApplicationServices,QD,Fonts.h)
#endif

namespace ZooLib {

#if ZCONFIG_SPI_Enabled(QuickDraw)

const ZDCFont ZDCFont::sApp9(applFont, ZDCFont::normal, 9);
const ZDCFont ZDCFont::sApp10Bold(applFont, ZDCFont::bold, 10);
const ZDCFont ZDCFont::sSystem(systemFont, ZDCFont::normal, 12);
const ZDCFont ZDCFont::sMonospaced9(kFontIDMonaco, ZDCFont::normal, 9);

#elif ZCONFIG_SPI_Enabled(Win)

// AG 98-07-15. This is not how things should be done, but it'll do for now
const ZDCFont ZDCFont::sApp9("Arial", ZDCFont::normal, 9);
const ZDCFont ZDCFont::sApp10Bold("Arial", ZDCFont::bold, 10);
const ZDCFont ZDCFont::sSystem("Arial", ZDCFont::normal, 12);
const ZDCFont ZDCFont::sMonospaced9("Courier", ZDCFont::normal, 9);

#elif ZCONFIG_SPI_Enabled(BeOS)

const ZDCFont ZDCFont::sApp9(*be_plain_font);
const ZDCFont ZDCFont::sApp10Bold(*be_bold_font);
const ZDCFont ZDCFont::sSystem(*be_plain_font);
const ZDCFont ZDCFont::sMonospaced9(*be_fixed_font);

#else

const ZDCFont ZDCFont::sApp9;
const ZDCFont ZDCFont::sApp10Bold;
const ZDCFont ZDCFont::sSystem;
const ZDCFont ZDCFont::sMonospaced9;

#endif

// ==================================================
ZDCFont::ZDCFont()
	{
	fSize = 12;
	fStyle = normal;
	}

ZDCFont::ZDCFont(const ZDCFont& inOther)
:	fFontName(inOther.fFontName),
	fSize(inOther.fSize),
	fStyle(inOther.fStyle)
	{}

ZDCFont::ZDCFont(const string8& inFontName, Style inStyle, int16 inSize)
:	fFontName(inFontName),
	fSize(inSize),
	fStyle(inStyle)
	{}

ZDCFont::ZDCFont(const UTF8* inFontName, Style inStyle, int16 inSize)
:	fFontName(inFontName),
	fSize(inSize),
	fStyle(inStyle)
	{}

ZDCFont::ZDCFont(const string16& inFontName, Style inStyle, int16 inSize)
:	fFontName(ZUnicode::sAsUTF8(inFontName)),
	fSize(inSize),
	fStyle(inStyle)
	{}

ZDCFont::ZDCFont(const UTF16* inFontName, Style inStyle, int16 inSize)
:	fFontName(ZUnicode::sAsUTF8(inFontName)),
	fSize(inSize),
	fStyle(inStyle)
	{}

ZDCFont& ZDCFont::operator=(const ZDCFont& inOther)
	{
	fFontName = inOther.fFontName;
	fStyle = inOther.fStyle;
	fSize = inOther.fSize;
	return *this;
	}

bool ZDCFont::operator==(const ZDCFont& inOther) const
	{
	if (fSize != inOther.fSize)
		return false;
	if (fStyle != inOther.fStyle)
		return false;
	return fFontName == inOther.fFontName;
	}

bool ZDCFont::operator!=(const ZDCFont& inOther) const
	{ return ! (*this == inOther); }

// ==================================================

#if ZCONFIG_SPI_Enabled(QuickDraw)
ZDCFont::ZDCFont(const TextStyle& inStyle)
	{
	Str255 fontName;
	::GetFontName(inStyle.tsFont, fontName);
	fFontName = ZUtil_string::sFromPString(fontName);

	fStyle = (Style)inStyle.tsFace;
	fSize = inStyle.tsSize;
	}

ZDCFont::ZDCFont(int16 inFontID, Style inStyle, int16 inSize)
	{
	Str255 fontName;
	::GetFontName(inFontID, fontName);
	fFontName = ZUtil_string::sFromPString(fontName);

	fStyle = inStyle;
	fSize = inSize;
	}

TextStyle ZDCFont::GetTextStyle() const
	{
	TextStyle theTextStyle;

	Str255 theName;
	ZUtil_string::sToPString(fFontName, theName, 255);
	#if ZCONFIG_SPI_Enabled(Carbon)
		theTextStyle.tsFont = ::FMGetFontFamilyFromName(theName);
	#else
		::GetFNum(theName, &theTextStyle.tsFont);
	#endif

	theTextStyle.tsFace = fStyle;
	theTextStyle.tsSize = fSize;
	theTextStyle.tsColor.red = 0;
	theTextStyle.tsColor.green = 0;
	theTextStyle.tsColor.blue = 0;
	return theTextStyle;
	}

int16 ZDCFont::GetFontID() const
	{
	Str255 theName;
	ZUtil_string::sToPString(fFontName, theName, 255);

	#if ZCONFIG_SPI_Enabled(Carbon)
		return ::FMGetFontFamilyFromName(theName);
	#else
		int16 theFontID;
		::GetFNum(theName, &theFontID);
		return theFontID;
	#endif
	}

void ZDCFont::SetFontID(int16 inID)
	{
	Str255 fontName;
	::GetFontName(inID, fontName);
	fFontName = ZUtil_string::sFromPString(fontName);
	}
#endif // ZCONFIG_SPI_Enabled(QuickDraw)

// ==================================================

#if ZCONFIG_SPI_Enabled(BeOS)
ZDCFont::ZDCFont(const BFont& inBFont)
	{
	}

void ZDCFont::GetBFont(BFont& outBFont)
	{
	outBFont = *be_plain_font;
	}
#endif // ZCONFIG_SPI_Enabled(BeOS)

// ==================================================

int16 ZDCFont::GetSize() const
	{ return fSize; }

void ZDCFont::SetSize(int16 inSize)
	{ fSize = inSize; }

ZDCFont::Style ZDCFont::GetStyle() const
	{ return fStyle; }

void ZDCFont::SetStyle(Style inStyle)
	{ fStyle = inStyle; }

string8 ZDCFont::GetName() const
	{ return fFontName; }

void ZDCFont::SetName(const string8& inFontName)
	{ fFontName = inFontName; }

#if 0
void ZDCFont::ToStream(const ZStreamW& inStream) const
	{
	ZString::sToStream(fFontName, inStream);
	inStream.WriteInt16(fSize);
	inStream.WriteInt16(fStyle);
	}

void ZDCFont::FromStream(const ZStreamR& inStream)
	{
	ZString::sFromStream(fFontName, inStream);
	fSize = inStream.ReadInt16();
	fStyle = inStream.ReadInt16();
	}
#endif

} // namespace ZooLib
