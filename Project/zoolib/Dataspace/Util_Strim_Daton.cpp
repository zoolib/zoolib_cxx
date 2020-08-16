// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Util_Strim_Daton.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

const ChanW_UTF& operator<<(const ChanW_UTF& w, const Dataspace::Daton& iDaton)
	{
	const Data_ZZ& theData = iDaton.GetData();
	sWrite(w, (const UTF8*)theData.GetPtr(), theData.GetSize(), nullptr, theData.GetSize(), nullptr);
	return w;
	}

} // namespace ZooLib
