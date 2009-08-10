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

#include "zoolib/netscape/ZNetscape_Variant.h"

NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * NPVariantBase

template <>
bool NPVariantBase::QGet_T<bool>(bool& oVal) const
	{
	if (type == NPVariantType_Bool)
		{
		oVal = value.boolValue;
		return true;
		}
	return false;
	}

template <>
bool NPVariantBase::QGet_T<int32>(int32& oVal) const
	{
	if (type == NPVariantType_Int32)
		{
		oVal = value.intValue;
		return true;
		}
	return false;
	}

template <>
bool NPVariantBase::QGet_T<double>(double& oVal) const
	{
	if (type == NPVariantType_Double)
		{
		oVal = value.doubleValue;
		return true;
		}
	return false;
	}

template <>
bool NPVariantBase::QGet_T<std::string>(std::string& oVal) const
	{
	if (type == NPVariantType_String)
		{
		oVal = std::string(
			sNPStringCharsConst(value.stringValue),
			sNPStringLengthConst(value.stringValue));
		return true;
		}
	return false;
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END
