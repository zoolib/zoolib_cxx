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

#ifndef __ZRelHead__
#define __ZRelHead__ 1
#include "zconfig.h"

#include <set>
#include <string>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZRelHead

class ZRelHead
	{
public:
	ZRelHead();
	ZRelHead(const ZRelHead& iOther);
	~ZRelHead();
	ZRelHead& operator=(const ZRelHead& iOther);

	explicit ZRelHead(bool iUniversal);

	ZRelHead(const char* iName);
	ZRelHead(const std::string& iName);

	ZRelHead(const std::string& iName1, const std::string& iName2);

	ZRelHead(const std::set<std::string>& iNames);
	ZRelHead(std::set<std::string>& ioNames, bool iKnowWhatImDoing);

	ZRelHead(const std::vector<std::string>& iNames);
	ZRelHead(const std::string* iNames, size_t iCount);

	bool operator==(const ZRelHead& iOther) const;
	bool operator!=(const ZRelHead& iOther) const;

	ZRelHead& operator&=(const ZRelHead& iOther);
	ZRelHead operator&(const ZRelHead& iOther) const;

	ZRelHead& operator|=(const ZRelHead& iOther);
	ZRelHead operator|(const ZRelHead& iOther) const;

	ZRelHead& operator-=(const ZRelHead& iOther);
	ZRelHead operator-(const ZRelHead& iOther) const;

	ZRelHead& operator^=(const ZRelHead& iOther);
	ZRelHead operator^(const ZRelHead& iOther) const;

	bool Contains(const ZRelHead& iOther) const;
	bool Contains(const std::string& iName) const;

	void Add(const std::string& iName);
	void Remove(const std::string& iName);

	void GetNames(bool& oUniversal, std::set<std::string>& oNames) const;

private:
	bool fUniversal;
	std::set<std::string> fNames;
	};

ZRelHead operator|(const std::string& iName, const ZRelHead& iRelHead);

ZRelHead operator&(const std::string& iName, const ZRelHead& iRelHead);

//ZRelHead operator|(const std::string& iLHS, const std::string& iRHS);

NAMESPACE_ZOOLIB_END

#endif // __ZRelHead__
