#include "zoolib/GameEngine/DrawPreprocess.h"

#include "zoolib/Log.h"

#include "zoolib/GameEngine/Util_Allocator.h"
#include "zoolib/GameEngine/Visitor_Rendered_Std.h"

#include "zoolib/ZMACRO_foreach.h"

#include <map>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Visitor_Preprocess (anonymous)

namespace { // anonymous

// Visitor_Preprocess transforms the tree into a flat list of nodes sorted front to back. The
// list is a group, each node is an BGM containing a leaf rendered (line,
// rect, sound, texture, buffer). We don't flatten the elements of a buffer up into the root.

class Visitor_Preprocess
:	public virtual Visitor_Rendered_AccumulateBlush
,	public virtual Visitor_Rendered_AccumulateGain
,	public virtual Visitor_Rendered_AccumulateMat
,	public virtual Visitor_Rendered_CelToTextures
,	public virtual Visitor_Rendered_DecomposeGroup
,	public virtual Visitor_Rendered_StringToTextures
	{
public:
	Visitor_Preprocess(const ZP<AssetCatalog>& iAssetCatalog, bool iShowNameFrame,
		const ZP<FontCatalog>& iFontCatalog,
		const GPoint& iGameSize)
	:	Visitor_Rendered_CelToTextures(iAssetCatalog, iShowNameFrame)
	,	Visitor_Rendered_StringToTextures(iFontCatalog)
	,	fScreenBounds(sRect<>(iGameSize))
	,	fMap(std::less<Rat>(), fRealAllocator)
		{}

// From Visitor_Rendered
	virtual void Visit_Rendered(const ZP<Rendered>& iRendered)
		{
		// Dispatch the default behavior.
		this->pInsertIntoMap(iRendered);
		}

	virtual void Visit_Rendered_Buffer(const ZP<Rendered_Buffer>& iRendered_Buffer)
		{
		// It's a buffer. We flatten the buffer's contents, but maintain the fact that
		// they *are* children of the buffer.
		if (ZP<Rendered> theRendered = iRendered_Buffer->GetRendered())
			{
			const int theWidth = iRendered_Buffer->GetWidth();
			const int theHeight = iRendered_Buffer->GetHeight();

			theRendered = sDrawPreprocess(theRendered,
				fAssetCatalog, fShowNameFrame, fFontCatalog, sPoint<GPoint>(theWidth, theHeight));

			theRendered =
				sRendered_Buffer(theWidth, theHeight, iRendered_Buffer->GetFill(), theRendered);

			this->pInsertIntoMap(theRendered);
			}
		}

	virtual void Visit_Rendered_Texture(const ZP<Rendered_Texture>& iRendered_Texture)
		{
		// We discard textures if they would not intersect the destination rectangle. This
		// is an optimization at the resource-management level. Drawing the texture wouldn't
		// generally be costly, but getting the data into texture form is.
		const CVec3 newLT = fMat * sCVec3<Rat>();

		const GPoint orgRB = WH(iRendered_Texture->GetBounds());

		const CVec3 newRB = fMat * sCVec3(X(orgRB), Y(orgRB), 0);

		const GRect visibleBounds = sRect<GRect>(
			sMin(X(newLT), X(newRB)),
			sMin(Y(newLT), Y(newRB)),
			sMax(X(newLT), X(newRB)),
			sMax(Y(newLT), Y(newRB)));

		const GRect intersection = visibleBounds & fScreenBounds;

		if (sNotEmpty(intersection))
			this->pInsertIntoMap(iRendered_Texture);
		}

// Our protocol
	ZP<Rendered_Group> Do(const ZP<Rendered>& iRendered)
		{
		iRendered->Accept_Rendered(*this);

		ZP<Rendered_Group> theGroup = sRendered_Group();
		foreacha (entry, fMap)
			theGroup->Append(entry.second);
		return theGroup;
		}

	void pInsertIntoMap(ZP<Rendered> iRendered)
		{
		// We're passed a pointer, so there's one less refcount manipulation happening.
		// We could/should use std::move.

		// Apply accumulated BGM.
		iRendered = sRendered_Blush(fBlush, iRendered);
		iRendered = sRendered_Gain(fGain, iRendered);
		iRendered = sRendered_Mat(fMat, iRendered);

		// Push (0,0,0) through our current accumulated
		// transformation, and take the resulting Z coordinate.
		const Rat theZ = (fMat * CVec3())[2];

		fMap.insert(std::make_pair(theZ, iRendered));
		}

private:
	RealAllocator fRealAllocator;

	typedef std::multimap<Rat,ZP<Rendered>, std::less<Rat>,
		TTAllocator<std::pair<const Rat, ZP<Rendered> > > > MultiMap_Rat2Rendered;

	const GRect fScreenBounds;
	MultiMap_Rat2Rendered fMap;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - sDrawPreprocess

ZP<Rendered> sDrawPreprocess(const ZP<Rendered>& iRendered,
	const ZP<AssetCatalog>& iAssetCatalog, bool iShowNameFrame,
	const ZP<FontCatalog>& iFontCatalog,
	const GPoint& iGameSize)
	{
	return Visitor_Preprocess(iAssetCatalog, iShowNameFrame, iFontCatalog, iGameSize).Do(iRendered);
	}

} // namespace GameEngine
} // namespace ZooLib
