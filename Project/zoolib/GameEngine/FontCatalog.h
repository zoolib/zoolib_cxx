#ifndef __ZooLib_GameEngine_FontCatalog_h__
#define __ZooLib_GameEngine_FontCatalog_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"

#include "zoolib/GameEngine/Rendered.h"

#include <map>
#include <set>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - FontStrike

class FontStrike
:	public ZCounted
	{
public:
	void GetVMetrics(float* oAscent, float* oDescent, float* oLineGap);
	void GetHMetrics(const UTF32 iCP, float* oAdvance, float* oLeftSideBearing);
	GRect Measure(const UTF32* iCPs, size_t iCount);
	};

// =================================================================================================
#pragma mark - FontCatalog

class FontCatalog
:	public ZCounted
	{
public:
	FontCatalog();
	virtual ~FontCatalog();

	ZRef<FontStrike> GetFontStrike(const string8& iName, float iSize);

private:
	MtxF fMtx;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_FontCatalog_h__
