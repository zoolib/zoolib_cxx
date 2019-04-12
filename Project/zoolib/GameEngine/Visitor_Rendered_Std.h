#ifndef __GameEngine_Visitor_Rendered_Std_h__
#define __GameEngine_Visitor_Rendered_Std_h__ 1
#include "zconfig.h"

#include "zoolib/GameEngine/AssetCatalog.h"
#include "zoolib/GameEngine/FontCatalog.h"
#include "zoolib/GameEngine/Rendered.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateBlush

class Visitor_Rendered_AccumulateBlush
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_AccumulateBlush();

	virtual void Visit_Rendered_Blush(const ZRef<Rendered_Blush>& iRendered_Blush);

protected:
	Blush fBlush;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateGain

class Visitor_Rendered_AccumulateGain
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_AccumulateGain();

	virtual void Visit_Rendered_Gain(const ZRef<Rendered_Gain>& iRendered_Gain);

protected:
	Gain fGain;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_AccumulateMat

class Visitor_Rendered_AccumulateMat
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_AccumulateMat();
	Visitor_Rendered_AccumulateMat(const Mat& iMat);

	virtual void Visit_Rendered_Mat(const ZRef<Rendered_Mat>& iRendered_Mat);

protected:
	Mat fMat;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_CelToTextures

class Visitor_Rendered_CelToTextures
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_CelToTextures(const ZRef<AssetCatalog>& iAssetCatalog, bool iShowNameFrame);

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
#pragma mark - Visitor_Rendered_LineToRect

class Visitor_Rendered_LineToRect
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Line(const ZRef<Rendered_Line>& iRendered_Line);
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_StringToTextures

class Visitor_Rendered_StringToTextures
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_StringToTextures(const ZRef<FontCatalog>& iFontCatalog);

	virtual void Visit_Rendered_String(const ZRef<Rendered_String>& iRendered_String);

protected:
	const ZRef<FontCatalog> fFontCatalog;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Visitor_Rendered_Std_h__
