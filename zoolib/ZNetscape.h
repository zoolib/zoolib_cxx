/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZNetscape__
#define __ZNetscape__ 1
#include "zconfig.h"

#include "ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Netscape)

#include "zoolib/ZCompat_npapi.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZDebug.h" // For ZAssert
#include "zoolib/ZNetscape_Macros.h"
#include "zoolib/ZRefCount.h"
#include "zoolib/ZUtil_STL.h"

#include <string>
#include <vector>


NAMESPACE_ZOOLIB_BEGIN

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * NPStream_Z

class NPStream_Z : public NPStream
	{
public:
	#if NP_VERSION_MINOR < NPVERS_HAS_RESPONSE_HEADERS
		const char* headers;
	#endif
	};

// =================================================================================================
#pragma mark -
#pragma mark * NPClass_Z

class NPClass_Z : public NPClass
	{
	NPClass_Z();
public:
	NPClass_Z(
		NPAllocateFunctionPtr allocate,
		NPDeallocateFunctionPtr deallocate,
		NPInvalidateFunctionPtr invalidate,
		NPHasMethodFunctionPtr hasMethod,
		NPInvokeFunctionPtr invoke,
		NPInvokeDefaultFunctionPtr invokeDefault,
		NPHasPropertyFunctionPtr hasProperty,
		NPGetPropertyFunctionPtr getProperty,
		NPSetPropertyFunctionPtr setProperty,
		NPRemovePropertyFunctionPtr removeProperty,
		NPEnumerationFunctionPtr enumerate);

	#if NP_CLASS_STRUCT_VERSION < 2
		NPEnumerationFunctionPtr enumerate;
	#endif
	};

// =================================================================================================
#pragma mark -
#pragma mark * NPNetscapeFuncs_Z

class NPNetscapeFuncs_Z : public NPNetscapeFuncs
	{
public:
	#if NP_VERSION_MINOR < NPVERS_HAS_POPUPS_ENABLED_STATE
		NPN_PushPopupsEnabledStateProcPtr pushpopupsenabledstate;
		NPN_PopPopupsEnabledStateProcPtr poppopupsenabledstate;
	#endif

	#if NP_VERSION_MINOR < NPVERS_HAS_NPOBJECT_ENUM
		NPN_EnumerateProcPtr enumerate;
	#endif

	#if NP_VERSION_MINOR < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL
		NPN_PluginThreadAsyncCallProcPtr pluginthreadasynccall;
		NPN_ConstructProcPtr construct;
	#endif

	#if 0
		NPN_ScheduleTimerProcPtr scheduletimer;
		NPN_UnscheduleTimerProcPtr unscheduletimer;
		NPN_PopUpContextMenuProcPtr popupcontextmenu;
	#endif
	};

// =================================================================================================
#pragma mark -
#pragma mark * sAsString

std::string sAsString(NPNVariable iVar);
std::string sAsString(NPPVariable iVar);

// =================================================================================================
#pragma mark -
#pragma mark * NPVariant_T

template <class T>
void spRelease_T(T&);

