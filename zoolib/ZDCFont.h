/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#ifndef __ZDCFont__
#define __ZDCFont__ 1
#include "zconfig.h"

#include "ZTypes.h"
#include "ZUnicode.h"

#if ZCONFIG(API_Graphics, QD)
#	if ZCONFIG(OS, MacOSX)
#		define __NOEXTENSIONS__
#		include <HIToolbox/TextEdit.h>
#	else
#		include <TextEdit.h> // For TextStyle
#	endif
#endif

#if ZCONFIG(API_Graphics, Be)
#	include <interface/Font.h>
#endif

#include <string>

class ZStreamR;
class ZStreamW;

class ZDCFont
	{
public:
	enum
		{
		normal = 0,
		bold = 1,
		italic = 2,
		underline = 4,
		outline = 8,
		shadow = 0x10,
		condense = 0x20,
		extend = 0x40
		};
	typedef int16 Style;

	ZDCFont();
	ZDCFont(const ZDCFont& inOther);
	ZDCFont(const string8& inFontName, Style inStyle, int16 inSize);
	ZDCFont(const UTF8* inFontName, Style inStyle, int16 inSize);

	ZDCFont(const string16& inFontName, Style inStyle, int16 inSize);
	ZDCFont(const UTF16* inFontName, Style inStyle, int16 inSize);

	ZDCFont& operator=(const ZDCFont& inOther);
	bool operator==(const ZDCFont& inOther) const;

#if ZCONFIG(API_Graphics, QD)
	ZDCFont(const TextStyle& inStyle);
	ZDCFont(int16 inFontID, Style inStyle, int16 inSize);

	TextStyle GetTextStyle() const;
	int16 GetFontID() const;
	void SetFontID(int16 inID);
#endif // ZCONFIG(API_Graphics, QD)

#if ZCONFIG(API_Graphics, Be)
	ZDCFont(const BFont& inBFont);

	void GetBFont(BFont& outBFont);
#endif // ZCONFIG(API_Graphics, Be)

	int16 GetSize() const;
	void SetSize(int16 inSize);

	ZDCFont::Style GetStyle() const;
	void SetStyle(Style inStyle);

	string8 GetName() const;
	void SetName(const string8& inFontName);

	void ToStream(const ZStreamW& inStream) const;
	void FromStream(const ZStreamR& inStream);

// AG 98-07-15. The ZDCFont constants are deprecated. Please use ZUIAttributeFactory (which
// may need expanding). These constants are here to make life a bit easier till
// ZUIAttributeFactory use is common. If you need additional fonts for some application,
// define them *in that application*. Not here.
	static const ZDCFont sApp9;
	static const ZDCFont sApp10Bold;
	static const ZDCFont sSystem;
	static const ZDCFont sMonospaced9;

private:
	string8 fFontName;
	int16 fSize;
	Style fStyle;
	};

#endif // ZDCFont
