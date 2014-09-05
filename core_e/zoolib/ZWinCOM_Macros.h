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

#ifndef __ZWinCOM_Macros_h__
#define __ZWinCOM_Macros_h__ 1
#include "zconfig.h"

#if ZCONFIG(Compiler, MSVC) || ZCONFIG(Compiler, CodeWarrior)

	#define ZMACRO_WinCOM_Class(className, baseClass, l, w0, w1, b0, b1, b2, b3, b4, b5, b6, b7) \
		MIDL_INTERFACE(ZMACRO_Stringify(l##-##w0##-##w1##-##b0##b1##-##b2##b3##b4##b5##b6##b7))\
		className : public baseClass {

	#define ZMACRO_WinCOM_Definition(className)

	#define ZMACRO_UUID(className) __uuidof(className)

#else

	#define ZMACRO_WinCOM_Class(className, baseClass, l, w0, w1, b0, b1, b2, b3, b4, b5, b6, b7) \
		struct className : public baseClass {\
		enum { ZIID_l=0x##l,\
			ZIID_w0=0x##w0,ZIID_w1=0x##w1,\
			ZIID_b0=0x##b0,ZIID_b1=0x##b1,ZIID_b2=0x##b2,ZIID_b3=0x##b3,\
			ZIID_b4=0x##b4,ZIID_b5=0x##b5,ZIID_b6=0x##b6,ZIID_b7=0x##b7};\
		static const IID sIID;

	#define ZMACRO_WinCOM_Definition(className)\
		const IID className::sIID = {ZIID_l,\
			ZIID_w0,ZIID_w1,\
			{ZIID_b0,ZIID_b1,ZIID_b2,ZIID_b3,\
			ZIID_b4,ZIID_b5,ZIID_b6,ZIID_b7}};

	#define ZMACRO_UUID(className) (className::sIID)

#endif

#endif // __ZWinCOM_Macros_h__
