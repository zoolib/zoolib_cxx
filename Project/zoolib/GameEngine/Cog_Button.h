// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/GameEngine/Cog.h"

namespace ZooLib {
namespace GameEngine {

Cog sCog_Button(
	const ZP<TouchListener>& iTouchListener,
	const Cog& iCog_UpOut,
	const Cog& iCog_DownIn,
	const Cog& iCog_DownOut,
	const Cog& iCog_Pushed
	);

} // namespace GameEngine
} // namespace ZooLib
