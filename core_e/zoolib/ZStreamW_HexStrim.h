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

#ifndef __ZStreamW_HexStrim_h__
#define __ZStreamW_HexStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZStrimW_InsertSeparator.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamW_HexStrim_Real

class ZStreamW_HexStrim_Real : public ZStreamW
	{
public:
	ZStreamW_HexStrim_Real(bool iUseUnderscore, const ZStrimW& iStrimSink);

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStrimW& fStrimSink;
	const char* fHexDigits;
	};

// =================================================================================================
// MARK: - ZStreamW_HexStrim

class ZStreamW_HexStrim : public ZStreamW
	{
public:
	ZStreamW_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize, const ZStrimW& iStrimSink);

	ZStreamW_HexStrim(const std::string& iByteSeparator,
		const std::string& iChunkSeparator, size_t iChunkSize,
		bool iUseUnderscore, const ZStrimW& iStrimSink);

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	ZStrimW_InsertSeparator fStrim;
	ZStreamW_HexStrim_Real fStream;
	};

} // namespace ZooLib

#endif // __ZStreamW_HexStrim_h__
