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

	virtual void Visit_Rendered_Blush(const ZP<Rendered_Blush>& iRendered_Blush);

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

	virtual void Visit_Rendered_Gain(const ZP<Rendered_Gain>& iRendered_Gain);

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

	virtual void Visit_Rendered_Mat(const ZP<Rendered_Mat>& iRendered_Mat);

protected:
	Mat fMat;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_CelToTextures

class Visitor_Rendered_CelToTextures
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_CelToTextures(const ZP<AssetCatalog>& iAssetCatalog, bool iShowNameFrame);

	virtual void Visit_Rendered_Cel(const ZP<Rendered_Cel>& iRendered_Cel);	

protected:
	const ZP<AssetCatalog> fAssetCatalog;
	const bool fShowNameFrame;
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_DecomposeGroup

class Visitor_Rendered_DecomposeGroup
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Group(const ZP<Rendered_Group>& iRendered_Group);
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_LineToRect

class Visitor_Rendered_LineToRect
:	public virtual Visitor_Rendered
	{
public:
	virtual void Visit_Rendered_Line(const ZP<Rendered_Line>& iRendered_Line);
	};

// =================================================================================================
#pragma mark - Visitor_Rendered_StringToTextures

class Visitor_Rendered_StringToTextures
:	public virtual Visitor_Rendered
	{
public:
	Visitor_Rendered_StringToTextures(const ZP<FontCatalog>& iFontCatalog);

	virtual void Visit_Rendered_String(const ZP<Rendered_String>& iRendered_String);

protected:
	const ZP<FontCatalog> fFontCatalog;
	};

} // namespace GameEngine
} // namespace ZooLib

#endif // __GameEngine_Visitor_Rendered_Std_h__
