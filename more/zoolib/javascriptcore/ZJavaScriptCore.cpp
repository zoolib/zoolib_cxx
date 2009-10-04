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

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZRef support

template <>
void sRetain_T(JSPropertyNameArrayRef iRef)
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
void sRetain_T(JSStringRef iRef)
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
void sRetain_T(JSClassRef iRef)
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
void sRetain_T(JSGlobalContextRef iRef)
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
void sRetain_T(JSValueRef iRef)
	{
	if (iRef)
		::JSValueProtect(ZJavaScriptCore::sCurrentContext(), iRef);
	}

template <>
void sRelease_T(JSValueRef iRef)
	{
	if (iRef)
		::JSValueUnprotect(ZJavaScriptCore::sCurrentContext(), iRef);
	}

template <>
void sRetain_T(JSObjectRef iRef)
	{
	if (iRef)
		::JSValueProtect(ZJavaScriptCore::sCurrentContext(), iRef);
	}

template <>
void sRelease_T(JSObjectRef iRef)
	{
	if (iRef)
		::JSValueUnprotect(ZJavaScriptCore::sCurrentContext(), iRef);
	}

namespace ZJavaScriptCore {

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::ContextSetter

static ZTSS::Key sContextKey = ZTSS::sCreate();

static JSContextRef spCurrentContext()
	{ return static_cast<JSContextRef>(ZTSS::sGet(sContextKey)); }

JSContextRef sCurrentContext()
	{
	JSContextRef theCR = spCurrentContext();
	ZAssert(theCR);
	return theCR;
	}

ContextSetter::ContextSetter(JSContextRef iJSContextRef)
	{
	fJSContextRef_Prior = spCurrentContext();
	ZTSS::sSet(sContextKey, iJSContextRef);
	}

ContextSetter::~ContextSetter()
	{ ZTSS::sSet(sContextKey, fJSContextRef_Prior); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::Context::Rep

class Context::Rep : public ZRefCountedWithFinalize
	{
public:
	Rep(ZRef<JSGlobalContextRef> iJSGlobalContextRef);
	virtual ~Rep();

	JSGlobalContextRef GetJSGlobalContextRef();

private:
	ZRef<JSGlobalContextRef> fRep;
	};

Context::Rep::Rep(ZRef<JSGlobalContextRef> iJSGlobalContextRef)
:	fRep(iJSGlobalContextRef)
	{}

Context::Rep::~Rep()
	{}

JSGlobalContextRef Context::Rep::GetJSGlobalContextRef()
	{ return fRep.Get(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::Context

Context::Context()
	{}

Context::Context(const Context& iOther)
:	fRep(iOther.fRep)
	{}

Context::~Context()
	{}

Context& Context::operator=(const Context& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Context::Context(JSContextRef iJSContextRef)
:	fRep(new Rep(const_cast<JSGlobalContextRef>(iJSContextRef)))
	{}

Context::Context(JSGlobalContextRef iJSGlobalContextRef)
:	fRep(new Rep(iJSGlobalContextRef))
	{}

Context::operator JSGlobalContextRef() const
	{
	if (fRep)
		return fRep->GetJSGlobalContextRef();
	return nullptr;
	}

bool Context::operator==(const Context& iOther) const
	{ return static_cast<JSGlobalContextRef>(*this) == static_cast<JSGlobalContextRef>(iOther); }

bool Context::operator!=(const Context& iOther) const
	{ return !(*this == iOther); }

ObjectRef Context::GetGlobalObjectRef() const
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
	{
	if (fRep)
		{
		return ZUnicode::sAsUTF8(
			static_cast<const UTF16*>(::JSStringGetCharactersPtr(fRep)),
			::JSStringGetLength(fRep));
		}
	return string8();
	}

string16 String::AsString16() const
	{
	if (fRep)
		{
		return string16(::JSStringGetCharactersPtr(fRep),
			::JSStringGetLength(fRep));
		}
	return string16();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::Value

Value::operator operator_bool_type() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeUndefined != ::JSValueGetType(sCurrentContext(), theRef))
			return operator_bool_generator_type::translate(true);
		}
	return operator_bool_generator_type::translate(false);
	}

Value::operator bool() const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeUndefined != ::JSValueGetType(sCurrentContext(), theRef))
			return true;
		}
	return false;
	}

Value::operator JSValueRef() const
	{ return inherited::Get(); }

