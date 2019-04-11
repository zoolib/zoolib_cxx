#ifndef __GameEngine_Visitor_Rendered_Std_h__
#define __GameEngine_Visitor_Rendered_Std_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateBlushGainMat

class Visitor_Rendered_AccumulateBlushGainMat
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_AccumulateBlushGainMat();
	Visitor_Rendered_AccumulateBlushGainMat(const BlushGainMat& iBlushGainMat);

	virtual void Visit_Rendered_BlushGainMat(
		const ZRef<Rendered_BlushGainMat>& iRendered_BlushGainMat);

protected:
	BlushGainMat fBlushGainMat;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeCel

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
#pragma mark - Visitor_Rendered_DecomposeGroup

class Visitor_Rendered_DecomposeGroup
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Group(const ZRef<Rendered_Group>& iRendered_Group);
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeString

class Visitor_Rendered_DecomposeString
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_DecomposeString(const ZRef<FontCatalog>& iFontCatalog);

	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String);

protected:
	const ZRef<FontCatalog> fFontCatalog;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_LineToRect

class Visitor_Rendered_LineToRect
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line);
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Visitor_Rendered_Std_h__
