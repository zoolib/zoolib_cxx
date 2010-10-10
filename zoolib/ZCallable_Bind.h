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

#ifndef __ZCallable_Bind__
#define __ZCallable_Bind__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_Bind {

// =================================================================================================
#pragma mark -
#pragma mark * SigTraits_T

struct Empty_t {};

template <class Signature>
struct SigTraits_T;

// -----
template <class R_p>
struct SigTraits_T<R_p(void)>
	{
	typedef R_p R;

	typedef Empty_t L0;
	typedef Empty_t L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef Empty_t R0;
	typedef Empty_t R1;
	typedef Empty_t R2;
	typedef Empty_t R3;
	typedef Empty_t R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)();
	};
// -----
template <class R_p,
	class P0>
struct SigTraits_T<R_p(P0)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef Empty_t L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P0 R0;
	typedef Empty_t R1;
	typedef Empty_t R2;
	typedef Empty_t R3;
	typedef Empty_t R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0);

	typedef R (SignatureL01)();

	typedef R (SignatureR01)();
	};
// -----
template <class R_p,
	class P0, class P1>
struct SigTraits_T<R_p(P0,P1)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef Empty_t L2;
	typedef Empty_t L3;
	typedef Empty_t L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P1 R0;
	typedef P0 R1;
	typedef Empty_t R2;
	typedef Empty_t R3;
	typedef Empty_t R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1);

	typedef R (SignatureL01)(P1);
	typedef R (SignatureL02)();

	typedef R (SignatureR01)(P0);
	typedef R (SignatureR02)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2>
struct SigTraits_T<R_p(P0,P1,P2)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef Empty_t L3;
	typedef Empty_t L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P2 R0;
	typedef P1 R1;
	typedef P0 R2;
	typedef Empty_t R3;
	typedef Empty_t R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2);

	typedef R (SignatureL01)(P1,P2);
	typedef R (SignatureL02)(P2);
	typedef R (SignatureL03)();

	typedef R (SignatureR01)(P0,P1);
	typedef R (SignatureR02)(P0);
	typedef R (SignatureR03)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3>
