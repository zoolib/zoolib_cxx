// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
#define __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
#include "zconfig.h"

#include "zoolib/ChanRU_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ZQ.h"

#include "zoolib/RelationalAlgebra/RelHead.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_RelHead {

// =================================================================================================
#pragma mark - Util_Strim_RelHead

void sWrite_PropName(const string8& iName, const ChanW_UTF& s);
void sWrite_RelHead(const RelHead& iRelHead, const ChanW_UTF& s);

ZQ<ColName> sQRead_PropName(const ChanRU_UTF& iChanRU);

ZQ<RelHead> sQFromStrim_RelHead(const ChanRU_UTF& iChanRU);

ZQ<std::pair<ColName,ColName>> sQFromStrim_Rename(const ChanRU_UTF& iChanRU);

ZQ<ConcreteHead> sQFromStrim_ConcreteHead(const ChanRU_UTF& iChanRU);

void sWrite_RenameWithOptional(
	const RelationalAlgebra::Rename& iRename, const RelationalAlgebra::RelHead& iOptional,
	const ChanW_UTF& w);

} // namespace Util_Strim_RelHead
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::RelHead& iRH);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::Rename& iRename);
const ChanW_UTF& operator<<(const ChanW_UTF& w, const RelationalAlgebra::ConcreteHead& iRH);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Util_Strim_RelHead_h__
