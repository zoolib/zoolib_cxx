/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#ifndef __ZooLib_Pixels_PixvalIters_h__
#define __ZooLib_Pixels_PixvalIters_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/PixvalAccessor.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalIterR

class PixvalIterR
	{
public:
	PixvalIterR(const PixvalDesc& iPixvalDesc, const void* iAddress, int iH);

	Pixval ReadInc();
	Pixval Read();

	void Inc();

	void Reset(const void* iAddress, int iH);

protected:
	PixvalAccessor fAccessor;
	const void* fAddress;

	int fH;
	};

// =================================================================================================
#pragma mark - PixvalIterW

class PixvalIterW
	{
public:
	PixvalIterW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH);

	void WriteInc(Pixval iPixval);
	void Write(Pixval iPixval);

	void Inc();

	void Reset(void* iAddress, int iH);

private:
	PixvalAccessor fAccessor;
	void* fAddress;

	int fH;
	};

// =================================================================================================
#pragma mark - PixvalIterRW

class PixvalIterRW
	{
public:
	PixvalIterRW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH);

	Pixval ReadInc();
	Pixval Read();

	void WriteInc(Pixval iPixval);
	void Write(Pixval iPixval);

	void Inc();

	void Reset(void* iAddress, int iH);

private:
	PixvalAccessor fAccessor;
	void* fAddress;

	int fH;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixvalIters_h__
