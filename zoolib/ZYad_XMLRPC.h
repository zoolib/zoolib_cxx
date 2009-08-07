/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZYad_XMLRPC__
#define __ZYad_XMLRPC__ 1
#include "zconfig.h"

#include "zoolib/ZML.h"
#include "zoolib/ZYad_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_XMLRPC

class ZYadParseException_XMLRPC : public ZYadParseException_Std
	{
public:
	ZYadParseException_XMLRPC(const std::string& iWhat);
	ZYadParseException_XMLRPC(const char* iWhat);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_XMLRPC

namespace ZYad_XMLRPC {

struct Request_t
	{
	std::string fMethodName;
	ZRef<ZYadListR> fParams;
	};

struct Response_t
	{
	ZRef<ZYadR> fResult;
	ZRef<ZYadR> fFault;
	};

bool sFromReader(ZRef<ZML::Reader> iReaderer, Request_t& oRequest);
bool sFromReader(ZRef<ZML::Reader> iReaderer, Response_t& oResponse);

void sToStrim(const ZML::StrimW& s, const Request_t& iRequest);
void sToStrim(const ZML::StrimW& s, const Response_t& iResponse);

} // namespace ZYad_XMLRPC

NAMESPACE_ZOOLIB_END

#endif // __ZYad_XMLRPC__
