// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Tests.h"

#include "zoolib/Chan_UTF_string.h"

#include "zoolib/Dataspace/Relater_Searcher.h"
#include "zoolib/Dataspace/Types.h" // For AbsentOptional_t

#include "zoolib/QueryEngine/Transform_Search.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Transform_ConsolidateRenames.h"
#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"
#include "zoolib/RelationalAlgebra/Transform_PushDownRestricts.h"
#include "zoolib/RelationalAlgebra/Util_Rel_Operators.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

#include "zoolib/ValPred/Util_Strim_Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_Any.h" // For CConst

namespace ZooLib {
namespace Dataspace {

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

namespace Tests {

using std::string;
using namespace RelationalAlgebra;

// =================================================================================================
#pragma mark -

static void spToChan(ZP<Expr_Rel> iRel, const ChanW_UTF& iChanW)
	{
	RA::Util_Strim_Rel::sToStrim_Parseable(iRel, iChanW);
	}

template <typename T>
T sParseFromChan(const ChanRU_UTF& iChanRU);

template <>
ZP<Expr_Rel> sParseFromChan(const ChanRU_UTF& iChanRU)
	{
	return RelationalAlgebra::Util_Strim_Rel::sQFromStrim(iChanRU);
	}

static bool spCheckRoundTripThroughChan(const ZP<Expr_Rel>& iRel)
	{
	bool success = true;

	if (0 != sCompare_T(iRel, iRel))
		success = false;

	string string1;
	spToChan(iRel, ChanW_UTF_string8(&string1));

	ZP<Expr_Rel> newRel = sParseFromChan<ZP<Expr_Rel>>(ChanRU_UTF_string8(string1));
	string string2;
	spToChan(newRel, ChanW_UTF_string8(&string2));

	if (string1 != string2)
		success = false;

	if (0 != sCompare_T(iRel, newRel))
		success = false;

	return success;
	}

#if 0

static void spTestRel(ZP<Expr_Rel> iRel)
	{
	ZP<Expr_Rel> theRel;

	theRel = iRel;
	theRel = QE::sTransform_Search(theRel);
	theRel = RA::Transform_DecomposeRestricts().Do(theRel);
	theRel = sTransform_PushDownRestricts_IntoSearch(theRel);

	theRel = iRel;
	theRel = RA::Transform_DecomposeRestricts().Do(theRel);
	theRel = RA::Transform_PushDownRestricts().Do(theRel);
	theRel = QE::sTransform_Search(theRel);

	ZLOGTRACE(eDebug);
	}

static ZP<Expr_Rel> spGetRel(const string& iName)
	{
	const ZP<Expr_Rel> fRel_DaemonInstance = sRel_DaemonInstance("Skate");

	ZP<Expr_Bool> theSpec = sTrue();
	theSpec &= CName("Type") == CConst("Runner");
	theSpec &= CName("SubType") == CConst("NovationLaunchpad");

	const RelHead theRH_Required = sRelHead("DaemonInstance", "Type", "SubType", "Device", "Name", "VarName");

	ZP<Expr_Rel> theRel = sConcrete(theRH_Required);
	theRel = theRel & theSpec;

	theRel = sApplyRel_DaemonInstance(theRel, fRel_DaemonInstance);
	return theRel;
	}
#endif  //0

static string spGetQuery();

static ZP<Expr_Rel> spGetRel2(const string& iName)
	{
	// ---
	{
	ZP<Expr_Rel> rel_Group = sConcrete(sRelHead("Type", "SubType", "Name"));
	rel_Group &= CName("Type") == CConst("UI");
	rel_Group &= CName("SubType") == CConst("Group");
	rel_Group &= CName("Name") == CConst(iName);

	rel_Group = sRename(rel_Group, "Group.Name", "Name");
	rel_Group = sRename(rel_Group, "Group.Type", "Type");
	rel_Group = sRename(rel_Group, "Group.SubType", "SubType");

	// --

	ZP<Expr_Rel> rel_Links = sConcrete(sRelHead("Type", "Group", "Member", "Index"));
	rel_Links &= CName("Type") == CConst("Link");
	rel_Links &= CName("Group") == CConst(iName);

	rel_Links = sRename(rel_Links, "Link.Type", "Type");
	rel_Links = sRename(rel_Links, "Link.Index", "Index");
	rel_Links = sRename(rel_Links, "Link.Group", "Group");
	rel_Links = sRename(rel_Links, "Link.Member", "Member");

	// --

	ZP<Expr_Rel> rel_UIMember = sConcrete(
		sRelHead("Type", "SubType", "Name"),
		sRelHead("Var", "Editable", "Name_Display", "Choices"));
	rel_UIMember &= CName("Type") == CConst("UI");

	rel_UIMember = sRename(rel_UIMember, "UIMember.Type", "Type");
	rel_UIMember = sRename(rel_UIMember, "UIMember.Name", "Name");
	rel_UIMember = sRename(rel_UIMember, "UIMember.SubType", "SubType");
	rel_UIMember = sRename(rel_UIMember, "UIMember.Var", "Var");
	rel_UIMember = sRename(rel_UIMember, "UIMember.Name_Display", "Name_Display");
	rel_UIMember = sRename(rel_UIMember, "UIMember.Editable", "Editable");
	rel_UIMember = sRename(rel_UIMember, "UIMember.Choices", "Choices");

	// --

	ZP<Expr_Rel> rel_Var_Outer = sConcrete(sRelHead("Type", "Name"));
	rel_Var_Outer &= (CName("Type") == CConst("Var"));
	rel_Var_Outer = sRename(rel_Var_Outer, "Var_Outer.Type", "Type");
	rel_Var_Outer = sRename(rel_Var_Outer, "Var_Outer.Name", "Name");

	ZP<Expr_Rel> rel_Var_Inner = sConcrete(sRelHead("Type", "Name", ""));
	rel_Var_Inner &= (CName("Type") == CConst("Var"));
	rel_Var_Inner &= (CName("Name") == CName("Var_Outer.Name"));

	rel_Var_Outer = sEmbed(rel_Var_Outer, sRelHead("Var_Outer.Name"), "Var_Instances", rel_Var_Inner);

	// Append a variable called "***Dummy***", to be used so that a group's (member * var) is never
	// empty. Alternatively, we *could* embed the var in the member -- not sure how I feel about
	// the double embedding.
	ZP<Expr_Rel> rel_Dummy = sDee()
		* NameVal("Var_Outer.Type", "Var")
		* NameVal("Var_Outer.Name", "***Dummy***")
		* NameVal("Var_Instances", AbsentOptional_t());

	rel_Var_Outer = rel_Var_Outer | rel_Dummy;
	// --

	ZP<Expr_Rel> rel_UIMember_Var = rel_UIMember * rel_Var_Outer;

	rel_UIMember_Var &=
		(((CName("UIMember.SubType") == CConst("Button")) | (CName("UIMember.SubType") == CConst("Group")))
			& (CName("Var_Instances") == CConst(AbsentOptional_t()))
			& (CName("Var_Outer.Name") == CConst("***Dummy***")))
		| (CName("UIMember.Name") == CName("Var_Outer.Name")
		| CName("UIMember.Var") == CName("Var_Outer.Name"));

	// This line is *much* slower
	// ZP<Expr_Rel> theRel = rel_Links * (rel_Group * rel_UIMember_Var);

	ZP<Expr_Rel> theRel = rel_Group * (rel_Links * rel_UIMember_Var);
	theRel &= (CName("Link.Group") == CName("Group.Name"));
	theRel &= (CName("Link.Member") == CName("UIMember.Name"));

	const RelHead identity = sRelHead("Link.Group", "Link.Index", "Link.Member", "Link.Type");

	const RelHead significant = sRelHead(
		"UIMember.Editable",
		"UIMember.Choices",
		"UIMember.Name_Display",
		"UIMember.SubType",
		"UIMember.Type",
		"UIMember.Var",
		"Var_Instances");

	return theRel;
	}
	}

void RunTests()
	{
	ZP<Expr_Rel> theRel = sParseFromChan<ZP<Expr_Rel>>(ChanRU_UTF_string8(spGetQuery()));
	spCheckRoundTripThroughChan(theRel);

//	ZP<Expr_Rel> relcons = sTransform_ConsolidateRenames(theRel);

	theRel = QE::sTransform_Search(theRel);
	theRel = RA::Transform_DecomposeRestricts().Do(theRel);
	theRel = sTransform_PushDownRestricts_IntoSearch(theRel);
	}

// ----------------

static ZP<Expr_Rel> spGetQuerySimplified2(const std::string iName)
	{
	ZP<Expr_Rel> theRel = sConcrete(
		sRelHead("Type", "SubType", "Name"),
		sRelHead("Name_Display"));

	theRel &= CName("Type") == CConst("UI");
	theRel &= CName("SubType") == CConst("Group");
	theRel &= CName("Name") == CConst(iName);
	return theRel;
	}

static string spGetQuerySimplified()
	{
string theQuery =
"Restrict(@pline_pscript_id == @pscript_id,"
"  Product("
"    Embed("
"      Restrict(@pline_isnew != true,"
"                                                Concrete([@pline_id, @pline_isnew, @pline_pscript_id])),"
"      [@pline_id],"
"      @\"%Deliveries\" = "
"        Restrict(@delivery_pline_id == @pline_id,"
"                  Rename(@delivery_date<--@DATA_delivery_date,"
"                            Rename(@DATA_delivery_date<--@delivery_date,"
"                              Concrete([@delivery_date, @delivery_pline_id]))))),"
"                        Concrete([@pscript_alarm, @pscript_date, @pscript_id, @pscript_isnew, @pscript_person_id])))";
	return theQuery;
	}

static string spGetQuery()
	{
	string theQuery =
"        Rename(@pline_when<--@DATA_pline_when,"
"          Comment(\"This is a comment\","
"                            Rename(@DATA_pline_when<--@pline_when,"
"                              Rename(@DATA_pline_startdate<--@pline_startdate,"
"                                                Concrete([@pline_alarm, @pline_startdate, @pline_when])))))";
	return theQuery;
	}

static string spGetQueryDis()
	{
	string theQuery =
"Restrict(@pline_pscript_id == @pscript_id,"
"  Product("
"    Embed("
"      Restrict(@pline_isnew != true,"
"        Rename(@pline_when<--@DATA_pline_when,"
"          Rename(@pline_startdate<--@DATA_pline_startdate,"
"            Rename(@pline_pscript_id<--@DATA_pline_pscript_id,"
"              Rename(@pline_isnew<--@DATA_pline_isnew,"
"                Rename(@pline_id<--@DATA_pline_id,"
"                  Rename(@pline_enabled<--@DATA_pline_enabled,"
"                    Rename(@pline_duration<--@DATA_pline_duration,"
"                      Rename(@pline_drug<--@DATA_pline_drug,"
"                        Rename(@pline_dose<--@DATA_pline_dose,"
"                          Rename(@pline_alarm<--@DATA_pline_alarm,"
"                            Rename(@DATA_pline_when<--@pline_when,"
"                              Rename(@DATA_pline_startdate<--@pline_startdate,"
"                                Rename(@DATA_pline_pscript_id<--@pline_pscript_id,"
"                                  Rename(@DATA_pline_isnew<--@pline_isnew,"
"                                    Rename(@DATA_pline_id<--@pline_id,"
"                                      Rename(@DATA_pline_enabled<--@pline_enabled,"
"                                        Rename(@DATA_pline_duration<--@pline_duration,"
"                                          Rename(@DATA_pline_drug<--@pline_drug,"
"                                            Rename(@DATA_pline_dose<--@pline_dose,"
"                                              Rename(@DATA_pline_alarm<--@pline_alarm,"
"                                                Concrete([@pline_alarm, @pline_dose, @pline_drug, @pline_duration, @pline_enabled, @pline_id, @pline_isnew, @pline_pscript_id, @pline_startdate, @pline_when])))))))))))))))))))))),"
"      [@pline_id],"
"      @\"%Deliveries\" = "
"        Restrict(@delivery_pline_id == @pline_id,"
"          Rename(@delivery_pline_id<--@DATA_delivery_pline_id,"
"            Rename(@delivery_notes<--@DATA_delivery_notes,"
"              Rename(@delivery_isnew<--@DATA_delivery_isnew,"
"                Rename(@delivery_id<--@DATA_delivery_id,"
"                  Rename(@delivery_date<--@DATA_delivery_date,"
"                    Rename(@DATA_delivery_pline_id<--@delivery_pline_id,"
"                      Rename(@DATA_delivery_notes<--@delivery_notes,"
"                        Rename(@DATA_delivery_isnew<--@delivery_isnew,"
"                          Rename(@DATA_delivery_id<--@delivery_id,"
"                            Rename(@DATA_delivery_date<--@delivery_date,"
"                              Concrete([@delivery_date, @delivery_id, @delivery_isnew, @delivery_notes, @delivery_pline_id]))))))))))))),"
"    Rename(@pscript_person_id<--@DATA_pscript_person_id,"
"      Rename(@pscript_isnew<--@DATA_pscript_isnew,"
"        Rename(@pscript_id<--@DATA_pscript_id,"
"          Rename(@pscript_date<--@DATA_pscript_date,"
"            Rename(@pscript_alarm<--@DATA_pscript_alarm,"
"              Rename(@DATA_pscript_person_id<--@pscript_person_id,"
"                Rename(@DATA_pscript_isnew<--@pscript_isnew,"
"                  Rename(@DATA_pscript_id<--@pscript_id,"
"                    Rename(@DATA_pscript_date<--@pscript_date,"
"                      Rename(@DATA_pscript_alarm<--@pscript_alarm,"
"                        Concrete([@pscript_alarm, @pscript_date, @pscript_id, @pscript_isnew, @pscript_person_id])))))))))))))";
	return theQuery;
	}

} // namespace Tests
} // namespace RelationalAlgebra
} // namespace ZooLib
