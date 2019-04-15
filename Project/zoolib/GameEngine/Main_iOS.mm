#include "zoolib/Starter_ThreadLoop.h"
#include "zoolib/StdIO.h"
#include "zoolib/Util_Debug.h"
#include "zoolib/Util_STL_map.h"

#include "zoolib/Apple/Callable_ObjC.h"
#include "zoolib/Apple/Cartesian_CG.h"
#include "zoolib/Apple/Map_CFPreferences.h"
#include "zoolib/Apple/Util_CF_Any.h"
#include "zoolib/Apple/Util_NS.h"
#include "zoolib/Apple/Val_NS.h"
#include "zoolib/Apple/ZRef_NS.h"

#include "zoolib/GameEngine/Game.h"
#include "zoolib/GameEngine/Sound_CoreAudio.h"
#include "zoolib/GameEngine/Texture_GL.h"
#include "zoolib/GameEngine/UIView_Game.h"

#include "ZenMox/ZenMox.h"
#include "ZenMox/Nook_Melange.h"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>

#include <UIKit/UIScreen.h>
#include <UIKit/UIViewController.h>
#include <UIKit/UIWindow.h>


// =================================================================================================
#pragma mark -

using std::map;
using std::vector;

using namespace ZenMox;
using namespace Zen;

using namespace Util_STL;

static const int kUpdateRate_Draw = 2;

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

//static
//ZQ<FileSpec> spQHomeDirResourceFS()
//	{
//	ZRef<NSString> theString = sAdopt& @"/Users/ag/GlassCeiling_Assets";//##??
//	if (theString)
//		{
//		const FileSpec theFileSpec = FileSpec::sRoot().Follow(Util_NS::sAsUTF8(theString));
//		if (theFileSpec.IsDir())
//			return theFileSpec;
//		}
//	return null;
//	}

FileSpec spResourceFS()
	{
//	if (ZQ<FileSpec> theQ = spQHomeDirResourceFS())
//		return *theQ;

	if (ZQ<FileSpec> theQ = spQAsFileSpec(
		sAdopt& ::CFBundleCopyResourcesDirectoryURL(::CFBundleGetMainBundle())))
		{ return theQ->Child("assets"); }

	return FileSpec();
	}

} // anonymous namespace


// =================================================================================================
#pragma mark - UIViewController_ZooLibGame

@interface UIViewController_ZooLibGame : UIViewController
	{
@public
	ZRef<UIView> fView_Outer;
	ZRef<UIView_Game> fGCView;
	ZRef<CADisplayLink> fDisplayLink;

	ZRef<Callable_Void> fCallable_RunGame;
	ZRef<Starter> fStarter;

	ZRef<Game> fGame;
	ZRef<SoundMeister_CoreAudio> fSoundMeister;
	ZRef<Nook_Melange> fNook_Melange;
//	ZRef<Nook_Advert> fNook_Advert;
//	ZRef<Nook_Globals> fNook_Globals;

	ZMtx fMtx_Draw;

	double fTime_Prior;
	double fTime_LastEntry;

	TouchMap fTouchMap;
	}

- (void)pDrawView;

@end // UIViewController_ZooLibGame interface

@implementation UIViewController_ZooLibGame

- (id)init
	{
	self = [super init];

	[self layoutAnimated:NO];

	return self;
	}

- (void)layoutAnimated:(BOOL)animated
	{}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
	{
	[super touchesBegan:touches withEvent:event];

	TouchSet theSet = [fGCView processTouches:touches withTouchMap:fTouchMap erasingFromMap:false];

	fGame->UpdateTouches(&theSet, nullptr, nullptr);
	}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesMoved:touches withEvent:event];

	TouchSet theSet = [fGCView processTouches:touches withTouchMap:fTouchMap erasingFromMap:false];

	fGame->UpdateTouches(nullptr, &theSet, nullptr);
	}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
	{
	[super touchesEnded:touches withEvent:event];

	TouchSet theSet = [fGCView processTouches:touches withTouchMap:fTouchMap erasingFromMap:true];

	fGame->UpdateTouches(nullptr, nullptr, &theSet);
	}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesCancelled:touches withEvent:event];

	TouchSet theSet = [fGCView processTouches:touches withTouchMap:fTouchMap erasingFromMap:true];

	fGame->UpdateTouches(nullptr, nullptr, &theSet);
	}

-(void)purge
	{ fGame->Purge(); }

- (void)pRunGame
	{
	[fGCView ensureIndependentContextExistsForThisThread];

	ZThread::sSetName("RunGame");

	ZRef<NSAutoreleasePool> pool = sAdopt& [[NSAutoreleasePool alloc] init];
	fGame->RunOnce(
		sPoint<GPoint>([fGCView backingSize]),
		sPoint<GPoint>(kWidth, kHeight));

	sQStart(fStarter, fCallable_RunGame);
	}

