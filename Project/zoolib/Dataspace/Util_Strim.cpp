// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Util_Strim.h"

#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/ValueOnce.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const std::set<RelationalAlgebra::RelHead>& iSet)
	{
	FalseOnce needsSeparator;
	foreacha (entry, iSet)
		{
		if (needsSeparator())
			ww << ", ";
		ww << entry;
		}
	return ww;
	}

} // namespace Dataspace
} // namespace ZooLib
