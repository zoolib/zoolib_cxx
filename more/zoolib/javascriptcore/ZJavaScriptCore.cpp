/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/javascriptcore/ZJavaScriptCore.h"

#include "zoolib/ZThread.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Any.h"
#include "zoolib/ZVal_Any.h"

namespace ZooLib {

using std::map;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZRef support

template <>
void sRetain_T(JSPropertyNameArrayRef& iRef)
	{
	if (iRef)
		::JSPropertyNameArrayRetain(iRef);
	}

template <>
void sRelease_T(JSPropertyNameArrayRef iRef)
	{
	if (iRef)
		::JSPropertyNameArrayRelease(iRef);
	}

template <>
void sRetain_T(JSStringRef& iRef)
	{
	if (iRef)
		::JSStringRetain(iRef);
	}

template <>
void sRelease_T(JSStringRef iRef)
	{
	if (iRef)
		::JSStringRelease(iRef);
	}

template <>
void sRetain_T(JSClassRef& iRef)
	{
	if (iRef)
		::JSClassRetain(iRef);
	}

template <>
void sRelease_T(JSClassRef iRef)
	{
	if (iRef)
		::JSClassRelease(iRef);
	}

template <>
void sRetain_T(JSGlobalContextRef& iRef)
	{
	if (iRef)
		::JSGlobalContextRetain(iRef);
	}

template <>
void sRelease_T(JSGlobalContextRef iRef)
	{
	if (iRef)
		::JSGlobalContextRelease(iRef);
	}

template <>
void sRetain_T(JSValueRef& iRef)
	{
	if (iRef)
		::JSValueProtect(ZJavaScriptCore::sCurrentContextRef(), iRef);
	}

template <>
void sRelease_T(JSValueRef iRef)
	{
	if (iRef)
		::JSValueUnprotect(ZJavaScriptCore::sCurrentContextRef(), iRef);
	}

template <>
void sRetain_T(JSObjectRef& iRef)
	{
	if (iRef)
		::JSValueProtect(ZJavaScriptCore::sCurrentContextRef(), iRef);
	}

template <>
void sRelease_T(JSObjectRef iRef)
	{
	if (iRef)
		::JSValueUnprotect(ZJavaScriptCore::sCurrentContextRef(), iRef);
	}

namespace ZJavaScriptCore {

static string8 spAsString8(JSStringRef iRef)
	{
	if (iRef)
		{
		return ZUnicode::sAsUTF8(
			static_cast<const UTF16*>(::JSStringGetCharactersPtr(iRef)),
			::JSStringGetLength(iRef));
		}
	return string8();
	}

static string16 spAsString16(JSStringRef iRef)
	{
	if (iRef)
		{
		return string16(
			::JSStringGetCharactersPtr(iRef),
			::JSStringGetLength(iRef));
		}
	return string16();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextRefSetter

static ZTSS::Key spContextRefKey = ZTSS::sCreate();

static JSContextRef spCurrentContextRef()
	{ return static_cast<JSContextRef>(ZTSS::sGet(spContextRefKey)); }

JSContextRef sCurrentContextRef()
	{
	JSContextRef theCR = spCurrentContextRef();
	ZAssert(theCR);
	return theCR;
	}

ContextRefSetter::ContextRefSetter(JSContextRef iJSContextRef)
	{
	fJSContextRef_Prior = spCurrentContextRef();
	ZTSS::sSet(spContextRefKey, iJSContextRef);
	}

ContextRefSetter::~ContextRefSetter()
	{ ZTSS::sSet(spContextRefKey, fJSContextRef_Prior); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextRef::Rep

class ContextRef::Rep : public ZCounted
	{
public:
	Rep(ZRef<JSGlobalContextRef> iJSGlobalContextRef);
	virtual ~Rep();

	JSGlobalContextRef GetJSGlobalContextRef();

private:
	ZRef<JSGlobalContextRef> fRep;
	};

ContextRef::Rep::Rep(ZRef<JSGlobalContextRef> iJSGlobalContextRef)
:	fRep(iJSGlobalContextRef)
	{}

ContextRef::Rep::~Rep()
	{}

JSGlobalContextRef ContextRef::Rep::GetJSGlobalContextRef()
	{ return fRep.Get(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextRef

ContextRef::ContextRef()
	{}

ContextRef::ContextRef(const ContextRef& iOther)
:	fRep(iOther.fRep)
	{}

ContextRef::~ContextRef()
	{}

ContextRef& ContextRef::operator=(const ContextRef& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ContextRef::ContextRef(ZRef<JSContextRef> iJSContextRef)
:	fRep(new Rep(const_cast<JSGlobalContextRef>(iJSContextRef.Get())))
	{}

ContextRef::ContextRef(ZRef<JSGlobalContextRef> iJSGlobalContextRef)
:	fRep(new Rep(iJSGlobalContextRef))
	{}

ContextRef::operator JSGlobalContextRef() const
	{
	if (fRep)
		return fRep->GetJSGlobalContextRef();
	return nullptr;
	}

bool ContextRef::operator==(const ContextRef& iOther) const
	{ return static_cast<JSGlobalContextRef>(*this) == static_cast<JSGlobalContextRef>(iOther); }

bool ContextRef::operator!=(const ContextRef& iOther) const
	{ return !(*this == iOther); }

ObjectRef ContextRef::GetGlobalObjectRef() const
	{ return ObjectRef(::JSContextGetGlobalObject(*this)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::String

String::String()
	{}

String::String(const String& iOther)
:	fRep(iOther.fRep)
	{}

String::~String()
	{}

String& String::operator=(const String& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

String::String(JSStringRef iJSStringRef)
:	fRep(iJSStringRef)
	{}

String::String(const Adopt_T<JSStringRef>& iJSStringRef)
:	fRep(iJSStringRef)
	{}

String::String(const ZRef<JSStringRef>& iJSStringRef)
:	fRep(iJSStringRef)
	{}

String::String(const string8& iString8)
:	fRep(Adopt(::JSStringCreateWithUTF8CString(iString8.c_str())))
	{}

String::String(const string16& iString16)
:	fRep(Adopt(::JSStringCreateWithCharacters(iString16.c_str(), iString16.length())))
	{}

String::operator JSStringRef() const
	{ return fRep.Get(); }

string8 String::AsString8() const
	{ return spAsString8(fRep); }

string16 String::AsString16() const
	{ return spAsString16(fRep); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::Value

bool Value::sQFromAny(const ZAny& iAny, Value& oVal)
	{
	int64 asInt64;
	double asDouble;
	if (false)
		{}
	else if (! iAny)
		{
		oVal = Value();
		}
	else if (sQCoerceInt(iAny, asInt64))
		{
		oVal = Value(double(asInt64));
		}
	else if (sQCoerceReal(iAny, asDouble))
		{
		oVal = Value(asDouble);
		}
	else if (const string* theValue = iAny.PGet<string>())
		{
		oVal = Value(*theValue);
		}
	else if (const ZType* theValue = iAny.PGet<ZType>())
		{
		oVal = Value(double(*theValue));
		}
	else if (const bool* theValue = iAny.PGet<bool>())
		{
		oVal = Value(*theValue);
		}
	else if (const ZTime* theValue = iAny.PGet<ZTime>())
		{
		oVal = Value(double(theValue->fVal));
		}
#if 0
	else if (const vector<ZAny>* theValue = iAny.PGet<vector<ZAny> >())
		{
		ZSeq_ZooLib theList;
		for (vector<ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			ZVal_ZooLib local;
			if (sQFromAny(*i, local))
				theList.Append(local);
			else
				theList.Append(ZVal_ZooLib());
			}
		oVal = theList;
		}
	else if (const ZSeq_Any* theValue = iAny.PGet<ZSeq_Any>())
		{
		ZSeq_ZooLib theList;
		for (int x = 0, count = theValue->Count(); x < count; ++x)
			{
			ZVal_ZooLib local;
			if (sQFromAny(theValue->Get(x), local))
				theList.Append(local);
			else
				theList.Append(ZVal_ZooLib());
			}
		oVal = theList;
		}
#endif
	else if (const map<string, ZAny>* theValue = iAny.PGet<map<string, ZAny> >())
		{
		ObjectRef theMap;
		for (map<string, ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			Value local;
			if (sQFromAny((*i).second, local))
				theMap.Set((*i).first, local);
			}
		oVal = theMap;
		}
	else if (const ZMap_Any* theValue = iAny.PGet<ZMap_Any>())
		{
		ObjectRef theMap;
		for (ZMap_Any::Index_t i = theValue->Begin(), end = theValue->End();
			i != end; ++i)
			{
			Value local;
			if (sQFromAny(theValue->Get(i), local))
				theMap.Set(theValue->NameOf(i), local);
			}
		oVal = theMap;
		}
	else
		{
		return false;
		}
	return true;
	}

Value Value::sDFromAny(const Value& iDefault, const ZAny& iAny)
	{
	Value result;
	if (sQFromAny(iAny, result))
		return result;
	return iDefault;
	}

Value Value::sFromAny(const ZAny& iAny)
	{ return sDFromAny(Value(), iAny); }

ZAny Value::AsAny() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		switch (::JSValueGetType(sCurrentContextRef(), theRef))
			{
			case kJSTypeBoolean:
				{
				return ZAny(bool(::JSValueToBoolean(sCurrentContextRef(), theRef)));
				}
			case kJSTypeNumber:
				{
				return ZAny(double(::JSValueToNumber(sCurrentContextRef(), theRef, nullptr)));
				}
			case kJSTypeString:
				{
				ZRef<JSStringRef> theStringRef =
					Adopt(::JSValueToStringCopy(sCurrentContextRef(), theRef, nullptr));
				return ZAny(spAsString8(theStringRef));
				}
			case kJSTypeObject:
				{
				return ObjectRef(const_cast<JSObjectRef>(theRef)).AsAny();
				}
			}
		}
	return ZAny();
	}

Value::operator operator_bool_type() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeUndefined != ::JSValueGetType(sCurrentContextRef(), theRef))
			return operator_bool_generator_type::translate(true);
		}
	return operator_bool_generator_type::translate(false);
	}

Value::operator bool() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeUndefined != ::JSValueGetType(sCurrentContextRef(), theRef))
			return true;
		}
	return false;
	}

Value::operator JSValueRef() const
	{ return inherited::Get(); }

JSValueRef& Value::OParam()
	{ return inherited::OParam(); }

Value::Value()
	{}

Value::Value(const Value& iOther)
:	inherited(iOther)
	{}

Value::~Value()
	{}

Value& Value::operator=(const Value& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Value::Value(ObjectImp* iOther)
:	inherited(iOther->GetJSObjectRef())
	{}

Value::Value(const ZRef<ObjectImp>& iOther)
:	inherited(iOther->GetJSObjectRef())
	{}

Value::Value(JSObjectRef iOther)
:	inherited(iOther)
	{}

Value::Value(JSValueRef iOther)
:	inherited(iOther)
	{}

Value::Value(bool iValue)
:	inherited(::JSValueMakeBoolean(sCurrentContextRef(), iValue))
	{}

Value::Value(double iValue)
:	inherited(::JSValueMakeNumber(sCurrentContextRef(), iValue))
	{}

Value::Value(const String& iValue)
:	inherited(::JSValueMakeString(sCurrentContextRef(), iValue))
	{}

Value::Value(const ObjectRef& iValue)
:	inherited(iValue)
	{}

Value& Value::operator=(JSObjectRef iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Value& Value::operator=(JSValueRef iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Value& Value::operator=(bool iValue)
	{
	inherited::operator=(::JSValueMakeBoolean(sCurrentContextRef(), iValue));
	return *this;
	}

Value& Value::operator=(double iValue)
	{
	inherited::operator=(::JSValueMakeNumber(sCurrentContextRef(), iValue));
	return *this;
	}

Value& Value::operator=(const String& iValue)
	{
	inherited::operator=(::JSValueMakeString(sCurrentContextRef(), iValue));
	return *this;
	}

Value& Value::operator=(const ObjectRef& iValue)
	{
	inherited::operator=(iValue);
	return *this;
	}

String Value::ToString() const
	{
	if (JSValueRef theRef = inherited::Get())
		return Adopt(::JSValueToStringCopy(sCurrentContextRef(), theRef, nullptr));

	return String();
	}

template <>
ZQ<bool> Value::QGet<bool>() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeBoolean == ::JSValueGetType(sCurrentContextRef(), theRef))
			return bool(::JSValueToBoolean(sCurrentContextRef(), theRef));
		}
	return null;
	}

template <>
ZQ<double> Value::QGet<double>() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeNumber == ::JSValueGetType(sCurrentContextRef(), theRef))
			{
			JSValueRef theEx = nullptr;
			double theValue = ::JSValueToNumber(sCurrentContextRef(), theRef, &theEx);
			if (!theEx)
				return theValue;
			}
		}
	return null;
	}

