/* ------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "ZUtil_TextCoder.h"

#include "ZTextCoder_iconv.h"
#include "ZTextCoder_ICU.h"
#include "ZTextCoder_Mac.h"
#include "ZTextCoder_Std.h"
#include "ZTextCoder_Unicode.h"
#include "ZTextCoder_Win.h"

#include "ZCONFIG_API.h"

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TextCoder

ZTextDecoder* ZUtil_TextCoder::sCreateDecoder(const string& iCharset)
	{
	string charsetLC = ZUnicode::sToLower(iCharset);
	if (charsetLC == "utf-8" || charsetLC == "utf8")
		{
		return new ZTextDecoder_UTF8;
		}
	else if (charsetLC == "ucs-4" || charsetLC == "ucs4" || charsetLC == "utf-32" || charsetLC == "utf32")
		{
		if (ZCONFIG(Endian, Big))
			return new ZTextDecoder_UTF32BE;
		else
			return new ZTextDecoder_UTF32LE;
		}
	else if (charsetLC == "utf-16" || charsetLC == "utf16")
		{
		if (ZCONFIG(Endian, Big))
			return new ZTextDecoder_UTF16BE;
		else
			return new ZTextDecoder_UTF16LE;
		}
	else if (charsetLC == "ucs-4be" || charsetLC == "ucs4be" || charsetLC == "utf-32be" || charsetLC == "utf32be")
		{
		return new ZTextDecoder_UTF32BE;
		}
	else if (charsetLC == "ucs-4le" || charsetLC == "ucs4le" || charsetLC == "utf-32le" || charsetLC == "utf32le")
		{
		return new ZTextDecoder_UTF32LE;
		}
	else if (charsetLC == "utf-16be" || charsetLC == "utf16be")
		{
		return new ZTextDecoder_UTF16BE;
		}
	else if (charsetLC == "utf-16le" || charsetLC == "utf16le")
		{
		return new ZTextDecoder_UTF16LE;
		}
	else if (charsetLC == "ascii")
		{
		return new ZTextDecoder_ASCII;
		}
	else if (charsetLC == "cp1252")
		{
		return new ZTextDecoder_CP1252;
		}
	else if (charsetLC == "cp850")
		{
		return new ZTextDecoder_CP850;
		}
	else if (charsetLC == "iso_8859-1" || charsetLC == "iso-8859-1")
		{
		return new ZTextDecoder_ISO8859_1;
		}
	else if (charsetLC == "macroman")
		{
		return new ZTextDecoder_MacRoman;
		}


	#if ZCONFIG_API_Enabled(TextCoder_Mac)
		try
			{ return new ZTextDecoder_Mac(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_Win)
		try
			{ return new ZTextDecoder_Win(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_iconv)
		try
			{ return new ZTextDecoder_iconv(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_ICU)
		try
			{ return new ZTextDecoder_ICU(iCharset); }
		catch (...)
			{}
	#endif

	return nil;
	}

ZTextEncoder* ZUtil_TextCoder::sCreateEncoder(const string& iCharset)
	{
	string charsetLC = ZUnicode::sToLower(iCharset);
	if (charsetLC == "utf-8" || charsetLC == "utf8")
		{
		return new ZTextEncoder_UTF8;
		}
	else if (charsetLC == "ucs-4" || charsetLC == "ucs4" || charsetLC == "utf-32" || charsetLC == "utf32")
		{
		if (ZCONFIG(Endian, Big))
			return new ZTextEncoder_UTF32BE;
		else
			return new ZTextEncoder_UTF32LE;
		}
	else if (charsetLC == "utf-16" || charsetLC == "utf16")
		{
		if (ZCONFIG(Endian, Big))
			return new ZTextEncoder_UTF16BE;
		else
			return new ZTextEncoder_UTF16LE;
		}
	else if (charsetLC == "ucs-4be" || charsetLC == "ucs4be" || charsetLC == "utf-32be" || charsetLC == "utf32be")
		{
		return new ZTextEncoder_UTF32BE;
		}
	else if (charsetLC == "ucs-4le" || charsetLC == "ucs4le" || charsetLC == "utf-32le" || charsetLC == "utf32le")
		{
		return new ZTextEncoder_UTF32LE;
		}
	else if (charsetLC == "utf-16be" || charsetLC == "utf16be")
		{
		return new ZTextEncoder_UTF16BE;
		}
	else if (charsetLC == "utf-16le" || charsetLC == "utf16le")
		{
		return new ZTextEncoder_UTF16LE;
		}
	else if (charsetLC == "ascii")
		{
		return new ZTextEncoder_ASCII;
		}
	else if (charsetLC == "cp1252")
		{
		return new ZTextEncoder_CP1252;
		}
	else if (charsetLC == "cp850")
		{
		return new ZTextEncoder_CP850;
		}
	else if (charsetLC == "iso_8859-1" || charsetLC == "iso-8859-1")
		{
		return new ZTextEncoder_ISO8859_1;
		}
	else if (charsetLC == "macroman")
		{
		return new ZTextEncoder_MacRoman;
		}

	#if ZCONFIG_API_Enabled(TextCoder_Mac)
		try
			{ return new ZTextEncoder_Mac(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_Win)
		try
			{ return new ZTextEncoder_Win(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_iconv)
		try
			{ return new ZTextEncoder_iconv(iCharset); }
		catch (...)
			{}
	#endif

	#if ZCONFIG_API_Enabled(TextCoder_ICU)
		try
			{ return new ZTextEncoder_ICU(iCharset); }
		catch (...)
			{}
	#endif
	
	return nil;
	}