struct SigTraits_T<R_p(P0,P1,P2,P3)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef Empty_t L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P3 R0;
	typedef P2 R1;
	typedef P1 R2;
	typedef P0 R3;
	typedef Empty_t R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3);

	typedef R (SignatureL01)(P1,P2,P3);
	typedef R (SignatureL02)(P2,P3);
	typedef R (SignatureL03)(P3);
	typedef R (SignatureL04)();

	typedef R (SignatureR01)(P0,P1,P2);
	typedef R (SignatureR02)(P0,P1);
	typedef R (SignatureR03)(P0);
	typedef R (SignatureR04)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef Empty_t L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P4 R0;
	typedef P3 R1;
	typedef P2 R2;
	typedef P1 R3;
	typedef P0 R4;
	typedef Empty_t R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4);

	typedef R (SignatureL01)(P1,P2,P3,P4);
	typedef R (SignatureL02)(P2,P3,P4);
	typedef R (SignatureL03)(P3,P4);
	typedef R (SignatureL04)(P4);
	typedef R (SignatureL05)();

	typedef R (SignatureR01)(P0,P1,P2,P3);
	typedef R (SignatureR02)(P0,P1,P2);
	typedef R (SignatureR03)(P0,P1);
	typedef R (SignatureR04)(P0);
	typedef R (SignatureR05)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef Empty_t L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P5 R0;
	typedef P4 R1;
	typedef P3 R2;
	typedef P2 R3;
	typedef P1 R4;
	typedef P0 R5;
	typedef Empty_t R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5);
	typedef R (SignatureL02)(P2,P3,P4,P5);
	typedef R (SignatureL03)(P3,P4,P5);
	typedef R (SignatureL04)(P4,P5);
	typedef R (SignatureL05)(P5);
	typedef R (SignatureL06)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4);
	typedef R (SignatureR02)(P0,P1,P2,P3);
	typedef R (SignatureR03)(P0,P1,P2);
	typedef R (SignatureR04)(P0,P1);
	typedef R (SignatureR05)(P0);
	typedef R (SignatureR06)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef Empty_t L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P6 R0;
	typedef P5 R1;
	typedef P4 R2;
	typedef P3 R3;
	typedef P2 R4;
	typedef P1 R5;
	typedef P0 R6;
	typedef Empty_t R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6);
	typedef R (SignatureL03)(P3,P4,P5,P6);
	typedef R (SignatureL04)(P4,P5,P6);
	typedef R (SignatureL05)(P5,P6);
	typedef R (SignatureL06)(P6);
	typedef R (SignatureL07)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4);
	typedef R (SignatureR03)(P0,P1,P2,P3);
	typedef R (SignatureR04)(P0,P1,P2);
	typedef R (SignatureR05)(P0,P1);
	typedef R (SignatureR06)(P0);
	typedef R (SignatureR07)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef Empty_t L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P7 R0;
	typedef P6 R1;
	typedef P5 R2;
	typedef P4 R3;
	typedef P3 R4;
	typedef P2 R5;
	typedef P1 R6;
	typedef P0 R7;
	typedef Empty_t R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7);
	typedef R (SignatureL04)(P4,P5,P6,P7);
	typedef R (SignatureL05)(P5,P6,P7);
	typedef R (SignatureL06)(P6,P7);
	typedef R (SignatureL07)(P7);
	typedef R (SignatureL08)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4);
	typedef R (SignatureR04)(P0,P1,P2,P3);
	typedef R (SignatureR05)(P0,P1,P2);
	typedef R (SignatureR06)(P0,P1);
	typedef R (SignatureR07)(P0);
	typedef R (SignatureR08)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef Empty_t L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P8 R0;
	typedef P7 R1;
	typedef P6 R2;
	typedef P5 R3;
	typedef P4 R4;
	typedef P3 R5;
	typedef P2 R6;
	typedef P1 R7;
	typedef P0 R8;
	typedef Empty_t R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8);
	typedef R (SignatureL05)(P5,P6,P7,P8);
	typedef R (SignatureL06)(P6,P7,P8);
	typedef R (SignatureL07)(P7,P8);
	typedef R (SignatureL08)(P8);
	typedef R (SignatureL09)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4);
	typedef R (SignatureR05)(P0,P1,P2,P3);
	typedef R (SignatureR06)(P0,P1,P2);
	typedef R (SignatureR07)(P0,P1);
	typedef R (SignatureR08)(P0);
	typedef R (SignatureR09)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef Empty_t LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef P9 R0;
	typedef P8 R1;
	typedef P7 R2;
	typedef P6 R3;
	typedef P5 R4;
	typedef P4 R5;
	typedef P3 R6;
	typedef P2 R7;
	typedef P1 R8;
	typedef P0 R9;
	typedef Empty_t RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9);
	typedef R (SignatureL06)(P6,P7,P8,P9);
	typedef R (SignatureL07)(P7,P8,P9);
	typedef R (SignatureL08)(P8,P9);
	typedef R (SignatureL09)(P9);
	typedef R (SignatureL10)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4);
	typedef R (SignatureR06)(P0,P1,P2,P3);
	typedef R (SignatureR07)(P0,P1,P2);
	typedef R (SignatureR08)(P0,P1);
	typedef R (SignatureR09)(P0);
	typedef R (SignatureR10)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef Empty_t LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef PA R0;
	typedef P9 R1;
	typedef P8 R2;
	typedef P7 R3;
	typedef P6 R4;
	typedef P5 R5;
	typedef P4 R6;
	typedef P3 R7;
	typedef P2 R8;
	typedef P1 R9;
	typedef P0 RA;
	typedef Empty_t RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA);
	typedef R (SignatureL07)(P7,P8,P9,PA);
	typedef R (SignatureL08)(P8,P9,PA);
	typedef R (SignatureL09)(P9,PA);
	typedef R (SignatureL10)(PA);
	typedef R (SignatureL11)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4);
	typedef R (SignatureR07)(P0,P1,P2,P3);
	typedef R (SignatureR08)(P0,P1,P2);
	typedef R (SignatureR09)(P0,P1);
	typedef R (SignatureR10)(P0);
	typedef R (SignatureR11)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef PB LB;
	typedef Empty_t LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef PB R0;
	typedef PA R1;
	typedef P9 R2;
	typedef P8 R3;
	typedef P7 R4;
	typedef P6 R5;
	typedef P5 R6;
	typedef P4 R7;
	typedef P3 R8;
	typedef P2 R9;
	typedef P1 RA;
	typedef P0 RB;
	typedef Empty_t RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA,PB);
	typedef R (SignatureL07)(P7,P8,P9,PA,PB);
	typedef R (SignatureL08)(P8,P9,PA,PB);
	typedef R (SignatureL09)(P9,PA,PB);
	typedef R (SignatureL10)(PA,PB);
	typedef R (SignatureL11)(PB);
	typedef R (SignatureL12)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR07)(P0,P1,P2,P3,P4);
	typedef R (SignatureR08)(P0,P1,P2,P3);
	typedef R (SignatureR09)(P0,P1,P2);
	typedef R (SignatureR10)(P0,P1);
	typedef R (SignatureR11)(P0);
	typedef R (SignatureR12)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef PB LB;
	typedef PC LC;
	typedef Empty_t LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef PC R0;
	typedef PB R1;
	typedef PA R2;
	typedef P9 R3;
	typedef P8 R4;
	typedef P7 R5;
	typedef P6 R6;
	typedef P5 R7;
	typedef P4 R8;
	typedef P3 R9;
	typedef P2 RA;
	typedef P1 RB;
	typedef P0 RC;
	typedef Empty_t RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL07)(P7,P8,P9,PA,PB,PC);
	typedef R (SignatureL08)(P8,P9,PA,PB,PC);
	typedef R (SignatureL09)(P9,PA,PB,PC);
	typedef R (SignatureL10)(PA,PB,PC);
	typedef R (SignatureL11)(PB,PC);
	typedef R (SignatureL12)(PC);
	typedef R (SignatureL13)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR07)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR08)(P0,P1,P2,P3,P4);
	typedef R (SignatureR09)(P0,P1,P2,P3);
	typedef R (SignatureR10)(P0,P1,P2);
	typedef R (SignatureR11)(P0,P1);
	typedef R (SignatureR12)(P0);
	typedef R (SignatureR13)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef PB LB;
	typedef PC LC;
	typedef PD LD;
	typedef Empty_t LE;
	typedef Empty_t LF;

	typedef PD R0;
	typedef PC R1;
	typedef PB R2;
	typedef PA R3;
	typedef P9 R4;
	typedef P8 R5;
	typedef P7 R6;
	typedef P6 R7;
	typedef P5 R8;
	typedef P4 R9;
	typedef P3 RA;
	typedef P2 RB;
	typedef P1 RC;
	typedef P0 RD;
	typedef Empty_t RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL07)(P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL08)(P8,P9,PA,PB,PC,PD);
	typedef R (SignatureL09)(P9,PA,PB,PC,PD);
	typedef R (SignatureL10)(PA,PB,PC,PD);
	typedef R (SignatureL11)(PB,PC,PD);
	typedef R (SignatureL12)(PC,PD);
	typedef R (SignatureL13)(PD);
	typedef R (SignatureL14)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR07)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR08)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR09)(P0,P1,P2,P3,P4);
	typedef R (SignatureR10)(P0,P1,P2,P3);
	typedef R (SignatureR11)(P0,P1,P2);
	typedef R (SignatureR12)(P0,P1);
	typedef R (SignatureR13)(P0);
	typedef R (SignatureR14)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef PB LB;
	typedef PC LC;
	typedef PD LD;
	typedef PE LE;
	typedef Empty_t LF;

	typedef PE R0;
	typedef PD R1;
	typedef PC R2;
	typedef PB R3;
	typedef PA R4;
	typedef P9 R5;
	typedef P8 R6;
	typedef P7 R7;
	typedef P6 R8;
	typedef P5 R9;
	typedef P4 RA;
	typedef P3 RB;
	typedef P2 RC;
	typedef P1 RD;
	typedef P0 RE;
	typedef Empty_t RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL07)(P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL08)(P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL09)(P9,PA,PB,PC,PD,PE);
	typedef R (SignatureL10)(PA,PB,PC,PD,PE);
	typedef R (SignatureL11)(PB,PC,PD,PE);
	typedef R (SignatureL12)(PC,PD,PE);
	typedef R (SignatureL13)(PD,PE);
	typedef R (SignatureL14)(PE);
	typedef R (SignatureL15)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR07)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR08)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR09)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR10)(P0,P1,P2,P3,P4);
	typedef R (SignatureR11)(P0,P1,P2,P3);
	typedef R (SignatureR12)(P0,P1,P2);
	typedef R (SignatureR13)(P0,P1);
	typedef R (SignatureR14)(P0);
	typedef R (SignatureR15)();
	};