template <>
ZQ<String> Value::QGet<String>() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeString == ::JSValueGetType(sCurrentContextRef(), theRef))
			{
			if (ZRef<JSStringRef> theStringRef =
				Adopt(::JSValueToStringCopy(sCurrentContextRef(), theRef, nullptr)))
				return theStringRef;
			}
		}
	return null;
	}

template <>
ZQ<string16> Value::QGet<string16>() const
	{
	if (ZQ<String> theQ = this->QGet<String>())
		return theQ.Get().AsString16();
	return null;
	}

template <>
ZQ<string8> Value::QGet<string8>() const
	{
	if (ZQ<String> theQ = this->QGet<String>())
		return theQ.Get().AsString8();
	return null;
	}

template <>
ZQ<ObjectRef> Value::QGet<ObjectRef>() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (::JSValueIsObject(sCurrentContextRef(), theRef))
			return ObjectRef(const_cast<JSObjectRef>(theRef));
		}
	return null;
	}

ZMACRO_ZValAccessors_Def_GetP(,Value, Bool, bool)
ZMACRO_ZValAccessors_Def_GetP(,Value, Double, double)
ZMACRO_ZValAccessors_Def_GetP(,Value, String, String)
ZMACRO_ZValAccessors_Def_GetP(,Value, String16, string16)
ZMACRO_ZValAccessors_Def_GetP(,Value, String8, string8)
ZMACRO_ZValAccessors_Def_GetP(,Value, ObjectRef, ObjectRef)

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ObjectRef

