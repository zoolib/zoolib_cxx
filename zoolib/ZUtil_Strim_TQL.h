/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_Strim_TQL__
#define __ZUtil_Strim_TQL__
#include "zconfig.h"

#include "ZStrim.h"
#include "ZTQL_Query.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim_TQL

namespace ZUtil_Strim_TQL {

struct Options
	{
	Options();

	std::string fEOLString;
	std::string fIndentString;
	};

void sToStrim(const ZStrimW& iStrimW, const ZTQL::Query& iQuery);
void sToStrim(const ZStrimW& iStrimW, ZRef<ZTQL::Node> iNode);

void sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	const ZTQL::Query& iQuery);

void sToStrim(const ZStrimW& iStrimW,
	size_t iInitialIndent, const Options& iOptions,
	ZRef<ZTQL::Node> iNode);

void sWrite_PropName(const ZStrimW& s, const ZTName& iTName);
void sWrite_RelHead(const ZStrimW& s, const ZTQL::RelHead& iRelHead);

} // namespace ZUtil_Strim_TQL

#endif // __ZUtil_Strim_TQL__
