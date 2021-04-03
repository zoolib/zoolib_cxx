// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_ML_h__
#define __ZooLib_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Chan_UTF.h"
#include "zoolib/Name.h"

#include <vector>

namespace ZooLib {
namespace ML {

typedef std::pair<Name, std::string> Attr_t;
typedef std::vector<Attr_t> Attrs_t;

typedef Callable<std::string(std::string)> Callable_Entity;

// =================================================================================================
#pragma mark - ML

std::string sReadReference(const ChanRU_UTF& iChanRU, const ZP<Callable_Entity>& iCallable);

bool sReadIdentifier(const ChanRU_UTF& iChanRU, std::string& oText);

bool sReadAttributeName(const ChanRU_UTF& iChanRU, std::string& oName);

bool sReadAttributeValue(
	const ChanRU_UTF& iChanRU,
	bool iRecognizeEntities, const ZP<Callable_Entity>& iCallable,
	std::string& oValue);

} // namespace ML
} // namespace ZooLib

#endif // __ZooLib_ML_h__
