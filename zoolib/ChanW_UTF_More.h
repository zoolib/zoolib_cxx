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

#ifndef __ZooLib_ChanW_UTF_More_h__
#define __ZooLib_ChanW_UTF_More_h__ 1
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanW_UTF_Native32

class ChanW_UTF_Native32
:	public ChanW_UTF
	{
public:
	virtual size_t Write(const UTF32* iSource, size_t iCount) = 0;
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU);
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);
	};

// =================================================================================================
// MARK: - ChanW_UTF_Native16

class ChanW_UTF_Native16
:	public ChanW_UTF
	{
public:
	virtual size_t Write(const UTF32* iSource, size_t iCount);
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU) = 0;
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);
	};

// =================================================================================================
// MARK: - ChanW_UTF_Native8

class ChanW_UTF_Native8
:	public ChanW_UTF
	{
public:
	virtual size_t Write(const UTF32* iSource, size_t iCount);
	virtual size_t WriteUTF16(const UTF16* iSource, size_t iCountCU);
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU) = 0;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanW_UTF_More_h__