// -----
template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
struct SigTraits_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
	typedef R_p R;

	typedef P0 L0;
	typedef P1 L1;
	typedef P2 L2;
	typedef P3 L3;
	typedef P4 L4;
	typedef P5 L5;
	typedef P6 L6;
	typedef P7 L7;
	typedef P8 L8;
	typedef P9 L9;
	typedef PA LA;
	typedef PB LB;
	typedef PC LC;
	typedef PD LD;
	typedef PE LE;
	typedef PF LF;

	typedef PF R0;
	typedef PE R1;
	typedef PD R2;
	typedef PC R3;
	typedef PB R4;
	typedef PA R5;
	typedef P9 R6;
	typedef P8 R7;
	typedef P7 R8;
	typedef P6 R9;
	typedef P5 RA;
	typedef P4 RB;
	typedef P3 RC;
	typedef P2 RD;
	typedef P1 RE;
	typedef P0 RF;

	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);

	typedef R (SignatureL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL06)(P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL07)(P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL08)(P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL09)(P9,PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL10)(PA,PB,PC,PD,PE,PF);
	typedef R (SignatureL11)(PB,PC,PD,PE,PF);
	typedef R (SignatureL12)(PC,PD,PE,PF);
	typedef R (SignatureL13)(PD,PE,PF);
	typedef R (SignatureL14)(PE,PF);
	typedef R (SignatureL15)(PF);
	typedef R (SignatureL16)();

	typedef R (SignatureR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SignatureR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SignatureR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SignatureR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SignatureR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SignatureR06)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SignatureR07)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SignatureR08)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SignatureR09)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SignatureR10)(P0,P1,P2,P3,P4,P5);
	typedef R (SignatureR11)(P0,P1,P2,P3,P4);
	typedef R (SignatureR12)(P0,P1,P2,P3);
	typedef R (SignatureR13)(P0,P1,P2);
	typedef R (SignatureR14)(P0,P1);
	typedef R (SignatureR15)(P0);
	typedef R (SignatureR16)();
	};

