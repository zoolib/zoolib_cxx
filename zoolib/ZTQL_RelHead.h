/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTQL_RelHead__
#define __ZTQL_RelHead__ 1
#include "zconfig.h"

#include "zoolib/ZTName.h" // For ZTName

#include <set>
#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::RelHead

namespace ZTQL {

class RelHead
	{
public:
	RelHead();
	RelHead(const ZTName& iName);
	RelHead(const std::set<ZTName>& iNames);
	RelHead(std::set<ZTName>& ioNames, bool iKnowWhatImDoing);
	RelHead(const std::vector<ZTName>& iNames);
	RelHead(const ZTName* iNames, size_t iCount);
	RelHead(const RelHead& iOther);
	~RelHead();

	RelHead& operator=(const RelHead& iOther);

	bool operator==(const RelHead& iOther) const;
	bool operator!=(const RelHead& iOther) const;

	RelHead& operator&=(const RelHead& iOther);
	RelHead operator&(const RelHead& iOther) const;

	RelHead& operator|=(const RelHead& iOther);
	RelHead operator|(const RelHead& iOther) const;

	RelHead& operator|=(const ZTName& iName);
	RelHead operator|(const ZTName& iName) const;

	RelHead& operator-=(const RelHead& iOther);
	RelHead operator-(const RelHead& iOther) const;

	RelHead& operator-=(const ZTName& iName);
	RelHead operator-(const ZTName& iName) const;

	RelHead& operator^=(const RelHead& iOther);
	RelHead operator^(const RelHead& iOther) const;

	bool Contains(const RelHead& iOther) const;
	bool Contains(const ZTName& iName) const;

	void Add(const ZTName& iName);

	void GetNames(std::set<ZTName>& oNames) const;

private:
	std::set<ZTName> fNames;
	};

RelHead operator|(const ZTName& iName, const RelHead& iRelHead);

RelHead operator&(const ZTName& iName, const RelHead& iRelHead);

} // namespace ZTQL

ZTQL::RelHead operator|(const ZTName& iLHS, const ZTName& iRHS);

#endif // __ZTQL_RelHead__
