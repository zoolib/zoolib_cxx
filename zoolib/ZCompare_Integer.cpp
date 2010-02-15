/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompare_Integer.h"

NAMESPACE_ZOOLIB_BEGIN

#define ZMACRO_InstantiateTemplate_And_CompareRegistration(t) \
	template <> int sCompare_T(const t& iL, const t& iR); \
	ZMACRO_CompareRegistration_T(t)
	
ZMACRO_InstantiateTemplate_And_CompareRegistration(bool);
ZMACRO_InstantiateTemplate_And_CompareRegistration(char);
ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned char);
ZMACRO_InstantiateTemplate_And_CompareRegistration(signed char);
ZMACRO_InstantiateTemplate_And_CompareRegistration(short);
ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned short);
ZMACRO_InstantiateTemplate_And_CompareRegistration(int);
ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned int);
ZMACRO_InstantiateTemplate_And_CompareRegistration(long);
ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned long);

#if ZCONFIG(Compiler, MSVC)

	ZMACRO_InstantiateTemplate_And_CompareRegistration(__int64);
	ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned __int64);

#else

	ZMACRO_InstantiateTemplate_And_CompareRegistration(long long);
	ZMACRO_InstantiateTemplate_And_CompareRegistration(unsigned long long);

#endif

NAMESPACE_ZOOLIB_END
