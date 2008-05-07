/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZUtil_Tuple.h"

#include "ZString.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Tuple

namespace ZUtil_Tuple {

// ----------

bool sInt64(const ZTValue& iTV, int64& oVal)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Int8: oVal = iTV.GetInt8(); return true;
		case eZType_Int16: oVal = iTV.GetInt16(); return true;
		case eZType_Int32: oVal = iTV.GetInt32(); return true;
		case eZType_Int64: oVal = iTV.GetInt64(); return true;
		case eZType_Float: oVal = int64(iTV.GetFloat()); return true;
		case eZType_Double: oVal = int64(iTV.GetDouble()); return true;
		case eZType_Bool: oVal = iTV.GetBool() ? 1 : 0; return true;
		case eZType_ID: oVal = iTV.GetID(); return true;
		}
	return false;
	}

int64 sInt64D(const ZTValue& iTV, int64 iDefault)
	{
	int64 result;
	if (sInt64(iTV, result))
		return result;
	return iDefault;
	}

int64 sInt64(const ZTValue& iTV)
	{ return sInt64D(iTV, 0); }

// ----------

bool sUInt64(const ZTValue& iTV, uint64& oVal)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Int8: oVal = uint8(iTV.GetInt8()); return true;
		case eZType_Int16: oVal = uint16(iTV.GetInt16()); return true;
		case eZType_Int32: oVal = uint32(iTV.GetInt32()); return true;
		case eZType_Int64: oVal = uint64(iTV.GetInt64()); return true;
		case eZType_Float: oVal = uint64(iTV.GetFloat()); return true;
		case eZType_Double: oVal = uint64(iTV.GetDouble()); return true;
		case eZType_Bool: oVal = iTV.GetBool() ? 1 : 0; return true;
		case eZType_ID: oVal = iTV.GetID(); return true;
		}
	return false;
	}

uint64 sUInt64D(const ZTValue& iTV, uint64 iDefault)
	{
	uint64 result;
	if (sUInt64(iTV, result))
		return result;
	return iDefault;
	}

uint64 sUInt64(const ZTValue& iTV)
	{ return sUInt64D(iTV, 0); }

// ----------

bool sDouble(const ZTValue& iTV, double& oVal)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Int8: oVal = iTV.GetInt8(); return true;
		case eZType_Int16: oVal = iTV.GetInt16(); return true;
		case eZType_Int32: oVal = iTV.GetInt32(); return true;
		case eZType_Int64: oVal = iTV.GetInt64(); return true;
		case eZType_Float: oVal = iTV.GetFloat(); return true;
		case eZType_Double: oVal = iTV.GetDouble(); return true;
		case eZType_Bool: oVal = iTV.GetBool() ? 1 : 0; return true;
		case eZType_ID: oVal = iTV.GetID(); return true;
		}
	return false;
	}

double sDoubleD(const ZTValue& iTV, double iDefault)
	{
	double result;
	if (sDouble(iTV, result))
		return result;
	return iDefault;
	}

double sDouble(const ZTValue& iTV)
	{ return sDoubleD(iTV, 0); }

// ----------

bool sUDouble(const ZTValue& iTV, double& oVal)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Int8: oVal = uint8(iTV.GetInt8()); return true;
		case eZType_Int16: oVal = uint16(iTV.GetInt16()); return true;
		case eZType_Int32: oVal = uint32(iTV.GetInt32()); return true;
		case eZType_Int64: oVal = uint64(iTV.GetInt64()); return true;
		case eZType_Float: oVal = iTV.GetFloat(); return true;
		case eZType_Double: oVal = iTV.GetDouble(); return true;
		case eZType_Bool: oVal = iTV.GetBool() ? 1 : 0; return true;
		case eZType_ID: oVal = iTV.GetID(); return true;
		}
	return false;
	}

double sUDoubleD(const ZTValue& iTV, double iDefault)
	{
	double result;
	if (sUDouble(iTV, result))
		return result;
	return iDefault;
	}

double sUDouble(const ZTValue& iTV)
	{ return sUDoubleD(iTV, 0); }

// ----------

bool sBool(const ZTValue& iTV, bool& oVal)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Int8: oVal = iTV.GetInt8(); return true;
		case eZType_Int16: oVal = iTV.GetInt16(); return true;
		case eZType_Int32: oVal = iTV.GetInt32(); return true;
		case eZType_Int64: oVal = iTV.GetInt64(); return true;
		case eZType_Float: oVal = iTV.GetFloat(); return true;
		case eZType_Double: oVal = iTV.GetDouble(); return true;
		case eZType_Bool: oVal = iTV.GetBool(); return true;
		case eZType_ID: oVal = iTV.GetID(); return true;
		}
	return false;
	}

bool sBoolD(const ZTValue& iTV, bool iDefault)
	{
	bool result;
	if (sBool(iTV, result))
		return result;
	return iDefault;
	}

bool sBool(const ZTValue& iTV)
	{ return sBoolD(iTV, false); }

// ----------

bool sGetValuei(const ZTuple& iTuple, const string& iNamei, ZTValue& oTV)
	{
	ZTuple::const_iterator i = sIteratorofi(iTuple, iNamei);
	if (i !=  iTuple.end())
		{
		oTV = iTuple.GetValue(i);
		return true;
		}
	return false;
	}

ZTValue sDGetValuei(const ZTuple& iTuple, const string& iNamei, const ZTValue& iDefault)
	{
	ZTValue result;
	if (sGetValuei(iTuple, iNamei, result))
		return result;
	return iDefault;
	}

ZTValue sGetValuei(const ZTuple& iTuple, const string& iNamei)
	{ return sDGetValuei(iTuple, iNamei, ZTValue()); }

ZTuple::const_iterator sIteratorofi(const ZTuple& iTuple, const string& iNamei)
	{
	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		{
		if (ZString::sEquali(iTuple.NameOf(i).AsString(), iNamei))
			return i;
		}
	return iTuple.end();
	}

} // namespace ZUtil_Tuple
