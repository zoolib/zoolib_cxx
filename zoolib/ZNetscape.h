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

#ifndef __ZNetscape__
#define __ZNetscape__ 1
#include "zconfig.h"

#include "ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Netscape)

#include "zoolib/ZCompat_npapi.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

namespace ZNetscape {

std::string sAsString(NPNVariable iVar);
std::string sAsString(NPPVariable iVar);

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantBase

class NPVariantBase : public NPVariant
	{
public:
	NPVariantBase();

	NPVariantBase(bool iValue);
	NPVariantBase(int32 iValue);
	NPVariantBase(double iValue);
	NPVariantBase(const std::string& iValue);

	bool IsVoid() const;
	bool IsNull() const;

	bool IsBool() const;
	bool IsInt32() const;
	bool IsDouble() const;
	bool IsString() const;
	bool IsObject() const;

	void SetVoid();
	void SetNull();

	bool GetBool() const;
	bool GetBool(bool& oValue) const;
	bool DGetBool(bool iDefault) const;

	int32 GetInt32() const;
	bool GetInt32(int32& oValue) const;
	int32 DGetInt32(int32 iDefault) const;

	double GetDouble() const;
	bool GetDouble(double& oValue) const;
	double DGetDouble(double iDefault) const;

	std::string GetString() const;
	bool GetString(std::string& oValue) const;
	std::string DGetString(const std::string& iDefault) const;

protected:
	void pSetString(const char* iChars, size_t iLength);
	void pSetString(const std::string& iString);
	};

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)

#endif // __ZNetscape__
