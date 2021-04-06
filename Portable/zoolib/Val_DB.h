// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Val_DB_h__
#define __ZooLib_Val_DB_h__ 1
#include "zconfig.h"

#if 0

	#include "zoolib/Val_T.h"
	#include <map>
	#include <vector>

	namespace ZooLib {

	typedef Any_T<struct Tag_Val_DB> Val_DB;

	bool operator<(const Val_DB& l, const Val_DB& r);
	bool operator!=(const Val_DB& l, const Val_DB& r);

	} // namespace ZooLib

#else

	#include "zoolib/Val_ZZ.h"

	namespace ZooLib {


	typedef Val_ZZ Val_DB;

	} // namespace ZooLib

#endif

#endif // __ZooLib_Val_ZZ_h__
