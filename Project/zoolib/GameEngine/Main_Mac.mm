// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Starter_ThreadLoop.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/StdIO.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF_Operators_string.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/Apple/Callable_ObjC.h"
#include "zoolib/Apple/Cartesian_NS.h"
#include "zoolib/Apple/Map_CFPreferences.h"
#include "zoolib/Apple/Util_CF_Any.h"
#include "zoolib/Apple/Util_NS.h"
#include "zoolib/Apple/ZP_NS.h"

#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/RenderGame.h" // For GameEngine::sPixelToGame
#include "zoolib/GameEngine/Sound_CoreAudio.h"

#include "zoolib/OpenGL/Compat_OpenGL.h"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>

#include <OpenGL/CGLTypes.h>
#include <OpenGL/OpenGL.h>

#include <QuartzCore/CVDisplayLink.h>

#import <AppKit/NSApplication.h>
#import <AppKit/NSOpenGL.h>
#import <AppKit/NSOpenGLView.h>
#import <AppKit/NSScreen.h>
#import <AppKit/NSWindow.h>

// To trigger static registrations
#include "zoolib/POSIX/File_POSIX.h"
#include "zoolib/POSIX/Net_Internet_Socket.h"
#include "zoolib/POSIX/Net_Local_Socket.h"

// =================================================================================================
#pragma mark -

using std::map;
using std::vector;

using namespace ZooLib;
using namespace GameEngine;

// =================================================================================================
#pragma mark - Helpers (anonymous)

