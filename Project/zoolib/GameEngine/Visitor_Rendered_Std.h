#ifndef __GameEngine_Visitor_Rendered_Std_h__
#define __GameEngine_Visitor_Rendered_Std_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - Visitor_Rendered_AccumulateAlphaGainMat

class Visitor_Rendered_AccumulateAlphaGainMat
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_AccumulateAlphaGainMat();
	Visitor_Rendered_AccumulateAlphaGainMat(const AlphaGainMat& iAlphaGainMat);

	virtual void Visit_Rendered_AlphaGainMat(
		const ZRef<Rendered_AlphaGainMat>& iRendered_AlphaGainMat);

protected:
	AlphaGainMat fAlphaGainMat;
	};

// =================================================================================================
// MARK: - Visitor_Rendered_DecomposeCel

class Visitor_Rendered_DecomposeCel
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_DecomposeCel(const ZRef<AssetCatalog>& iAssetCatalog, bool iShowNameFrame);

	virtual void Visit_Rendered_Cel(const ZRef<Rendered_Cel>& iRendered_Cel);	

protected:
	const ZRef<AssetCatalog> fAssetCatalog;
	const bool fShowNameFrame;
	};

// =================================================================================================
// MARK: - Visitor_Rendered_DecomposeGroup

class Visitor_Rendered_DecomposeGroup
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Group(const ZRef<Rendered_Group>& iRendered_Group);
	};

// =================================================================================================
// MARK: - Visitor_Rendered_LineToRect

class Visitor_Rendered_LineToRect
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Visitor_Rendered_Std_h__
