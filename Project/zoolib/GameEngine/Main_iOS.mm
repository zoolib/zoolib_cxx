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
#include "zoolib/GameEngine/UIView_Game.h"

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>

#include <UIKit/UIScreen.h>
#include <UIKit/UIViewController.h>
#include <UIKit/UIWindow.h>

// =================================================================================================
#pragma mark -

using std::map;
using std::vector;

using namespace ZooLib;
using namespace GameEngine;

using namespace Util_STL;

static const int kUpdateRate_Draw = 2;

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

FileSpec spResourceFS()
	{
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
	ZP<UIView> fView_Outer;
	ZP<UIView_Game> fUIView_Game;
	ZP<CADisplayLink> fDisplayLink;

	ZP<Game> fGame;

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

	TouchSet theSet = [fUIView_Game processTouches:touches withTouchMap:fTouchMap erasingFromMap:false gameSize:fGame->GetGameSize()];

	fGame->UpdateTouches(&theSet, nullptr, nullptr);
	}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesMoved:touches withEvent:event];

	TouchSet theSet = [fUIView_Game processTouches:touches withTouchMap:fTouchMap erasingFromMap:false gameSize:fGame->GetGameSize()];

	fGame->UpdateTouches(nullptr, &theSet, nullptr);
	}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
	{
	[super touchesEnded:touches withEvent:event];

	TouchSet theSet = [fUIView_Game processTouches:touches withTouchMap:fTouchMap erasingFromMap:true gameSize:fGame->GetGameSize()];

	fGame->UpdateTouches(nullptr, nullptr, &theSet);
	}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesCancelled:touches withEvent:event];

	TouchSet theSet = [fUIView_Game processTouches:touches withTouchMap:fTouchMap erasingFromMap:true gameSize:fGame->GetGameSize()];

	fGame->UpdateTouches(nullptr, nullptr, &theSet);
	}

-(void)purge
	{
	fGame->Purge();
	}

- (void)pDrawView
	{
	//## Log this, see if it's called before/during black screen?
	[fUIView_Game beforeDraw];
	fGame->Draw(
		Time::sSystem(),
		[fUIView_Game backingSize],
		sCallable<void()>(fUIView_Game, @selector(afterDraw)));
	}

- (void)loadView
	{
	CGRect mainScreenBounds = [[UIScreen mainScreen] bounds];
	if (mainScreenBounds.size.height > mainScreenBounds.size.width)
		{
		// Urg. Force landscape. Not needed when GCView is top level.
		std::swap(mainScreenBounds.size.height, mainScreenBounds.size.width);
		}

	fView_Outer = sAdopt& [[UIView alloc] initWithFrame:mainScreenBounds];

	fUIView_Game = sAdopt& [[UIView_Game alloc] initWithFrame:mainScreenBounds];

	[fView_Outer addSubview:fUIView_Game];

	self.view = fView_Outer;

	fGame = sMakeGame(spResourceFS(), false);

	fGame->Resume();

	fDisplayLink =
		sAdopt& [[UIScreen mainScreen] displayLinkWithTarget:self selector:@selector(pDrawView)];

	[fDisplayLink setFrameInterval:kUpdateRate_Draw];

	[fDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
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
	ZP<UIViewController_ZooLibGame> fUIViewController_ZooLibGame;
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
