#include "zoolib/ZGeometry.h"

NAMESPACE_ZOOLIB_BEGIN

void test()
{

ZGPointf thePoint;
thePoint += ZGExtentf(2, 3);

ZGRectf theRect1, theRect2;

theRect1 &= theRect2;
theRect1 |= theRect2;

theRect1 = theRect1 & theRect2;
theRect1 = theRect1 | theRect2;
}

NAMESPACE_ZOOLIB_END