// =================================================================================================
#pragma mark -
#pragma mark * ParamTraits

template <class T> struct ParamTraits
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ParamTraits<const T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

template <class T> struct ParamTraits<T&>
	{
	typedef const T ConstRef_Remove_t;
	typedef const T& ConstRef_Add_t;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL01

template <class Signature_p>
class CallableL01
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL01>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL01(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L1 P0;
	typedef typename SigTraits::L2 P1;
	typedef typename SigTraits::L3 P2;
	typedef typename SigTraits::L4 P3;
	typedef typename SigTraits::L5 P4;
	typedef typename SigTraits::L6 P5;
	typedef typename SigTraits::L7 P6;
	typedef typename SigTraits::L8 P7;
	typedef typename SigTraits::L9 P8;
	typedef typename SigTraits::LA P9;
	typedef typename SigTraits::LB PA;
	typedef typename SigTraits::LC PB;
	typedef typename SigTraits::LD PC;
	typedef typename SigTraits::LE PD;
	typedef typename SigTraits::LF PE;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL01>::Call;

private:
	R Call()
		{ return fCallable->Call(f0); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->Call(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL02

template <class Signature_p>
class CallableL02
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL02>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL02(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L2 P0;
	typedef typename SigTraits::L3 P1;
	typedef typename SigTraits::L4 P2;
	typedef typename SigTraits::L5 P3;
	typedef typename SigTraits::L6 P4;
	typedef typename SigTraits::L7 P5;
	typedef typename SigTraits::L8 P6;
	typedef typename SigTraits::L9 P7;
	typedef typename SigTraits::LA P8;
	typedef typename SigTraits::LB P9;
	typedef typename SigTraits::LC PA;
	typedef typename SigTraits::LD PB;
	typedef typename SigTraits::LE PC;
	typedef typename SigTraits::LF PD;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL02>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->Call(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL03

template <class Signature_p>
class CallableL03
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL03>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL03(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L3 P0;
	typedef typename SigTraits::L4 P1;
	typedef typename SigTraits::L5 P2;
	typedef typename SigTraits::L6 P3;
	typedef typename SigTraits::L5 P4;
	typedef typename SigTraits::L8 P5;
	typedef typename SigTraits::L9 P6;
	typedef typename SigTraits::LA P7;
	typedef typename SigTraits::LB P8;
	typedef typename SigTraits::LC P9;
	typedef typename SigTraits::LD PA;
	typedef typename SigTraits::LE PB;
	typedef typename SigTraits::LF PC;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL03>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f2;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL04

template <class Signature_p>
class CallableL04
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL04>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL04(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L4 P0;
	typedef typename SigTraits::L5 P1;
	typedef typename SigTraits::L6 P2;
	typedef typename SigTraits::L7 P3;
	typedef typename SigTraits::L8 P4;
	typedef typename SigTraits::L9 P5;
	typedef typename SigTraits::LA P6;
	typedef typename SigTraits::LB P7;
	typedef typename SigTraits::LC P8;
	typedef typename SigTraits::LD P9;
	typedef typename SigTraits::LE PA;
	typedef typename SigTraits::LF PB;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL04>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL05

template <class Signature_p>
class CallableL05
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL05>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL05(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L5 P0;
	typedef typename SigTraits::L6 P1;
	typedef typename SigTraits::L7 P2;
	typedef typename SigTraits::L8 P3;
	typedef typename SigTraits::L9 P4;
	typedef typename SigTraits::LA P5;
	typedef typename SigTraits::LB P6;
	typedef typename SigTraits::LC P7;
	typedef typename SigTraits::LD P8;
	typedef typename SigTraits::LE P9;
	typedef typename SigTraits::LF PA;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL05>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL06

template <class Signature_p>
class CallableL06
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL06>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL06(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L6 P0;
	typedef typename SigTraits::L7 P1;
	typedef typename SigTraits::L8 P2;
	typedef typename SigTraits::L9 P3;
	typedef typename SigTraits::LA P4;
	typedef typename SigTraits::LB P5;
	typedef typename SigTraits::LC P6;
	typedef typename SigTraits::LD P7;
	typedef typename SigTraits::LE P8;
	typedef typename SigTraits::LF P9;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL06>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL07

template <class Signature_p>
class CallableL07
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL07>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL07(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L7 P0;
	typedef typename SigTraits::L8 P1;
	typedef typename SigTraits::L9 P2;
	typedef typename SigTraits::LA P3;
	typedef typename SigTraits::LB P4;
	typedef typename SigTraits::LC P5;
	typedef typename SigTraits::LD P6;
	typedef typename SigTraits::LE P7;
	typedef typename SigTraits::LF P8;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL07>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6, i7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL08

template <class Signature_p>
class CallableL08
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL08>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL08(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L8 P0;
	typedef typename SigTraits::L9 P1;
	typedef typename SigTraits::LA P2;
	typedef typename SigTraits::LB P3;
	typedef typename SigTraits::LC P4;
	typedef typename SigTraits::LD P5;
	typedef typename SigTraits::LE P6;
	typedef typename SigTraits::LF P7;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL08>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5, i6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5, i6, i7); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL09

template <class Signature_p>
class CallableL09
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL09>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL09(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L9 P0;
	typedef typename SigTraits::LA P1;
	typedef typename SigTraits::LB P2;
	typedef typename SigTraits::LC P3;
	typedef typename SigTraits::LD P4;
	typedef typename SigTraits::LE P5;
	typedef typename SigTraits::LF P6;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL09>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4, i5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4, i5, i6); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL10

template <class Signature_p>
class CallableL10
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL10>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL10(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LA P0;
	typedef typename SigTraits::LB P1;
	typedef typename SigTraits::LC P2;
	typedef typename SigTraits::LD P3;
	typedef typename SigTraits::LE P4;
	typedef typename SigTraits::LF P5;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL10>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3, i4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3, i4, i5); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL11

template <class Signature_p>
class CallableL11
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL11>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL11(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LB P0;
	typedef typename SigTraits::LC P1;
	typedef typename SigTraits::LD P2;
	typedef typename SigTraits::LE P3;
	typedef typename SigTraits::LF P4;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL11>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2, i3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2, i3, i4); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL12

template <class Signature_p>
class CallableL12
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL12>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL12(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::LB>::ConstRef_Add_t iB,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LC P0;
	typedef typename SigTraits::LD P1;
	typedef typename SigTraits::LE P2;
	typedef typename SigTraits::LF P3;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL12>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1, i2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1, i2, i3); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::LB>::ConstRef_Remove_t fB;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL13

template <class Signature_p>
class CallableL13
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL13>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL13(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::LB>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::LC>::ConstRef_Add_t iC,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LD P0;
	typedef typename SigTraits::LE P1;
	typedef typename SigTraits::LF P2;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL13>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1, i2); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::LB>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::LC>::ConstRef_Remove_t fC;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL14

template <class Signature_p>
class CallableL14
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL14>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL14(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::LB>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::LC>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::LD>::ConstRef_Add_t iD,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LE P0;
	typedef typename SigTraits::LF P1;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL14>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0, i1); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::LB>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::LC>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::LD>::ConstRef_Remove_t fD;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL15

template <class Signature_p>
class CallableL15
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL15>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL15(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::LB>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::LC>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::LD>::ConstRef_Add_t iD,
		typename ParamTraits<typename SigTraits::LE>::ConstRef_Add_t iE,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::LF P0;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL15>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, i0); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::LB>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::LC>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::LD>::ConstRef_Remove_t fD;
	typename ParamTraits<typename SigTraits::LE>::ConstRef_Remove_t fE;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableL16

template <class Signature_p>
class CallableL16
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureL16>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableL16(
		typename ParamTraits<typename SigTraits::L0>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::L1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::L2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::L3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::L4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::L5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::L6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::L7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::L8>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::L9>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::LA>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::LB>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::LC>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::LD>::ConstRef_Add_t iD,
		typename ParamTraits<typename SigTraits::LE>::ConstRef_Add_t iE,
		typename ParamTraits<typename SigTraits::LF>::ConstRef_Add_t iF,
		const ZRef<Callable_t>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE), fF(iF)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureL16>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

	typename ParamTraits<typename SigTraits::L0>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::L1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::L2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::L3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::L4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::L5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::L6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::L7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::L8>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::L9>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::LA>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::LB>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::LC>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::LD>::ConstRef_Remove_t fD;
	typename ParamTraits<typename SigTraits::LE>::ConstRef_Remove_t fE;
	typename ParamTraits<typename SigTraits::LF>::ConstRef_Remove_t fF;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR01

template <class Signature_p>
class CallableR01
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR01>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR01(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i0)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;
	typedef typename SigTraits::LA PA;
	typedef typename SigTraits::LB PB;
	typedef typename SigTraits::LC PC;
	typedef typename SigTraits::LD PD;
	typedef typename SigTraits::LE PE;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR01>::Call;

private:
	R Call()
		{ return fCallable->Call(f0); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, f0); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, f0); }

	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f0;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR02

template <class Signature_p>
class CallableR02
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR02>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR02(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i1)
	:	f0(i0), f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;
	typedef typename SigTraits::LA PA;
	typedef typename SigTraits::LB PB;
	typedef typename SigTraits::LC PC;
	typedef typename SigTraits::LD PD;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR02>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0, f1); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, f0, f1); }

	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f1;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR03

template <class Signature_p>
class CallableR03
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR03>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR03(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i2)
	:	f0(i0), f1(i1), f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;
	typedef typename SigTraits::LA PA;
	typedef typename SigTraits::LB PB;
	typedef typename SigTraits::LC PC;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR03>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1, f2); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0, f1, f2); }

	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f2;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR04

template <class Signature_p>
class CallableR04
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR04>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR04(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i3)
	:	f0(i0), f1(i1), f2(i2), f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;
	typedef typename SigTraits::LA PA;
	typedef typename SigTraits::LB PB;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR04>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2, f3); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1, f2, f3); }

	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f3;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR05

