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

#ifndef __ZRA_RelHead__
#define __ZRA_RelHead__ 1
#include "zconfig.h"

#include "zoolib/ZSet_T.h"

#include "zoolib/zra/ZRA_RelName.h"

#include <set>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Rename_t

// This needs to be in ZooLib, or we get ambiguities with the one defined in ZValPred_T.h

typedef std::map<std::string, std::string> Rename_t;

Rename_t sInverted(const Rename_t& iRename);

namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * RelHead

typedef ZSet_T<RelName> RelHead;

RelName sPrefixAdd(const RelName& iPrefix, const RelName& iRelName);
RelName sPrefixRemove(const RelName& iPrefix, const RelName& iRelName);

RelHead sPrefixAdd(const RelName& iPrefix, const RelHead& iRelHead);
RelHead sPrefixRemove(const RelName& iPrefix, const RelHead& iRelHead);

// =================================================================================================
#pragma mark -
#pragma mark * RelHead operators

inline RelHead operator&(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead & RelHead::key_type(iElem); }

inline RelHead operator&(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead & RelHead::key_type(iElem); }

inline RelHead operator|(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead | RelHead::key_type(iElem); }

inline RelHead operator|(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead | RelHead::key_type(iElem); }

inline RelHead operator-(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead - RelHead::key_type(iElem); }

inline RelHead operator^(const char* iElem, const RelHead& iRelHead)
	{ return iRelHead ^ RelHead::key_type(iElem); }

inline RelHead operator^(const RelHead& iRelHead, const char* iElem)
	{ return iRelHead ^ RelHead::key_type(iElem); }

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_RelHead__