template <class T>
class NPVariant_T : public NPVariant
	{
private:
	void pCopyFrom(const NPVariant& iOther)
		{
		switch (iOther.type)
			{
			case NPVariantType_Void:
			case NPVariantType_Null:
				break;
			case NPVariantType_Bool:
				{
				value.boolValue = iOther.value.boolValue;
				break;
				}
			case NPVariantType_Int32:
				{
				value.intValue = iOther.value.intValue;
				break;
				}
			case NPVariantType_Double:
				{
				value.doubleValue = iOther.value.doubleValue;
				break;
				}
			case NPVariantType_String:
				{
				this->pSetString(
					sNPStringCharsConst(iOther.value.stringValue),
					sNPStringLengthConst(iOther.value.stringValue));
				break;
				}
			case NPVariantType_Object:
				{
				value.objectValue = iOther.value.objectValue;
				static_cast<T*>(value.objectValue)->Retain();
				break;
				}
			}
		type = iOther.type;		
		}

	void pRelease()
		{ spRelease_T(*this); }

public:
    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES_T(NPVariant_T<T>,
    	operator_bool_generator_type, operator_bool_type);

	operator operator_bool_type() const
		{ return operator_bool_generator_type::translate(type != NPVariantType_Void); }

	typedef T Object_t;

	NPVariant_T()
		{ type = NPVariantType_Void; }

	NPVariant_T(const NPVariant& iOther)
		{
		ZAssert(this != &iOther);
		this->pCopyFrom(iOther);
		}

	NPVariant_T(const NPVariant_T& iOther)
		{
		ZAssert(this != &iOther);
		this->pCopyFrom(iOther);
		}

	~NPVariant_T()
		{ this->pRelease(); }

	NPVariant_T& operator=(const NPVariant& iOther)
		{
		if (this != &iOther)
			{
			this->pRelease();
			this->pCopyFrom(iOther);
			}
		return *this;
		}

	NPVariant_T& operator=(const NPVariant_T& iOther)
		{
		if (this != &iOther)
			{
			this->pRelease();
			this->pCopyFrom(iOther);
			}
		return *this;
		}

	NPVariant_T(bool iValue)
		{
		type = NPVariantType_Bool;
		value.boolValue = iValue;
		}

	NPVariant_T(int32 iValue)
		{
		type = NPVariantType_Int32;
		value.intValue = iValue;
		}

	NPVariant_T(double iValue)
		{
		type = NPVariantType_Double;
		value.doubleValue = iValue;
		}

	NPVariant_T(const std::string& iValue)
		{
		this->pSetString(iValue);
		type = NPVariantType_String;
		}

	NPVariant_T(T* iValue)
		{
		type = NPVariantType_Void;
		this->SetObject(iValue);
		}

	template <class O>
	NPVariant_T(const ZRef<O>& iValue)
		{
		type = NPVariantType_Void;
		this->SetObject(iValue.GetObject());
		}

	NPVariant_T& operator=(bool iValue)
		{
		this->SetBool(iValue);
		return *this;
		}

	NPVariant_T& operator=(int32 iValue)
		{
		this->SetInt32(iValue);
		return *this;
		}

	NPVariant_T& operator=(double iValue)
		{
		this->SetDouble(iValue);
		return *this;
		}

	NPVariant_T& operator=(const std::string& iValue)
		{
		this->SetString(iValue);
		return *this;
		}

	NPVariant_T& operator=(T* iValue)
		{
		this->SetObject(iValue);
		return *this;
		}

	template <class O>
	NPVariant_T& operator=(const ZRef<O>& iValue)
		{
		this->SetObject(iValue.GetObject());
		return *this;
		}

	bool IsVoid() const
		{ return type == NPVariantType_Void; }

	bool IsNull() const
		{ return type == NPVariantType_Null; }

	bool IsBool() const
		{ return type == NPVariantType_Bool; }

	bool IsInt32() const
		{ return type == NPVariantType_Int32; }

	bool IsDouble() const
		{ return type == NPVariantType_Double; }

	bool IsString() const
		{ return type == NPVariantType_String; }

	bool IsObject() const
		{ return type == NPVariantType_Object; }

	void SetVoid()
		{
		this->pRelease();
		type = NPVariantType_Void;
		}

	void SetNull()
		{
		this->pRelease();
		type = NPVariantType_Null;
		}

	void SetBool(bool iValue)
		{
		this->pRelease();
		value.boolValue = iValue;
		type = NPVariantType_Bool;
		}

	void SetInt32(int32 iValue)
		{
		this->pRelease();
		value.intValue = iValue;
		type = NPVariantType_Int32;
		}

	void SetDouble(double iValue)
		{
		this->pRelease();
		value.doubleValue = iValue;
		type = NPVariantType_Double;
		}

	void SetString(const std::string& iValue)
		{
		this->pRelease();
		this->pSetString(iValue);
		type = NPVariantType_String;
		}

	bool GetBool() const
		{ return this->DGetBool(false); }

	bool GetBool(bool& oValue) const
		{
		if (type != NPVariantType_Bool)
			return false;
		oValue = value.boolValue;
		return true;
		}

	bool DGetBool(bool iDefault) const
		{
		if (type == NPVariantType_Bool)
			return value.boolValue;
		return iDefault;
		}

	int32 GetInt32() const
		{ return this->DGetInt32(0); }

	bool GetInt32(int32& oValue) const
		{
		if (type != NPVariantType_Int32)
			return false;
		oValue = value.intValue;
		return true;
		}

	int32 DGetInt32(int32 iDefault) const
		{
		if (type == NPVariantType_Int32)
			return value.intValue;
		return iDefault;
		}

	double GetDouble() const
		{ return this->DGetDouble(0); }
		
	bool GetDouble(double& oValue) const
		{
		if (type != NPVariantType_Double)
			return false;
		oValue = value.doubleValue;
		return true;
		}

	double DGetDouble(double iDefault) const
		{
		if (type == NPVariantType_Double)
			return value.doubleValue;
		return iDefault;
		}

	std::string GetString() const
		{ return this->DGetString(std::string()); }

	bool GetString(std::string& oValue) const
		{
		if (type != NPVariantType_String)
			return false;

		oValue = std::string(
			sNPStringCharsConst(value.stringValue),
			sNPStringLengthConst(value.stringValue));
		return true;
		}

	std::string DGetString(const std::string& iDefault) const
		{
		if (type != NPVariantType_String)
			return iDefault;

		return std::string(
			sNPStringCharsConst(value.stringValue),
			sNPStringLengthConst(value.stringValue));
		}

	T* CopyObject() const
		{ return this->DCopyObject(nil); }

	bool CopyObject(T*& oValue) const
		{
		if (type != NPVariantType_Object)
			return false;
		oValue = static_cast<T*>(value.objectValue);

		oValue->Retain();
		return true;	
		}

	T* DCopyObject(T* iDefault) const
		{
		T* result = iDefault;
		if (type == NPVariantType_Object)
			result = static_cast<T*>(value.objectValue);

		result->Retain();
		return result;
		}

	T* GetObject() const
		{ return this->DGetObject(nil); }

	bool GetObject(T*& oValue) const
		{
		if (type != NPVariantType_Object)
			return false;
		oValue = static_cast<T*>(value.objectValue);
		return true;	
		}

	T* DGetObject(T* iDefault) const
		{
		if (type == NPVariantType_Object)
			return static_cast<T*>(value.objectValue);

		return iDefault;
		}

	void SetObject(T* iValue)
		{
		this->pRelease();
		value.objectValue = iValue;
		iValue->Retain();
		type = NPVariantType_Object;	
		}

private:
	void pSetString(const char* iChars, size_t iLength)
		{
		sNPStringLength(value.stringValue) = iLength;
		sNPStringChars(value.stringValue) = static_cast<char*>(malloc(iLength));
		strncpy(sNPStringChars(value.stringValue), iChars, iLength);
		}

	void pSetString(const std::string& iString)
		{
		if (size_t theLength = iString.length())
			this->pSetString(iString.data(), theLength);
		else
			this->pSetString(nil, 0);
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Object_T

template <class Variant_t>
class Object_T : public Variant_t::Object_t
	{
public:
	typedef typename Variant_t::Object_t Object_t;

protected:
	Object_T()
		{
		this->_class = &sNPClass;
		this->referenceCount = 0;
		}

	virtual ~Object_T()
		{}

	virtual void Imp_Invalidate()
		{}

	virtual bool Imp_HasMethod(const std::string& iName)
		{ return false; }

	virtual bool Imp_Invoke(
		const std::string& iName, const Variant_t* iArgs, size_t iCount, Variant_t& oResult)
		{ return false; }

	virtual bool Imp_InvokeDefault(const Variant_t* iArgs, size_t iCount, Variant_t& oResult)
		{ return false; }

	virtual bool Imp_HasProperty(const std::string& iName)
		{
		using std::string;
		using std::vector;
		vector<string> theNames;
		if (this->Imp_Enumerate(theNames))
			return ZUtil_STL::sContains(theNames, iName);
		
		return false;
		}

	virtual bool Imp_HasProperty(int32_t iInt)
		{ return false; }

	virtual bool Imp_GetProperty(const std::string& iName, Variant_t& oResult)
		{ return false; }

	virtual bool Imp_GetProperty(int32_t iInt, Variant_t& oResult)
		{ return false; }

	virtual bool Imp_SetProperty(const std::string& iName, const Variant_t& iValue)
		{ return false; }

	virtual bool Imp_SetProperty(int32_t iInt, const Variant_t& iValue)
		{ return false; }

	virtual bool Imp_RemoveProperty(const std::string& iName)
		{ return false; }

	virtual bool Imp_RemoveProperty(int32_t iInt)
		{ return false; }

	virtual bool Imp_Enumerate(NPIdentifier*& oIDs, uint32_t& oCount)
		{
		using std::string;
		using std::vector;
		vector<string> theNames;
		if (this->Imp_Enumerate(theNames))
			{
			oCount = theNames.size();
			oIDs = static_cast<NPIdentifier*>(malloc(sizeof(NPIdentifier) * theNames.size()));
			for (size_t x = 0; x < oCount; ++x)
				oIDs[x] = Object_t::sAsNPI(theNames[x]);

			return true;
			}
		return false;
		}

	virtual bool Imp_Enumerate(std::vector<std::string>& oNames)
		{ return false; }

private:
	static NPObject* sAllocate(NPP npp, NPClass *aClass)
		{
		ZUnimplemented();
		return nil;
		}
	
	static void sDeallocate(NPObject* npobj)
		{
		ZNETSCAPE_BEFORE
			delete static_cast<Object_T*>(npobj);
		ZNETSCAPE_AFTER_VOID
		}

	static void sInvalidate(NPObject* npobj)
		{
		ZNETSCAPE_BEFORE
			static_cast<Object_T*>(npobj)->Imp_Invalidate();
		ZNETSCAPE_AFTER_VOID
		}

	static bool sHasMethod(NPObject* npobj, NPIdentifier name)
		{
		ZNETSCAPE_BEFORE
			return static_cast<Object_T*>(npobj)->Imp_HasMethod(Object_t::sAsString(name));
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sInvoke(NPObject* npobj,
		NPIdentifier name, const NPVariant* args, uint32_t argCount, NPVariant* result)
		{
		ZNETSCAPE_BEFORE
			return static_cast<Object_T*>(npobj)->Imp_Invoke(
				Object_t::sAsString(name),
				static_cast<const Variant_t*>(args),
				argCount,
				*static_cast<Variant_t*>(result));
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sInvokeDefault(NPObject* npobj,
		const NPVariant* args, uint32_t argCount, NPVariant* result)
		{
		ZNETSCAPE_BEFORE
			return static_cast<Object_T*>(npobj)->Imp_InvokeDefault(
				static_cast<const Variant_t*>(args),
				argCount,
				*static_cast<Variant_t*>(result));
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sHasProperty(NPObject* npobj, NPIdentifier name)
		{
		ZNETSCAPE_BEFORE
			{
			if (Object_t::sIsString(name))
				return static_cast<Object_T*>(npobj)->Imp_HasProperty(Object_t::sAsString(name));
			else
				return static_cast<Object_T*>(npobj)->Imp_HasProperty(Object_t::sAsInt(name));
			}
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sGetProperty(NPObject* npobj, NPIdentifier name, NPVariant* result)
		{
		ZNETSCAPE_BEFORE
			if (Object_t::sIsString(name))
				{
				return static_cast<Object_T*>(npobj)->Imp_GetProperty(
					Object_t::sAsString(name),
					*static_cast<Variant_t*>(result));
				}
			else
				{
				return static_cast<Object_T*>(npobj)->Imp_GetProperty(
					Object_t::sAsInt(name),
					*static_cast<Variant_t*>(result));
				}
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sSetProperty(NPObject* npobj, NPIdentifier name, const NPVariant* value)
		{
		ZNETSCAPE_BEFORE
			if (Object_t::sIsString(name))
				{
				return static_cast<Object_T*>(npobj)->Imp_SetProperty(
					Object_t::sAsString(name),
					*static_cast<const Variant_t*>(value));
				}
			else
				{
				return static_cast<Object_T*>(npobj)->Imp_SetProperty(
					Object_t::sAsInt(name),
					*static_cast<const Variant_t*>(value));
				}
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sRemoveProperty(NPObject* npobj, NPIdentifier name)
		{
		ZNETSCAPE_BEFORE
			if (Object_t::sIsString(name))
				return static_cast<Object_T*>(npobj)->Imp_RemoveProperty(Object_t::sAsString(name));
			else
				return static_cast<Object_T*>(npobj)->Imp_RemoveProperty(Object_t::sAsInt(name));
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static bool sEnumerate(NPObject* npobj, NPIdentifier** oIdentifiers, uint32_t* oCount)
		{
		ZNETSCAPE_BEFORE
			return static_cast<Object_T*>(npobj)->Imp_Enumerate(*oIdentifiers, *oCount);
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static NPClass_Z sNPClass;
	};

#define ZNETSCAPE_OBJECT_SETUP(a) \
template<> \
NPClass_Z Object_T<a>::sNPClass( \
	Object_T<a>::sAllocate, \
	Object_T<a>::sDeallocate, \
	Object_T<a>::sInvalidate, \
	Object_T<a>::sHasMethod, \
	Object_T<a>::sInvoke, \
	Object_T<a>::sInvokeDefault, \
	Object_T<a>::sHasProperty, \
	Object_T<a>::sGetProperty, \
	Object_T<a>::sSetProperty, \
	Object_T<a>::sRemoveProperty, \
	Object_T<a>::sEnumerate \
	)

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)

#endif // __ZNetscape__
