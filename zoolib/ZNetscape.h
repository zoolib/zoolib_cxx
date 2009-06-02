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

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCompat_npapi.h"
#include "zoolib/ZCompat_operator_bool.h"
#include "zoolib/ZDebug.h" // For ZAssert
#include "zoolib/ZRef.h"
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

NPClass_Z* sGetClass(NPObject* obj);

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
void* spMalloc_T(T&, size_t);

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

	/*explicit*/ NPVariant_T(bool iValue)
		{
		type = NPVariantType_Bool;
		value.boolValue = iValue;
		}

	/*explicit*/ NPVariant_T(int32 iValue)
		{
		type = NPVariantType_Int32;
		value.intValue = iValue;
		}

	/*explicit*/ NPVariant_T(double iValue)
		{
		type = NPVariantType_Double;
		value.doubleValue = iValue;
		}

	NPVariant_T(const std::string& iValue)
		{
		this->pSetString(iValue);
		type = NPVariantType_String;
		}

	NPVariant_T(const char* iValue)
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

	NPVariant_T& operator=(const char* iValue)
		{
		this->SetString(iValue);
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

	operator ZRef<T>() const
		{ return ZRef<T>(this->GetObject()); }

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
		if (type != NPVariantType_Bool)
			return iDefault;
		return value.boolValue;
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
		if (type != NPVariantType_Int32)
			return iDefault;
		return value.intValue;
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
		if (type != NPVariantType_Double)
			return iDefault;
		return value.doubleValue;
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
		{ return this->DCopyObject(nullptr); }

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
		{ return this->DGetObject(nullptr); }

	bool GetObject(T*& oValue) const
		{
		if (type != NPVariantType_Object)
			return false;
		oValue = static_cast<T*>(value.objectValue);
		return true;	
		}

	T* DGetObject(T* iDefault) const
		{
		if (type != NPVariantType_Object)
			return iDefault;
		return static_cast<T*>(value.objectValue);
		}

	void SetObject(T* iValue)
		{
		iValue->Retain();
		this->pRelease();
		value.objectValue = iValue;
		type = NPVariantType_Object;	
		}

private:
	void pSetString(const char* iChars, size_t iLength)
		{
		sNPStringLength(value.stringValue) = iLength;
		char* p = static_cast<char*>(spMalloc_T(*this, iLength + 1));
		strncpy(p, iChars, iLength);
		sNPStringChars(value.stringValue) = p;
		}

	void pSetString(const std::string& iString)
		{
		if (size_t theLength = iString.length())
			this->pSetString(iString.data(), theLength);
		else
			this->pSetString(nullptr, 0);
		}
	};

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // __ZNetscape__
