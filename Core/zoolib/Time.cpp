// Copyright (c) 2021 Andrew Green
// MIT License. http://www.zoolib.org

#include "zoolib/Time.h"

namespace ZooLib {
namespace Time {

using namespace std::chrono;


double sNow()
	{
	return Duration(system_clock::now().time_since_epoch()).count();
	}

double sSystem()
	{
	return Duration(steady_clock::now().time_since_epoch()).count();
	}

} // namespace Time
} // namespace ZooLib
