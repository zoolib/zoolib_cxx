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

#ifndef __ZRA_RelHead_h__
#define __ZRA_RelHead_h__ 1
#include "zconfig.h"

#include "zoolib/ZSet_T.h"

#include "zoolib/zra/ZRA_RelName.h"

#include <map>
#include <set>

namespace ZooLib {
namespace ZRA {

// =================================================================================================
// MARK: - Rename

typedef std::map<RelName,RelName> Rename;

Rename sInverted(const Rename& iRename);

// =================================================================================================
// MARK: - RelHead

typedef ZSet_T<RelName> RelHead;

// =================================================================================================
// MARK: - RelHead operators

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

// =================================================================================================
// MARK: - Prefix

bool sHasPrefix(const RelName& iPrefix, const RelName& iRelName);
RelName sPrefixInserted(const RelName& iPrefix, const RelName& iRelName);
RelName sPrefixErased(const RelName& iPrefix, const RelName& iRelName);
RelName sRenamed(const Rename& iRename, const RelName& iRelName);

bool sHasPrefix(const RelName& iPrefix, const RelHead& iRelHead);
RelHead sPrefixInserted(const RelName& iPrefix, const RelHead& iRelHead);
RelHead sPrefixErased(const RelName& iPrefix, const RelHead& iRelHead);
RelHead sRenamed(const Rename& iRename, const RelHead& iRelHead);

} // namespace ZRA
} // namespace ZooLib

#endif // __ZRA_RelHead_h__
