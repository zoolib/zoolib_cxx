#include "UIViewController+Subtitle.h"

#include "zoolib/Apple/Cartesian_CG.h"
#include "zoolib/Apple/ZRef_NS.h"

#import <Foundation/NSInvocation.h>
#import <UIKit/UIKit.h>

using namespace ZooLib;

namespace { // anonymous

ZRef<UILabel> spMakeUILabel()
	{
	UILabel* label = [[UILabel alloc] initWithFrame:CGRectZero];
	label.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	label.backgroundColor = [UIColor clearColor];
	label.textAlignment = NSTextAlignmentCenter;
	if (false && UIUserInterfaceIdiomPhone == UI_USER_INTERFACE_IDIOM())
		{
		label.textColor = [UIColor whiteColor];
		label.shadowColor = [UIColor darkGrayColor];
		label.shadowOffset = CGSizeMake(0, -1);
		}
	else
		{
		label.textColor = [UIColor darkGrayColor];
		label.shadowColor = [UIColor whiteColor];
		label.shadowOffset = CGSizeMake(0, 1);
		}
	label.minimumFontSize = 8;
	label.adjustsFontSizeToFitWidth = YES;
	return sAdopt& label;
	}

struct Views_t
	{
	ZRef<UIView> fWrapper;
	ZRef<UILabel> fTitle;
	ZRef<UILabel> fSubtitle;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark UIViewController+Subtitle

@implementation UIViewController (Subtitle)

- (void)pUpdateTitleLayout:(Views_t)iViews;
	{
	const CGRect frame = [iViews.fWrapper bounds];

	if ([[iViews.fSubtitle text] length])
		{
		[iViews.fSubtitle setFrame:sWithTB(0, 20, frame)];
		[iViews.fTitle setFrame:sWithTB(B(frame) - 28, B(frame) - 4, frame)];
		}
	else
		{
		[iViews.fTitle setFrame:sCenteredY(CY(frame), sWithHT(24, frame))];
		}
	}

- (Views_t)pGetViews
	{
	if (UIView* theWrapper = [self.navigationItem titleView])
		{
		if (NSArray* theSubviews = [theWrapper subviews])
			{
			if ([theSubviews count] == 2)
				{
				if (UIView* theTitle = [theSubviews objectAtIndex:0])
					{
					if ([theTitle isKindOfClass:[UILabel class]])
						{
						if (UIView* theSubtitle = [theSubviews objectAtIndex:1])
							{
							if ([theSubtitle isKindOfClass:[UILabel class]])
								{
								Views_t result;
								result.fWrapper = theWrapper;
								result.fTitle = (UILabel*)theTitle;
								result.fSubtitle = (UILabel*)theSubtitle;
								return result;
								}
							}
						}
					}
				}
			}
		}

	const CGRect frame = sRect<CGRect>(4000, 44);
	ZRef<UIView> theTitleWrapper = sAdopt& [[UIView alloc] initWithFrame:frame];
	[theTitleWrapper setBackgroundColor:[UIColor clearColor]];
	[theTitleWrapper setAutoresizingMask:UIViewAutoresizingFlexibleWidth];

	ZRef<UILabel> theLabel_Title = spMakeUILabel();
	[theLabel_Title setText:[self title]];
	[theLabel_Title setFont:[UIFont boldSystemFontOfSize:20]];
	[theTitleWrapper addSubview:theLabel_Title];

	ZRef<UILabel> theLabel_Subtitle = spMakeUILabel();
	[theLabel_Subtitle setFont:[UIFont boldSystemFontOfSize:13]];
	[theTitleWrapper addSubview:theLabel_Subtitle];

	[self.navigationItem setTitleView:theTitleWrapper];

	Views_t result;
	result.fWrapper = theTitleWrapper;
	result.fTitle = theLabel_Title;
	result.fSubtitle = theLabel_Subtitle;
	return result;
	}

- (NSString*)pTitleAt:(NSUInteger)iIndex
	{
	if (UIView* theWrapper = [self.navigationItem titleView])
		{
		if (NSArray* theSubviews = [theWrapper subviews])
			{
			if ([theSubviews count] > 1)
				{
				UIView* theUILabel = [theSubviews objectAtIndex:iIndex];
				if ([theUILabel isKindOfClass:[UILabel class]])
					return [(UILabel*)theUILabel text]; 
				}
			}
		}
	return @" ";
	}

- (UILabel*)specialTitleUILabel
	{
	Views_t theViews = [self pGetViews];
	return theViews.fTitle;
	}

- (NSString*)specialTitle
	{ return [self pTitleAt:0]; }

- (void)setSpecialTitle:(NSString*)iTitle
	{
	[self setTitle:iTitle];
	Views_t theViews = [self pGetViews];
	[theViews.fTitle setText:iTitle];
	[self pUpdateTitleLayout:theViews];
	}

- (UILabel*)subtitleUILabel
	{
	Views_t theViews = [self pGetViews];
	return theViews.fSubtitle;
	}

- (NSString*)subtitle
	{ return [self pTitleAt:1]; }

- (void)setSubtitle:(NSString*)iSubtitle
	{
	Views_t theViews = [self pGetViews];
	[theViews.fSubtitle setText:iSubtitle];
	[self pUpdateTitleLayout:theViews];
	}

@end // implementation UIViewController (Subtitle)