template <class Signature_p>
class CallableR05
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR05>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR05(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i4)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;
	typedef typename SigTraits::LA PA;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR05>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3, f4); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2, f3, f4); }

	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f4;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR06

template <class Signature_p>
class CallableR06
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR06>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR06(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i5)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;
	typedef typename SigTraits::L9 P9;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR06>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4, f5); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3, f4, f5); }

	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f5;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR07

template <class Signature_p>
class CallableR07
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR07>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR07(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i6)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;
	typedef typename SigTraits::L8 P8;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR07>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5, f6); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4, f5, f6); }

	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f6;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR08

template <class Signature_p>
class CallableR08
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR08>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR08(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i7)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;
	typedef typename SigTraits::L7 P7;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR08>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6, f7); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5, f6, f7); }

	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f7;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR09

template <class Signature_p>
class CallableR09
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR09>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR09(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i8)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;
	typedef typename SigTraits::L6 P6;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR09>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f8;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR10

template <class Signature_p>
class CallableR10
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR10>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR10(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t i9)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;
	typedef typename SigTraits::L5 P5;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR10>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t f9;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR11

template <class Signature_p>
class CallableR11
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR11>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR11(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iA)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;
	typedef typename SigTraits::L4 P4;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR11>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fA;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR12

template <class Signature_p>
class CallableR12
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR12>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR12(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RB>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iB)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;
	typedef typename SigTraits::L3 P3;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR12>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	typename ParamTraits<typename SigTraits::RB>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fB;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR13

