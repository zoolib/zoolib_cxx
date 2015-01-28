/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_String_h__
#define __ZStream_String_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStreamRPos_String

/// ZStreamRPos_String provides a ZStreamRPos interface to a referenced standard library string.

class ZStreamRPos_String : public ZStreamRPos
	{
public:
	ZStreamRPos_String(const std::string& inString);
	~ZStreamRPos_String();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();

protected:
	const std::string& fString;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamWPos_String

/// ZStreamWPos_String provides a ZStreamWPos interface to a referenced standard library string.

class ZStreamWPos_String : public ZStreamWPos
	{
public:
	ZStreamWPos_String(std::string& inString);
	~ZStreamWPos_String();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

protected:
	std::string& fString;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamRWPos_String

/// ZStreamRWPos_String provides a ZStreamRWPos interface to a referenced standard library string.

class ZStreamRWPos_String : public ZStreamRWPos
	{
public:
	ZStreamRWPos_String(std::string& inString);
	~ZStreamRWPos_String();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 inPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 inSize);

protected:
	std::string& fString;
	uint64 fPosition;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZStream_String_h__
