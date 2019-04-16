#ifndef __ZooLib_GameEngine_UIView_Game_h__
#define __ZooLib_GameEngine_UIView_Game_h__ 1

#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#include "zoolib/GameEngine/Touch.h"

#include <map>

#include <QuartzCore/CAEAGLLayer.h>
#include <UIKit/UIView.h>

// =================================================================================================
#pragma mark -

using ZooLib::ZRef;
using ZooLib::GameEngine::Touch;
using ZooLib::GameEngine::TouchSet;

typedef std::map<UITouch*,ZRef<Touch> > TouchMap;

namespace ZooLib {
namespace GameEngine {

} // namespace ZooLib
} // namespace GameEngine

// =================================================================================================
#pragma mark -

@interface UIView_Game : UIView
	{
@public
	GLint fBackingWidth;
	GLint fBackingHeight;

	ZooLib::ZRef<EAGLContext> fContext;
	GLuint fViewRenderbuffer;
	GLuint fViewFramebuffer;
	}

-(id)initWithFrame:(CGRect)frame;
- (id)initGLES;
- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;

- (CGPoint)backingSize;
- (void)beforeDraw;
- (void)afterDraw;

- (TouchSet)processTouches:(NSSet*)touches withTouchMap:(TouchMap&) ioTouchMap erasingFromMap:(bool)erasing gameSize:(CGPoint)iGameSize;

@end // UIView_Game interface

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __ZooLib_GameEngine_UIView_Game_h__
