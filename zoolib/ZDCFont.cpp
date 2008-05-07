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

#include "ZDCFont.h"

#include "ZDebug.h"
#include "ZStream.h"
#include "ZString.h"
#include "ZUnicode.h"

#if ZCONFIG(API_Graphics, QD)
#	if ZCONFIG(OS, MacOSX)
#		include <QD/Fonts.h>
#	else
#		include <Fonts.h>
#	endif
#endif

#if ZCONFIG(API_Graphics, QD)

const ZDCFont ZDCFont::sApp9(applFont, ZDCFont::normal, 9);
const ZDCFont ZDCFont::sApp10Bold(applFont, ZDCFont::bold, 10);
const ZDCFont ZDCFont::sSystem(systemFont, ZDCFont::normal, 12);
const ZDCFont ZDCFont::sMonospaced9(kFontIDMonaco, ZDCFont::normal, 9);

#elif ZCONFIG(OS, Win32)

// AG 98-07-15. This is not how things should be done, but it'll do for now
const ZDCFont ZDCFont::sApp9("Arial", ZDCFont::normal, 9);
const ZDCFont ZDCFont::sApp10Bold("Arial", ZDCFont::bold, 10);
const ZDCFont ZDCFont::sSystem("Arial", ZDCFont::normal, 12);
const ZDCFont ZDCFont::sMonospaced9("Courier", ZDCFont::normal, 9);

#elif ZCONFIG(API_Graphics, Be)

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
	fStyle(inOther.fStyle),
	fSize(inOther.fSize)
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

// ==================================================

#if ZCONFIG(API_Graphics, QD)
ZDCFont::ZDCFont(const TextStyle& inStyle)
	{
	Str255 fontName;
	::GetFontName(inStyle.tsFont, fontName);
	fFontName = ZString::sFromPString(fontName);

	fStyle = (Style)inStyle.tsFace;
	fSize = inStyle.tsSize;
	}

ZDCFont::ZDCFont(int16 inFontID, Style inStyle, int16 inSize)
	{
	Str255 fontName;
	::GetFontName(inFontID, fontName);
	fFontName = ZString::sFromPString(fontName);

	fStyle = inStyle;
	fSize = inSize;
	}

TextStyle ZDCFont::GetTextStyle() const
	{
	TextStyle theTextStyle;

#if ZCONFIG(OS, Carbon)
	theTextStyle.tsFont = ::FMGetFontFamilyFromName(ZString::sAsPString(fFontName));
#else
	::GetFNum(ZString::sAsPString(fFontName), &theTextStyle.tsFont);
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
#if ZCONFIG(OS, Carbon)
	return ::FMGetFontFamilyFromName(ZString::sAsPString(fFontName));
#else
	int16 theFontID;
	::GetFNum(ZString::sAsPString(fFontName), &theFontID);
	return theFontID;
#endif
	}

void ZDCFont::SetFontID(int16 inID)
	{
	Str255 fontName;
	::GetFontName(inID, fontName);
	fFontName = ZString::sFromPString(fontName);
	}
#endif // ZCONFIG(API_Graphics, QD)

// ==================================================

#if ZCONFIG(API_Graphics, Be)
ZDCFont::ZDCFont(const BFont& inBFont)
	{
	}

void ZDCFont::GetBFont(BFont& outBFont)
	{
	outBFont = *be_plain_font;
	}
#endif // ZCONFIG(API_Graphics, Be)

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