JSValueRef& Value::OParam()
	{
	inherited::Clear();
	return inherited::GetPtrRef();
	}

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

Value::Value(FunctionImp* iOther)
:	inherited(iOther->GetJSOR())
	{}

Value::Value(const ZRef<FunctionImp>& iOther)
:	inherited(iOther->GetJSOR())
	{}

Value::Value(JSObjectRef iOther)
:	inherited(iOther)
	{}

Value::Value(JSValueRef iOther)
:	inherited(iOther)
	{}

Value::Value(bool iValue)
:	inherited(::JSValueMakeBoolean(sCurrentContext(), iValue))
	{}

Value::Value(double iValue)
:	inherited(::JSValueMakeNumber(sCurrentContext(), iValue))
	{}

Value::Value(const String& iValue)
:	inherited(::JSValueMakeString(sCurrentContext(), iValue))
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
	inherited::operator=(::JSValueMakeBoolean(sCurrentContext(), iValue));
	return *this;
	}

Value& Value::operator=(double iValue)
	{
	inherited::operator=(::JSValueMakeNumber(sCurrentContext(), iValue));
	return *this;
	}

Value& Value::operator=(const String& iValue)
	{
	inherited::operator=(::JSValueMakeString(sCurrentContext(), iValue));
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
		return Adopt(::JSValueToStringCopy(sCurrentContext(), theRef, nil));

	return String();
	}

template <>
bool Value::QGet_T<bool>(bool& oVal) const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeBoolean == ::JSValueGetType(sCurrentContext(), theRef))
			{
			oVal = ::JSValueToBoolean(sCurrentContext(), theRef);
			return true;
			}
		}
	return false;
	}

template <>
bool Value::QGet_T<double>(double& oVal) const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeNumber == ::JSValueGetType(sCurrentContext(), theRef))
			{
			JSValueRef theEx;
			oVal = ::JSValueToNumber(sCurrentContext(), theRef, &theEx);
			return !theEx;
			}
		}
	return false;
	}

template <>
bool Value::QGet_T<String>(String& oVal) const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (kJSTypeString == ::JSValueGetType(sCurrentContext(), theRef))
			{
			if (ZRef<JSStringRef> theStringRef =
				Adopt(::JSValueToStringCopy(sCurrentContext(), theRef, nil)))
				{
				oVal = theStringRef;
				return true;
				}
			}
		}
	return false;
	}

