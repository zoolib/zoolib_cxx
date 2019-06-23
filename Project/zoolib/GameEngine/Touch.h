#ifndef __ZooLib_GameEngine_Touch_h__
#define __ZooLib_GameEngine_Touch_h__ 1

#include "zoolib/GameEngine/Types.h"

#include <set>

namespace ZooLib {
namespace GameEngine {

// =================================================================================================
#pragma mark - Touch

class Touch
:	public Counted
	{
public:
	int64 fFingerID;
	CVec3 fPos;
	};

typedef std::set<ZP<Touch> > TouchSet;

// =================================================================================================
#pragma mark - TouchListener

class TouchListener
:	public Counted
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

	std::set<ZP<Touch> > fActive;
	std::set<ZP<Touch> > fDowns;
	std::set<ZP<Touch> > fMoves;
	std::set<ZP<Touch> > fUps;

private:
	GRect fBounds;

	ZQ<Mat> fMatQ;
	ZQ<Mat> fInverseMatQ;
	};

// =================================================================================================
#pragma mark -

bool sTouchIn(const ZP<TouchListener>& iTouchListener);
bool sTouchOut(const ZP<TouchListener>& iTouchListener);
bool sTouchUp(const ZP<TouchListener>& iTouchListener);

} // namespace GameEngine
} // namespace ZooLib

#endif // __ZooLib_GameEngine_Touch_h__
