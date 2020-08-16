// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Dee.h"
#include "zoolib/QueryEngine/Walker_Product.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark - Walker_Product

Walker_Product::Walker_Product(const ZP<Walker>& iWalker_Left, const ZP<Walker>& iWalker_Right)
:	fWalker_Left(iWalker_Left)
,	fWalker_Right(iWalker_Right)
,	fNeedLoadLeft(true)
	{}

Walker_Product::~Walker_Product()
	{}

void Walker_Product::Rewind()
	{
	this->Called_Rewind();
	fWalker_Left->Rewind();
	fNeedLoadLeft = true;
	}

ZP<Walker> Walker_Product::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	std::map<string8,size_t> leftOffsets;
	fWalker_Left = fWalker_Left->Prime(iOffsets, leftOffsets, ioBaseOffset);

	fResults_Left.resize(ioBaseOffset);
	oOffsets.insert(leftOffsets.begin(), leftOffsets.end());

	map<string8,size_t> combined = iOffsets;
	combined.insert(leftOffsets.begin(), leftOffsets.end());

	fWalker_Right = fWalker_Right->Prime(combined, oOffsets, ioBaseOffset);

	if (not fWalker_Left || not fWalker_Right)
		return null;

	if (fWalker_Left.DynamicCast<Walker_Dee>())
		return fWalker_Right;
	else if (fWalker_Right.DynamicCast<Walker_Dee>())
		return fWalker_Left;

	return this;
	}

bool Walker_Product::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	for (;;)
		{
		if (fNeedLoadLeft)
			{
			fWalker_Right->Rewind();
			fNeedLoadLeft = false;

			if (not fWalker_Left->QReadInc(ioResults))
				return false;

			std::copy_n(ioResults, fResults_Left.size(), fResults_Left.begin());
			}
		else
			{
			std::copy(fResults_Left.begin(), fResults_Left.end(), ioResults);
			}

		if (fWalker_Right->QReadInc(ioResults))
			return true;

		fNeedLoadLeft = true;
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
