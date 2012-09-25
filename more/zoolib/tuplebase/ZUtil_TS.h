/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZUtil_TS__
#define __ZUtil_TS__
#include "zconfig.h"

#include "zoolib/ZTuple.h"
#include "zoolib/ZTypes.h"

#include <map>
#include <stdexcept>

namespace ZooLib {

class ZStreamR;
class ZStreamW;
class ZStreamRPos;
class ZStreamWPos;
class ZStrimU;
class ZStrimW;

namespace ZUtil_TS {

class Ex_Base : public std::runtime_error
	{
protected:
	Ex_Base(const char* what);
	Ex_Base(const std::string& what);
	};

class Ex_MagicTextMissing : public Ex_Base
	{
public:
	Ex_MagicTextMissing();
	};

class Ex_IDInvalid : public Ex_Base
	{
public:
	Ex_IDInvalid();
	};

class Ex_IDOutOfSequence : public Ex_Base
	{
public:
	Ex_IDOutOfSequence();
	};

class Ex_IDOutOfBounds : public Ex_Base
	{
public:
	Ex_IDOutOfBounds();
	};

class Ex_IDDuplicate : public Ex_Base
	{
public:
	Ex_IDDuplicate();
	};

class Ex_OversizedStream : public Ex_Base
	{
public:
	Ex_OversizedStream();
	};

class Ex_MalformedText : public Ex_Base
	{
public:
	Ex_MalformedText(const char* what);
	Ex_MalformedText(const std::string& what);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Source

class Source
	{
public:
	virtual bool Get(uint64& oID, ZTuple& oTuple) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Sink

class Sink
	{
public:
	virtual bool Set(uint64 iID, const ZTuple& iTuple) = 0;
	virtual void Clear() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Source_Map

class Source_Map : public Source
	{
public:
	Source_Map(const std::map<uint64, ZTuple>& iMap);

// From Source
	virtual bool Get(uint64& oID, ZTuple& oTuple);

private:
	std::map<uint64, ZTuple>::const_iterator fIter;
	std::map<uint64, ZTuple>::const_iterator fEnd;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS::Sink_Map

class Sink_Map : public Sink
	{
public:
	Sink_Map(std::map<uint64, ZTuple>& oMap);

// From Sink
	virtual bool Set(uint64 iID, const ZTuple& iTuple);
	virtual void Clear();

private:
	std::map<uint64, ZTuple>& fMap;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_TS

void sToStream(uint64 iNextUnusedID, Source& iSource, const ZStreamWPos& iStreamWPos);
void sFromStream(Sink& iSink, uint64& oNextUnusedID, const ZStreamR& iStreamR);

void sToStrim(uint64 iNextUnusedID, Source& iSource, const ZStrimW& iStrimW);
void sFromStrim(Sink& iSink, uint64& oNextUnusedID, const ZStrimU& iStrimU);

void sRead(const ZStreamRPos& iStreamRPos, uint64& oNextUnusedID, Sink& iSink);

void sWriteMagicText(const ZStreamW& iStreamW);

} // namespace ZUtil_TS

} // namespace ZooLib

#endif // __ZUtil_TS__
