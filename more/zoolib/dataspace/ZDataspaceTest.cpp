
#include "zoolib/dataspace/ZDataspace_Dataspace.h"
#include "zoolib/dataspace/ZDataspace_Source_Dataset.h"
#include "zoolib/dataspace/ZDataspace_Source_Union.h"
#include "zoolib/dataspace/ZDataspace_Util_Strim.h"

#include "zoolib/ZCallable_Function.h"
#include "zoolib/ZExpr_Bool.h"
#include "zoolib/ZFile.h"
#include "zoolib/ZStrim.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStrimU_Unreader.h"
#include "zoolib/ZStrimmer_Streamer.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_ZooLibStrim.h"

#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"

#include "zoolib/zra/ZRA_Util_RelOperators.h"
#include "zoolib/zra/ZRA_Util_Strim_Rel.h"

#include "zoolib/zra/ZRA_Util_Strim_RelHead.h"

using namespace ZooLib;
using namespace ZRA;
using namespace ZDataspace;
using namespace ZDataset;

static void spInsertFrom(const string8& iPath, ZRef<Dataset> iDataset)
	{
	if (ZRef<ZStreamerR> theStreamerR = ZFileSpec(iPath).OpenR())
		{
		ZRef<ZStrimmerR> theStrimmerR_StreamUTF8 =
			new ZStrimmerR_Streamer_T<ZStrimR_StreamUTF8>(theStreamerR);

		ZRef<ZStrimmerU> theStrimmerU_Unreader =
			new ZStrimmerU_FT<ZStrimU_Unreader>(theStrimmerR_StreamUTF8);

		for (;;)
			{
			if (ZRef<ZYadR, false> theYadR = ZYad_ZooLibStrim::sMakeYadR(theStrimmerU_Unreader))
				{ break; }
			else
				{
				ZData_Any theData;
				ZYad_ZooLibStrim::sToStrim(theYadR,
					ZStrimW_StreamUTF8(ZStreamRWPos_Data_T<ZData_Any>(theData)));

				iDataset->Insert(theData);
				}
			}
		}
	}

// -- 

RelHead sRelHead_person()
	{
	const string8 array[] =
		{
		"person_id",
		"person_name"
		};
	return RelHead(array, countof(array));
	}

ZRef<ZRA::Expr_Rel> sRel_People()
	{ return ZRA::sConcrete(sRelHead_person()); }

// -- 

RelHead sRelHead_pscript()
	{
	const string8 array[] =
		{
		"pscript_id",
		"pscript_person_id",
		"pscript_date"
		};
	return RelHead(array, countof(array));
	}

ZRef<ZRA::Expr_Rel> sRel_pscripts()
	{ return ZRA::sConcrete(sRelHead_pscript()); }

// -- 

void spCallback(ZRef<Sieve> iSieve, bool)
	{
	}

void sDataspaceTest(const ZStrimW& w)
	{
	ZRef<Source_Union> theSource_Union = new Source_Union;

	Nombre theNombre;
	ZRef<Clock> theClock = Clock::sSeed();

	{
	ZRef<Dataset> theDataset = new Dataset(Nombre(theNombre, 1), theClock);
	spInsertFrom("/Users/ag/MedMan1.txt", theDataset);
	theSource_Union->InsertSource(new Source_Dataset(theDataset), string8());
	}

	{
	ZRef<Dataset> theDataset = new Dataset(Nombre(theNombre, 2), theClock);
	spInsertFrom("/Users/ag/MedMan2.txt", theDataset);
	theSource_Union->InsertSource(new Source_Dataset(theDataset), "person_");
	}

	Dataspace theDataspace(theSource_Union);

	ZRef<Sieve_Callable::Callable> theCallable = MakeCallable(spCallback);
	ZRef<Sieve> theSieve = new Sieve_Callable(theCallable);
//	ZRef<ZRA::Expr_Rel> theRel = sRel_pscripts();
	ZRef<ZRA::Expr_Rel> theRel = sRel_People() * sRel_pscripts();
	theRel &= CName("pscript_person_id") == CName("person_id");
	theDataspace.Register(theSieve, theRel);
	
	theDataspace.Update();
	theDataspace.Update();
	theDataspace.Update();
	theDataspace.Update();
	}
