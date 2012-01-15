/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZCommandLine_h__
#define __ZCommandLine_h__ 1
#include "zconfig.h"

#include "zoolib/ZStrim.h"

namespace ZooLib {

class ZStrimW;

// =================================================================================================
// MARK: - ZCommandLine

class ZCommandLine
	{
public:
	class Opt;
	friend class Opt;

	class String;
	class Boolean;
	class Int64;
	class TValue;

	enum EFlags
		{
		eOptional,
		eRequired
		};

protected:
	ZCommandLine();
	~ZCommandLine();

public:
	bool Parse(int argc, char** argv);
	bool Parse(const ZStrimW& iStrimErrors, int argc, char** argv);
	bool Parse(bool iUpdateArgs, int& ioArgc, char**& ioArgv);
	bool Parse(bool iUpdateArgs, const ZStrimW& iStrimErrors, int& ioArgc, char**& ioArgv);

	operator const void*() const;

	void WriteUsage(const ZStrimW& s) const;
	void WriteUsageExtended(const ZStrimW& s) const;
	void WriteUsageExtended(const ZStrimW& s, const std::string& iIndent) const;

private:
	bool pParse(bool iUpdateArgs, const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv);
	bool pParseOne(const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv);
	void pAppendOpt(Opt* iOpt);

	static ZCommandLine* sCommandLineCurrent;

	Opt* fHead;
	Opt* fTail;
	};

inline const ZStrimW& operator<<(const ZStrimW& s, const ZCommandLine& iCmdLine)
	{
	iCmdLine.WriteUsage(s);
	return s;
	}

// =================================================================================================
// MARK: - ZCommandLine::Opt

class ZCommandLine::Opt
	{
	friend class ZCommandLine;
protected:
	Opt(const std::string& iName, const std::string& iDescription, EFlags iFlags, bool iHasDefault);
	virtual ~Opt();

public:
	bool HasValue() const;

	virtual void WriteDefault(const ZStrimW& s);
	virtual bool Parse(const char* iLexeme, const ZStrimW* iStrimErrors) = 0;

private:
	Opt* fNext;

	std::string fDescription;
	bool fIsRequired;

protected:
	std::string fName;
	bool fHasValue;
	bool fHasDefault;
	};

// =================================================================================================
// MARK: - ZCommandLine::String

class ZCommandLine::String : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	String(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	String(const std::string& iName, const std::string& iDescription, const std::string& iDefault);
	String(const std::string& iName, const std::string& iDescription);

	virtual void WriteDefault(const ZStrimW& s);
	virtual bool Parse(const char* iLexeme, const ZStrimW* iStrimErrors);

	const std::string& operator()() const;

private:
	std::string fDefault;
	std::string fValue;
	};

// =================================================================================================
// MARK: - ZCommandLine::Boolean

class ZCommandLine::Boolean : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	Boolean(const std::string& iName, const std::string& iDescription);

	virtual bool Parse(const char* iLexeme, const ZStrimW* iStrimErrors);

	bool operator()() const;
	};

// =================================================================================================
// MARK: - ZCommandLine::Integer

class ZCommandLine::Int64 : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	Int64(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	Int64(const std::string& iName, const std::string& iDescription, int64 iDefault);
	Int64(const std::string& iName, const std::string& iDescription);

	virtual void WriteDefault(const ZStrimW& s);
	virtual bool Parse(const char* iLexeme, const ZStrimW* iStrimErrors);

	int64 operator()() const;

private:
	int64 fDefault;
	int64 fValue;
	};

} // namespace ZooLib

#endif // __ZCommandLine_h__
