/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/Compat_algorithm.h" // for std::min
#include "zoolib/TextCoderAliases.h"

using std::min;
using std::string;

namespace ZooLib {

// =================================================================================================
#pragma mark - TextCoderAliases

namespace { // anonymous
struct CharsetEntry_t
	{
	const char* fName;
	int fMIB;
	};
} // anonymous namespace

static const CharsetEntry_t spCharsetTable[] =
	{
		{ "csmacintosh", 2027 },
		{ "mac", 2027 },
		{ "macintosh", 2027 },
		{ "x-mac-roman", 2027 },
		{ "mac-roman", 2027 },
		{ "macroman", 2027 },

		{ "cp1252", 2252 },
		{ "windows-1252", 2252 },
		{ "winlatin1", 2252 },
		{ "x-ansi", 2252 },

		{ "8859_1", 4 },
		{ "cp819", 4 },
		{ "csisolatin1", 4 },
		{ "ibm819", 4 },
		{ "iso-8859-1", 4 },
		{ "iso-ir-100", 4 },
		{ "iso8859-1", 4 },
		{ "iso_8859-1", 4 },
		{ "iso_8859-1:1987", 4 },
		{ "l1", 4 },
		{ "latin1", 4 },

		{ "x-nextstep", -1 },

		{ "ansi_x3.4-1968", 3 },
		{ "ansi_x3.4-1986", 3 },
		{ "ascii", 3 },
		{ "cp367", 3 },
		{ "csascii", 3 },
		{ "ibm367", 3 },
		{ "iso-ir-6", 3 },
		{ "iso-ir-6us", 3 },
		{ "iso646-us", 3 },
		{ "iso_646.irv:1991", 3 },
		{ "us", 3 },
		{ "us-ascii", 3 },

		{ "csunicode", 1000 },
		{ "iso-10646-ucs-2", 1000 },

		{ "unicode-1-1-utf-8", 106 },
		{ "unicode-2-0-utf-8", 106 },
		{ "utf-16", 106 },
		{ "utf-8", 106 },
		{ "x-unicode-2-0-utf-8", 106 },

		{ "x-mac-japanese", -1 },

		{ "x-mac-chinesetrad", -1 },
		{ "x-mac-trad-chinese", -1 },

		{ "csksc56011987", 36 },
		{ "iso-ir-149", 36 },
		{ "korean", 36 },
		{ "ks_c_5601", 36 },
		{ "ks_c_5601-1987", 36 },
		{ "ks_c_5601-1989", 36 },
		{ "ks_c_5601_1987", 36 },
		{ "ksc5601", 36 },
		{ "ksc_5601", 36 },
		{ "x-mac-korean", 36 },

		{ "x-mac-arabic", -1 },
		{ "x-mac-hebrew", -1 },
		{ "x-mac-greek", -1 },
		{ "x-mac-cyrillic", -1 },
		{ "x-mac-devanagari", -1 },
		{ "x-mac-gurmukhi", -1 },
		{ "x-mac-gujarati", -1 },
		{ "x-mac-thai", -1 },
		{ "x-mac-chinesesimp", -1 },
		{ "x-mac-simp-chinese", -1 },
		{ "x-mac-tibetan", -1 },
		{ "x-mac-ce", -1 },
		{ "x-mac-centraleurroman", -1 },
		{ "x-mac-symbol", -1 },
		{ "x-mac-dingbats", -1 },
		{ "x-mac-turkish", -1 },
		{ "x-mac-croatian", -1 },
		{ "x-mac-icelandic", -1 },
		{ "x-mac-romanian", -1 },
		{ "x-mac-farsi", -1 },
		{ "x-mac-ukrainian", -1 },
		{ "x-mac-vt100", -1 },

		{ "csisolatin2", 5 },
		{ "iso-8859-2", 5 },
		{ "iso-ir-101", 5 },
		{ "iso8859-2", 5 },
		{ "iso_8859-2", 5 },
		{ "iso_8859-2:1987", 5 },
		{ "l2", 5 },
		{ "latin2", 5 },

		{ "csisolatin3", 6 },
		{ "iso-8859-3", 6 },
		{ "iso-ir-109", 6 },
		{ "iso_8859-3", 6 },
		{ "iso_8859-3:1988", 6 },
		{ "l3", 6 },
		{ "latin3", 6 },

		{ "csisolatin4", 7 },
		{ "iso-8859-4", 7 },
		{ "iso-ir-110", 7 },
		{ "iso_8859-4", 7 },
		{ "iso_8859-4:1988", 7 },
		{ "l4", 7 },
		{ "latin4", 7 },

		{ "csisolatincyrillic", 8 },
		{ "cyrillic", 8 },
		{ "iso-8859-5", 8 },
		{ "iso-ir-144", 8 },
		{ "iso_8859-5", 8 },
		{ "iso_8859-5:1988", 8 },

		{ "arabic", 9 },
		{ "asmo-708", 9 },
		{ "csisolatinarabic", 9 },
		{ "ecma-114", 9 },
		{ "iso-8859-6", 9 },
		{ "iso-ir-127", 9 },
		{ "iso_8859-6", 9 },
		{ "iso_8859-6:1987", 9 },

		{ "csisolatingreek", 10 },
		{ "ecma-118", 10 },
		{ "elot_928", 10 },
		{ "greek", 10 },
		{ "greek8", 10 },
		{ "iso-8859-7", 10 },
		{ "iso-ir-126", 10 },
		{ "iso_8859-7", 10 },
		{ "iso_8859-7:1987", 10 },

		{ "csiso88598e", 11 },
		{ "csiso88598i", 11 },
		{ "csisolatinhebrew", 11 },
		{ "dos-862", 11 },
		{ "hebrew", 11 },
		{ "iso-8859-8", 11 },
		{ "iso-8859-8-e", 11 },
		{ "iso-8859-8-i", 11 },
		{ "iso-ir-138", 11 },
		{ "iso_8859-8", 11 },
		{ "iso_8859-8-e", 11 },
		{ "iso_8859-8-i", 11 },
		{ "iso_8859-8:1988", 11 },
		{ "logical", 11 },
		{ "visual", 11 },

		{ "csisolatin5", 12 },
		{ "iso-8859-9", 12 },
		{ "iso-ir-148", 12 },
		{ "iso_8859-9", 12 },
		{ "iso_8859-9:1989", 12 },
		{ "l5", 12 },
		{ "latin5", 12 },

		{ "csisolatin6", 13 },
		{ "iso-8859-10", 13 },
		{ "iso-ir-157", 13 },
		{ "iso_8859-10:1992", 13 },
		{ "l6", 13 },
		{ "latin6", 13 },

		{ "iso-8859-11", -1 },

		{ "iso-8859-13", 109 },

		{ "iso-8859-14", 110 },
		{ "iso-celtic", 110 },
		{ "iso-ir-199", 110 },
		{ "iso_8859-14", 110 },
		{ "iso_8859-14:1998", 110 },
		{ "l8", 110 },
		{ "latin8", 110 },

		{ "csisolatin9", 111 },
		{ "iso-8859-15", 111 },
		{ "iso_8859-15", 111 },
		{ "l9", 111 },
		{ "latin-9", 111 },
		{ "latin9", 111 },

		{ "437", 2011 },
		{ "cp437", 2011 },
		{ "cspc8codepage437", 2011 },
		{ "ibm437", 2011 },

		{ "cp737", -1 },
		{ "ibm737", -1 },

		{ "cp500", 2087 },
		{ "cp775", 2087 },
		{ "csibm500", 2087 },
		{ "cspc775baltic", 2087 },
		{ "ebcdic-cp-be", 2087 },
		{ "ebcdic-cp-ch", 2087 },
		{ "ibm500", 2087 },
		{ "ibm775", 2087 },

		{ "850", 2009 },
		{ "cp850", 2009 },
		{ "cspc850multilingual", 2009 },
		{ "ibm850", 2009 },

		{ "852", 2010 },
		{ "cp852", 2010 },
		{ "cspcp852", 2010 },
		{ "ibm852", 2010 },

		{ "857", 2047 },
		{ "cp857", 2047 },
		{ "csibm857", 2047 },
		{ "ibm857", 2047 },

		{ "861", 2049 },
		{ "cp-is", 2049 },
		{ "cp861", 2049 },
		{ "csibm861", 2049 },
		{ "ibm861", 2049 },

		{ "cp864", 2051 },
		{ "csibm864", 2051 },
		{ "dos-720", 2051 },
		{ "ibm864", 2051 },

		{ "866", 2086 },
		{ "cp866", 2086 },
		{ "csibm866", 2086 },
		{ "ibm866", 2086 },

		{ "869", 2054 },
		{ "cp-gr", 2054 },
		{ "cp869", 2054 },
		{ "csibm869", 2054 },
		{ "ibm869", 2054 },

		{ "cp874", 2259 },
		{ "dos-874", 2259 },
		{ "tis-620", 2259 },
		{ "windows-874", 2259 },

		{ "cp932", 2024 },
		{ "csshiftjis", 2024 },
		{ "cswindows31j", 2024 },
		{ "ms_kanji", 2024 },
		{ "shift-jis", 2024 },
		{ "shift_jis", 2024 },
		{ "windows-31j", 2024 },
		{ "x-ms-cp932", 2024 },
		{ "x-sjis", 2024 },

		{ "cp949", -1 },

		{ "cp950", -1 },

		{ "windows-1250", 2250 },
		{ "winlatin2", 2250 },
		{ "x-cp1250", 2250 },

		{ "wincyrillic", 2251 },
		{ "windows-1251", 2251 },
		{ "x-cp1251", 2251 },

		{ "windows-1253", 2253 },
		{ "wingreek", 2253 },

		{ "windows-1254", 2254 },
		{ "winturkish", 2254 },

		{ "windows-1255", 2255 },
		{ "winhebrew", 2255 },

		{ "cp1256", 2256 },
		{ "winarabic", 2256 },
		{ "windows-1256", 2256 },

		{ "winbaltic", 2257 },
		{ "windows-1257", 2257 },

		{ "johab", -1 },

		{ "windows-1258", 2258 },
		{ "winvietnamese", 2258 },

		{ "cshalfwidthkatakana", 15 },
		{ "jis_x0201", 15 },
		{ "x0201", 15 },

		{ "csiso87jisx0208", 63 },
		{ "iso-ir-87", 63 },
		{ "jis_c6226-1983", 63 },
		{ "jis_x0208-1983", 63 },
		{ "x0208", 63 },

		{ "jis_x0208-1990", -1 },

		{ "csiso159jisx02121990", 98 },
		{ "iso-ir-159", 98 },
		{ "jis_x0212-1990", 98 },
		{ "x0212", 98 },

		{ "csiso42jisc62261978", 49 },
		{ "iso-ir-42", 49 },
		{ "jis_c6226-1978", 49 },

		{ "chinese", 57 },
		{ "csgb231280", 57 },
		{ "csiso58gb231280", 57 },
		{ "gb2312-80", 57 },
		{ "gb231280", 57 },
		{ "gb_2312-80", 57 },
		{ "iso-ir-58", 57 },

		{ "x-gbk", -1 },

		{ "gb18030", 114 },

		{ "csiso2022jp", 39 },
		{ "iso-2022-jp", 39 },

		{ "csiso2022jp2", 40 },
		{ "iso-2022-jp-2", 40 },

		{ "iso-2022-jp-1", -1 },

		{ "iso-2022-jp-3", -1 },

		{ "iso-2022-cn", 104 },

		{ "iso-2022-cn-ext", 105 },

		{ "csiso2022kr", 37 },
		{ "iso-2022-kr", 37 },

		{ "cseucpkdfmtjapanese", 18 },
		{ "euc-jp", 18 },
		{ "extended_unix_code_packed_format_for_japanese", 18 },
		{ "x-euc", 18 },
		{ "x-euc-jp", 18 },

		{ "cn-gb", 113 },
		{ "cp936", 113 },
		{ "csgb2312", 113 },
		{ "euc-cn", 113 },
		{ "gb2312", 113 },
		{ "gbk", 113 },
		{ "ms936", 113 },
		{ "windows-936", 113 },
		{ "x-euc-cn", 113 },

		{ "euc-tw", -1 },

		{ "cseuckr", 38 },
		{ "euc-kr", 38 },

		{ "cskoi8r", 2084 },
		{ "koi", 2084 },
		{ "koi8", 2084 },
		{ "koi8-r", 2084 },
		{ "koi8r", 2084 },

		{ "big5", 2026 },
		{ "cn-big5", 2026 },
		{ "csbig5", 2026 },
		{ "x-x-big5", 2026 },

		{ "x-mac-roman-latin1", -1 },

		{ "hz-gb-2312", 2085 },

		{ "big5-hkscs", 2101 },

		{ "cp037", 2028 },
		{ "csibm037", 2028 },
		{ "ebcdic-cp-ca", 2028 },
		{ "ebcdic-cp-nl", 2028 },
		{ "ebcdic-cp-us", 2028 },
		{ "ebcdic-cp-wt", 2028 },
		{ "ibm037", 2028 },
		{ 0, 0 }
	};

std::vector<std::string> sGetTextCoderAliases(const std::string& iName)
	{
	std::vector<std::string> result;
	const CharsetEntry_t* found = nullptr;
	for (const CharsetEntry_t* current = spCharsetTable; current->fName && !found; ++current)
		{
		if (iName == current->fName)
			found = current;
		}

	if (found)
		{
		int theMIB = found->fMIB;
		if (theMIB != -1)
			{
			for (const CharsetEntry_t* current = spCharsetTable; current->fName; ++current)
				{
				if (theMIB == current->fMIB)
					result.push_back(current->fName);
				}
			}
		}

	return result;
	}

} // namespace ZooLib
