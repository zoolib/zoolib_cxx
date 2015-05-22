#ifndef __UIViewController_Subtitle__
#define __UIViewController_Subtitle__ 1
#include "zconfig.h"

#import <UIKit/UIViewController.h>

// =================================================================================================
#pragma mark -
#pragma mark UIViewController+Subtitle

@interface UIViewController (Subtitle)

- (UILabel*)specialTitleUILabel;
- (NSString*)specialTitle;
- (void)setSpecialTitle:(NSString*)iTitle;

- (UILabel*)subtitleUILabel;
- (NSString*)subtitle;
- (void)setSubtitle:(NSString*)iSubtitle;

@end // interface UIViewController (Subtitle)

#endif // __UIViewController_Subtitle__
