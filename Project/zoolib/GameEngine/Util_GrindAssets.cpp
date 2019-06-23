#include "zoolib/GameEngine/Util.h"
#include "zoolib/GameEngine/Util_Assets.h"
#include "zoolib/GameEngine/Util_GrindAssets.h"

#include "zoolib/Log.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"

#include "zoolib/Apple/Cartesian_CG.h"

#include "zoolib/ZBigRegion.h"

#if ZMACRO_IOS
	#include ZMACINCLUDE2(CoreGraphics,CoreGraphics.h)
#else
	#include ZMACINCLUDE3(ApplicationServices,CoreGraphics,CoreGraphics.h)
#endif

#if not ZMACRO_IOS && not defined(__ANDROID__)
	#include "zoolib/ZDCPixmapCoder_PNG.h"
#endif

namespace GameEngine {
namespace Util {

using std::map;
using std::vector;

using namespace Util_string;

// =================================================================================================
// MARK: -

#if not ZMACRO_IOS && not defined(__ANDROID__)

typedef Multi2<ZDCPixmap,ZPointPOD> PixmapOffset;

static
PixmapOffset spTrim(const ZDCPixmap& iPM)
	{
	ZPointPOD theSize = iPM.Size();
	int32 xMin = X(theSize)-1;
	int32 xMax = 0;
	int32 yMin = Y(theSize)-1;
	int32 yMax = 0;
	
	for (int32 y = 0; y < Y(theSize); ++y)
		{
		for (int32 x = 0; x < X(theSize); ++x)
			{
			ZRGBA theRGBA = iPM.GetPixel(x, y);
			if (theRGBA.alpha)
				{
				xMin = sMin(x, xMin);
				xMax = sMax(x, xMax);
				yMin = sMin(y, yMin);
				yMax = sMax(y, yMax);
				}
			}
		}
	return PixmapOffset(
		ZDCPixmap(iPM, sRectPOD(xMin, yMin, xMax+1, yMax+1)),
		sPointPOD(xMin, yMin));
	}

static
ZBigRegion spRegion(const ZDCPixmap& iPM)
	{
	ZBigRegionAccumulator theAcc;

	const ZBigRegion rgn1(sPointPOD(1,1));

	const ZPointPOD theSize = iPM.Size();
	
	for (int32 y = 0; y < Y(theSize); ++y)
		{
		for (int32 x = 0; x < X(theSize); ++x)
			{
			if (iPM.GetPixel(x, y).alpha)
				theAcc.Include(rgn1 + sPointPOD(x,y));
			}
		}

	return theAcc.Get();
	}

static
Val_Any spAsVal(const CVec3& iCVec3)
	{
	Seq_Any theSeq;
	theSeq.Append(iCVec3[0]);
	theSeq.Append(iCVec3[1]);
	theSeq.Append(iCVec3[2]);
	return theSeq;
	}

static
Val_Any spAsVal(const GRect& iRect)
	{
	Seq_Any theSeq;
	theSeq.Append(L(iRect));
	theSeq.Append(T(iRect));
	theSeq.Append(R(iRect));
	theSeq.Append(B(iRect));
	return theSeq;
	}

static
Val_Any spAsVal(const ZPointPOD& iPoint)
	{
	Seq_Any theSeq;
	theSeq.Append(X(iPoint));
	theSeq.Append(Y(iPoint));
	return theSeq;
	}

static
Val_Any spAsVal(const ZRectPOD& iRect)
	{
	Seq_Any theSeq;
	theSeq.Append(spAsVal(LT(iRect)));
	theSeq.Append(spAsVal(WH(iRect)));
	return theSeq;
	}

// =================================================================================================
// MARK: - Tiler

class Tiler
	{
public:
	Tiler(const ZDCPixmap& iPixmap, int iPaddingX, int iPaddingY)
	:	fPixmap(iPixmap)
	,	fPaddingX(iPaddingX)
	,	fPaddingY(iPaddingY)
	,	fRoot(new Node)
	,	fGrowHorizontal(false)
	,	fCapX(0)
	,	fCapY(0)
	,	fUsedX(0)
	,	fUsedY(0)
		{}		
	
