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

#include "ZVal.h"

NAMESPACE_ZOOLIB_BEGIN

/**
\defgroup ZVal Variant Types

Variant types show up all over the place, in COM, CFTypes, interfacing with Javascript and other
languages etc. The APIs for each differ wildly even though they're conceptually very similar. To
make life easier ZooLib provides several type suites that put a consistent API atop variants and
related types so the the terminology, constraints and facilities are homogenous.

The three essential types provided in each suite are Val, Seq and Map. A Val is what's often called
a variant, a 'disjoint union' or a 'tagged union'. It has
\link ValueSemantics value semantics\endlink,
holding a single value of some type at any point in time. Assigning a new value of a supported type
replaces the prior value. The type of the stored value and the value itself are both accessible.

Seq is a sequence of Vals -- an array or vector. The number of elements in the Seq is called its
count, each element is accessed using a zero-based index.

Map is an associative map, from name to Val. A name is a std::string, although for particular suites
there may be another form that is native (a fourcc for example).

Seq and Map also maintain
\link ValueSemantics value semantics\endlink.

In each suite the types are named \c ZVal_XXX, \c ZSeq_XXX and \c ZMap_XXX, where \c XXX is an
appropriate distinguishing string. Some of the suites are placed within a namespace YYY, wherein
the types are named \c YYY::Val, \c YYY::Seq and \c YYY:Map.

When working with a Val you may use simple assignment, or using a templated Set method:
\code
Val theVal = 27;
theVal.Set_T<long>(19);
\endcode
The latter makes the stored type explicit. The actual stored type is usually important, because
you can only retrieve the value if you know what type was used to store it (think long vs int, or
even unsigned vs signed).

There are several ways to get a value. Which you use depends on what you're doing:
\code
// Assuming a Val in theVal
int theInt1 = theVal.Get_T<int>();
int theInt2 = theVal.DGet_T<int>(19);
int theInt3;
if (theVal.QGet_T<int>(theInt3))
	{} // use theInt3
if (const int* theIntPtr = theVal.PGet_T<int>())
	{} // use *theIntptr
\endcode
The first is the simplest -- simply call \c Get_T<yyy>, and you'll be returned a \c yyy. The actual
value will be either the default value for a \c yyy (e.g. zero for numbers), or it will be the
value that was stored.

The second example gives you control over what the default value will be, in this case we're
passing 19, so theInt2 will be 19 or whatever int was previously assigned to theVal.

The third example puts the stored value in theInt3 and returns true, or leaves theInt3 untouched
and returns false. The 'Q' stands for Query (cf LISPish function names).

The final example is not available in all Val suites. For those where there is an accessible
storage location we can return a pointer to the actual value iff what's stored is of the correct
type. This generally is available with tagged unions, not with more abstracted APIs.

For those APIs with a fixed repertoire of supported types, methods named \c SetXXX, \c GetXXX,
\c DGetXXX and \c QGetXXX are often provided, where \c XXX is the generalized typename
(eg \c Int32, \c Int64, \c Float etc).

The Get/Set API for Seq is based on that for Val, but with an initial index parameter. In addition
it adds a \c Count method returning the number of Vals in the Seq, and generally has
\c Erase, \c Insert and \c Append methods.

Map is similar, but here the initial parameter is a name of some sort. Generally a std::string can
be used, which is converted to an underlying type if appropriate. There's usually
also some \c Index_t type, which may be an integer or a more complex type -- the Map will have
\c Begin and \c End methods, which respectively return an \c Index_t referring to the first stored
Val and an \c Index_t that referring to the position just past the end of the map.

Where possible Val, Seq and Map are type-compatible with the appropriate native type. For example,
a ZWinCOM::Variant can be used where a VARIANT would be expected. In other cases the native
API itself does not have an equivalent of Val, so our Val is a subclass of ZAny, and we
do appropriate marshalling at the Map and Seq level.

There is one more set of ZooLib types that are conceptually part of the ZVal API. ZData_XXX is 
equivalent to a CFData or NSData -- it's simply a block of bytes. ZData_CF and ZData_NS wrap
CFData and NSData respectively, and ZData_Any is signature compatible with them both and is
implemented as a vector<char>.
*/

/* ZVal_Any, ZVal_AppleEvent, ZVal_CFType, ZVal_NSObject, ZWinRegistry::Val, ZPhotoshop::Val,
ZNetscape::VariantG, ZNetscape::VariantH and ZWinCOM::Variant. As far as possible the classes
are type compatible with native types e.g a ZWinCOM::Variant can be used where a VARIANT
would be expected. Oftentimes there's no native ZVal type, instead the native API has */

NAMESPACE_ZOOLIB_END
