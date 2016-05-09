/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#ifndef __ZooLib_DeriveFrom_h__
#define __ZooLib_DeriveFrom_h__

#include "zconfig.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark DeriveFrom

class DeriveFrom0
	{};

template <class P0>
class DeriveFrom1
:	public virtual DeriveFrom0
,	public virtual P0
	{};

template <class P0, class P1>
class DeriveFrom2
:	public virtual DeriveFrom1<    P1>
,	public virtual DeriveFrom1<P0    >
	{};

template <class P0, class P1, class P2>
class DeriveFrom3
:	public virtual DeriveFrom2<    P1, P2>
,	public virtual DeriveFrom2<P0,     P2>
,	public virtual DeriveFrom2<P0, P1    >
	{};

template <class P0, class P1, class P2, class P3>
class DeriveFrom4
:	public virtual DeriveFrom3<    P1, P2, P3>
,	public virtual DeriveFrom3<P0,     P2, P3>
,	public virtual DeriveFrom3<P0, P1,     P3>
,	public virtual DeriveFrom3<P0, P1, P2    >
	{};

template <class P0, class P1, class P2, class P3, class P4>
class DeriveFrom5
:	public virtual DeriveFrom4<    P1, P2, P3, P4>
,	public virtual DeriveFrom4<P0,     P2, P3, P4>
,	public virtual DeriveFrom4<P0, P1,     P3, P4>
,	public virtual DeriveFrom4<P0, P1, P2,     P4>
,	public virtual DeriveFrom4<P0, P1, P2, P3    >
	{};

template <class P0, class P1, class P2, class P3, class P4, class P5>
class DeriveFrom6
:	public virtual DeriveFrom5<    P1, P2, P3, P4, P5>
,	public virtual DeriveFrom5<P0,     P2, P3, P4, P5>
,	public virtual DeriveFrom5<P0, P1,     P3, P4, P5>
,	public virtual DeriveFrom5<P0, P1, P2,     P4, P5>
,	public virtual DeriveFrom5<P0, P1, P2, P3,     P5>
,	public virtual DeriveFrom5<P0, P1, P2, P3, P4    >
	{};

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6>
class DeriveFrom7
:	public virtual DeriveFrom6<    P1, P2, P3, P4, P5, P6>
,	public virtual DeriveFrom6<P0,     P2, P3, P4, P5, P6>
,	public virtual DeriveFrom6<P0, P1,     P3, P4, P5, P6>
,	public virtual DeriveFrom6<P0, P1, P2,     P4, P5, P6>
,	public virtual DeriveFrom6<P0, P1, P2, P3,     P5, P6>
,	public virtual DeriveFrom6<P0, P1, P2, P3, P4,     P6>
,	public virtual DeriveFrom6<P0, P1, P2, P3, P4, P5    >
	{};

template <class P0, class P1, class P2, class P3, class P4, class P5, class P6, class P7>
class DeriveFrom8
:	public virtual DeriveFrom7<    P1, P2, P3, P4, P5, P6, P7>
,	public virtual DeriveFrom7<P0,     P2, P3, P4, P5, P6, P7>
,	public virtual DeriveFrom7<P0, P1,     P3, P4, P5, P6, P7>
,	public virtual DeriveFrom7<P0, P1, P2,     P4, P5, P6, P7>
,	public virtual DeriveFrom7<P0, P1, P2, P3,     P5, P6, P7>
,	public virtual DeriveFrom7<P0, P1, P2, P3, P4,     P6, P7>
,	public virtual DeriveFrom7<P0, P1, P2, P3, P4, P5,     P7>
,	public virtual DeriveFrom7<P0, P1, P2, P3, P4, P5, P6    >
	{};

} // namespace ZooLib

#endif // __ZooLib_DeriveFrom_h__
