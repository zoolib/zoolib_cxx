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

#ifndef __ZCallable_Bind_h__
#define __ZCallable_Bind_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"

namespace ZooLib {
namespace ZCallable_Bind {

using namespace ZCallableUtil;

// =================================================================================================
// MARK: - ST (Signature Traits)

struct Empty_t {};

template <class Signature>
struct ST_T;

template <class R_p, class Signature>
struct STReturn;

// =================================================================================================
// MARK: - Signature Traits (specialized for 0 params)

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
// MARK: - Signature Traits (specialized for 1 param)

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
// MARK: - Signature Traits (specialized for 2 params)

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
// MARK: - Signature Traits (specialized for 3 params)

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
// MARK: - Signature Traits (specialized for 4 params)

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
// MARK: - Signature Traits (specialized for 5 params)

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
// MARK: - Signature Traits (specialized for 6 params)

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
// MARK: - Signature Traits (specialized for 7 params)

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
// MARK: - Signature Traits (specialized for 8 params)

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
// MARK: - Signature Traits (specialized for 9 params)

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
// MARK: - Signature Traits (specialized for 10 params)

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
// MARK: - Signature Traits (specialized for 11 params)

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
// MARK: - Signature Traits (specialized for 12 params)

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
// MARK: - Signature Traits (specialized for 13 params)

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
// MARK: - Signature Traits (specialized for 14 params)

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
// MARK: - Signature Traits (specialized for 15 params)

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
// MARK: - Signature Traits (specialized for 16 params)

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
// MARK: - Callable_L01

template <class Signature_p>
class Callable_L01
:	public ZCallable<typename ST_T<Signature_p>::SL01>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->QCall(f0, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE); }

	typename VT<typename ST::L0>::F f0;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L02

template <class Signature_p>
class Callable_L02
:	public ZCallable<typename ST_T<Signature_p>::SL02>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->QCall(f0, f1, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L03

template <class Signature_p>
class Callable_L03
:	public ZCallable<typename ST_T<Signature_p>::SL03>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(f0, f1, f2, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L04

template <class Signature_p>
class Callable_L04
:	public ZCallable<typename ST_T<Signature_p>::SL04>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(f0, f1, f2, f3, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L05

template <class Signature_p>
class Callable_L05
:	public ZCallable<typename ST_T<Signature_p>::SL05>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L06

template <class Signature_p>
class Callable_L06
:	public ZCallable<typename ST_T<Signature_p>::SL06>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9); }

	typename VT<typename ST::L0>::F f0;
	typename VT<typename ST::L1>::F f1;
	typename VT<typename ST::L2>::F f2;
	typename VT<typename ST::L3>::F f3;
	typename VT<typename ST::L4>::F f4;
	typename VT<typename ST::L5>::F f5;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_L07

template <class Signature_p>
class Callable_L07
:	public ZCallable<typename ST_T<Signature_p>::SL07>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6, i7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, i0, i1, i2, i3, i4, i5, i6, i7, i8); }

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
// MARK: - Callable_L08

template <class Signature_p>
class Callable_L08
:	public ZCallable<typename ST_T<Signature_p>::SL08>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5, i6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, i0, i1, i2, i3, i4, i5, i6, i7); }

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
// MARK: - Callable_L09

template <class Signature_p>
class Callable_L09
:	public ZCallable<typename ST_T<Signature_p>::SL09>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4, i5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, i0, i1, i2, i3, i4, i5, i6); }

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
// MARK: - Callable_L10

template <class Signature_p>
class Callable_L10
:	public ZCallable<typename ST_T<Signature_p>::SL10>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3, i4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, i0, i1, i2, i3, i4, i5); }

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
// MARK: - Callable_L11

template <class Signature_p>
class Callable_L11
:	public ZCallable<typename ST_T<Signature_p>::SL11>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2, i3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, i0, i1, i2, i3, i4); }

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
// MARK: - Callable_L12

template <class Signature_p>
class Callable_L12
:	public ZCallable<typename ST_T<Signature_p>::SL12>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1, i2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, i0, i1, i2, i3); }

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
// MARK: - Callable_L13

template <class Signature_p>
class Callable_L13
:	public ZCallable<typename ST_T<Signature_p>::SL13>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, i0, i1, i2); }

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
// MARK: - Callable_L14

template <class Signature_p>
class Callable_L14
:	public ZCallable<typename ST_T<Signature_p>::SL14>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, i0, i1); }

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
// MARK: - Callable_L15

template <class Signature_p>
class Callable_L15
:	public ZCallable<typename ST_T<Signature_p>::SL15>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, i0); }

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
// MARK: - Callable_L16

template <class Signature_p>
class Callable_L16
:	public ZCallable<typename ST_T<Signature_p>::SL16>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

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
// MARK: - sBindL