- (void)pDrawView
	{
	//## Log this, see if it's called before/during black screen?
	ZLOGTRACE(eDebug);
	[fGCView beforeDraw];
	fGame->Draw(
		Time::sSystem(),
		sPoint<GPoint>([fGCView backingSize]),
		sPoint<GPoint>([fGCView gameSize]),
		[fGCView->fContext API] >= kEAGLRenderingAPIOpenGLES2,
		sCallable<void()>(fGCView, @selector(afterDraw)));
	}

- (void)loadView
	{
	CGRect mainScreenBounds = [[UIScreen mainScreen] bounds];
	if (mainScreenBounds.size.height > mainScreenBounds.size.width)
		{
		// Urg. Force landscape. Not needed when GCView is top level.
		swap(mainScreenBounds.size.height, mainScreenBounds.size.width);
		}

	fView_Outer = sAdopt& [[UIView alloc] initWithFrame:mainScreenBounds];

	CGPoint gameSize = sPoint<CGPoint>(kWidth, kHeight);

	fGCView = sAdopt& [[UIView_Game alloc] initWithFrame:mainScreenBounds andGameSize:gameSize];

	[fView_Outer addSubview:fGCView];

	self.view = fView_Outer;

	fTime_LastEntry = 0;

	FileSpec resourceFS = spResourceFS();

	fSoundMeister = new SoundMeister_CoreAudio(resourceFS.Child("audio"));

	fGame = new Game(
		resourceFS,
		sCallable_TextureFromPixmap_GL(),
		true, [fGCView preferSmallArt],
		fSoundMeister);

//	fNook_Globals = new Nook_Globals(fGame->GetNookScope());

//	fNook_Globals->fLevelMax = sDRat(0, Util_CF::sAsAny(Map_CFPreferences().Get("LevelMax")));

	fDisplayLink =
		sAdopt& [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(pDrawView)];

	[fDisplayLink setFrameInterval:kUpdateRate_Draw];

	[fDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

	fCallable_RunGame = sCallable<void()>(self, @selector(pRunGame));
	fStarter = sStarter_ThreadLoop("RunGame");

	Melange_t theMelange = sMakeMelange(fStarter, null);

	fNook_Melange = new Nook_Melange(fGame->GetNookScope(), theMelange);

	sQStart(fStarter, fCallable_RunGame);
	}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
	{ return UIInterfaceOrientationIsLandscape(interfaceOrientation); }

-(void)didEnterBackground
	{
	[fDisplayLink setPaused:YES];
	}

-(void)willEnterForeground
	{
	[fDisplayLink setPaused:NO];
	}

@end // UIViewController_ZooLibGame implementation

// =================================================================================================
#pragma mark - UIApplicationDelegate_ZooLibGame

@interface UIApplicationDelegate_ZooLibGame : UIResponder <UIApplicationDelegate>
	{
	ZRef<UIViewController_ZooLibGame> fUIViewController_ZooLibGame;
	}

@property (strong, nonatomic) UIWindow *window;

@end // UIApplicationDelegate_ZooLibGame interface

@implementation UIApplicationDelegate_ZooLibGame

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
	{ [fUIViewController_ZooLibGame purge]; }

- (BOOL)application:(UIApplication *)application
	didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
	{
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

	fUIViewController_ZooLibGame = sAdopt& [[UIViewController_ZooLibGame alloc] init];

	[self.window setRootViewController:fUIViewController_ZooLibGame];

	[self.window makeKeyAndVisible];

	// Is this the point at which we're showing black? Force it to go black? Use some other
	// color as the basic view background color?

	return YES;
	}

- (void)applicationDidEnterBackground:(UIApplication *)application
	{
	[fUIViewController_ZooLibGame didEnterBackground];
	}

- (void)applicationWillEnterForeground:(UIApplication *)application
	{
	[fUIViewController_ZooLibGame willEnterForeground];
	}


- (void)applicationWillResignActive:(UIApplication *)application {
	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
	// Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end // UIApplicationDelegate_ZooLibGame implementation

// =================================================================================================
#pragma mark - main

int main(int argc, char *argv[])
	{
	Util_Debug::sInstall();
	if (ZCONFIG_Debug)
		Util_Debug::sSetLogPriority(Log::eDebug-1);
	else
		Util_Debug::sSetLogPriority(Log::eWarning);

	@autoreleasepool
		{
		return UIApplicationMain(argc, argv, nil, NSStringFromClass([UIApplicationDelegate_ZooLibGame class]));
		}
	}