namespace {

ZQ<FileSpec> spQAsFileSpec(const ZP<CFURLRef>& iURLRef)
	{
	vector<UInt8> theBuf(1024);
	if (::CFURLGetFileSystemRepresentation(iURLRef, true, &theBuf[0], theBuf.size()))
		return FileSpec((char*)&theBuf[0]);
	return null;
	}

static FileSpec spResourceFS()
	{
	if (ZQ<FileSpec> theQ = spQAsFileSpec(
		sAdopt& ::CFBundleCopyResourcesDirectoryURL(::CFBundleGetMainBundle())))
		{ return theQ->Child("assets"); }

	return FileSpec();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - NSOpenGLView_ZooLibGame

@interface NSOpenGLView_ZooLibGame : NSOpenGLView
	{
	CVDisplayLinkRef fDisplayLink;

	GPoint fWH;

	ZP<Game> fGame;

	ZP<Touch> fCurrentTouch;
	}

@end // interface NSOpenGLView_ZooLibGame

@implementation NSOpenGLView_ZooLibGame

-(void)kill
	{
	ZLOGTRACE(eDebug);
	::CVDisplayLinkStop(fDisplayLink);
	fGame->TearDown();
	sStartOnNewThread_ProcessIsAboutToExit();
	}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;
	{
	const NSOpenGLPixelFormatAttribute attrs[] =
		{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		0
		};

	ZP<NSOpenGLPixelFormat> pf =
		sAdopt& [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

	self = [super initWithFrame:frameRect pixelFormat:pf];

	[self setWantsBestResolutionOpenGLSurface:YES];

	fGame = sMakeGame(spResourceFS(), false, null, null);

	fGame->Resume();

	return self;
	}

- (void) dealloc
	{	
	::CVDisplayLinkStop(fDisplayLink);

	::CVDisplayLinkRelease(fDisplayLink);

	[super dealloc];
	}

-(void)pFlipBuffers
	{
	NSOpenGLContext* theOpenGLContext = [self openGLContext];
	[theOpenGLContext makeCurrentContext];

	::glFlush();

	CGLContextObj theCGLContextObj = (CGLContextObj)[theOpenGLContext CGLContextObj];
	::CGLFlushDrawable(theCGLContextObj);
	}

- (void) drawView:(double)theTimestamp
	{
	[[self openGLContext] makeCurrentContext];

	fGame->Draw(
		theTimestamp,
		fWH,
		sCallable<void()>(self, @selector(pFlipBuffers)));
	}

static CVReturn spDisplayLinkCallback(
	CVDisplayLinkRef displayLink,
	const CVTimeStamp* now,
	const CVTimeStamp* outputTime,
	CVOptionFlags flagsIn,
	CVOptionFlags* flagsOut,
	void* iRefcon)
	{
	ZP<NSAutoreleasePool> pool = sAdopt& [[NSAutoreleasePool alloc] init];
	Nanoseconds theTime = ::AbsoluteToNanoseconds(*((AbsoluteTime*)&outputTime->hostTime));
	double asDouble = (*((uint64*)&theTime))/ 1e9;

	[(NSOpenGLView_ZooLibGame*)iRefcon drawView:asDouble];

	return kCVReturnSuccess;
	}

- (void) prepareOpenGL
	{
	[super prepareOpenGL];
	
	fWH = sPoint<GPoint>(WH([self convertRectToBacking:[self bounds]]));

	NSOpenGLContext* theOpenGLContext = [self openGLContext];

	[theOpenGLContext makeCurrentContext];

	[theOpenGLContext setValues:sConstPtr(GLint(1)) forParameter:NSOpenGLCPSwapInterval];

	::CVDisplayLinkCreateWithActiveCGDisplays(&fDisplayLink);

	::CVDisplayLinkSetOutputCallback(fDisplayLink, &spDisplayLinkCallback, self);

	CGLContextObj theCGLContextObj = (CGLContextObj)[theOpenGLContext CGLContextObj];

	CGLPixelFormatObj theCGLPixelFormatObj =
		(CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];

	::CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(
		fDisplayLink, theCGLContextObj, theCGLPixelFormatObj);

	::CVDisplayLinkStart(fDisplayLink);
	}

- (CVec3)pAsCVec3:(NSEvent*)iEvent
	{
	// Location in potentially scaled coords
	NSPoint inWindow = [iEvent locationInWindow];

	// The view may not be at [0,0]
	NSPoint thePoint = [self convertPoint:inWindow fromView:nil];

	// But it will be flipped
	Y(thePoint) = H([self bounds]) - Y(thePoint);

	// The game works in terms of backing pixels.
	thePoint = [self convertPointToBacking:thePoint];
	return sHomogenous(GameEngine::sPixelToGame(fWH, fGame->GetGameSize(), sPoint<GPoint>(thePoint)));
	}

- (BOOL)acceptsFirstResponder
	{
	return YES;
	}

- (void)keyDown:(NSEvent *)iEvent
	{
	if (not [iEvent isARepeat])
		{
		if (ZP<NSString> theString = [iEvent charactersIgnoringModifiers])
			{
			if ([theString length])
				{
				const unichar theChar = [theString characterAtIndex:0];
				fGame->UpdateKeyDowns(theChar);
				}
			}
		}
	}

- (void)mouseDown:(NSEvent*)iEvent
	{
	fCurrentTouch = new Touch;

	fCurrentTouch->fPos = [self pAsCVec3:iEvent];

	TouchSet theSet;
	theSet.insert(fCurrentTouch);

	fGame->UpdateTouches(&theSet, nullptr, nullptr);
	}

- (void)mouseUp:(NSEvent*)iEvent
	{
	if (fCurrentTouch)
		{
		fCurrentTouch->fPos = [self pAsCVec3:iEvent];

		TouchSet theSet;
		theSet.insert(fCurrentTouch);

		fGame->UpdateTouches(nullptr, nullptr, &theSet);

		fCurrentTouch.Clear();
		}
	}

- (void)mouseMoved:(NSEvent*)iEvent
	{
	if (fCurrentTouch)
		{
		fCurrentTouch->fPos = [self pAsCVec3:iEvent];

		TouchSet theSet;
		theSet.insert(fCurrentTouch);

		fGame->UpdateTouches(nullptr, &theSet, nullptr);
		}
	}

- (void)mouseDragged:(NSEvent*)iEvent
	{
	if (fCurrentTouch)
		{
		fCurrentTouch->fPos = [self pAsCVec3:iEvent];

		TouchSet theSet;
		theSet.insert(fCurrentTouch);

		fGame->UpdateTouches(nullptr, &theSet, nullptr);
		}
	}

@end // implementation NSOpenGLView_ZooLibGame

// =================================================================================================
#pragma mark - NSApplicationDelegate_ZooLibGame

@interface NSApplicationDelegate_ZooLibGame : NSObject
	{
@public
	ZP<NSWindow> fWindow;
	}

@end // interface NSApplicationDelegate_ZooLibGame

@implementation NSApplicationDelegate_ZooLibGame

-(id)init
	{
	self = [super init];
	return self;
	}

- (void) awakeFromNib
	{
	const NSRect screenRect = [[[NSScreen screens] objectAtIndex:0] frame];
//	Rat theHeight = sMin(800.0, H(screenRect) - 44);
	Rat theHeight = sMin(4000.0, H(screenRect) - 44);
//	Rat theHeight = sMin(1024.0, H(screenRect) - 44);
	Rat theWidth = theHeight * 9.0/16.0;
	const NSRect theBounds = sRect<NSRect>(theWidth, theHeight) + sPoint<NSPoint>(0, 44);

	fWindow = sAdopt& [[NSWindow alloc]
		initWithContentRect:sFlippedY(theBounds, H(screenRect))
		styleMask:NSTitledWindowMask
		backing:NSBackingStoreBuffered
		defer:NO];

	[fWindow setAcceptsMouseMovedEvents:YES];

	ZP<NSOpenGLView_ZooLibGame> theView = sAdopt& [[NSOpenGLView_ZooLibGame alloc] init];
	[fWindow setContentView:theView];
	[fWindow makeKeyAndOrderFront:self];
	}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
	{
	NSOpenGLView_ZooLibGame* theView = (NSOpenGLView_ZooLibGame*)[fWindow contentView];
	[theView kill];
	return NSTerminateNow;
	}

@end // implementation NSApplicationDelegate_ZooLibGame

// =================================================================================================
#pragma mark - main

using namespace Util_STL;

int main(int argc, const char *argv[])
	{
	Util_Debug::sInstall();

	if (ZCONFIG_Debug)
		Util_Debug::sSetLogPriority(Log::eDebug);
	else
		Util_Debug::sSetLogPriority(Log::eWarning);

	@autoreleasepool
		{
		return NSApplicationMain(argc, argv);
		}
	}

#endif // ZCONFIG_SPI_Enabled(MacOSX)