	ZQ<ZRectPOD> Insert(const ZDCPixmap& iPixmap)
		{
		const ZPointPOD theSourceSize = iPixmap.Size();
		const ZPointPOD thePaddedSize = theSourceSize + sPointPOD(2*fPaddingX,2*fPaddingY);

		for (;;)
			{
			int theX, theY;
			if (spInsert(fRoot, fCapX, fCapY, X(thePaddedSize), Y(thePaddedSize), 0, 0, theX, theY))
				{
				const ZPointPOD destLocation = sPointPOD(theX+fPaddingX, theY+fPaddingY);

				fPixmap.CopyFrom(destLocation, iPixmap, sRect(theSourceSize));

				const ZRectPOD result = sRect(theSourceSize) + destLocation;

				fUsedX = sMax(fUsedX, R(result) + fPaddingX);
				fUsedY = sMax(fUsedY, B(result) + fPaddingY);

				return result;
				}
			else
				{
				const int newCapX = fCapX + X(thePaddedSize);
				const int newCapY = fCapY + Y(thePaddedSize);
				fGrowHorizontal = not fGrowHorizontal;
				if (fGrowHorizontal && newCapX <= X(fPixmap.Size()))
					{
					ZP<Node> newRoot = new Node;
					newRoot->fVertical = true;
					newRoot->fPos = fCapX;
					newRoot->f0 = fRoot;
					newRoot->f1 = new Node;
					fRoot = newRoot;
					fCapX = newCapX;
					}
				else if (newCapY <= Y(fPixmap.Size()))
					{
					ZP<Node> newRoot = new Node;
					newRoot->fVertical = false;
					newRoot->fPos = fCapY;
					newRoot->f0 = fRoot;
					newRoot->f1 = new Node;
					fRoot = newRoot;
					fCapY = newCapY;
					}
				else
					{
					return null;
					}
				}
			}		
		}
	
	ZDCPixmap GetPixmap()
		{ return ZDCPixmap(fPixmap, sRectPOD(fUsedX, fUsedY)); }

// -----

private:
	struct Node
	:	public CountedWithoutFinalize
		{
		Node() : fPos(0) {}
		
		int fPos;
		bool fVertical;
		ZP<Node> f0, f1;
		};

	ZDCPixmap fPixmap;
	const int fPaddingX;
	const int fPaddingY;
	ZP<Node> fRoot;
	bool fGrowHorizontal;
	int fCapX;
	int fCapY;
	int fUsedX;
	int fUsedY;

// -----