template <class Callable>
ZRef<ZCallable<typename Callable::Signature> >
sBindL(
	const ZRef<Callable>& iCallable)
	{ return iCallable; }

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL01> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L01<typename Callable::Signature>(
		i0,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL02> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L02<typename Callable::Signature>(
		i0, i1,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL03> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L03<typename Callable::Signature>(
		i0, i1, i2,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL04> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L04<typename Callable::Signature>(
		i0, i1, i2, i3,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL05> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L05<typename Callable::Signature>(
		i0, i1, i2, i3, i4,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL06> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L06<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL07> >
sBindL(
	typename VT<typename ST_T<typename Callable::Signature>::L0>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::L1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::L2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::L3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::L4>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::L5>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::L6>::P i6,
	const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_L07<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL08> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L08<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL09> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L09<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL10> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L10<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL11> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L11<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL12> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L12<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL13> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L13<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL14> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L14<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL15> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L15<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE,
		iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SL16> >
sBindL(
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
	if (not iCallable)
		return null;
	return new Callable_L16<typename Callable::Signature>(
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF,
		iCallable);
	}

// =================================================================================================
// MARK: - Callable_R01

template <class Signature_p>
class Callable_R01
:	public ZCallable<typename ST_T<Signature_p>::SR01>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, f0); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, f0); }

	typename VT<typename ST::R0>::F f0;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R02

template <class Signature_p>
class Callable_R02
:	public ZCallable<typename ST_T<Signature_p>::SR02>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0, f1); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, f0, f1); }

	typename VT<typename ST::R1>::F f0;
	typename VT<typename ST::R0>::F f1;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R03

template <class Signature_p>
class Callable_R03
:	public ZCallable<typename ST_T<Signature_p>::SR03>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1, f2); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, f0, f1, f2); }

	typename VT<typename ST::R2>::F f0;
	typename VT<typename ST::R1>::F f1;
	typename VT<typename ST::R0>::F f2;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R04

template <class Signature_p>
class Callable_R04
:	public ZCallable<typename ST_T<Signature_p>::SR04>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2, f3); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, f0, f1, f2, f3); }

	typename VT<typename ST::R3>::F f0;
	typename VT<typename ST::R2>::F f1;
	typename VT<typename ST::R1>::F f2;
	typename VT<typename ST::R0>::F f3;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R05

template <class Signature_p>
class Callable_R05
:	public ZCallable<typename ST_T<Signature_p>::SR05>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3, f4); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, f0, f1, f2, f3, f4); }

	typename VT<typename ST::R4>::F f0;
	typename VT<typename ST::R3>::F f1;
	typename VT<typename ST::R2>::F f2;
	typename VT<typename ST::R1>::F f3;
	typename VT<typename ST::R0>::F f4;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R06

template <class Signature_p>
class Callable_R06
:	public ZCallable<typename ST_T<Signature_p>::SR06>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4, f5); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, f0, f1, f2, f3, f4, f5); }

	typename VT<typename ST::R5>::F f0;
	typename VT<typename ST::R4>::F f1;
	typename VT<typename ST::R3>::F f2;
	typename VT<typename ST::R2>::F f3;
	typename VT<typename ST::R1>::F f4;
	typename VT<typename ST::R0>::F f5;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_R07

template <class Signature_p>
class Callable_R07
:	public ZCallable<typename ST_T<Signature_p>::SR07>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5, f6); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, f0, f1, f2, f3, f4, f5, f6); }

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
// MARK: - Callable_R08

template <class Signature_p>
class Callable_R08
:	public ZCallable<typename ST_T<Signature_p>::SR08>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6, f7); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, f0, f1, f2, f3, f4, f5, f6, f7); }

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
// MARK: - Callable_R09

template <class Signature_p>
class Callable_R09
:	public ZCallable<typename ST_T<Signature_p>::SR09>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, f0, f1, f2, f3, f4, f5, f6, f7, f8); }

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
// MARK: - Callable_R10

template <class Signature_p>
class Callable_R10
:	public ZCallable<typename ST_T<Signature_p>::SR10>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(i0, i1, i2, i3, i4, i5, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9); }

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
// MARK: - Callable_R11

template <class Signature_p>
class Callable_R11
:	public ZCallable<typename ST_T<Signature_p>::SR11>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{return fCallable->QCall(i0, i1, i2, i3, i4, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA); }

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
// MARK: - Callable_R12

template <class Signature_p>
class Callable_R12
:	public ZCallable<typename ST_T<Signature_p>::SR12>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(i0, i1, i2, i3, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB); }

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
// MARK: - Callable_R13

template <class Signature_p>
class Callable_R13
:	public ZCallable<typename ST_T<Signature_p>::SR13>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(i0, i1, i2, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC); }

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
// MARK: - Callable_R14

template <class Signature_p>
class Callable_R14
:	public ZCallable<typename ST_T<Signature_p>::SR14>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(i0, i1, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD); }

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
// MARK: - Callable_R15

template <class Signature_p>
class Callable_R15
:	public ZCallable<typename ST_T<Signature_p>::SR15>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(i0, f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE); }

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
// MARK: - Callable_R16

template <class Signature_p>
class Callable_R16
:	public ZCallable<typename ST_T<Signature_p>::SR16>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, fA, fB, fC, fD, fE, fF); }

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
// MARK: - sBindR

