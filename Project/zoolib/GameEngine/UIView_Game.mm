#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"

#include "zoolib/Apple/Cartesian_CG.h"
#include "zoolib/Apple/Val_NS.h"

#include "zoolib/GameEngine/RenderGame.h" // For GameEngine::sPixelToGame
#include "zoolib/GameEngine/Types.h"
#include "zoolib/GameEngine/UIView_Game.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

#include <UIKit/UIScreen.h>

using namespace ZooLib;
using namespace Util_STL;

using GameEngine::CVec3;

// =================================================================================================
#pragma mark - UIView_Game

@implementation UIView_Game

+ (Class) layerClass
	{ return [CAEAGLLayer class]; }

- (TouchSet)processTouches:(NSSet*)touches
	withTouchMap:(TouchMap&) ioTouchMap
	erasingFromMap:(bool)erasing
	gameSize:(GPoint)iGameSize
	{
	TouchSet result;
	NSEnumerator* e = [[touches allObjects] objectEnumerator];
	for (id ob = [e nextObject]; ob; ob = [e nextObject])
		{
		UITouch* theUITouch = (UITouch*)ob;
		if (theUITouch.view != self)
			continue;

		ZRef<Touch> theTouch;
		if (ZQ<ZRef<Touch> > theQ = sQGet(ioTouchMap, theUITouch))
			{
			theTouch = *theQ;
			}
		else
			{
			theTouch = new Touch;
			sInsertMust(ioTouchMap, theUITouch, theTouch);
			}

		sInsertMust(result, theTouch);

		if (erasing)
			sEraseMust(ioTouchMap, theUITouch);

		const GPoint locationInView = sPoint<GPoint>([theUITouch locationInView:self]) * self.contentScaleFactor;
		const GPoint pixelSize = sPoint<GPoint>(fBackingWidth, fBackingHeight);
		theTouch->fPos = sHomogenous(GameEngine::sPixelToGame(pixelSize, iGameSize, locationInView));
		}
	return result;
	}

-(id)initWithFrame:(CGRect)frame
	{
	[super initWithFrame:frame];

	// Opt in to the physical scaling on retina screens.
	self.contentScaleFactor = [[UIScreen mainScreen] scale];

	[self setMultipleTouchEnabled:YES];

	[self initGLES];

	return self;
	}

-(id)initGLES
	{
	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;

	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = Map_NS()
		.Set(kEAGLDrawablePropertyRetainedBacking, false)
		.Set(kEAGLDrawablePropertyColorFormat, kEAGLColorFormatRGBA8);

	fContext = sAdopt& [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];

	if (not fContext)
		fContext = sAdopt& [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

	if (not fContext)
		fContext = sAdopt& [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

	[EAGLContext setCurrentContext:fContext];

	return self;
	}

-(GPoint)backingSize
	{
	return sPoint<GPoint>(fBackingWidth, fBackingHeight);
	}

-(void)layoutSubviews
	{
	[EAGLContext setCurrentContext:fContext];
	[self destroyFramebuffer];
	[self createFramebuffer];
	}

- (BOOL)createFramebuffer
	{
	::glGenFramebuffers(1, &fViewFramebuffer);
	::glBindFramebuffer(GL_FRAMEBUFFER, fViewFramebuffer);

	::glGenRenderbuffers(1, &fViewRenderbuffer);
	::glBindRenderbuffer(GL_RENDERBUFFER, fViewRenderbuffer);

	[fContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(id<EAGLDrawable>)self.layer];

	::glFramebufferRenderbuffer(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, fViewRenderbuffer);

	::glGetRenderbufferParameteriv(
		GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &fBackingWidth);

	::glGetRenderbufferParameteriv(
		GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &fBackingHeight);

	if (::glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
		NSLog(@"failed to make complete framebuffer object %x",
			glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return NO;
		}

	return YES;
	}

- (void)destroyFramebuffer
	{
	::glDeleteFramebuffers(1, &fViewFramebuffer);
	fViewFramebuffer = 0;

	::glDeleteRenderbuffers(1, &fViewRenderbuffer);
	fViewRenderbuffer = 0;
	}

- (void)ensureIndependentContextExistsForThisThread
	{
	if (not [EAGLContext currentContext])
		{
		EAGLContext* theContext = [[EAGLContext alloc] initWithAPI:[fContext API] sharegroup:[fContext sharegroup]];
		[EAGLContext setCurrentContext:theContext];
		}
	}

- (void)beforeDraw
	{
	[EAGLContext setCurrentContext:fContext];

	::glBindFramebuffer(GL_FRAMEBUFFER, fViewFramebuffer);

	ZAssert(::glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

- (void)afterDraw
	{
	[EAGLContext setCurrentContext:fContext];

	::glBindRenderbuffer(GL_RENDERBUFFER, fViewRenderbuffer);

	[fContext presentRenderbuffer:GL_RENDERBUFFER];
	}

- (void)dealloc
	{
	[EAGLContext setCurrentContext:fContext];
	[self destroyFramebuffer];
	[EAGLContext setCurrentContext:nil];

	fContext.Clear();

	[super dealloc];
	}

@end // GCView implementation
