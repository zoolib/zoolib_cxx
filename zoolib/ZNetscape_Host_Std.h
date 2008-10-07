/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green
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

#ifndef __ZNetscape_Host_Std__
#define __ZNetscape_Host_Std__ 1
#include "zconfig.h"

#include "zoolib/ZNetscape_Host.h"

#include <map>
#include <vector>

namespace ZNetscape {

class Host;
class HostMeister;

// =================================================================================================
#pragma mark -
#pragma mark * HostMeister_Std

class HostMeister_Std : public HostMeister
	{
public:
	HostMeister_Std();
	virtual ~HostMeister_Std();

	virtual void* MemAlloc(uint32 size);

	virtual void MemFree(void* ptr);

	virtual uint32 MemFlush(uint32 size);

	virtual void ReloadPlugins(NPBool reloadPages);

	virtual void* GetJavaEnv();

	virtual void ReleaseVariantValue(NPVariant* variant);

	virtual NPIdentifier GetStringIdentifier(const NPUTF8* name);

	virtual void GetStringIdentifiers(
		const NPUTF8* *names, int32_t nameCount, NPIdentifier* identifiers);

	virtual NPIdentifier GetIntIdentifier(int32_t intid);

	virtual int32_t IntFromIdentifier(NPIdentifier identifier);

	virtual bool IdentifierIsString(NPIdentifier identifier);

	virtual NPUTF8* UTF8FromIdentifier(NPIdentifier identifier);

	virtual NPObject* RetainObject(NPObject* obj);

	virtual void ReleaseObject(NPObject* obj);

	virtual void SetException(NPObject* obj, const NPUTF8* message);

private:
	struct Identifier
		{
		bool fIsString;
		union
			{
			const UTF8* fAsString;
			int fAsInt;
			};
		};

	std::map<std::string, Identifier*> fMap_Strings;
	std::map<int, Identifier*> fMap_Ints;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Host_Std

class Host_Std : public Host
	{
public:
	Host_Std(ZRef<GuestFactory> iGuestFactory);
	virtual ~Host_Std();

	virtual NPError GetURLNotify(NPP npp,
		const char* URL, const char* window, void* notifyData);

	virtual NPError PostURLNotify(NPP npp,
		const char* URL, const char* window,
		uint32 len, const char* buf, NPBool file, void* notifyData);

	virtual NPError RequestRead(NPStream* stream, NPByteRange* rangeList);

	virtual NPError NewStream(NPP npp,
		NPMIMEType type, const char* window, NPStream** stream);

	virtual int32 Write(NPP npp, NPStream* stream, int32 len, void* buffer);

	virtual NPError DestroyStream(NPP npp, NPStream* stream, NPReason reason);

	virtual void Status(NPP npp, const char* message);

	virtual const char* UserAgent(NPP npp);

	virtual NPError GetValue(NPP npp, NPNVariable variable, void* ret_value);

	virtual NPError SetValue(NPP npp, NPPVariable variable, void* value);

	virtual void InvalidateRect(NPP npp, NPRect* rect);

	virtual void InvalidateRegion(NPP npp, NPRegion region);

	virtual void ForceRedraw(NPP npp);

	virtual NPError GetURL(NPP npp, const char* URL, const char* window);

	virtual NPError PostURL(NPP npp,
		const char* URL, const char* window, uint32 len, const char* buf, NPBool file);

	virtual void* GetJavaPeer(NPP npp);

	virtual NPObject* CreateObject(NPP npp, NPClass* aClass);

	virtual bool Invoke(NPP npp,
		NPObject* obj, NPIdentifier methodName, const NPVariant* args, unsigned argCount,
		NPVariant* result);

	virtual bool InvokeDefault(NPP npp,
		NPObject* obj, const NPVariant* args, unsigned argCount, NPVariant* result);

	virtual bool Evaluate(NPP npp,
		NPObject* obj, NPString* script, NPVariant* result);

	virtual bool GetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, NPVariant* result);

	virtual bool SetProperty(NPP npp,
		NPObject* obj, NPIdentifier propertyName, const NPVariant* value);

	virtual bool HasProperty(NPP, NPObject* npobj, NPIdentifier propertyName);

	virtual bool HasMethod(NPP npp, NPObject* npobj, NPIdentifier methodName);

	virtual bool RemoveProperty(NPP npp, NPObject* obj, NPIdentifier propertyName);

private:
	class HTTPer;
	friend class HTTPer;

	void pHTTPerFinished(HTTPer* iHTTPer, void* iNotifyData,
		const std::string& iURL, const std::string& iMIME, const ZMemoryBlock& iHeaders,
		ZRef<ZStreamerRCon> iStreamerRCon);
	
	ZooLib::ZMutex fMutex;
	std::vector<HTTPer*> fHTTPers;
	};

} // namespace ZNetscape

#endif // __ZNetscape_Host_Std__