template <class Callable>
ZRef<ZCallable<typename Callable::Signature> >
sBindR(
	const ZRef<Callable>& iCallable)
	{ return iCallable; }

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR01> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i0)
	{
	if (not iCallable)
		return null;
	return new Callable_R01<typename Callable::Signature>(
		iCallable,
		i0);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR02> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i1)
	{
	if (not iCallable)
		return null;
	return new Callable_R02<typename Callable::Signature>(
		iCallable,
		i0, i1);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR03> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i2)
	{
	if (not iCallable)
		return null;
	return new Callable_R03<typename Callable::Signature>(
		iCallable,
		i0, i1, i2);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR04> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i3)
	{
	if (not iCallable)
		return null;
	return new Callable_R04<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR05> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i4)
	{
	if (not iCallable)
		return null;
	return new Callable_R05<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR06> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i5)
	{
	if (not iCallable)
		return null;
	return new Callable_R06<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR07> >
sBindR(
	const ZRef<Callable>& iCallable,
	typename VT<typename ST_T<typename Callable::Signature>::R6>::P i0,
	typename VT<typename ST_T<typename Callable::Signature>::R5>::P i1,
	typename VT<typename ST_T<typename Callable::Signature>::R4>::P i2,
	typename VT<typename ST_T<typename Callable::Signature>::R3>::P i3,
	typename VT<typename ST_T<typename Callable::Signature>::R2>::P i4,
	typename VT<typename ST_T<typename Callable::Signature>::R1>::P i5,
	typename VT<typename ST_T<typename Callable::Signature>::R0>::P i6)
	{
	if (not iCallable)
		return null;
	return new Callable_R07<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR08> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R08<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR09> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R09<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR10> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R10<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR11> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R11<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR12> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R12<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR13> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R13<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR14> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R14<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR15> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R15<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
	}

template <class Callable>
ZRef<ZCallable<typename ST_T<typename Callable::Signature>::SR16> >
sBindR(
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
	if (not iCallable)
		return null;
	return new Callable_R16<typename Callable::Signature>(
		iCallable,
		i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
	}

// =================================================================================================
// MARK: - Callable_IgnoreParams

template <class Signature_p>
class Callable_IgnoreParams
:	public ZCallable<Signature_p>
	{
public:
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

private:
	ZQ<R> QCall()
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{ return fCallable->QCall(fCallable); }

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{ return fCallable->QCall(fCallable); }

	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - sBind_IgnoreParams

template <class Callable>
ZRef<Callable> sBind_IgnoreParams(const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_IgnoreParams<typename Callable::Signature>(iCallable);
	}

// =================================================================================================
// MARK: - Callable_Return

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

private:
	ZQ<R> QCall()
		{
		sCall(fCallable);
		return fR;
		}

	ZQ<R> QCall(P0 i0)
		{
		sCall(fCallable, i0);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1)
		{
		sCall(fCallable, i0, i1);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2)
		{
		sCall(fCallable, i0, i1, i2);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		sCall(fCallable, i0, i1, i2, i3);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		sCall(fCallable, i0, i1, i2, i3, i4);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE);
		return fR;
		}

	ZQ<R> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		sCall(fCallable, i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF);
		return fR;
		}

	typename VT<R>::F fR;
	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - Callable_Return (specialized for void)

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

private:
	ZQ<void> QCall()
		{
		if (fCallable->QCall(fCallable))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0)
		{
		if (fCallable->QCall(i0))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1)
		{
		if (fCallable->QCall(i0, i1))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2)
		{
		if (fCallable->QCall(i0, i1, i2))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3)
		{
		if (fCallable->QCall(i0, i1, i2, i3))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE))
			return notnull;
		return null;
		}

	ZQ<void> QCall(P0 i0, P1 i1, P2 i2, P3 i3, P4 i4, P5 i5, P6 i6, P7 i7,
		P8 i8, P9 i9, PA iA, PB iB, PC iC, PD iD, PE iE, PF iF)
		{
		if (fCallable->QCall(i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, iA, iB, iC, iD, iE, iF))
			return notnull;
		return null;
		}

	ZRef<Callable> fCallable;
	};

// =================================================================================================
// MARK: - sBind_Return

template <class R, class Callable>
ZRef<ZCallable<typename STReturn<R, typename Callable::Signature>::Signature> >
sBind_Return(R iR, const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_Return<R, typename Callable::Signature>(iR, iCallable);
	}

template <class Callable>
ZRef<ZCallable<typename STReturn<void, typename Callable::Signature>::Signature> >
sBind_ReturnVoid(const ZRef<Callable>& iCallable)
	{
	if (not iCallable)
		return null;
	return new Callable_Return<void, typename Callable::Signature>(iCallable);
	}

} // namespace ZCallable_Bind

using ZCallable_Bind::sBindL;
using ZCallable_Bind::sBindR;

using ZCallable_Bind::sBind_IgnoreParams;

using ZCallable_Bind::sBind_Return;
using ZCallable_Bind::sBind_ReturnVoid;

} // namespace ZooLib

#endif // __ZCallable_Bind_h__
