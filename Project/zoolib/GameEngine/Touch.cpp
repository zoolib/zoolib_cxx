#include "zoolib/GameEngine/Touch.h"

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
// MARK: - TouchListener

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

void TouchListener::Munge(const Mat& iMat, const Mat& iInverseMat)
	{
	if (fMatQ)
		*fMatQ = *fMatQ * iMat;

	if (fInverseMatQ)
		*fInverseMatQ = iInverseMat * *fInverseMatQ;
	}

} // namespace GameEngine
} // namespace ZooLib