template <class Signature_p>
class CallableR13
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR13>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR13(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RC>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::RB>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iC)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;
	typedef typename SigTraits::L2 P2;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR13>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	typename ParamTraits<typename SigTraits::RC>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::RB>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fC;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR14

template <class Signature_p>
class CallableR14
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR14>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR14(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RD>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::RC>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::RB>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iD)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;
	typedef typename SigTraits::L1 P1;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR14>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	typename ParamTraits<typename SigTraits::RD>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::RC>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::RB>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fD;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR15

template <class Signature_p>
class CallableR15
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR15>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR15(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RE>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::RD>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::RC>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::RB>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t iD,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iE)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;
	typedef typename SigTraits::L0 P0;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR15>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	typename ParamTraits<typename SigTraits::RE>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::RD>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::RC>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::RB>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t fD;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fE;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * CallableR16

template <class Signature_p>
class CallableR16
:	public ZCallable<typename SigTraits_T<Signature_p>::SignatureR16>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable_t;
	typedef SigTraits_T<Signature> SigTraits;

	CallableR16(
		const ZRef<Callable_t>& iCallable,
		typename ParamTraits<typename SigTraits::RF>::ConstRef_Add_t i0,
		typename ParamTraits<typename SigTraits::RE>::ConstRef_Add_t i1,
		typename ParamTraits<typename SigTraits::RD>::ConstRef_Add_t i2,
		typename ParamTraits<typename SigTraits::RC>::ConstRef_Add_t i3,
		typename ParamTraits<typename SigTraits::RB>::ConstRef_Add_t i4,
		typename ParamTraits<typename SigTraits::RA>::ConstRef_Add_t i5,
		typename ParamTraits<typename SigTraits::R9>::ConstRef_Add_t i6,
		typename ParamTraits<typename SigTraits::R8>::ConstRef_Add_t i7,
		typename ParamTraits<typename SigTraits::R7>::ConstRef_Add_t i8,
		typename ParamTraits<typename SigTraits::R6>::ConstRef_Add_t i9,
		typename ParamTraits<typename SigTraits::R5>::ConstRef_Add_t iA,
		typename ParamTraits<typename SigTraits::R4>::ConstRef_Add_t iB,
		typename ParamTraits<typename SigTraits::R3>::ConstRef_Add_t iC,
		typename ParamTraits<typename SigTraits::R2>::ConstRef_Add_t iD,
		typename ParamTraits<typename SigTraits::R1>::ConstRef_Add_t iE,
		typename ParamTraits<typename SigTraits::R0>::ConstRef_Add_t iF)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE), fF(iF)
	,	fCallable(iCallable)
		{}

	typedef typename SigTraits::R R;

	using ZCallable<typename SigTraits_T<Signature_p>::SignatureR16>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

	typename ParamTraits<typename SigTraits::RF>::ConstRef_Remove_t f0;
	typename ParamTraits<typename SigTraits::RE>::ConstRef_Remove_t f1;
	typename ParamTraits<typename SigTraits::RD>::ConstRef_Remove_t f2;
	typename ParamTraits<typename SigTraits::RC>::ConstRef_Remove_t f3;
	typename ParamTraits<typename SigTraits::RB>::ConstRef_Remove_t f4;
	typename ParamTraits<typename SigTraits::RA>::ConstRef_Remove_t f5;
	typename ParamTraits<typename SigTraits::R9>::ConstRef_Remove_t f6;
	typename ParamTraits<typename SigTraits::R8>::ConstRef_Remove_t f7;
	typename ParamTraits<typename SigTraits::R7>::ConstRef_Remove_t f8;
	typename ParamTraits<typename SigTraits::R6>::ConstRef_Remove_t f9;
	typename ParamTraits<typename SigTraits::R5>::ConstRef_Remove_t fA;
	typename ParamTraits<typename SigTraits::R4>::ConstRef_Remove_t fB;
	typename ParamTraits<typename SigTraits::R3>::ConstRef_Remove_t fC;
	typename ParamTraits<typename SigTraits::R2>::ConstRef_Remove_t fD;
	typename ParamTraits<typename SigTraits::R1>::ConstRef_Remove_t fE;
	typename ParamTraits<typename SigTraits::R0>::ConstRef_Remove_t fF;
	ZRef<Callable_t> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * BindL

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL01> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL01<typename Callable::Signature>
		(i0,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL02> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL02<typename Callable::Signature>
		(i0, i1,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL03> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL03<typename Callable::Signature>
		(i0, i1, i2,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL04> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL04<typename Callable::Signature>
		(i0, i1, i2, i3,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL05> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL05<typename Callable::Signature>
		(i0, i1, i2, i3, i4,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL06> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL06<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL07> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL07<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL08> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL08<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL09> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL09<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL10> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL10<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL11> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL11<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL12> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LB>::ConstRef_Add_t iB,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL12<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL13> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LB>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LC>::ConstRef_Add_t iC,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL13<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL14> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LB>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LC>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LD>::ConstRef_Add_t iD,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL14<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL15> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LB>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LC>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LD>::ConstRef_Add_t iD,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LE>::ConstRef_Add_t iE,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL15<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureL16> >
BindL(
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L0>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L8>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::L9>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LA>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LB>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LC>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LD>::ConstRef_Add_t iD,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LE>::ConstRef_Add_t iE,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::LF>::ConstRef_Add_t iF,
	const ZRef<Callable>& iCallable)
	{
	return new CallableL16<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF,
		iCallable);
	}

// =================================================================================================
#pragma mark -
#pragma mark * BindR

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR01> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i0)
	{
	return new CallableR01<typename Callable::Signature>
		(iCallable,
		i0);
	}
// We need to reverse the order of these params
template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR02> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i1)
	{
	return new CallableR02<typename Callable::Signature>
		(iCallable,
		i0, i1);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR03> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i2)
	{
	return new CallableR03<typename Callable::Signature>
		(iCallable,
		i0, i1, i2);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR04> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i3)
	{
	return new CallableR04<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR05> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i4)
	{
	return new CallableR05<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR06> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i5)
	{
	return new CallableR06<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR07> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i6)
	{
	return new CallableR07<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR08> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i7)
	{
	return new CallableR08<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR09> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i8)
	{
	return new CallableR09<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR10> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t i9)
	{
	return new CallableR10<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR11> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iA)
	{
	return new CallableR11<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR12> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RB>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iB)
	{
	return new CallableR12<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR13> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RC>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RB>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iC)
	{
	return new CallableR13<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR14> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RD>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RC>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RB>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iD)
	{
	return new CallableR14<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR15> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RE>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RD>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RC>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RB>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t iD,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iE)
	{
	return new CallableR15<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
	}

template <class Callable>
ZRef<ZCallable<typename SigTraits_T<typename Callable::Signature>::SignatureR16> >
BindR(
	const ZRef<Callable>& iCallable,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RF>::ConstRef_Add_t i0,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RE>::ConstRef_Add_t i1,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RD>::ConstRef_Add_t i2,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RC>::ConstRef_Add_t i3,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RB>::ConstRef_Add_t i4,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::RA>::ConstRef_Add_t i5,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R9>::ConstRef_Add_t i6,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R8>::ConstRef_Add_t i7,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R7>::ConstRef_Add_t i8,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R6>::ConstRef_Add_t i9,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R5>::ConstRef_Add_t iA,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R4>::ConstRef_Add_t iB,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R3>::ConstRef_Add_t iC,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R2>::ConstRef_Add_t iD,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R1>::ConstRef_Add_t iE,
	typename ParamTraits<typename SigTraits_T<typename Callable::Signature>::R0>::ConstRef_Add_t iF)
	{
	return new CallableR16<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
	}

} // namespace ZCallable_Bind

using ZCallable_Bind::BindL;
using ZCallable_Bind::BindR;

} // namespace ZooLib

#endif // __ZCallable_Bind__
