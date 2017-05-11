/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZStrimmerFromStrim_h__
#define __ZStrimmerFromStrim_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrimmer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerFromStrimR

class ZStrimmerFromStrimR
	{
public:
	// TODO: In C++11 we will want to disallow rvalue refs here.
	ZStrimmerFromStrimR(const ZStrimR& iStrim);
	~ZStrimmerFromStrimR();

	operator ZRef<ZStrimmerR>();

private:
	class Strimmer;
	ZRef<Strimmer> fStrimmer;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerFromStrimU

class ZStrimmerFromStrimU
	{
public:
	ZStrimmerFromStrimU(const ZStrimU& iStrim);
	~ZStrimmerFromStrimU();

	operator ZRef<ZStrimmerU>();

private:
	class Strimmer;
	ZRef<Strimmer> fStrimmer;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStrimmerFromStrimW

class ZStrimmerFromStrimW
	{
public:
	ZStrimmerFromStrimW(const ZStrimW& iStrim);
	~ZStrimmerFromStrimW();

	operator ZRef<ZStrimmerW>();

private:
	class Strimmer;
	ZRef<Strimmer> fStrimmer;
	};

} // namespace ZooLib

#endif // __ZStrimmerFromStrim_h__
