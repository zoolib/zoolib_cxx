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

#include <string>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

namespace ZNetscape {

std::string sAsString(NPNVariable iVar);
std::string sAsString(NPPVariable iVar);

// =================================================================================================
#pragma mark -
#pragma mark * NPVariant_T

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
		{ sRelease(*this); }

	NPVariant_T& operator=(const NPVariant& iOther)
		{
		if (this != &iOther)
			{
			sRelease(*this);
			this->pCopyFrom(iOther);
			}
		return *this;
		}

	NPVariant_T& operator=(const NPVariant_T& iOther)
		{
		if (this != &iOther)
			{
			sRelease(*this);
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
		sRelease(*this);
		type = NPVariantType_Void;
		}

	void SetNull()
		{
		sRelease(*this);
		type = NPVariantType_Null;
		}

	void SetBool(bool iValue)
		{
		sRelease(*this);
		value.boolValue = iValue;
		type = NPVariantType_Bool;
		}

	void SetInt32(int32 iValue)
		{
		sRelease(*this);
		value.intValue = iValue;
		type = NPVariantType_Int32;
		}

	void SetDouble(double iValue)
		{
		sRelease(*this);
		value.doubleValue = iValue;
		type = NPVariantType_Double;
		}

	void SetString(const std::string& iValue)
		{
		sRelease(*this);
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
		sRelease(*this);
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
protected:
	typedef typename Variant_t::Object_t Object_t;

	Object_T()
		{
		this->_class = &sNPClass;
		this->referenceCount = 1;
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
		{ return false; }

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
				return static_cast<Object_T*>(npobj)->RemoveProperty(Object_t::sAsString(name));
			else
				return static_cast<Object_T*>(npobj)->RemoveProperty(Object_t::sAsInt(name));
		ZNETSCAPE_AFTER_RETURN_FALSE
		}

	static NPClass sNPClass;
	};

#define ZNETSCAPE_OBJECT_SETUP(a) \
template<> \
NPClass Object_T<a>::sNPClass = \
	{ \
	1, \
	Object_T<a>::sAllocate, \
	Object_T<a>::sDeallocate, \
	Object_T<a>::sInvalidate, \
	Object_T<a>::sHasMethod, \
	Object_T<a>::sInvoke, \
	Object_T<a>::sInvokeDefault, \
	Object_T<a>::sHasProperty, \
	Object_T<a>::sGetProperty, \
	Object_T<a>::sSetProperty, \
	Object_T<a>::sRemoveProperty \
	}

} // namespace ZNetscape

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Netscape)

#endif // __ZNetscape__
