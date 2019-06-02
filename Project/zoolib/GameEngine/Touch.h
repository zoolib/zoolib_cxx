#ifndef __ZooLib_GameEngine_Touch_h__
#define __ZooLib_GameEngine_Touch_h__ 1

#include "zoolib/GameEngine/Types.h"

#include <set>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Touch

class Touch
:	public ZCounted
	{
public:
	int64 fFingerID;
	CVec3 fPos;
	};

typedef std::set<ZRef<Touch> > TouchSet;

// =================================================================================================
#pragma mark - TouchListener

class TouchListener
:	public ZCounted
	{
public:
	TouchListener(bool iExclusive);
	virtual ~TouchListener();
	
	bool Contains(CVec3 iPos);

	void SetBounds(const GRect& iBounds);
	GRect GetBoundsForDebug();

	void SetInverseMat(const Mat& iInverseMat);
	void SetMat(const Mat& iMat);

	const Mat& GetInverseMat();
	const Mat& GetMat();

	void Munge(const Mat& iMat, const Mat& iInverseMat);

	const bool fExclusive;

	size_t fMaxTouches;

	std::set<ZRef<Touch> > fActive;
	std::set<ZRef<Touch> > fDowns;
	std::set<ZRef<Touch> > fMoves;
	std::set<ZRef<Touch> > fUps;

private:
	GRect fBounds;

	ZQ<Mat> fMatQ;
	ZQ<Mat> fInverseMatQ;
	};

// =================================================================================================
#pragma mark -

bool sTouchIn(const ZRef<TouchListener>& iTouchListener);
bool sTouchOut(const ZRef<TouchListener>& iTouchListener);
bool sTouchUp(const ZRef<TouchListener>& iTouchListener);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Touch_h__
