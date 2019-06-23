#ifndef __ZooLib_GameEngine_Nook_h__
#define __ZooLib_GameEngine_Nook_h__ 1

#include "zoolib/Counted.h"
#include "zoolib/Name.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/ZMACRO_auto.h"

#include <set>
#include <typeinfo>
#include <vector>

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace GameEngine {

class Nook;

// =================================================================================================
#pragma mark -

class NookScope
:	public ZCounted
	{
public:
	NookScope();
	virtual ~NookScope();

	void NewEra();

	ZP<Nook> GetOne(const Name& iName);

	std::vector<ZP<Nook> > GetAll(const Name& iName);

private:
	bool pIsFullCycle(Nook* iNook);
	void pInitialize(Nook* iNook);
	void pFinalize(Nook* iNook);

	ZMtx fMtx;
	typedef std::pair<Name,Nook*> NameNook;
	typedef std::set<NameNook> NameNookSet;
	NameNookSet fNooks_Adding;
	NameNookSet fNooks_JustAdded;
	NameNookSet fNooks_All;
	friend class Nook;
	};

// =================================================================================================
#pragma mark - Nook

class Nook
:	public ZCounted
	{
public:
	Nook(const ZP<NookScope>& iNookScope);
	
// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

	bool IsFullCycle();

	ZP<NookScope> GetNookScope();

private:
	friend class NookScope;
	const WP<NookScope> fNookScopeWP;
	// Used by NookScope
	Name fName;
	};

// =================================================================================================
#pragma mark - NookSet

template <class Nook_p>
class NookSet
	{
public:
	NookSet()
		{}

	std::set<WP<Nook_p> > fNookWPs;

	std::vector<ZP<Nook_p> > Get()
		{
		std::vector<ZP<Nook_p> > result;
		for (ZMACRO_auto_(ii, fNookWPs.begin());
			ii != fNookWPs.end(); /*no inc*/)
			{
			if (ZP<Nook> theNook = *ii)
				{
				result.push_back(theNook.StaticCast<Nook_p>());
				++ii;
				}
			else
				{
				ii = Util_STL::sEraseInc(fNookWPs, ii);
				}
			}
		return result;
		}
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Nook_h__
