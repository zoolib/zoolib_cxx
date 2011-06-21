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

using namespace ZCallableUtil;

// =================================================================================================
#pragma mark -
#pragma mark * ST (Signature Traits)

struct Empty_t {};

template <class Signature>
struct ST_T;

template <class R_p, class Signature>
struct STReturn;

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 0 params)

template <class R_p>
struct ST_T<R_p(void)>
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
	};

template <class R, class Ignore>
struct STReturn<R,Ignore(void)>
	{
	typedef R (Signature)();
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 1 param)

template <class R_p,
	class P0>
struct ST_T<R_p(P0)>
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

	typedef R (SL01)();

	typedef R (SR01)();
	};

template <class R, class Ignore,
	class P0>
struct STReturn<R,Ignore(P0)>
	{
	typedef R (Signature)(P0);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 2 params)

template <class R_p,
	class P0, class P1>
struct ST_T<R_p(P0,P1)>
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

	typedef R (SL01)(P1);
	typedef R (SL02)();

	typedef R (SR01)(P0);
	typedef R (SR02)();
	};

template <class R, class Ignore,
	class P0, class P1>
struct STReturn<R,Ignore(P0,P1)>
	{
	typedef R (Signature)(P0,P1);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 3 params)

template <class R_p,
	class P0, class P1, class P2>
struct ST_T<R_p(P0,P1,P2)>
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

	typedef R (SL01)(P1,P2);
	typedef R (SL02)(P2);
	typedef R (SL03)();

	typedef R (SR01)(P0,P1);
	typedef R (SR02)(P0);
	typedef R (SR03)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2>
struct STReturn<R,Ignore(P0,P1,P2)>
	{
	typedef R (Signature)(P0,P1,P2);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 4 params)

template <class R_p,
	class P0, class P1, class P2, class P3>
struct ST_T<R_p(P0,P1,P2,P3)>
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

	typedef R (SL01)(P1,P2,P3);
	typedef R (SL02)(P2,P3);
	typedef R (SL03)(P3);
	typedef R (SL04)();

	typedef R (SR01)(P0,P1,P2);
	typedef R (SR02)(P0,P1);
	typedef R (SR03)(P0);
	typedef R (SR04)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3>