template <>
bool Value::QGet_T<string16>(string16& oVal) const
	{
	String theString;
	if (this->QGet_T<String>(theString))
		{
		oVal = theString.AsString16();
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<string8>(string8& oVal) const
	{
	String theString;
	if (this->QGet_T<String>(theString))
		{
		oVal = theString.AsString8();
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<ObjectRef>(ObjectRef& oVal) const
	{
	if (JSValueRef theRef = inherited::Get())
		{
		if (::JSValueIsObject(sCurrentContext(), theRef))
			{
			oVal = ObjectRef(const_cast<JSObjectRef>(theRef));
			return true;
			}
		}
	return false;
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

ObjectRef::ObjectRef()
:	inherited(::JSObjectMake(sCurrentContext(), nil, nil))
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

bool ObjectRef::QGet(const string8& iName, Value& oVal) const
	{	
	JSValueRef theEx;
	JSValueRef theResult = ::JSObjectGetProperty(sCurrentContext(),
		inherited::Get(), String(iName), &theEx);
	if (!theEx)
		{
		oVal = Value(theResult);
		return true;
		}
	return false;
	}

bool ObjectRef::QGet(size_t iIndex, Value& oVal) const
	{
	JSValueRef theEx;
	JSValueRef theResult = ::JSObjectGetPropertyAtIndex(sCurrentContext(),
		inherited::Get(), iIndex, &theEx);
	if (!theEx)
		{
		oVal = Value(theResult);
		return true;
		}
	return false;
	}

Value ObjectRef::DGet(const Value& iDefault, const string8& iName) const
	{
	Value result;
	if (this->QGet(iName, result))
		return result;
	return iDefault;
	}

Value ObjectRef::DGet(const Value& iDefault, size_t iIndex) const
	{
	Value result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Value ObjectRef::Get(const string8& iName) const
	{ return this->DGet(Value(), iName); }

Value ObjectRef::Get(size_t iIndex) const
	{ return this->DGet(Value(), iIndex); }

bool ObjectRef::Set(const string8& iName, const Value& iValue)
	{
	JSValueRef theEx;
	::JSObjectSetProperty(sCurrentContext(), inherited::Get(), String(iName), iValue, 0, &theEx);
	return !theEx;
	}

bool ObjectRef::Set(size_t iIndex, const Value& iValue)
	{
	JSValueRef theEx;
	::JSObjectSetPropertyAtIndex(sCurrentContext(), inherited::Get(), iIndex, iValue, &theEx);
	return !theEx;
	}

bool ObjectRef::Erase(const string8& iName)
	{
	return ::JSObjectDeleteProperty(sCurrentContext(),
		inherited::Get(), String(iName), nullptr);
	}

/*
bool ObjectRef::Erase(size_t iIndex)
	{
	}
*/

Value ObjectRef::CallAsFunction(const ObjectRef& iThis,
	const Value* iArgs, size_t iArgCount,
	Value& oEx)
	{
	return ::JSObjectCallAsFunction(sCurrentContext(),
		inherited::Get(),
		iThis,
		iArgCount, reinterpret_cast<const JSValueRef*>(iArgs),
		&oEx.OParam());
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZJavaScriptCore::FunctionImp

FunctionImp::FunctionImp()
:	fJSObjectRef(nil)
	{
	JSObjectRef theOR = ::JSObjectMake(sCurrentContext(), spGetJSClass(), this);
	ZAssert(theOR == fJSObjectRef);
	}

FunctionImp::~FunctionImp()
	{
	ZAssert(!fJSObjectRef);
	}

void FunctionImp::Finalize()
	{
	this->FinalizationComplete();
	this->pTossIfAppropriate();
	}

void FunctionImp::pTossIfAppropriate()
	{
	if (this->GetRefCount() == 0 && !fJSObjectRef)
		delete this;
	}

JSObjectRef FunctionImp::GetJSOR()
	{ return fJSObjectRef; }

void FunctionImp::Initialize(JSContextRef iJSContextRef, JSObjectRef iJSObjectRef)
	{
	ZAssert(!fJSObjectRef);
	fJSObjectRef = iJSObjectRef;
	}

void FunctionImp::Finalize(JSObjectRef iJSObjectRef)
	{
	ZAssert(fJSObjectRef == iJSObjectRef);
	fJSObjectRef = nil;
	this->pTossIfAppropriate();
	}

Value FunctionImp::CallAsFunction(const ObjectRef& iFunction, const ObjectRef& iThis,
	const Value* iArgs, size_t iArgCount, Value& oEx)
	{
	return Value();
	}

void FunctionImp::spInitialize(JSContextRef ctx, JSObjectRef object)
	{
	ContextSetter cs(ctx);
	if (ZRef<FunctionImp> fun = spFromRef(object))
		fun->Initialize(ctx, object);
	}

void FunctionImp::spFinalize(JSObjectRef object)
	{
	// Hmm -- pretend there's no current context.
	ContextSetter cs(nil);
	if (ZRef<FunctionImp> fun = spFromRef(object))
		fun->Finalize(object);
	}

JSValueRef FunctionImp::spCallAsFunction(JSContextRef ctx,
	JSObjectRef function, JSObjectRef thisObject,
	size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
	{
	ContextSetter cs(ctx);
	if (ZRef<FunctionImp> fun = spFromRef(function))
		{
		Value ex;
		Value result = fun->CallAsFunction(Context(ctx),
			ObjectRef(function), ObjectRef(thisObject),
			reinterpret_cast<const Value*>(arguments), argumentCount,
			ex);

		if (exception)
			*exception = ex;
		}
	return nullptr;
	}

ZRef<FunctionImp> FunctionImp::spFromRef(JSObjectRef object)
	{
	if (object)
		return static_cast<FunctionImp*>(::JSObjectGetPrivate(object));
	return ZRef<FunctionImp>();
	}

JSClassRef FunctionImp::spGetJSClass()
	{
	static JSClassDefinition classDefinition =
		{
		0, kJSClassAttributeNone,
		"FunctionImp", 0,
		0, 0,
		spInitialize, spFinalize,
		0, 0, 0, 0, 0,
		spCallAsFunction,
		0,
		0, 0
		};

	static JSClassRef spClassRef = ::JSClassCreate(&classDefinition);

	return spClassRef;	
	}

} // namespace ZJavaScriptCore

NAMESPACE_ZOOLIB_END
