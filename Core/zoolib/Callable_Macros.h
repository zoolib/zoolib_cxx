/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZooLib_Callable_Macros_h__
#define __ZooLib_Callable_Macros_h__ 1
#include "zconfig.h"

// =================================================================================================
#pragma mark - ZooLib::CallableUtil

namespace ZooLib {
namespace CallableUtil {

template <class T> struct VT
	{
	typedef T Mutable;

	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<const T&>
	{
	typedef T Mutable;

	typedef const T Field;
	typedef const T& Param;

	typedef Field F;
	typedef Param P;
	};

template <class T> struct VT<T&>
	{
	typedef T Mutable;

	typedef T Field;
	typedef T& Param;

	typedef Field F;
	typedef Param P;
	};

} // namespace CallableUtil
} // namespace ZooLib

// =================================================================================================
#pragma mark - ZooLib::Callable_Macros

#define ZMACRO_Callable_typedef_V
#define ZMACRO_Callable_typedef_0 typedef P0_p P0
#define ZMACRO_Callable_typedef_1 ZMACRO_Callable_typedef_0;typedef P1_p P1
#define ZMACRO_Callable_typedef_2 ZMACRO_Callable_typedef_1;typedef P2_p P2
#define ZMACRO_Callable_typedef_3 ZMACRO_Callable_typedef_2;typedef P3_p P3
#define ZMACRO_Callable_typedef_4 ZMACRO_Callable_typedef_3;typedef P4_p P4
#define ZMACRO_Callable_typedef_5 ZMACRO_Callable_typedef_4;typedef P5_p P5
#define ZMACRO_Callable_typedef_6 ZMACRO_Callable_typedef_5;typedef P6_p P6
#define ZMACRO_Callable_typedef_7 ZMACRO_Callable_typedef_6;typedef P7_p P7
#define ZMACRO_Callable_typedef_8 ZMACRO_Callable_typedef_7;typedef P8_p P8
#define ZMACRO_Callable_typedef_9 ZMACRO_Callable_typedef_8;typedef P9_p P9
#define ZMACRO_Callable_typedef_A ZMACRO_Callable_typedef_9;typedef PA_p PA
#define ZMACRO_Callable_typedef_B ZMACRO_Callable_typedef_A;typedef PB_p PB
#define ZMACRO_Callable_typedef_C ZMACRO_Callable_typedef_B;typedef PC_p PC
#define ZMACRO_Callable_typedef_D ZMACRO_Callable_typedef_C;typedef PD_p PD
#define ZMACRO_Callable_typedef_E ZMACRO_Callable_typedef_D;typedef PE_p PE
#define ZMACRO_Callable_typedef_F ZMACRO_Callable_typedef_E;typedef PF_p PF

#define ZMACRO_Callable_PV
#define ZMACRO_Callable_P0 P0_p
#define ZMACRO_Callable_P1 ZMACRO_Callable_P0,P1_p
#define ZMACRO_Callable_P2 ZMACRO_Callable_P1,P2_p
#define ZMACRO_Callable_P3 ZMACRO_Callable_P2,P3_p
#define ZMACRO_Callable_P4 ZMACRO_Callable_P3,P4_p
#define ZMACRO_Callable_P5 ZMACRO_Callable_P4,P5_p
#define ZMACRO_Callable_P6 ZMACRO_Callable_P5,P6_p
#define ZMACRO_Callable_P7 ZMACRO_Callable_P6,P7_p
#define ZMACRO_Callable_P8 ZMACRO_Callable_P7,P8_p
#define ZMACRO_Callable_P9 ZMACRO_Callable_P8,P9_p
#define ZMACRO_Callable_PA ZMACRO_Callable_P9,PA_p
#define ZMACRO_Callable_PB ZMACRO_Callable_PA,PB_p
#define ZMACRO_Callable_PC ZMACRO_Callable_PB,PC_p
#define ZMACRO_Callable_PD ZMACRO_Callable_PC,PD_p
#define ZMACRO_Callable_PE ZMACRO_Callable_PD,PE_p
#define ZMACRO_Callable_PF ZMACRO_Callable_PE,PF_p

#define ZMACRO_Callable_Class_PV
#define ZMACRO_Callable_Class_P0 class P0_p
#define ZMACRO_Callable_Class_P1 ZMACRO_Callable_Class_P0,class P1_p
#define ZMACRO_Callable_Class_P2 ZMACRO_Callable_Class_P1,class P2_p
#define ZMACRO_Callable_Class_P3 ZMACRO_Callable_Class_P2,class P3_p
#define ZMACRO_Callable_Class_P4 ZMACRO_Callable_Class_P3,class P4_p
#define ZMACRO_Callable_Class_P5 ZMACRO_Callable_Class_P4,class P5_p
#define ZMACRO_Callable_Class_P6 ZMACRO_Callable_Class_P5,class P6_p
#define ZMACRO_Callable_Class_P7 ZMACRO_Callable_Class_P6,class P7_p
#define ZMACRO_Callable_Class_P8 ZMACRO_Callable_Class_P7,class P8_p
#define ZMACRO_Callable_Class_P9 ZMACRO_Callable_Class_P8,class P9_p
#define ZMACRO_Callable_Class_PA ZMACRO_Callable_Class_P9,class PA_p
#define ZMACRO_Callable_Class_PB ZMACRO_Callable_Class_PA,class PB_p
#define ZMACRO_Callable_Class_PC ZMACRO_Callable_Class_PB,class PC_p
#define ZMACRO_Callable_Class_PD ZMACRO_Callable_Class_PC,class PD_p
#define ZMACRO_Callable_Class_PE ZMACRO_Callable_Class_PD,class PE_p
#define ZMACRO_Callable_Class_PF ZMACRO_Callable_Class_PE,class PF_p

#define ZMACRO_Callable_PiV
#define ZMACRO_Callable_Pi0 P0_p i0
#define ZMACRO_Callable_Pi1 ZMACRO_Callable_Pi0,P1_p i1
#define ZMACRO_Callable_Pi2 ZMACRO_Callable_Pi1,P2_p i2
#define ZMACRO_Callable_Pi3 ZMACRO_Callable_Pi2,P3_p i3
#define ZMACRO_Callable_Pi4 ZMACRO_Callable_Pi3,P4_p i4
#define ZMACRO_Callable_Pi5 ZMACRO_Callable_Pi4,P5_p i5
#define ZMACRO_Callable_Pi6 ZMACRO_Callable_Pi5,P6_p i6
#define ZMACRO_Callable_Pi7 ZMACRO_Callable_Pi6,P7_p i7
#define ZMACRO_Callable_Pi8 ZMACRO_Callable_Pi7,P8_p i8
#define ZMACRO_Callable_Pi9 ZMACRO_Callable_Pi8,P9_p i9
#define ZMACRO_Callable_PiA ZMACRO_Callable_Pi9,PA_p iA
#define ZMACRO_Callable_PiB ZMACRO_Callable_PiA,PB_p iB
#define ZMACRO_Callable_PiC ZMACRO_Callable_PiB,PC_p iC
#define ZMACRO_Callable_PiD ZMACRO_Callable_PiC,PD_p iD
#define ZMACRO_Callable_PiE ZMACRO_Callable_PiD,PE_p iE
#define ZMACRO_Callable_PiF ZMACRO_Callable_PiE,PF_p iF

#define ZMACRO_Callable_ConstRef_PiV
#define ZMACRO_Callable_ConstRef_Pi0 const P0_p& i0
#define ZMACRO_Callable_ConstRef_Pi1 ZMACRO_Callable_ConstRef_Pi0,const P1_p& i1
#define ZMACRO_Callable_ConstRef_Pi2 ZMACRO_Callable_ConstRef_Pi1,const P2_p& i2
#define ZMACRO_Callable_ConstRef_Pi3 ZMACRO_Callable_ConstRef_Pi2,const P3_p& i3
#define ZMACRO_Callable_ConstRef_Pi4 ZMACRO_Callable_ConstRef_Pi3,const P4_p& i4
#define ZMACRO_Callable_ConstRef_Pi5 ZMACRO_Callable_ConstRef_Pi4,const P5_p& i5
#define ZMACRO_Callable_ConstRef_Pi6 ZMACRO_Callable_ConstRef_Pi5,const P6_p& i6
#define ZMACRO_Callable_ConstRef_Pi7 ZMACRO_Callable_ConstRef_Pi6,const P7_p& i7
#define ZMACRO_Callable_ConstRef_Pi8 ZMACRO_Callable_ConstRef_Pi7,const P8_p& i8
#define ZMACRO_Callable_ConstRef_Pi9 ZMACRO_Callable_ConstRef_Pi8,const P9_p& i9
#define ZMACRO_Callable_ConstRef_PiA ZMACRO_Callable_ConstRef_Pi9,const PA_p& iA
#define ZMACRO_Callable_ConstRef_PiB ZMACRO_Callable_ConstRef_PiA,const PB_p& iB
#define ZMACRO_Callable_ConstRef_PiC ZMACRO_Callable_ConstRef_PiB,const PC_p& iC
#define ZMACRO_Callable_ConstRef_PiD ZMACRO_Callable_ConstRef_PiC,const PD_p& iD
#define ZMACRO_Callable_ConstRef_PiE ZMACRO_Callable_ConstRef_PiD,const PE_p& iE
#define ZMACRO_Callable_ConstRef_PiF ZMACRO_Callable_ConstRef_PiE,const PF_p& iF

#define ZMACRO_Callable_iV
#define ZMACRO_Callable_i0 i0
#define ZMACRO_Callable_i1 ZMACRO_Callable_i0,i1
#define ZMACRO_Callable_i2 ZMACRO_Callable_i1,i2
#define ZMACRO_Callable_i3 ZMACRO_Callable_i2,i3
#define ZMACRO_Callable_i4 ZMACRO_Callable_i3,i4
#define ZMACRO_Callable_i5 ZMACRO_Callable_i4,i5
#define ZMACRO_Callable_i6 ZMACRO_Callable_i5,i6
#define ZMACRO_Callable_i7 ZMACRO_Callable_i6,i7
#define ZMACRO_Callable_i8 ZMACRO_Callable_i7,i8
#define ZMACRO_Callable_i9 ZMACRO_Callable_i8,i9
#define ZMACRO_Callable_iA ZMACRO_Callable_i9,iA
#define ZMACRO_Callable_iB ZMACRO_Callable_iA,iB
#define ZMACRO_Callable_iC ZMACRO_Callable_iB,iC
#define ZMACRO_Callable_iD ZMACRO_Callable_iC,iD
#define ZMACRO_Callable_iE ZMACRO_Callable_iD,iE
#define ZMACRO_Callable_iF ZMACRO_Callable_iE,iF

#define ZMACRO_Callable_VTV
#define ZMACRO_Callable_VT0 typename CallableUtil::VT<P0_p>::P i0
#define ZMACRO_Callable_VT1 ZMACRO_Callable_VT0,typename CallableUtil::VT<P1_p>::P i1
#define ZMACRO_Callable_VT2 ZMACRO_Callable_VT1,typename CallableUtil::VT<P2_p>::P i2
#define ZMACRO_Callable_VT3 ZMACRO_Callable_VT2,typename CallableUtil::VT<P3_p>::P i3
#define ZMACRO_Callable_VT4 ZMACRO_Callable_VT3,typename CallableUtil::VT<P4_p>::P i4
#define ZMACRO_Callable_VT5 ZMACRO_Callable_VT4,typename CallableUtil::VT<P5_p>::P i5
#define ZMACRO_Callable_VT6 ZMACRO_Callable_VT5,typename CallableUtil::VT<P6_p>::P i6
#define ZMACRO_Callable_VT7 ZMACRO_Callable_VT6,typename CallableUtil::VT<P7_p>::P i7
#define ZMACRO_Callable_VT8 ZMACRO_Callable_VT7,typename CallableUtil::VT<P8_p>::P i8
#define ZMACRO_Callable_VT9 ZMACRO_Callable_VT8,typename CallableUtil::VT<P9_p>::P i9
#define ZMACRO_Callable_VTA ZMACRO_Callable_VT9,typename CallableUtil::VT<PA_p>::P iA
#define ZMACRO_Callable_VTB ZMACRO_Callable_VTA,typename CallableUtil::VT<PB_p>::P iB
#define ZMACRO_Callable_VTC ZMACRO_Callable_VTB,typename CallableUtil::VT<PC_p>::P iC
#define ZMACRO_Callable_VTD ZMACRO_Callable_VTC,typename CallableUtil::VT<PD_p>::P iD
#define ZMACRO_Callable_VTE ZMACRO_Callable_VTD,typename CallableUtil::VT<PE_p>::P iE
#define ZMACRO_Callable_VTF ZMACRO_Callable_VTE,typename CallableUtil::VT<PF_p>::P iF

#endif // __ZooLib_Callable_Macros_h__