ZAny ObjectRef::AsAny() const
	{
	if (JSObjectRef thisRef = inherited::Get())
		{
		if (ZQ<double> theQ = this->Get("length").QGetDouble())
			{
			ZSeq_Any result;
			for (size_t x = 0; x < theQ.Get(); ++x)
				result.Append(this->Get(x).AsAny());
			return ZAny(result);
			}
		else
			{
			ZMap_Any result;
			ZRef<JSPropertyNameArrayRef> theNames =
				Adopt(::JSObjectCopyPropertyNames(sCurrentContextRef(), thisRef));
			for (size_t x = 0, count = ::JSPropertyNameArrayGetCount(theNames); x < count; ++x)
				{
				const string8 theName =
					spAsString8(::JSPropertyNameArrayGetNameAtIndex(theNames, x));
				result.Set(theName, this->Get(theName).AsAny());
				}
			return ZAny(result);
			}
		}
	return ZAny();
	}

ObjectRef::ObjectRef()
:	inherited(::JSObjectMake(sCurrentContextRef(), nullptr, nullptr))
	{}

ObjectRef::ObjectRef(const ObjectRef& iOther)
:	inherited(iOther)
	{}

ObjectRef::~ObjectRef()
	{}

ObjectRef& ObjectRef::operator=(const ObjectRef& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ObjectRef::ObjectRef(const ZRef<JSObjectRef>& iOther)
:	inherited(iOther)
	{}

ObjectRef& ObjectRef::operator=(const ZRef<JSObjectRef>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZQ<Value> ObjectRef::QGet(const string8& iName) const
	{
	JSValueRef theEx = nullptr;
	JSValueRef theResult = ::JSObjectGetProperty(sCurrentContextRef(),
		inherited::Get(), String(iName), &theEx);
	if (!theEx)
		return Value(theResult);
	return null;
	}

Value ObjectRef::DGet(const Value& iDefault, const string8& iName) const
	{
	if (ZQ<Value> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

Value ObjectRef::Get(const string8& iName) const
	{ return this->DGet(Value(), iName); }

bool ObjectRef::Set(const string8& iName, const Value& iValue)
	{
	JSValueRef theEx = nullptr;
	::JSObjectSetProperty(sCurrentContextRef(),
		inherited::Get(), String(iName), iValue, 0, &theEx);
	return !theEx;
	}

bool ObjectRef::Erase(const string8& iName)
	{
	return ::JSObjectDeleteProperty(sCurrentContextRef(),
		inherited::Get(), String(iName), nullptr);
	}

bool ObjectRef::IsSeq() const
	{ return this->QGet("length"); }

ZQ<Value> ObjectRef::QGet(size_t iIndex) const
	{
	JSValueRef theEx = nullptr;
	JSValueRef theResult = ::JSObjectGetPropertyAtIndex(sCurrentContextRef(),
		inherited::Get(), iIndex, &theEx);
	if (!theEx)
		return Value(theResult);
	return null;
	}

Value ObjectRef::DGet(const Value& iDefault, size_t iIndex) const
	{
	if (ZQ<Value> theQ = this->QGet(iIndex))
		return theQ.Get();
	return iDefault;
	}

Value ObjectRef::Get(size_t iIndex) const
	{ return this->DGet(Value(), iIndex); }

bool ObjectRef::Set(size_t iIndex, const Value& iValue)
	{
	JSValueRef theEx = nullptr;
	::JSObjectSetPropertyAtIndex(sCurrentContextRef(),
		inherited::Get(), iIndex, iValue, &theEx);
	return !theEx;
	}

bool ObjectRef::Erase(size_t iIndex)
	{
	if (ObjectRef splice = this->Get("splice").GetObjectRef())
		{
		Value args[] = { Value(double(iIndex)), Value(double(1)) };
		Value ex;
		splice.CallAsFunction(*this, args, countof(args), ex);
		return !ex;
		}
	return false;
	}

Value ObjectRef::CallAsFunction(const ObjectRef& iThis,
	const Value* iArgs, size_t iArgCount,
	Value& oEx)
	{
	return ::JSObjectCallAsFunction(sCurrentContextRef(),
		inherited::Get(),
		iThis,
		iArgCount, reinterpret_cast<const JSValueRef*>(iArgs),
		&oEx.OParam());
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ObjectImp

ObjectImp::ObjectImp()
:	fJSObjectRef(nullptr)
	{
	JSObjectRef theOR = ::JSObjectMake(sCurrentContextRef(), spGetJSClass(), this);
	ZAssert(theOR == fJSObjectRef);
	}

ObjectImp::~ObjectImp()
	{
	ZAssert(!fJSObjectRef);
	}

void ObjectImp::Finalize()
	{
	this->FinishFinalize();
	this->pTossIfAppropriate();
	}

void ObjectImp::pTossIfAppropriate()
	{
	if (!fJSObjectRef && !this->IsReferenced())
		delete this;
	}

JSObjectRef ObjectImp::GetJSObjectRef()
	{ return fJSObjectRef; }

void ObjectImp::Initialize(JSContextRef iJSContextRef, JSObjectRef iJSObjectRef)
	{
	ZAssert(!fJSObjectRef);
	fJSObjectRef = iJSObjectRef;
	}

void ObjectImp::Finalize(JSObjectRef iJSObjectRef)
	{
	ZAssert(fJSObjectRef == iJSObjectRef);
	fJSObjectRef = nullptr;
	this->pTossIfAppropriate();
	}

bool ObjectImp::HasProperty(const ObjectRef& iThis, const String& iPropName)
	{ return false; }

Value ObjectImp::GetProperty(
	const ObjectRef& iThis, const String& iPropName, Value& oEx)
	{
	return Value();
	}

bool ObjectImp::SetProperty(
	const ObjectRef& iThis, const String& iPropName, const Value& iVal, Value& oEx)
	{ return false; }

void ObjectImp::GetPropertyNames(
	const ObjectRef& iThis, JSPropertyNameAccumulatorRef propertyNames)
	{}

Value ObjectImp::CallAsFunction(const ObjectRef& iFunction, const ObjectRef& iThis,
	const Value* iArgs, size_t iArgCount, Value& oEx)
	{
	return Value();
	}

void ObjectImp::spInitialize(JSContextRef ctx, JSObjectRef object)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		fun->Initialize(ctx, object);
	}

void ObjectImp::spFinalize(JSObjectRef object)
	{
	// Hmm -- pretend there's no current context.
	ContextRefSetter cs(nullptr);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		fun->Finalize(object);
	}

bool ObjectImp::spHasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		return fun->HasProperty(ObjectRef(object), String(propertyName));

	return false;
	}

JSValueRef ObjectImp::spGetProperty(JSContextRef ctx,
	JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		{
		Value ex;
		Value result = fun->GetProperty(ObjectRef(object), String(propertyName), ex);

		if (exception)
			*exception = ex;

		return result;
		}

	return Value();
	}

bool ObjectImp::spSetProperty(JSContextRef ctx,
	JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		{
		Value ex;
		bool result = fun->SetProperty(ObjectRef(object), String(propertyName), value, ex);

		if (exception)
			*exception = ex;

		return result;
		}

	return false;
	}

void ObjectImp::spGetPropertyNames(JSContextRef ctx,
	JSObjectRef object, JSPropertyNameAccumulatorRef propertyNames)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(object))
		fun->GetPropertyNames(ObjectRef(object), propertyNames);
	}

JSValueRef ObjectImp::spCallAsFunction(JSContextRef ctx,
	JSObjectRef function, JSObjectRef thisObject,
	size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
	{
	ContextRefSetter cs(ctx);
	if (ZRef<ObjectImp> fun = spFromRef(function))
		{
		Value ex;
		Value result = fun->CallAsFunction(ObjectRef(function), ObjectRef(thisObject),
			reinterpret_cast<const Value*>(arguments), argumentCount,
			ex);

		if (exception)
			*exception = ex;

		return result;
		}
	return nullptr;
	}

ZRef<ObjectImp> ObjectImp::spFromRef(JSObjectRef object)
	{
	if (object)
		return static_cast<ObjectImp*>(::JSObjectGetPrivate(object));
	return null;
	}

JSClassRef ObjectImp::spGetJSClass()
	{
	static JSClassDefinition classDefinition =
		{
		0, kJSClassAttributeNone,
		"ObjectImp", 0,
		0, 0,
		spInitialize, spFinalize,
		spHasProperty,
		spGetProperty,
		spSetProperty,
		0,
		spGetPropertyNames,
		spCallAsFunction,
		0,
		0, 0
		};

	static JSClassRef spClassRef = ::JSClassCreate(&classDefinition);

	return spClassRef;
	}

} // namespace ZJavaScriptCore

} // namespace ZooLib
