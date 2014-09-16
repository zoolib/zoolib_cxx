/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_ChanW_UTF_string_h__
#define __ZooLib_ChanW_UTF_string_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF_More.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanW_UTF_string

template <class UTF_t> class ChanW_UTF_string;

// =================================================================================================
// MARK: - ChanW_UTF_string<string32>

template <>
class ChanW_UTF_string<UTF32>
:	public ChanW_UTF_Native32
	{
public:
	ChanW_UTF_string(string32* ioString);

	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

protected:
	string32* fString;
	};

typedef ChanW_UTF_string<UTF32> ChanW_UTF_string32;

// =================================================================================================
// MARK: - ChanW_UTF_string<string32>

template <>
class ChanW_UTF_string<UTF16>
:	public ChanW_UTF_Native16
	{
public:
	ChanW_UTF_string(string16* ioString);

	virtual size_t Write(const UTF16* iSource, size_t iCountCU);

protected:
	string16* fString;
	};

typedef ChanW_UTF_string<UTF16> ChanW_UTF_string16;

// =================================================================================================
// MARK: - ChanW_UTF_string<string32>

template <>
class ChanW_UTF_string<UTF8>
:	public ChanW_UTF_Native8
	{
public:
	ChanW_UTF_string(string8* ioString);

	virtual size_t Write(const UTF8* iSource, size_t iCountCU);

protected:
	string8* fString;
	};

typedef ChanW_UTF_string<UTF8> ChanW_UTF_string8;

} // namespace ZooLib

#endif // __ZooLib_ChanW_UTF_string_h__