	static
	bool spInsert(
		const ZP<Node>& iNode,
		const int iCapW, const int iCapH,
		const int iDesW, const int iDesH,
		const int iX, const int iY,
		int& oX, int& oY)
		{
		if (iCapW < iDesW || iCapH < iDesH)
			{
			// Can't fit.
			return false;
			}

		if (not iNode->f0)
			{
			// Leaf node

			if (iNode->fPos)
				{
				// But it's in use.
				return false;
				}

			iNode->f0 = new Node;
			iNode->f1 = new Node;

			if (iCapW == iDesW)
				{
				oX = iX;
				oY = iY;
				// Perfect width
				if (iCapH == iDesH)
					{
					// And height
					iNode->fVertical = true;
					iNode->fPos = iDesW;
					iNode->f0->fPos = -1;
					return true;
					}
				else
					{
					// Split horizontally.
					iNode->fVertical = false;
					iNode->fPos = iDesH;
					iNode->f0->fPos = -1;
					return true;
					}
				}
			else if (iCapH == iDesH)
				{
				oX = iX;
				oY = iY;

				// Perfect height, split vertically
				iNode->fVertical = true;
				iNode->fPos = iDesW;
				iNode->f0->fPos = -1;
				return true;
				}
			else
				{
				iNode->fVertical = true;
				iNode->fPos = iDesW;
				}
			}

		if (iNode->fVertical)
			{
			// It's split vertically.
			if (spInsert(iNode->f0, iNode->fPos, iCapH, iDesW, iDesH, iX, iY, oX, oY))
				return true;
			return spInsert(
				iNode->f1, iCapW - iNode->fPos, iCapH, iDesW, iDesH, iX + iNode->fPos, iY, oX, oY);
			}
		else
			{
			// Split horizontally.
			if (spInsert(iNode->f0, iCapW, iNode->fPos, iDesW, iDesH, iX, iY, oX, oY))
				return true;
			return spInsert(
				iNode->f1, iCapW, iCapH - iNode->fPos, iDesW, iDesH, iX, iY + iNode->fPos, oX, oY);
			}
		}
	};

// =================================================================================================
// MARK: -

static const int kPadding = 1;

typedef Multi2<ZDCPixmap,ZPointPOD> PixmapReg;
typedef Multi3<ZDCPixmap,ZPointPOD,string8> PixmapRegName;

static
vector<ZRectPOD> spDecompose(const ZBigRegion& iRgn)
	{
	vector<ZRectPOD> result;
	iRgn.Decompose(result);
	return result;
	}

static void spDecompose(
	int iMaxDim, int iBaseDim, const PixmapRegName& iPRN, vector<PixmapRegName>& ioPRNs)
	{
	vector<ZRectPOD> theRects;

	// Quantized region.
	{
	ZBigRegionAccumulator quant;
	const int q = iBaseDim;
	const ZBigRegion theRgn = spRegion(iPRN.f0);
	const ZRectPOD theBounds = theRgn.Bounds();
	const vector<ZRectPOD> decomposed = spDecompose(theRgn);
	foreachv (const ZRectPOD& theRect, decomposed)
		{
		const ZRectPOD current = sRectPOD(
			(L(theRect) / q) * q, (T(theRect) / q) * q,
			(R(theRect + q - 1) / q) * q, (B(theRect + q - 1) / q) * q);
		
		quant.Include(theBounds & current);
		}

	quant.Get().Decompose(theRects);
	}

	// We have a list of rectangles within the source.
	// Now let's break up any that are too big.
	vector<ZRectPOD> smallRects;
	foreachv (const ZRectPOD& theRect, theRects)
		{
		for (int theTop = T(theRect); theTop < B(theRect); /*no inc*/)
			{
			const int theBottom = sMin(theTop + iMaxDim, B(theRect));
			for (int theLeft = L(theRect); theLeft < R(theRect); /*no inc*/)
				{
				const int theRight = sMin(theLeft + iMaxDim, R(theRect));
				smallRects.push_back(sRectPOD(theLeft, theTop, theRight, theBottom));
				theLeft = theRight;
				}
			theTop = theBottom;
			}
		}

	// Append chunks to ioPRNs
	foreachv (const ZRectPOD& theRect, smallRects)
		{		
		ioPRNs.push_back(
			PixmapRegName(ZDCPixmap(iPRN.f0, theRect), iPRN.f1 - LT(theRect), iPRN.f2));
		}
	}

static
ZDCPixmap spRescale(const ZDCPixmap& iPixmap, Rat iScale)
	{
	ZP<CGColorSpaceRef> theColorSpace = sAdopt& ::CGColorSpaceCreateDeviceRGB();

	const ZPointPOD sourceSize = iPixmap.Size();

	ZDCPixmapNS::RasterDesc theRD = iPixmap.GetRasterDesc();

	const int theDepth = theRD.fPixvalDesc.fDepth;
	ZAssert(theDepth == 24 || theDepth == 32);

	ZP<CGDataProviderRef> theSourceProvider = sAdopt& ::CGDataProviderCreateWithData(
		nullptr, iPixmap.GetBaseAddress(), theDepth/8 * X(sourceSize), nullptr);

	ZP<CGImageRef> theSourceImageRef = sAdopt& ::CGImageCreate(
		X(sourceSize), Y(sourceSize),
		8, theDepth,
		iPixmap.GetRasterDesc().fRowBytes,
		theColorSpace,
		theDepth == 32 ? kCGImageAlphaLast : kCGImageAlphaNone,
		theSourceProvider,
		nullptr, true, kCGRenderingIntentDefault);

	using Cartesian::operator *;
	const ZPointPOD targetSize = sourceSize * iScale;
	ZDCPixmap target(targetSize, ZDCPixmapNS::eFormatStandard_RGBA_32, ZRGBA(0,0,0,0));

	ZP<CGContextRef> targetCG = sAdopt& ::CGBitmapContextCreate(
		target.GetBaseAddress(),
		X(targetSize), Y(targetSize),
		8,
		target.GetRasterDesc().fRowBytes,
		theColorSpace, 
		kCGImageAlphaPremultipliedLast);
		
	::CGContextDrawImage(targetCG, sRect<CGRect>(targetSize), theSourceImageRef);
	
	return target;
	}

static
void spGrindArt(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap)
	{
	const int maxSheetDim = iBaseDim << iDoublings;

	const string8 prefix = iFS.Name();
	if (ZLOGF(w, eDebug))
		w << "Processing Art: " << prefix;

	map<string8,Map_Any> theName2Stuff;
	
	vector<PixmapRegName> thePRNs;

	for (FileIter iter = iFS; iter; iter.Advance())
		{
		const FileSpec current = iter.Current();
		if (ZQ<string8> theNameQ = sQWithoutSuffix(".png", current.Name()))
			{
			const string8 theName = *theNameQ;

			if (ZLOGF(w, eDebug))
				w << "Processing file: " << current.Name();

			if (ZP<ZStreamerR> theStreamerR = current.OpenR())
				{
				ZDCPixmap thePM = sPixmap_PNG(theStreamerR);
					
				if (iScale != 1.0)
					thePM = spRescale(thePM, iScale);

				thePRNs.push_back(PixmapRegName(thePM, sPointPOD(0), theName));

				Map_Any theStuff = sGet(sQReadMap_Any(current.Sibling(theName + ".txt")));
				if (iScale != 1.0)
					{
					const CVec3 baseReg = sCVec3(0, theStuff["Reg"]);
					theStuff["Reg"] = spAsVal(baseReg * iScale);

					const CVec3 baseScale = sCVec3(1, theStuff["Scale"]);
					theStuff["Scale"] = spAsVal(baseScale / iScale);

					const GRect baseInset = sQGRect(theStuff["Inset"]).DGet(GRect());
					theStuff["Inset"] = spAsVal(baseInset * iScale);
					}
				theName2Stuff[theName] = theStuff;				
				}
			}
		}
	
	vector<PixmapRegName> newPRNs;
	foreachv (const PixmapRegName& thePRN, thePRNs)
		spDecompose(maxSheetDim - kPadding*2, iBaseDim, thePRN, newPRNs);
	
	// Do a sort of newPRNS...
	{
	struct Compare_PRN
		{
		bool operator()(const PixmapRegName& iL, const PixmapRegName& iR)
			{
			if (int compare = ZooLib::sCompare_T<int>(X(iL.f0.Size()), X(iR.f0.Size())))
				return compare > 0;
			return Y(iL.f0.Size()) > Y(iR.f0.Size());
			}
		};

	sort(newPRNs.begin(), newPRNs.end(), Compare_PRN());
	}
	
	if (newPRNs.size())
		{
		for (int theSheetNumber = 0; /*no test*/; ++theSheetNumber)
			{
			vector<PixmapRegName> failed;

			const string8 theSheetName = "Art_" + prefix + sStringf("_%02d", theSheetNumber);

			Tiler theTiler(
				ZDCPixmap(
					sPointPOD(maxSheetDim),
					ZDCPixmapNS::eFormatEfficient_Color_32,
					ZRGBA(0,0)),
				kPadding, kPadding);

			foreacha (entry, newPRNs)
				{
				if (ZQ<ZRectPOD> theBounds = theTiler.Insert(entry.f0))
					{
					Seq_Any& thing1 = theName2Stuff[entry.f2]["Frames"].Mut<Seq_Any>();
					Seq_Any& thing2 = thing1[0].Mut<Seq_Any>();
					
					Map_Any theMeta;
					theMeta["Bounds"] = spAsVal(*theBounds);
					theMeta["Reg"] = spAsVal(entry.f1);
					theMeta["SheetName"] = theSheetName;
					thing2.Append(theMeta);
					}
				else
					{
					if (ZLOGF(w, eDebug+1))
						w << "Failure with sheet: " << theSheetName << ", art: " << entry.f2;
					failed.push_back(entry);
					}
				}
			
			if (failed.size() == newPRNs.size())
				{
				// Failure, and we didn't make any progress on this iteration.
				if (ZLOGF(w, eDebug+1))
					w << "Hard failure";
				break;
				}

			if (ZP<ZStreamerW> theSW = sCreateW_Truncate(iFS_Sheets.Child(theSheetName + ".png")))
				ZDCPixmapEncoder_PNG::sWritePixmap(*theSW, theTiler.GetPixmap());

			if (failed.empty())
				{
				// Success.
				break;
				}

			newPRNs = failed;
			}
		
		foreacha (entry, theName2Stuff)
			ioMap[entry.first] = entry.second;
		}
	}

void sGrindArt(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap)
	{
	Map_Any theMap;
	// First do the top level
	spGrindArt(iScale, iBaseDim, iDoublings, iFS, iFS_Sheets, ioMap);

	// Then the clustered stuff.
	for (FileIter iter = iFS; iter; iter.Advance())
		{
		const FileSpec& current = iter.Current().Follow();
		if (current.IsDir())
			spGrindArt(iScale, iBaseDim, iDoublings, iter.Current(), iFS_Sheets, ioMap);
		}
	}

// =================================================================================================
// MARK: -
static
void spGrindAnim(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap)
	{
	const int maxSheetDim = iBaseDim << iDoublings;

	map<string8,FileSpec> theFiles;
	Map_Any entry;
	if (not sReadAnim(iFS, theFiles, entry))
		return;

	const string8 prefix = iFS.Name();

	if (ZLOGF(w, eDebug))
		w << "Processing Anim: " << prefix;

	if (iScale != 1.0)
		{
		const CVec3 baseReg = sCVec3(0, entry["Reg"]);
		entry["Reg"] = spAsVal(baseReg * iScale);

		const CVec3 baseScale = sCVec3(1, entry["Scale"]);
		entry["Scale"] = spAsVal(baseScale / iScale);

		const GRect baseInset = sQGRect(entry["Inset"]).DGet(GRect());
		entry["Inset"] = spAsVal(baseInset * iScale);
		}

	typedef Multi2<PixmapOffset,size_t> PixmapOffsetFrame;

	vector<PixmapOffsetFrame> thePOFs;

	{
	size_t frameNumber = 0;
	foreacha (entry, theFiles)
		{
		if (ZP<ZStreamerR> theStreamerR = entry.second.OpenR())
			{
			ZDCPixmap thePM = ZDCPixmapDecoder_PNG::sReadPixmap(*theStreamerR);

			if (iScale != 1.0)
				thePM = spRescale(thePM, iScale);

			thePOFs.push_back(PixmapOffsetFrame(spTrim(thePM), frameNumber++));
			}
		}
	}

	{
	struct Compare_POF
		{
		bool operator()(const PixmapOffsetFrame& iL, const PixmapOffsetFrame& iR)
			{
			if (int compare = ZooLib::sCompare_T<int>(X(iL.f0.f0.Size()), X(iR.f0.f0.Size())))
				return compare > 0;
			return Y(iL.f0.f0.Size()) > Y(iR.f0.f0.Size());
			}
		};

	sort(thePOFs.begin(), thePOFs.end(), Compare_POF());
	}

	if (thePOFs.size())
		{
		Seq_Any& theFrames = sMut<Seq_Any>(entry["Frames"]);
		for (int theSheetNumber = 0; /*no test*/; ++theSheetNumber)
			{
			vector<PixmapOffsetFrame> failed;
			const string8 theSheetName = "Sheet_" + prefix + sStringf("_%02d", theSheetNumber);

			Tiler theTiler(
				ZDCPixmap(
					sPointPOD(maxSheetDim),
					ZDCPixmapNS::eFormatEfficient_Color_32,
					ZRGBA(0,0)),
				kPadding, kPadding);

			foreacha (entry, thePOFs)
				{
				if (ZQ<ZRectPOD> theBounds = theTiler.Insert(entry.f0.f0))
					{
					Map_Any theFrame;
					theFrame["SheetName"] = theSheetName;
					theFrame["Bounds"] = spAsVal(*theBounds);
					theFrame["Reg"] = spAsVal(-sPoint<CVec3>(entry.f0.f1));
					theFrames[entry.f1] = theFrame;
					}
				else
					{
					failed.push_back(entry);
					}
				}

			if (failed.size() == thePOFs.size())
				{
				// Failure, and we didn't make any progress on this iteration.
				if (ZLOGF(w, eErr))
					w << "Hard failure";
				break;
				}

			if (ZP<ZStreamerW> theSW = sCreateW_Truncate(iFS_Sheets.Child(theSheetName + ".png")))
				ZDCPixmapEncoder_PNG::sWritePixmap(*theSW, theTiler.GetPixmap());

			if (failed.empty())
				{
				// Success.
				break;
				}

			thePOFs = failed;
			}

		ioMap[prefix] = entry;
		}
	}

void sGrindAnim(
	Rat iScale, int iBaseDim, int iDoublings,
	const FileSpec& iFS, const FileSpec& iFS_Sheets, Map_Any& ioMap)
	{
	for (FileIter iter = iFS; iter; iter.Advance())
		spGrindAnim(iScale, iBaseDim, iDoublings, iter.Current(), iFS_Sheets, ioMap);
	}

#endif // not ZMACRO_IOS && not defined(__ANDROID__)

} // namespace Util
} // namespace GameEngine
