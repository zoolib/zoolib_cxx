#ifndef __ZooLib_UIKit_TVCell_h__
#define __ZooLib_UIKit_TVCell_h__ 1
#include "zconfig.h"

#import <UIKit/UITableView.h>

#include "zoolib/Apple/ZRef_NS.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {
namespace UIKit {

template <class CellClass_t>
ZRef<CellClass_t> spGetCell_T(UITableView* iView, bool iReusable = true)
	{
	ZRef<CellClass_t> theCell;
	NSString* theReuseIdentifier = nil;

	if (iReusable)
		{
		theReuseIdentifier = NSStringFromClass([CellClass_t class]);
		if (iView)
			theCell = (CellClass_t*)[iView dequeueReusableCellWithIdentifier:theReuseIdentifier];
		}

	if (not theCell)
		{
		theCell = Adopt_T<CellClass_t>([CellClass_t alloc]);

		theCell = [theCell
			initWithStyle:UITableViewCellStyleDefault
			reuseIdentifier:theReuseIdentifier];
		}

	if ([theCell respondsToSelector:@selector(setTableView:)])
		[theCell setTableView:iView];

	return theCell;
	}

ZRef<UITableViewCell> sGetCell_Simple(UITableView* iView, NSString* iText, bool iReusable = true);

} // namespace UIKit
} // namespace ZooLib

#endif // __ZooLib_UIKit_TVCell_h__
