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

#include "zoolib/ZAny.h"

#if ! ZCONFIG_SPI_Enabled(boost)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZAny

ZAny::ZAny()
:	content(0)
	{}

ZAny::ZAny(const ZAny& other)
:	content(other.content ? other.content->clone() : 0)
	{}

ZAny::~ZAny()
	{ delete content; }

ZAny& ZAny::operator=(ZAny rhs)
	{
	rhs.swap(*this);
	return *this;
	}

ZAny& ZAny::swap(ZAny& rhs)
	{
	std::swap(content, rhs.content);
	return *this;
	}

bool ZAny::empty() const
	{ return !content; }

const std::type_info& ZAny::type() const
	{ return content ? content->type() : typeid(void); }

NAMESPACE_ZOOLIB_END

#endif // ! ZCONFIG_SPI_Enabled(boost)
