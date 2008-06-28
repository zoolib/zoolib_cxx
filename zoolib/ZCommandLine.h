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

#ifndef __ZCommandLine__
#define __ZCommandLine__ 1

#include "zconfig.h"

#include "zoolib/ZTuple.h"

class ZStrimW;

// =================================================================================================
#pragma mark -
#pragma mark * ZCommandLine

class ZCommandLine
	{
	class Opt;
	friend class Opt;

public:
	class String;
	class Boolean;
	class Int64;
	class TValue;
	typedef TValue TupleValue;

	enum EFlags
		{
		eRequired,
		eOptional
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
	bool Internal_Parse(bool iUpdateArgs, const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv);
	bool Internal_ParseOne(const ZStrimW* iStrimErrors, int& ioArgc, char**& ioArgv);
	void Internal_AppendOpt(Opt* iOpt);

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
#pragma mark -
#pragma mark * ZCommandLine::Opt

class ZCommandLine::Opt
	{
	friend class ZCommandLine;
protected:
	Opt(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	virtual ~Opt() { /* virtual to force allocation of vtable, so dynamic_cast works */ }

public:
	bool HasValue() const;

private:
	Opt* fNext;

	std::string fName;
	std::string fDescription;
	bool fIsRequired;

protected:
	bool fHasValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommandLine::String

class ZCommandLine::String : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	String(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	String(const std::string& iName, const std::string& iDescription, const std::string& iDefault);
	String(const std::string& iName, const std::string& iDescription);

	const std::string& operator()() const;

private:
	bool fHasDefault;
	std::string fDefault;
	std::string fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommandLine::Boolean

class ZCommandLine::Boolean : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	Boolean(const std::string& iName, const std::string& iDescription);
	Boolean(const std::string& iName, const std::string& iDescription, bool iDefault);

	bool operator()() const;

private:
	bool fHasDefault;
	bool fDefault;
	bool fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommandLine::Integer

class ZCommandLine::Int64 : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	Int64(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	Int64(const std::string& iName, const std::string& iDescription, int64 iDefault);
	Int64(const std::string& iName, const std::string& iDescription);

	int64 operator()() const;

private:
	bool fHasDefault;
	int64 fDefault;
	int64 fValue;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommandLine::TValue

class ZCommandLine::TValue : public ZCommandLine::Opt
	{
	friend class ZCommandLine;
public:
	TValue(const std::string& iName, const std::string& iDescription, EFlags iFlags);
	TValue(const std::string& iName, const std::string& iDescription, const ZTValue& iDefault);
	TValue(const std::string& iName, const std::string& iDescription);

	const ZTValue& operator()() const;

private:
	bool fHasDefault;
	ZTValue fDefault;
	ZTValue fValue;
	};

#endif // __ZCommandLine__
