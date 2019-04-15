#include "zoolib/Callable_Bind.h"
#include "zoolib/Callable_Function.h"
#include "zoolib/Starter_ThreadLoop.h"
#include "zoolib/StdIO.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF_Operators_string.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/Apple/Callable_ObjC.h"
#include "zoolib/Apple/Cartesian_NS.h"
#include "zoolib/Apple/Map_CFPreferences.h"
#include "zoolib/Apple/Util_CF_Any.h"
#include "zoolib/Apple/Util_NS.h"
#include "zoolib/Apple/ZRef_NS.h"

#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/Sound_CoreAudio.h"
#include "zoolib/GameEngine/Texture_GL.h"

#include "ZenMox/ZenMox.h"
#include "ZenMox/Nook_Keys.h"
#include "ZenMox/Nook_Melange.h"

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

#import "NSString+SymlinksAndAliases.h"

// =================================================================================================
#pragma mark -

using std::map;
using std::vector;

using namespace ZenMox;
using namespace Zen;

using Dataspace::Melange_t;

const Rat kWidth = 1024;
const Rat kHeight = 768;

// =================================================================================================
#pragma mark - Helpers (anonymous)

namespace {

ZQ<FileSpec> spQAsFileSpec(const ZRef<CFURLRef>& iURLRef)
	{
	vector<UInt8> theBuf(1024);
	if (::CFURLGetFileSystemRepresentation(iURLRef, true, &theBuf[0], theBuf.size()))
		return FileSpec((char*)&theBuf[0]);
	return null;
	}

static
ZQ<FileSpec> spQHomeDirResourceFS()
	{
	ZRef<NSString> theString = sAdopt& @"~/ZenMox_Assets";//##??
	theString = [theString stringByResolvingSymlinksAndAliases];
	if (theString)
		{
		const FileSpec theFileSpec = FileSpec::sRoot().Follow(Util_NS::sAsUTF8(theString));
		if (theFileSpec.IsDir())
			return theFileSpec;
		}
	return null;
	}

static FileSpec spResourceFS()
	{
	if (ZQ<FileSpec> theQ = spQHomeDirResourceFS())
		return *theQ;

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
	ZRef<Callable_Void> fCallable_RunGame;
	ZRef<Starter> fStarter;

	ZRef<Game> fGame;
	
//##	ZRef<Nook_Globals> fNook_Globals;
	ZRef<Nook_Keys> fNook_Keys;
	ZRef<Nook_Melange> fNook_Melange;

	ZRef<Touch> fCurrentTouch;
	ZMtx fMtx;
	vector<int> fKeys;
	}

@end // interface NSOpenGLView_ZooLibGame

@implementation NSOpenGLView_ZooLibGame

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat*)format;
	{
	const NSOpenGLPixelFormatAttribute attrs[] =
		{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		0
		};

	ZRef<NSOpenGLPixelFormat> pf =
		sAdopt& [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

	self = [super initWithFrame:frameRect pixelFormat:pf];

	FileSpec resourceFS = spResourceFS();

	ZRef<SoundMeister_CoreAudio> theSoundMeister =
		new SoundMeister_CoreAudio(resourceFS.Child("audio"));

	fGame = new Game(
		resourceFS,
		sCallable_TextureFromPixmap_GL(),
		false, false,
		theSoundMeister);

//##	fNook_Globals = new Nook_Globals(fGame->GetNookScope());
//##	fNook_Globals->fLevelMax = sDRat(0, Util_CF::sAsAny(Map_CFPreferences().Get("LevelMax")));

	fNook_Keys = new Nook_Keys(fGame->GetNookScope());

	fCallable_RunGame = sCallable<void()>(self, @selector(pRunGame));
	fStarter = sStarter_ThreadLoop("RunGame");

	Melange_t theMelange = Zen::sMakeMelange(fStarter, null);

	fNook_Melange = new Nook_Melange(fGame->GetNookScope(), theMelange);

	return self;
	}

- (void) dealloc
	{	
	::CVDisplayLinkStop(fDisplayLink);

	::CVDisplayLinkRelease(fDisplayLink);

	[super dealloc];
	}

- (void)pRunGame
	{
	NSOpenGLContext* theOpenGLContext = [self openGLContext];
	[theOpenGLContext makeCurrentContext];

	ZRef<NSAutoreleasePool> pool = sAdopt& [[NSAutoreleasePool alloc] init];

	{
	ZAcqMtx acq(fMtx);
	swap(fKeys, fNook_Keys->fKeys);
	fKeys.clear();
	}

	fGame->RunOnce(
		fWH,
		sPoint<GPoint>(kWidth, kHeight));

	ZThread::sSleep(0.05);

	sQStart(fStarter, fCallable_RunGame);
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
		sPoint<GPoint>(kWidth, kHeight),
		true,
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
	ZRef<NSAutoreleasePool> pool = sAdopt& [[NSAutoreleasePool alloc] init];
	Nanoseconds theTime = ::AbsoluteToNanoseconds(*((AbsoluteTime*)&outputTime->hostTime));
	double asDouble = (*((uint64*)&theTime))/ 1e9;

	[(NSOpenGLView_ZooLibGame*)iRefcon drawView:asDouble];

	return kCVReturnSuccess;
	}

- (void) prepareOpenGL
	{
	[super prepareOpenGL];
	
	fWH = sPoint<GPoint>(WH([self bounds]));

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

	sQStart(fStarter, fCallable_RunGame);
	}

- (CVec3)pAsCVec3:(NSEvent*)iEvent
	{
	NSPoint thePoint = [self convertPoint:[iEvent locationInWindow] fromView:nil];
	Y(thePoint) = H([self bounds]) - Y(thePoint);
	return sPoint<CVec3>(thePoint);
	}

- (BOOL)acceptsFirstResponder
	{
	return YES;
	}

- (void)keyDown:(NSEvent *)iEvent
	{
	if (not [iEvent isARepeat])
		{
		if (ZRef<NSString> theString = [iEvent charactersIgnoringModifiers])
			{
			if ([theString length])
				{
				ZAcqMtx acq(fMtx);
				const unichar theChar = [theString characterAtIndex:0];
				fKeys.push_back(theChar);
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
	ZRef<NSWindow> fWindow;
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
	const NSRect iBounds = sRect<NSRect>(kWidth, kHeight) + sPoint<NSPoint>(0, 44);
	
	fWindow = sAdopt& [[NSWindow alloc]
		initWithContentRect:sFlippedY(iBounds, H(screenRect))
		styleMask:NSTitledWindowMask
		backing:NSBackingStoreBuffered
		defer:NO];

	[fWindow setAcceptsMouseMovedEvents:YES];

	ZRef<NSOpenGLView_ZooLibGame> theView = sAdopt& [[NSOpenGLView_ZooLibGame alloc] init];
	[fWindow setContentView:theView];
	[fWindow makeKeyAndOrderFront:self];
	}

@end // implementation NSApplicationDelegate_ZooLibGame

// =================================================================================================
#pragma mark - main

int main(int argc, const char *argv[])
	{
	Util_Debug::sInstall();

	if (ZCONFIG_Debug)
		Util_Debug::sSetLogPriority(Log::eDebug + 0);
	else
		Util_Debug::sSetLogPriority(Log::eWarning);

	@autoreleasepool
		{
		return NSApplicationMain(argc, argv);
		}
	}