struct STReturn<R,Ignore(P0,P1,P2,P3)>
	{
	typedef R (Signature)(P0,P1,P2,P3);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 5 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4>
struct ST_T<R_p(P0,P1,P2,P3,P4)>
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

	typedef R (SL01)(P1,P2,P3,P4);
	typedef R (SL02)(P2,P3,P4);
	typedef R (SL03)(P3,P4);
	typedef R (SL04)(P4);
	typedef R (SL05)();

	typedef R (SR01)(P0,P1,P2,P3);
	typedef R (SR02)(P0,P1,P2);
	typedef R (SR03)(P0,P1);
	typedef R (SR04)(P0);
	typedef R (SR05)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 6 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5);
	typedef R (SL02)(P2,P3,P4,P5);
	typedef R (SL03)(P3,P4,P5);
	typedef R (SL04)(P4,P5);
	typedef R (SL05)(P5);
	typedef R (SL06)();

	typedef R (SR01)(P0,P1,P2,P3,P4);
	typedef R (SR02)(P0,P1,P2,P3);
	typedef R (SR03)(P0,P1,P2);
	typedef R (SR04)(P0,P1);
	typedef R (SR05)(P0);
	typedef R (SR06)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 7 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6);
	typedef R (SL02)(P2,P3,P4,P5,P6);
	typedef R (SL03)(P3,P4,P5,P6);
	typedef R (SL04)(P4,P5,P6);
	typedef R (SL05)(P5,P6);
	typedef R (SL06)(P6);
	typedef R (SL07)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5);
	typedef R (SR02)(P0,P1,P2,P3,P4);
	typedef R (SR03)(P0,P1,P2,P3);
	typedef R (SR04)(P0,P1,P2);
	typedef R (SR05)(P0,P1);
	typedef R (SR06)(P0);
	typedef R (SR07)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 8 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7);
	typedef R (SL03)(P3,P4,P5,P6,P7);
	typedef R (SL04)(P4,P5,P6,P7);
	typedef R (SL05)(P5,P6,P7);
	typedef R (SL06)(P6,P7);
	typedef R (SL07)(P7);
	typedef R (SL08)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5);
	typedef R (SR03)(P0,P1,P2,P3,P4);
	typedef R (SR04)(P0,P1,P2,P3);
	typedef R (SR05)(P0,P1,P2);
	typedef R (SR06)(P0,P1);
	typedef R (SR07)(P0);
	typedef R (SR08)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 9 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8);
	typedef R (SL04)(P4,P5,P6,P7,P8);
	typedef R (SL05)(P5,P6,P7,P8);
	typedef R (SL06)(P6,P7,P8);
	typedef R (SL07)(P7,P8);
	typedef R (SL08)(P8);
	typedef R (SL09)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5);
	typedef R (SR04)(P0,P1,P2,P3,P4);
	typedef R (SR05)(P0,P1,P2,P3);
	typedef R (SR06)(P0,P1,P2);
	typedef R (SR07)(P0,P1);
	typedef R (SR08)(P0);
	typedef R (SR09)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 10 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9);
	typedef R (SL05)(P5,P6,P7,P8,P9);
	typedef R (SL06)(P6,P7,P8,P9);
	typedef R (SL07)(P7,P8,P9);
	typedef R (SL08)(P8,P9);
	typedef R (SL09)(P9);
	typedef R (SL10)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5);
	typedef R (SR05)(P0,P1,P2,P3,P4);
	typedef R (SR06)(P0,P1,P2,P3);
	typedef R (SR07)(P0,P1,P2);
	typedef R (SR08)(P0,P1);
	typedef R (SR09)(P0);
	typedef R (SR10)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 11 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA);
	typedef R (SL06)(P6,P7,P8,P9,PA);
	typedef R (SL07)(P7,P8,P9,PA);
	typedef R (SL08)(P8,P9,PA);
	typedef R (SL09)(P9,PA);
	typedef R (SL10)(PA);
	typedef R (SL11)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5);
	typedef R (SR06)(P0,P1,P2,P3,P4);
	typedef R (SR07)(P0,P1,P2,P3);
	typedef R (SR08)(P0,P1,P2);
	typedef R (SR09)(P0,P1);
	typedef R (SR10)(P0);
	typedef R (SR11)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 12 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA,PB);
	typedef R (SL06)(P6,P7,P8,P9,PA,PB);
	typedef R (SL07)(P7,P8,P9,PA,PB);
	typedef R (SL08)(P8,P9,PA,PB);
	typedef R (SL09)(P9,PA,PB);
	typedef R (SL10)(PA,PB);
	typedef R (SL11)(PB);
	typedef R (SL12)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR06)(P0,P1,P2,P3,P4,P5);
	typedef R (SR07)(P0,P1,P2,P3,P4);
	typedef R (SR08)(P0,P1,P2,P3);
	typedef R (SR09)(P0,P1,P2);
	typedef R (SR10)(P0,P1);
	typedef R (SR11)(P0);
	typedef R (SR12)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 13 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL06)(P6,P7,P8,P9,PA,PB,PC);
	typedef R (SL07)(P7,P8,P9,PA,PB,PC);
	typedef R (SL08)(P8,P9,PA,PB,PC);
	typedef R (SL09)(P9,PA,PB,PC);
	typedef R (SL10)(PA,PB,PC);
	typedef R (SL11)(PB,PC);
	typedef R (SL12)(PC);
	typedef R (SL13)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR06)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR07)(P0,P1,P2,P3,P4,P5);
	typedef R (SR08)(P0,P1,P2,P3,P4);
	typedef R (SR09)(P0,P1,P2,P3);
	typedef R (SR10)(P0,P1,P2);
	typedef R (SR11)(P0,P1);
	typedef R (SR12)(P0);
	typedef R (SR13)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 14 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL06)(P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL07)(P7,P8,P9,PA,PB,PC,PD);
	typedef R (SL08)(P8,P9,PA,PB,PC,PD);
	typedef R (SL09)(P9,PA,PB,PC,PD);
	typedef R (SL10)(PA,PB,PC,PD);
	typedef R (SL11)(PB,PC,PD);
	typedef R (SL12)(PC,PD);
	typedef R (SL13)(PD);
	typedef R (SL14)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR06)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR07)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR08)(P0,P1,P2,P3,P4,P5);
	typedef R (SR09)(P0,P1,P2,P3,P4);
	typedef R (SR10)(P0,P1,P2,P3);
	typedef R (SR11)(P0,P1,P2);
	typedef R (SR12)(P0,P1);
	typedef R (SR13)(P0);
	typedef R (SR14)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 15 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL06)(P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL07)(P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL08)(P8,P9,PA,PB,PC,PD,PE);
	typedef R (SL09)(P9,PA,PB,PC,PD,PE);
	typedef R (SL10)(PA,PB,PC,PD,PE);
	typedef R (SL11)(PB,PC,PD,PE);
	typedef R (SL12)(PC,PD,PE);
	typedef R (SL13)(PD,PE);
	typedef R (SL14)(PE);
	typedef R (SL15)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR06)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR07)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR08)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR09)(P0,P1,P2,P3,P4,P5);
	typedef R (SR10)(P0,P1,P2,P3,P4);
	typedef R (SR11)(P0,P1,P2,P3);
	typedef R (SR12)(P0,P1,P2);
	typedef R (SR13)(P0,P1);
	typedef R (SR14)(P0);
	typedef R (SR15)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Signature Traits (specialized for 16 params)

template <class R_p,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
struct ST_T<R_p(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
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

	typedef R (SL01)(P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL02)(P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL03)(P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL04)(P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL05)(P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL06)(P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL07)(P7,P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL08)(P8,P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL09)(P9,PA,PB,PC,PD,PE,PF);
	typedef R (SL10)(PA,PB,PC,PD,PE,PF);
	typedef R (SL11)(PB,PC,PD,PE,PF);
	typedef R (SL12)(PC,PD,PE,PF);
	typedef R (SL13)(PD,PE,PF);
	typedef R (SL14)(PE,PF);
	typedef R (SL15)(PF);
	typedef R (SL16)();

	typedef R (SR01)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE);
	typedef R (SR02)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD);
	typedef R (SR03)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC);
	typedef R (SR04)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB);
	typedef R (SR05)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA);
	typedef R (SR06)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9);
	typedef R (SR07)(P0,P1,P2,P3,P4,P5,P6,P7,P8);
	typedef R (SR08)(P0,P1,P2,P3,P4,P5,P6,P7);
	typedef R (SR09)(P0,P1,P2,P3,P4,P5,P6);
	typedef R (SR10)(P0,P1,P2,P3,P4,P5);
	typedef R (SR11)(P0,P1,P2,P3,P4);
	typedef R (SR12)(P0,P1,P2,P3);
	typedef R (SR13)(P0,P1,P2);
	typedef R (SR14)(P0,P1);
	typedef R (SR15)(P0);
	typedef R (SR16)();
	};

template <class R, class Ignore,
	class P0, class P1, class P2, class P3,
	class P4, class P5, class P6, class P7,
	class P8, class P9, class PA, class PB,
	class PC, class PD, class PE, class PF>
