#include "zoolib/GameEngine/Touch.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - TouchListener

TouchListener::TouchListener(bool iExclusive)
:	fExclusive(iExclusive)
,	fMaxTouches(1)
	{}

TouchListener::~TouchListener()
	{}

void TouchListener::SetInverseMat(const Mat& iInverseMat)
	{
	fInverseMatQ = iInverseMat;
	fMatQ.Clear();
	}

void TouchListener::SetMat(const Mat& iMat)
	{
	fMatQ = iMat;
	fInverseMatQ.Clear();
	}

const Mat& TouchListener::GetInverseMat()
	{
	if (not fInverseMatQ)
		fInverseMatQ = sInverse(*fMatQ);
	return *fInverseMatQ;
	}

const Mat& TouchListener::GetMat()
	{
	if (not fMatQ)
		fMatQ = sInverse(*fInverseMatQ);
	return *fMatQ;
	}

//void TouchListener::Munge(const Mat& iMat, const Mat& iInverseMat)
//	{
//	if (fMatQ)
//		*fMatQ = *fMatQ * iMat;
//
//	if (fInverseMatQ)
//		*fInverseMatQ = iInverseMat * *fInverseMatQ;
//	}

// =================================================================================================
#pragma mark -

bool sTouchIn(const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fActive.empty())
		return false;

	if (iTouchListener->fUps.size())
		return false;

	ZRef<Touch> theTouch = *iTouchListener->fActive.begin();
	const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
	if (sContains(iTouchListener->fBounds, localPos))
		return true;

	return false;
	}

bool sTouchOut(const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fActive.empty())
		return false;

	ZRef<Touch> theTouch = *iTouchListener->fActive.begin();
	const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
	if (sContains(iTouchListener->fBounds, localPos))
		return false;

	return true;
	}

bool sTouchUp(const ZRef<TouchListener>& iTouchListener)
	{
	if (iTouchListener->fUps.size())
		{
		ZRef<Touch> theTouch = *iTouchListener->fUps.begin();
		const CVec3 localPos = iTouchListener->GetMat() * theTouch->fPos;
		if (sContains(iTouchListener->fBounds, localPos))
			return true;
		}
	return false;
	}

} // namespace GameEngine
} // namespace ZooLib