struct STReturn<R,Ignore(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF)>
	{
	typedef R (Signature)(P0,P1,P2,P3,P4,P5,P6,P7,P8,P9,PA,PB,PC,PD,PE,PF);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L01

template <class Signature_p>
class Callable_L01
:	public ZCallable<typename ST_T<Signature_p>::SL01>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L01(
		typename VT<typename ST::L0>::P i0,
		const ZRef<Callable>& iCallable)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L1 P0;
	typedef typename ST::L2 P1;
	typedef typename ST::L3 P2;
	typedef typename ST::L4 P3;
	typedef typename ST::L5 P4;
	typedef typename ST::L6 P5;
	typedef typename ST::L7 P6;
	typedef typename ST::L8 P7;
	typedef typename ST::L9 P8;
	typedef typename ST::LA P9;
	typedef typename ST::LB PA;
	typedef typename ST::LC PB;
	typedef typename ST::LD PC;
	typedef typename ST::LE PD;
	typedef typename ST::LF PE;

	using ZCallable<typename ST_T<Signature_p>::SL01>::Call;

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

	typename VT<typename ST::L0>::F f0;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L02

template <class Signature_p>
class Callable_L02
:	public ZCallable<typename ST_T<Signature_p>::SL02>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L02(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L2 P0;
	typedef typename ST::L3 P1;
	typedef typename ST::L4 P2;
	typedef typename ST::L5 P3;
	typedef typename ST::L6 P4;
	typedef typename ST::L7 P5;
	typedef typename ST::L8 P6;
	typedef typename ST::L9 P7;
	typedef typename ST::LA P8;
	typedef typename ST::LB P9;
	typedef typename ST::LC PA;
	typedef typename ST::LD PB;
	typedef typename ST::LE PC;
	typedef typename ST::LF PD;

	using ZCallable<typename ST_T<Signature_p>::SL02>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L03

template <class Signature_p>
class Callable_L03
:	public ZCallable<typename ST_T<Signature_p>::SL03>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L03(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L3 P0;
	typedef typename ST::L4 P1;
	typedef typename ST::L5 P2;
	typedef typename ST::L6 P3;
	typedef typename ST::L5 P4;
	typedef typename ST::L8 P5;
	typedef typename ST::L9 P6;
	typedef typename ST::LA P7;
	typedef typename ST::LB P8;
	typedef typename ST::LC P9;
	typedef typename ST::LD PA;
	typedef typename ST::LE PB;
	typedef typename ST::LF PC;

	using ZCallable<typename ST_T<Signature_p>::SL03>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L1>::F f2;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L04

template <class Signature_p>
class Callable_L04
:	public ZCallable<typename ST_T<Signature_p>::SL04>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L04(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L4 P0;
	typedef typename ST::L5 P1;
	typedef typename ST::L6 P2;
	typedef typename ST::L7 P3;
	typedef typename ST::L8 P4;
	typedef typename ST::L9 P5;
	typedef typename ST::LA P6;
	typedef typename ST::LB P7;
	typedef typename ST::LC P8;
	typedef typename ST::LD P9;
	typedef typename ST::LE PA;
	typedef typename ST::LF PB;

	using ZCallable<typename ST_T<Signature_p>::SL04>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L05

template <class Signature_p>
class Callable_L05
:	public ZCallable<typename ST_T<Signature_p>::SL05>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L05(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L5 P0;
	typedef typename ST::L6 P1;
	typedef typename ST::L7 P2;
	typedef typename ST::L8 P3;
	typedef typename ST::L9 P4;
	typedef typename ST::LA P5;
	typedef typename ST::LB P6;
	typedef typename ST::LC P7;
	typedef typename ST::LD P8;
	typedef typename ST::LE P9;
	typedef typename ST::LF PA;

	using ZCallable<typename ST_T<Signature_p>::SL05>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L06

template <class Signature_p>
class Callable_L06
:	public ZCallable<typename ST_T<Signature_p>::SL06>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L06(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L6 P0;
	typedef typename ST::L7 P1;
	typedef typename ST::L8 P2;
	typedef typename ST::L9 P3;
	typedef typename ST::LA P4;
	typedef typename ST::LB P5;
	typedef typename ST::LC P6;
	typedef typename ST::LD P7;
	typedef typename ST::LE P8;
	typedef typename ST::LF P9;

	using ZCallable<typename ST_T<Signature_p>::SL06>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L07

template <class Signature_p>
class Callable_L07
:	public ZCallable<typename ST_T<Signature_p>::SL07>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L07(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L7 P0;
	typedef typename ST::L8 P1;
	typedef typename ST::L9 P2;
	typedef typename ST::LA P3;
	typedef typename ST::LB P4;
	typedef typename ST::LC P5;
	typedef typename ST::LD P6;
	typedef typename ST::LE P7;
	typedef typename ST::LF P8;

	using ZCallable<typename ST_T<Signature_p>::SL07>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L08

template <class Signature_p>
class Callable_L08
:	public ZCallable<typename ST_T<Signature_p>::SL08>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L08(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L8 P0;
	typedef typename ST::L9 P1;
	typedef typename ST::LA P2;
	typedef typename ST::LB P3;
	typedef typename ST::LC P4;
	typedef typename ST::LD P5;
	typedef typename ST::LE P6;
	typedef typename ST::LF P7;

	using ZCallable<typename ST_T<Signature_p>::SL08>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L09

template <class Signature_p>
class Callable_L09
:	public ZCallable<typename ST_T<Signature_p>::SL09>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L09(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L9 P0;
	typedef typename ST::LA P1;
	typedef typename ST::LB P2;
	typedef typename ST::LC P3;
	typedef typename ST::LD P4;
	typedef typename ST::LE P5;
	typedef typename ST::LF P6;

	using ZCallable<typename ST_T<Signature_p>::SL09>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L10

template <class Signature_p>
class Callable_L10
:	public ZCallable<typename ST_T<Signature_p>::SL10>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L10(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LA P0;
	typedef typename ST::LB P1;
	typedef typename ST::LC P2;
	typedef typename ST::LD P3;
	typedef typename ST::LE P4;
	typedef typename ST::LF P5;

	using ZCallable<typename ST_T<Signature_p>::SL10>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L11

template <class Signature_p>
class Callable_L11
:	public ZCallable<typename ST_T<Signature_p>::SL11>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L11(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LB P0;
	typedef typename ST::LC P1;
	typedef typename ST::LD P2;
	typedef typename ST::LE P3;
	typedef typename ST::LF P4;

	using ZCallable<typename ST_T<Signature_p>::SL11>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L12

template <class Signature_p>
class Callable_L12
:	public ZCallable<typename ST_T<Signature_p>::SL12>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L12(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		typename VT<typename ST::LB>::P iB,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LC P0;
	typedef typename ST::LD P1;
	typedef typename ST::LE P2;
	typedef typename ST::LF P3;

	using ZCallable<typename ST_T<Signature_p>::SL12>::Call;

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

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	typename VT<typename ST::LB>::F fB;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L13

template <class Signature_p>
class Callable_L13
:	public ZCallable<typename ST_T<Signature_p>::SL13>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L13(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		typename VT<typename ST::LB>::P iB,
		typename VT<typename ST::LC>::P iC,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LD P0;
	typedef typename ST::LE P1;
	typedef typename ST::LF P2;

	using ZCallable<typename ST_T<Signature_p>::SL13>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1, i2); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	typename VT<typename ST::LB>::F fB;
	typename VT<typename ST::LC>::F fC;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L14

template <class Signature_p>
class Callable_L14
:	public ZCallable<typename ST_T<Signature_p>::SL14>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L14(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		typename VT<typename ST::LB>::P iB,
		typename VT<typename ST::LC>::P iC,
		typename VT<typename ST::LD>::P iD,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LE P0;
	typedef typename ST::LF P1;

	using ZCallable<typename ST_T<Signature_p>::SL14>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0, i1); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	typename VT<typename ST::LB>::F fB;
	typename VT<typename ST::LC>::F fC;
	typename VT<typename ST::LD>::F fD;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L15

template <class Signature_p>
class Callable_L15
:	public ZCallable<typename ST_T<Signature_p>::SL15>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L15(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		typename VT<typename ST::LB>::P iB,
		typename VT<typename ST::LC>::P iC,
		typename VT<typename ST::LD>::P iD,
		typename VT<typename ST::LE>::P iE,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::LF P0;

	using ZCallable<typename ST_T<Signature_p>::SL15>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	R Call(P0 i0)
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, i0); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	typename VT<typename ST::LB>::F fB;
	typename VT<typename ST::LC>::F fC;
	typename VT<typename ST::LD>::F fD;
	typename VT<typename ST::LE>::F fE;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_L16

template <class Signature_p>
class Callable_L16
:	public ZCallable<typename ST_T<Signature_p>::SL16>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_L16(
		typename VT<typename ST::L0>::P i0,
		typename VT<typename ST::L1>::P i1,
		typename VT<typename ST::L2>::P i2,
		typename VT<typename ST::L3>::P i3,
		typename VT<typename ST::L4>::P i4,
		typename VT<typename ST::L5>::P i5,
		typename VT<typename ST::L6>::P i6,
		typename VT<typename ST::L7>::P i7,
		typename VT<typename ST::L8>::P i8,
		typename VT<typename ST::L9>::P i9,
		typename VT<typename ST::LA>::P iA,
		typename VT<typename ST::LB>::P iB,
		typename VT<typename ST::LC>::P iC,
		typename VT<typename ST::LD>::P iD,
		typename VT<typename ST::LE>::P iE,
		typename VT<typename ST::LF>::P iF,
		const ZRef<Callable>& iCallable)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE), fF(iF)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;

	using ZCallable<typename ST_T<Signature_p>::SL16>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	typename VT<typename ST::L6>::F f6;
	typename VT<typename ST::L7>::F f7;
	typename VT<typename ST::L8>::F f8;
	typename VT<typename ST::L9>::F f9;
	typename VT<typename ST::LA>::F fA;
	typename VT<typename ST::LB>::F fB;
	typename VT<typename ST::LC>::F fC;
	typename VT<typename ST::LD>::F fD;
	typename VT<typename ST::LE>::F fE;
	typename VT<typename ST::LF>::F fF;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * BindL

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL01> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L01<typename Callable::Signature>
		(i0,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL02> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L02<typename Callable::Signature>
		(i0, i1,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL03> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L03<typename Callable::Signature>
		(i0, i1, i2,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL04> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L04<typename Callable::Signature>
		(i0, i1, i2, i3,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL05> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L05<typename Callable::Signature>
		(i0, i1, i2, i3, i4,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL06> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L06<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL07> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L07<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL08> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L08<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL09> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L09<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL10> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L10<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL11> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L11<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL12> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::LB>::P iB,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L12<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL13> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::LB>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::LC>::P iC,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L13<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL14> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::LB>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::LC>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::LD>::P iD,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L14<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL15> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::LB>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::LC>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::LD>::P iD,
	typename VT<typename ST_T<typename Callable::Signature>::LE>::P iE,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L15<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL16> >
BindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::L7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::L8>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::L9>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::LA>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::LB>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::LC>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::LD>::P iD,
	typename VT<typename ST_T<typename Callable::Signature>::LE>::P iE,
	typename VT<typename ST_T<typename Callable::Signature>::LF>::P iF,
	const ZRef<Callable>& iCallable)
	{
	return new Callable_L16<typename Callable::Signature>
		(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF,
		iCallable);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R01

template <class Signature_p>
class Callable_R01
:	public ZCallable<typename ST_T<Signature_p>::SR01>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R01(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R0>::P i0)
	:	f0(i0)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;
	typedef typename ST::LD PD;
	typedef typename ST::LE PE;

	using ZCallable<typename ST_T<Signature_p>::SR01>::Call;

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

	typename VT<typename ST::R0>::F f0;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R02

template <class Signature_p>
class Callable_R02
:	public ZCallable<typename ST_T<Signature_p>::SR02>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R02(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R1>::P i0,
		typename VT<typename ST::R0>::P i1)
	:	f0(i0), f1(i1)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;
	typedef typename ST::LD PD;

	using ZCallable<typename ST_T<Signature_p>::SR02>::Call;

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

	typename VT<typename ST::R1>::F f0;
	typename VT<typename ST::R0>::F f1;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R03

template <class Signature_p>
class Callable_R03
:	public ZCallable<typename ST_T<Signature_p>::SR03>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R03(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R2>::P i0,
		typename VT<typename ST::R1>::P i1,
		typename VT<typename ST::R0>::P i2)
	:	f0(i0), f1(i1), f2(i2)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;

	using ZCallable<typename ST_T<Signature_p>::SR03>::Call;

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

	typename VT<typename ST::R2>::F f0;
	typename VT<typename ST::R1>::F f1;
	typename VT<typename ST::R0>::F f2;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R04

template <class Signature_p>
class Callable_R04
:	public ZCallable<typename ST_T<Signature_p>::SR04>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R04(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R3>::P i0,
		typename VT<typename ST::R2>::P i1,
		typename VT<typename ST::R1>::P i2,
		typename VT<typename ST::R0>::P i3)
	:	f0(i0), f1(i1), f2(i2), f3(i3)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;

	using ZCallable<typename ST_T<Signature_p>::SR04>::Call;

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

	typename VT<typename ST::R3>::F f0;
	typename VT<typename ST::R2>::F f1;
	typename VT<typename ST::R1>::F f2;
	typename VT<typename ST::R0>::F f3;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R05

template <class Signature_p>
class Callable_R05
:	public ZCallable<typename ST_T<Signature_p>::SR05>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R05(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R4>::P i0,
		typename VT<typename ST::R3>::P i1,
		typename VT<typename ST::R2>::P i2,
		typename VT<typename ST::R1>::P i3,
		typename VT<typename ST::R0>::P i4)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;

	using ZCallable<typename ST_T<Signature_p>::SR05>::Call;

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

	typename VT<typename ST::R4>::F f0;
	typename VT<typename ST::R3>::F f1;
	typename VT<typename ST::R2>::F f2;
	typename VT<typename ST::R1>::F f3;
	typename VT<typename ST::R0>::F f4;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R06

template <class Signature_p>
class Callable_R06
:	public ZCallable<typename ST_T<Signature_p>::SR06>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R06(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R5>::P i0,
		typename VT<typename ST::R4>::P i1,
		typename VT<typename ST::R3>::P i2,
		typename VT<typename ST::R2>::P i3,
		typename VT<typename ST::R1>::P i4,
		typename VT<typename ST::R0>::P i5)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;

	using ZCallable<typename ST_T<Signature_p>::SR06>::Call;

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

	typename VT<typename ST::R5>::F f0;
	typename VT<typename ST::R4>::F f1;
	typename VT<typename ST::R3>::F f2;
	typename VT<typename ST::R2>::F f3;
	typename VT<typename ST::R1>::F f4;
	typename VT<typename ST::R0>::F f5;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R07

template <class Signature_p>
class Callable_R07
:	public ZCallable<typename ST_T<Signature_p>::SR07>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R07(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R6>::P i0,
		typename VT<typename ST::R5>::P i1,
		typename VT<typename ST::R4>::P i2,
		typename VT<typename ST::R3>::P i3,
		typename VT<typename ST::R2>::P i4,
		typename VT<typename ST::R1>::P i5,
		typename VT<typename ST::R0>::P i6)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;

	using ZCallable<typename ST_T<Signature_p>::SR07>::Call;

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

	typename VT<typename ST::R6>::F f0;
	typename VT<typename ST::R5>::F f1;
	typename VT<typename ST::R4>::F f2;
	typename VT<typename ST::R3>::F f3;
	typename VT<typename ST::R2>::F f4;
	typename VT<typename ST::R1>::F f5;
	typename VT<typename ST::R0>::F f6;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R08

template <class Signature_p>
class Callable_R08
:	public ZCallable<typename ST_T<Signature_p>::SR08>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R08(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R7>::P i0,
		typename VT<typename ST::R6>::P i1,
		typename VT<typename ST::R5>::P i2,
		typename VT<typename ST::R4>::P i3,
		typename VT<typename ST::R3>::P i4,
		typename VT<typename ST::R2>::P i5,
		typename VT<typename ST::R1>::P i6,
		typename VT<typename ST::R0>::P i7)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;

	using ZCallable<typename ST_T<Signature_p>::SR08>::Call;

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

	typename VT<typename ST::R7>::F f0;
	typename VT<typename ST::R6>::F f1;
	typename VT<typename ST::R5>::F f2;
	typename VT<typename ST::R4>::F f3;
	typename VT<typename ST::R3>::F f4;
	typename VT<typename ST::R2>::F f5;
	typename VT<typename ST::R1>::F f6;
	typename VT<typename ST::R0>::F f7;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R09

template <class Signature_p>
class Callable_R09
:	public ZCallable<typename ST_T<Signature_p>::SR09>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R09(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R8>::P i0,
		typename VT<typename ST::R7>::P i1,
		typename VT<typename ST::R6>::P i2,
		typename VT<typename ST::R5>::P i3,
		typename VT<typename ST::R4>::P i4,
		typename VT<typename ST::R3>::P i5,
		typename VT<typename ST::R2>::P i6,
		typename VT<typename ST::R1>::P i7,
		typename VT<typename ST::R0>::P i8)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;

	using ZCallable<typename ST_T<Signature_p>::SR09>::Call;

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

	typename VT<typename ST::R8>::F f0;
	typename VT<typename ST::R7>::F f1;
	typename VT<typename ST::R6>::F f2;
	typename VT<typename ST::R5>::F f3;
	typename VT<typename ST::R4>::F f4;
	typename VT<typename ST::R3>::F f5;
	typename VT<typename ST::R2>::F f6;
	typename VT<typename ST::R1>::F f7;
	typename VT<typename ST::R0>::F f8;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R10

template <class Signature_p>
class Callable_R10
:	public ZCallable<typename ST_T<Signature_p>::SR10>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R10(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::R9>::P i0,
		typename VT<typename ST::R8>::P i1,
		typename VT<typename ST::R7>::P i2,
		typename VT<typename ST::R6>::P i3,
		typename VT<typename ST::R5>::P i4,
		typename VT<typename ST::R4>::P i5,
		typename VT<typename ST::R3>::P i6,
		typename VT<typename ST::R2>::P i7,
		typename VT<typename ST::R1>::P i8,
		typename VT<typename ST::R0>::P i9)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;

	using ZCallable<typename ST_T<Signature_p>::SR10>::Call;

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

	typename VT<typename ST::R9>::F f0;
	typename VT<typename ST::R8>::F f1;
	typename VT<typename ST::R7>::F f2;
	typename VT<typename ST::R6>::F f3;
	typename VT<typename ST::R5>::F f4;
	typename VT<typename ST::R4>::F f5;
	typename VT<typename ST::R3>::F f6;
	typename VT<typename ST::R2>::F f7;
	typename VT<typename ST::R1>::F f8;
	typename VT<typename ST::R0>::F f9;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R11

template <class Signature_p>
class Callable_R11
:	public ZCallable<typename ST_T<Signature_p>::SR11>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R11(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RA>::P i0,
		typename VT<typename ST::R9>::P i1,
		typename VT<typename ST::R8>::P i2,
		typename VT<typename ST::R7>::P i3,
		typename VT<typename ST::R6>::P i4,
		typename VT<typename ST::R5>::P i5,
		typename VT<typename ST::R4>::P i6,
		typename VT<typename ST::R3>::P i7,
		typename VT<typename ST::R2>::P i8,
		typename VT<typename ST::R1>::P i9,
		typename VT<typename ST::R0>::P iA)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;

	using ZCallable<typename ST_T<Signature_p>::SR11>::Call;

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

	typename VT<typename ST::RA>::F f0;
	typename VT<typename ST::R9>::F f1;
	typename VT<typename ST::R8>::F f2;
	typename VT<typename ST::R7>::F f3;
	typename VT<typename ST::R6>::F f4;
	typename VT<typename ST::R5>::F f5;
	typename VT<typename ST::R4>::F f6;
	typename VT<typename ST::R3>::F f7;
	typename VT<typename ST::R2>::F f8;
	typename VT<typename ST::R1>::F f9;
	typename VT<typename ST::R0>::F fA;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R12

template <class Signature_p>
class Callable_R12
:	public ZCallable<typename ST_T<Signature_p>::SR12>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R12(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RB>::P i0,
		typename VT<typename ST::RA>::P i1,
		typename VT<typename ST::R9>::P i2,
		typename VT<typename ST::R8>::P i3,
		typename VT<typename ST::R7>::P i4,
		typename VT<typename ST::R6>::P i5,
		typename VT<typename ST::R5>::P i6,
		typename VT<typename ST::R4>::P i7,
		typename VT<typename ST::R3>::P i8,
		typename VT<typename ST::R2>::P i9,
		typename VT<typename ST::R1>::P iA,
		typename VT<typename ST::R0>::P iB)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;

	using ZCallable<typename ST_T<Signature_p>::SR12>::Call;

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

	typename VT<typename ST::RB>::F f0;
	typename VT<typename ST::RA>::F f1;
	typename VT<typename ST::R9>::F f2;
	typename VT<typename ST::R8>::F f3;
	typename VT<typename ST::R7>::F f4;
	typename VT<typename ST::R6>::F f5;
	typename VT<typename ST::R5>::F f6;
	typename VT<typename ST::R4>::F f7;
	typename VT<typename ST::R3>::F f8;
	typename VT<typename ST::R2>::F f9;
	typename VT<typename ST::R1>::F fA;
	typename VT<typename ST::R0>::F fB;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R13

template <class Signature_p>
class Callable_R13
:	public ZCallable<typename ST_T<Signature_p>::SR13>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R13(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RC>::P i0,
		typename VT<typename ST::RB>::P i1,
		typename VT<typename ST::RA>::P i2,
		typename VT<typename ST::R9>::P i3,
		typename VT<typename ST::R8>::P i4,
		typename VT<typename ST::R7>::P i5,
		typename VT<typename ST::R6>::P i6,
		typename VT<typename ST::R5>::P i7,
		typename VT<typename ST::R4>::P i8,
		typename VT<typename ST::R3>::P i9,
		typename VT<typename ST::R2>::P iA,
		typename VT<typename ST::R1>::P iB,
		typename VT<typename ST::R0>::P iC)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;

	using ZCallable<typename ST_T<Signature_p>::SR13>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	typename VT<typename ST::RC>::F f0;
	typename VT<typename ST::RB>::F f1;
	typename VT<typename ST::RA>::F f2;
	typename VT<typename ST::R9>::F f3;
	typename VT<typename ST::R8>::F f4;
	typename VT<typename ST::R7>::F f5;
	typename VT<typename ST::R6>::F f6;
	typename VT<typename ST::R5>::F f7;
	typename VT<typename ST::R4>::F f8;
	typename VT<typename ST::R3>::F f9;
	typename VT<typename ST::R2>::F fA;
	typename VT<typename ST::R1>::F fB;
	typename VT<typename ST::R0>::F fC;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R14

template <class Signature_p>
class Callable_R14
:	public ZCallable<typename ST_T<Signature_p>::SR14>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R14(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RD>::P i0,
		typename VT<typename ST::RC>::P i1,
		typename VT<typename ST::RB>::P i2,
		typename VT<typename ST::RA>::P i3,
		typename VT<typename ST::R9>::P i4,
		typename VT<typename ST::R8>::P i5,
		typename VT<typename ST::R7>::P i6,
		typename VT<typename ST::R6>::P i7,
		typename VT<typename ST::R5>::P i8,
		typename VT<typename ST::R4>::P i9,
		typename VT<typename ST::R3>::P iA,
		typename VT<typename ST::R2>::P iB,
		typename VT<typename ST::R1>::P iC,
		typename VT<typename ST::R0>::P iD)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;

	using ZCallable<typename ST_T<Signature_p>::SR14>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	typename VT<typename ST::RD>::F f0;
	typename VT<typename ST::RC>::F f1;
	typename VT<typename ST::RB>::F f2;
	typename VT<typename ST::RA>::F f3;
	typename VT<typename ST::R9>::F f4;
	typename VT<typename ST::R8>::F f5;
	typename VT<typename ST::R7>::F f6;
	typename VT<typename ST::R6>::F f7;
	typename VT<typename ST::R5>::F f8;
	typename VT<typename ST::R4>::F f9;
	typename VT<typename ST::R3>::F fA;
	typename VT<typename ST::R2>::F fB;
	typename VT<typename ST::R1>::F fC;
	typename VT<typename ST::R0>::F fD;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R15

template <class Signature_p>
class Callable_R15
:	public ZCallable<typename ST_T<Signature_p>::SR15>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R15(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RE>::P i0,
		typename VT<typename ST::RD>::P i1,
		typename VT<typename ST::RC>::P i2,
		typename VT<typename ST::RB>::P i3,
		typename VT<typename ST::RA>::P i4,
		typename VT<typename ST::R9>::P i5,
		typename VT<typename ST::R8>::P i6,
		typename VT<typename ST::R7>::P i7,
		typename VT<typename ST::R6>::P i8,
		typename VT<typename ST::R5>::P i9,
		typename VT<typename ST::R4>::P iA,
		typename VT<typename ST::R3>::P iB,
		typename VT<typename ST::R2>::P iC,
		typename VT<typename ST::R1>::P iD,
		typename VT<typename ST::R0>::P iE)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;

	using ZCallable<typename ST_T<Signature_p>::SR15>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	R Call(P0 i0)
		{ return fCallable->Call(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	typename VT<typename ST::RE>::F f0;
	typename VT<typename ST::RD>::F f1;
	typename VT<typename ST::RC>::F f2;
	typename VT<typename ST::RB>::F f3;
	typename VT<typename ST::RA>::F f4;
	typename VT<typename ST::R9>::F f5;
	typename VT<typename ST::R8>::F f6;
	typename VT<typename ST::R7>::F f7;
	typename VT<typename ST::R6>::F f8;
	typename VT<typename ST::R5>::F f9;
	typename VT<typename ST::R4>::F fA;
	typename VT<typename ST::R3>::F fB;
	typename VT<typename ST::R2>::F fC;
	typename VT<typename ST::R1>::F fD;
	typename VT<typename ST::R0>::F fE;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_R16

template <class Signature_p>
class Callable_R16
:	public ZCallable<typename ST_T<Signature_p>::SR16>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_R16(
		const ZRef<Callable>& iCallable,
		typename VT<typename ST::RF>::P i0,
		typename VT<typename ST::RE>::P i1,
		typename VT<typename ST::RD>::P i2,
		typename VT<typename ST::RC>::P i3,
		typename VT<typename ST::RB>::P i4,
		typename VT<typename ST::RA>::P i5,
		typename VT<typename ST::R9>::P i6,
		typename VT<typename ST::R8>::P i7,
		typename VT<typename ST::R7>::P i8,
		typename VT<typename ST::R6>::P i9,
		typename VT<typename ST::R5>::P iA,
		typename VT<typename ST::R4>::P iB,
		typename VT<typename ST::R3>::P iC,
		typename VT<typename ST::R2>::P iD,
		typename VT<typename ST::R1>::P iE,
		typename VT<typename ST::R0>::P iF)
	:	f0(i0), f1(i1), f2(i2), f3(i3), f4(i4), f5(i5), f6(i6), f7(i7)
	,	f8(i8), f9(i9), fA(iA), fB(iB), fC(iC), fD(iD), fE(iE), fF(iF)
	,	fCallable(iCallable)
		{}

	typedef typename ST::R R;

	using ZCallable<typename ST_T<Signature_p>::SR16>::Call;

private:
	R Call()
		{ return fCallable->Call(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

	typename VT<typename ST::RF>::F f0;
	typename VT<typename ST::RE>::F f1;
	typename VT<typename ST::RD>::F f2;
	typename VT<typename ST::RC>::F f3;
	typename VT<typename ST::RB>::F f4;
	typename VT<typename ST::RA>::F f5;
	typename VT<typename ST::R9>::F f6;
	typename VT<typename ST::R8>::F f7;
	typename VT<typename ST::R7>::F f8;
	typename VT<typename ST::R6>::F f9;
	typename VT<typename ST::R5>::F fA;
	typename VT<typename ST::R4>::F fB;
	typename VT<typename ST::R3>::F fC;
	typename VT<typename ST::R2>::F fD;
	typename VT<typename ST::R1>::F fE;
	typename VT<typename ST::R0>::F fF;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * BindR

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR01> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i0)
	{
	return new Callable_R01<typename Callable::Signature>
		(iCallable,
		i0);
	}
// We need to reverse the order of these params
template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR02> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i1)
	{
	return new Callable_R02<typename Callable::Signature>
		(iCallable,
		i0, i1);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR03> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i2)
	{
	return new Callable_R03<typename Callable::Signature>
		(iCallable,
		i0, i1, i2);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR04> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i3)
	{
	return new Callable_R04<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR05> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i4)
	{
	return new Callable_R05<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR06> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i5)
	{
	return new Callable_R06<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR07> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i6)
	{
	return new Callable_R07<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR08> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i7)
	{
	return new Callable_R08<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR09> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i8)
	{
	return new Callable_R09<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR10> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i9)
	{
	return new Callable_R10<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR11> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iA)
	{
	return new Callable_R11<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR12> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RB>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iB)
	{
	return new Callable_R12<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR13> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RC>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::RB>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iC)
	{
	return new Callable_R13<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR14> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RD>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::RC>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::RB>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iD)
	{
	return new Callable_R14<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR15> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RE>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::RD>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::RC>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::RB>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P iD,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iE)
	{
	return new Callable_R15<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR16> >
BindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::RF>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::RE>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::RD>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::RC>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::RB>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::RA>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R9>::P i6,
	typename VT<typename ST_T<typename Callable::Signature>::R8>::P i7,
	typename VT<typename ST_T<typename Callable::Signature>::R7>::P i8,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i9,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P iA,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P iB,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P iC,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P iD,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P iE,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P iF)
	{
	return new Callable_R16<typename Callable::Signature>
		(iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Callable_IgnoreParams

template <class Signature_p>
class Callable_IgnoreParams
:	public ZCallable<Signature_p>
	{
public:
	typedef Signature_p Signature;
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;

	Callable_IgnoreParams(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

	typedef typename ST::R R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;
	typedef typename ST::LD PD;
	typedef typename ST::LE PE;
	typedef typename ST::LF PF;

	using ZCallable<Signature_p>::Call;

private:
	R Call()
		{ return fCallable->Call(); }

	R Call(P0 i0)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->Call(); }

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ return fCallable->Call(); }

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * IgnoreParams

template <class Callable>
ZRef<Callable> IgnoreParams(const ZRef<Callable>& iCallable)
	{ return new Callable_IgnoreParams<typename Callable::Signature>(iCallable); }

// =================================================================================================
#pragma mark -
#pragma mark * Callable_Return

template <class R_p, class Signature_p>
class Callable_Return
:	public ZCallable<typename STReturn<R_p, Signature_p>::Signature>
	{
public:
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;
	typedef typename STReturn<R_p, Signature_p>::Signature Signature;

	Callable_Return(typename VT<R_p>::P iR, const ZRef<Callable>& iCallable)
	:	fR(iR)
	,	fCallable(iCallable)
		{}

	typedef R_p R;
	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;
	typedef typename ST::LD PD;
	typedef typename ST::LE PE;
	typedef typename ST::LF PF;

	using ZCallable<Signature>::Call;

private:
	R Call()
		{
		fCallable->Call();
		return fR;
		}

	R Call(P0 i0)
		{
		fCallable->Call(i0);
		return fR;
		}

	R Call(P0 i0, P1 i1)
		{
		fCallable->Call(i0, i1);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2)
		{
		fCallable->Call(i0, i1, i2);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		fCallable->Call(i0, i1, i2, i3);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		fCallable->Call(i0, i1, i2, i3, i4);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		return fR;
		}

	R Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		return fR;
		}

	typename VT<R>::F fR;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Callable_Return (specialized for void)

template <class Signature_p>
class Callable_Return<void, Signature_p>
:	public ZCallable<typename STReturn<void, Signature_p>::Signature>
	{
public:
	typedef ZCallable<Signature_p> Callable;
	typedef ST_T<Signature_p> ST;
	typedef typename STReturn<void, Signature_p>::Signature Signature;

	Callable_Return(const ZRef<Callable>& iCallable)
	:	fCallable(iCallable)
		{}

	typedef typename ST::L0 P0;
	typedef typename ST::L1 P1;
	typedef typename ST::L2 P2;
	typedef typename ST::L3 P3;
	typedef typename ST::L4 P4;
	typedef typename ST::L5 P5;
	typedef typename ST::L6 P6;
	typedef typename ST::L7 P7;
	typedef typename ST::L8 P8;
	typedef typename ST::L9 P9;
	typedef typename ST::LA PA;
	typedef typename ST::LB PB;
	typedef typename ST::LC PC;
	typedef typename ST::LD PD;
	typedef typename ST::LE PE;
	typedef typename ST::LF PF;

	using ZCallable<Signature>::Call;

private:
	void Call()
		{ fCallable->Call(); }

	void Call(P0 i0)
		{ fCallable->Call(i0); }

	void Call(P0 i0, P1 i1)
		{ fCallable->Call(i0, i1); }

	void Call(P0 i0, P1 i1, P2 i2)
		{ fCallable->Call(i0, i1, i2); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3)
		{ fCallable->Call(i0, i1, i2, i3); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ fCallable->Call(i0, i1, i2, i3, i4); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

	void Call(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ fCallable->Call(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF); }

	ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * BindReturn

template <class R, class Callable>
ZRef<ZCallable<typename STReturn<R, typename Callable::Signature>::Signature> >
BindReturn(R iR, const ZRef<Callable>& iCallable)
	{ return new Callable_Return<R, typename Callable::Signature> (iR, iCallable); }

template <class Callable>
ZRef<ZCallable<typename STReturn<void, typename Callable::Signature>::Signature> >
BindReturnVoid(const ZRef<Callable>& iCallable)
	{ return new Callable_Return<void, typename Callable::Signature>(iCallable); }

} // namespace ZCallable_Bind

using ZCallable_Bind::BindL;
using ZCallable_Bind::BindR;

using ZCallable_Bind::IgnoreParams;

using ZCallable_Bind::BindReturn;
using ZCallable_Bind::BindReturnVoid;

} // namespace ZooLib

#endif // __ZCallable_Bind__
