/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */
#include "zoolib/uikit/UITVController_WithSections.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#include "zoolib/Callable_ObjC.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZRef_NS.h"
#include "zoolib/ZUtil_NS.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#import <UIKit/UIDevice.h>
#import <UIKit/UIGestureRecognizerSubclass.h>

#import <QuartzCore/CATransaction.h>

namespace ZooLib {
namespace UIKit {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
// MARK: - Helpers

static void spUpdatePopovers()
	{
	[[NSNotificationCenter defaultCenter]
		postNotificationName:@"UIPopoverControllerShouldMove"
		object:nil];
	}

NSArray* sMakeNSIndexPathArray(size_t iSectionIndex, size_t iBaseRowIndex, size_t iCount)
	{
	NSUInteger theIndices[2];
	theIndices[0] = iSectionIndex;
	NSMutableArray* theArray = [NSMutableArray arrayWithCapacity:iCount];
	for (size_t xx = 0; xx < iCount; ++xx)
		{
		theIndices[1] = iBaseRowIndex + xx;
		[theArray addObject:[NSIndexPath indexPathWithIndexes:&theIndices[0] length:2]];
		}
	return theArray;
	}

NSIndexSet* sMakeIndexSet(size_t iIndex)
	{ return [NSIndexSet indexSetWithIndex:iIndex]; }

static bool spIsVersion4OrLater()
	{
	static ZQ<bool> resultQ;
	if (not resultQ)
		resultQ = [[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0;
	return *resultQ;
	}

// =================================================================================================
// MARK: - Section

Section::Section(ZRef<SectionBody> iBody)
:	fHideWhenEmpty(false)
,	fBody(iBody)
	{}

ZRef<SectionBody> Section::GetBody()
	{ return fBody; }

bool Section::HideWhenEmpty()
	{ return fHideWhenEmpty; }

ZQ<CGFloat> Section::QHeaderHeight()
	{ return fHeaderHeight; }

ZQ<CGFloat> Section::QFooterHeight()
	{ return fFooterHeight; }

ZQ<string8> Section::QHeaderTitle()
	{ return fHeaderTitle; }

ZQ<string8> Section::QFooterTitle()
	{ return fFooterTitle; }

ZRef<UIView> Section::QHeaderView()
	{ return fHeaderView; }

ZRef<UIView> Section::QFooterView()
	{ return fFooterView; }

ZQ<UITableViewRowAnimation> Section::QSectionAnimation_Insert()
	{ return fSectionAnimation_Insert; }

ZQ<UITableViewRowAnimation> Section::QSectionAnimation_Delete()
	{ return fSectionAnimation_Delete; }

ZQ<UITableViewRowAnimation> Section::QSectionAnimation_Reload()
	{ return fSectionAnimation_Reload; }

UITableViewRowAnimation Section::SectionAnimation_Insert()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Insert())
		return *theQ;
	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation Section::SectionAnimation_Delete()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Delete())
		return *theQ;
	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation Section::SectionAnimation_Reload()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Reload())
		return *theQ;
	return UITableViewRowAnimationNone;
	}

// =================================================================================================
// MARK: - SectionBody::RowMeta

SectionBody::RowMeta::RowMeta()
:	fBase(0)
,	fLimit(0)
	{}

void SectionBody::RowMeta::UpdateCount(size_t iCount)
	{
	fBase += fLimit;
	fLimit = iCount;
	}

// =================================================================================================
// MARK: - SectionBody::RowUpdate

SectionBody::RowUpdate::RowUpdate(
	RowMeta& ioRowMeta, std::map<size_t, UITableViewRowAnimation>& ioMap)
:	fRowMeta(ioRowMeta)
,	fMap(ioMap)
	{}

void SectionBody::RowUpdate::Add(size_t iIndex, UITableViewRowAnimation iRowAnimation)
	{ this->AddRange(iIndex, 1, iRowAnimation); }

void SectionBody::RowUpdate::AddAll(UITableViewRowAnimation iRowAnimation)
	{ this->AddRange(0, fRowMeta.fLimit, iRowAnimation); }

void SectionBody::RowUpdate::AddRange(
	size_t iStart, size_t iCount, UITableViewRowAnimation iRowAnimation)
	{
	ZAssert(iStart <= fRowMeta.fLimit);
	if (iCount)
		{
		ZAssert(iStart + iCount <= fRowMeta.fLimit);
		while (iCount--)
			ZUtil_STL::sInsertMust(0, fMap, fRowMeta.fBase + iStart++, iRowAnimation);
		}
	}

// =================================================================================================
// MARK: - SectionBody

void SectionBody::ViewWillAppear(UITableView* iTV)
	{}

void SectionBody::ViewDidAppear(UITableView* iTV)
	{}

void SectionBody::ViewWillDisappear(UITableView* iTV)
	{}

void SectionBody::ViewDidDisappear(UITableView* iTV)
	{}

bool SectionBody::FindSectionBody(ZRef<SectionBody> iSB, size_t& ioRow)
	{ return iSB == this; }

// =================================================================================================
// MARK: - SectionBody_Concrete

SectionBody_Concrete::SectionBody_Concrete()
:	fRowAnimation_Reload(UITableViewRowAnimationNone)
,	fRowAnimation_Insert(UITableViewRowAnimationRight)
,	fRowAnimation_Delete(UITableViewRowAnimationRight)
,	fApplyAccessory(true)
	{}

bool SectionBody_Concrete::FindSectionBody(ZRef<SectionBody> iSB, size_t& ioRow)
	{
	if (iSB == this)
		return true;
	ioRow += this->NumberOfRows();
	return false;
	}

void SectionBody_Concrete::ApplyAccessory(size_t iRowIndex, ZRef<UITableViewCell> ioCell)
	{
	if ([ioCell accessoryView])
		{}
	else if (fCallable_ButtonTapped)
		{
		[ioCell setAccessoryType:UITableViewCellAccessoryDetailDisclosureButton];
		}
	else if (this->CanSelect(false, iRowIndex).DGet(false))
		{
		[ioCell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
		}

	if ([ioCell editingAccessoryView])
		{}
	else if (fCallable_ButtonTapped_Editing)
		{
		[ioCell setEditingAccessoryType:UITableViewCellAccessoryDetailDisclosureButton];
		}
	else if (this->CanSelect(true, iRowIndex).DGet(false))
		{
		[ioCell setEditingAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
		}
	}

ZQ<UITableViewCellEditingStyle> SectionBody_Concrete::QEditingStyle(size_t iRowIndex)
	{ return fEditingStyle; }

bool SectionBody_Concrete::CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex)
	{ return false; }

ZQ<bool> SectionBody_Concrete::QShouldIndentWhileEditing(size_t iRowIndex)
	{ return fShouldIndentWhileEditing; }

ZQ<CGFloat> SectionBody_Concrete::QRowHeight(size_t iRowIndex)
	{ return fRowHeight; }

ZQ<NSInteger> SectionBody_Concrete::QIndentationLevel(size_t iRowIndex)
	{ return fIndentationLevel; }

bool SectionBody_Concrete::ButtonTapped(UITVHandler_WithSections* iTVC,
	UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex)
	{
	if ([iTableView isEditing])
		{
		if (fCallable_ButtonTapped_Editing)
			{
			return fCallable_ButtonTapped_Editing->Call(
				iTVC, iTableView, iIndexPath, this, iRowIndex);
			}
		}
	else
		{
		if (fCallable_ButtonTapped)
			return fCallable_ButtonTapped->Call(iTVC, iTableView, iIndexPath, this, iRowIndex);
		}
	return false;
	}

bool SectionBody_Concrete::RowSelected(UITVHandler_WithSections* iTVC,
	UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex)
	{
	if ([iTableView isEditing])
		{
		if (fCallable_RowSelected_Editing)
			{
			return fCallable_RowSelected_Editing->Call(
				iTVC, iTableView, iIndexPath, this, iRowIndex);
			}
		}
	else
		{
		if (fCallable_RowSelected)
			return fCallable_RowSelected->Call(iTVC, iTableView, iIndexPath, this, iRowIndex);
		}
	return false;
	}

ZQ<bool> SectionBody_Concrete::CanSelect(bool iEditing, size_t iRowIndex)
	{
	if (iEditing)
		{
		if (fCallable_RowSelected_Editing)
			return true;
		}
	else
		{
		if (fCallable_RowSelected)
			return true;
		}
	return false;
	}

UITableViewRowAnimation SectionBody_Concrete::RowAnimation_Insert()
	{ return fRowAnimation_Insert; }

UITableViewRowAnimation SectionBody_Concrete::RowAnimation_Delete()
	{ return fRowAnimation_Insert; }

UITableViewRowAnimation SectionBody_Concrete::RowAnimation_Reload()
	{ return fRowAnimation_Reload; }

// =================================================================================================
// MARK: - SectionBody_SingleRow

SectionBody_SingleRow::SectionBody_SingleRow(ZRef<UITableViewCell> iCell)
:	fCell_Pending(iCell)
	{
	ZAssert(!fCell_Pending || not [fCell_Pending reuseIdentifier]);
	}

size_t SectionBody_SingleRow::NumberOfRows()
	{
	if (fCell_Current)
		return 1;
	return 0;
	}

void SectionBody_SingleRow::PreUpdate()
	{
	ZAssert(!fCell_Pending || not [fCell_Pending reuseIdentifier]);
	fCell_New = fCell_Pending;
	}

bool SectionBody_SingleRow::WillBeEmpty()
	{ return not fCell_New; }

void SectionBody_SingleRow::Update_NOP()
	{}

void SectionBody_SingleRow::Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
	RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload)
	{
	if (fCell_Current)
		ioRowMeta_Old.UpdateCount(1);

	if (fCell_New)
		ioRowMeta_New.UpdateCount(1);

	if (fCell_Current)
		{
		if (fCell_New)
			{
			if (fCell_Current != fCell_New)
				{
				fCell_Current = fCell_New;
				ioRowUpdate_Reload.Add(0, this->RowAnimation_Reload());
				}
			}
		else
			{
			ioRowUpdate_Delete.Add(0, this->RowAnimation_Delete());
			}
		}
	else
		{
		if (fCell_New)
			ioRowUpdate_Insert.Add(0, this->RowAnimation_Insert());
		}
	}

void SectionBody_SingleRow::Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New)
	{
	if (fCell_New)
		{
		ioRowMeta_New.UpdateCount(1);
		ioRowUpdate_New.Add(0, this->RowAnimation_Insert());
		}
	}

void SectionBody_SingleRow::Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old)
	{
	if (fCell_Current)
		{
		ioRowMeta_Old.UpdateCount(1);
		ioRowUpdate_Old.Add(0, this->RowAnimation_Delete());
		}
	}

void SectionBody_SingleRow::FinishUpdate()
	{ fCell_Current = fCell_New; }

ZRef<UITableViewCell> SectionBody_SingleRow::UITableViewCellForRow(
	UITableView* iView, size_t iRowIndex,
	bool& ioIsPreceded, bool& ioIsSucceeded)
	{
	if (fApplyAccessory)
		this->ApplyAccessory(0, fCell_Current);
	return fCell_Current;
	}

// =================================================================================================
// MARK: - SectionBody_Multi

size_t SectionBody_Multi::NumberOfRows()
	{
	size_t count = 0;
	foreacha (aa, fBodies)
		count += aa->NumberOfRows();
	return count;
	}

void SectionBody_Multi::PreUpdate()
	{
	foreacha (aa, fBodies_Pending)
		aa->PreUpdate();
	}

bool SectionBody_Multi::WillBeEmpty()
	{
	foreacha (aa, fBodies_Pending)
		{
		if (not aa->WillBeEmpty())
			return false;
		}
	return true;
	}

void SectionBody_Multi::Update_NOP()
	{
	foreacha (aa, fBodies_Pending)
		aa->Update_NOP();
	}

void SectionBody_Multi::Update_Normal(
	RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
	RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload)
	{
	const size_t endOld = fBodies.size();
	size_t iterNew = 0;
	const size_t endNew = fBodies_Pending.size();
	for (size_t iterOld = 0; iterOld < endOld; ++iterOld)
		{
		const ZRef<SectionBody> bodyOld = fBodies[iterOld];
		const size_t inNew = find(fBodies_Pending.begin() + iterNew, fBodies_Pending.end(), bodyOld)
				- fBodies_Pending.begin();
		if (inNew == endNew)
			{
			// It's no longer in fBodies so delete its rows.
			bodyOld->Update_Delete(ioRowMeta_Old, ioRowUpdate_Delete);
			}
		else
			{
			// Insert any new bodies.
			while (iterNew < inNew)
				{
				ZRef<SectionBody> theBody = fBodies_Pending[iterNew];
				theBody->Update_Insert(ioRowMeta_New, ioRowUpdate_Insert);
				++iterNew;
				}

			// It's still in fBodies. Give it the opportunity to apply any changes.
			bodyOld->Update_Normal(ioRowMeta_Old, ioRowMeta_New,
				ioRowUpdate_Insert, ioRowUpdate_Delete, ioRowUpdate_Reload);

			iterNew = inNew + 1;
			}
		}

	// Insert any remaining bodies.
	while (iterNew < endNew)
		{
		ZRef<SectionBody> theBody = fBodies_Pending[iterNew];
		theBody->Update_Insert(ioRowMeta_New, ioRowUpdate_Insert);
		++iterNew;
		}
	}

void SectionBody_Multi::Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New)
	{
	foreacha (aa, fBodies_Pending)
		aa->Update_Insert(ioRowMeta_New, ioRowUpdate_New);
	}

void SectionBody_Multi::Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old)
	{
	foreacha (aa, fBodies_Pending)
		aa->Update_Delete(ioRowMeta_Old, ioRowUpdate_Old);
	}

void SectionBody_Multi::FinishUpdate()
	{
	fBodies = fBodies_Pending;
	foreacha (aa, fBodies)
		aa->FinishUpdate();
	}

void SectionBody_Multi::ViewWillAppear(UITableView* iTV)
	{
	foreacha (aa, fBodies)
		aa->ViewWillAppear(iTV);
	}

void SectionBody_Multi::ViewDidAppear(UITableView* iTV)
	{
	foreacha (aa, fBodies)
		aa->ViewDidAppear(iTV);
	}

void SectionBody_Multi::ViewWillDisappear(UITableView* iTV)
	{
	foreacha (aa, fBodies)
		aa->ViewWillDisappear(iTV);
	}

void SectionBody_Multi::ViewDidDisappear(UITableView* iTV)
	{
	foreacha (aa, fBodies)
		aa->ViewDidDisappear(iTV);
	}

bool SectionBody_Multi::FindSectionBody(ZRef<SectionBody> iSB, size_t& ioRow)
	{
	foreacha (aa, fBodies)
		{
		if (aa->FindSectionBody(iSB, ioRow))
			return true;
		}
	return false;
	}

ZRef<UITableViewCell> SectionBody_Multi::UITableViewCellForRow(
	UITableView* iView, size_t iRowIndex,
	bool& ioIsPreceded, bool& ioIsSucceeded)
	{
	if (iRowIndex != 0)
		ioIsPreceded = true;
	size_t localRowIndex;
	bool isSucceeded = false;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex, &isSucceeded))
		{
		bool localPreceded = false, localSucceeded = false;
		if (ZRef<UITableViewCell> result =
			theBody->UITableViewCellForRow(iView, localRowIndex, localPreceded, localSucceeded))
			{
			if (localSucceeded || isSucceeded)
				ioIsSucceeded = true;
			return result;
			}
		}
	return null;
	}

ZQ<UITableViewCellEditingStyle> SectionBody_Multi::QEditingStyle(size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theBody->QEditingStyle(localRowIndex))
			return theQ;
		}
	return null;
	}

bool SectionBody_Multi::CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (theBody->CommitEditingStyle(iStyle, localRowIndex))
			return true;
		}
	return false;
	}

ZQ<bool> SectionBody_Multi::QShouldIndentWhileEditing(size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<bool> theQ = theBody->QShouldIndentWhileEditing(localRowIndex))
			return theQ;
		}
	return null;
	}

ZQ<CGFloat> SectionBody_Multi::QRowHeight(size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<CGFloat> theQ = theBody->QRowHeight(localRowIndex))
			return theQ;
		}
	return null;
	}

ZQ<NSInteger> SectionBody_Multi::QIndentationLevel(size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<NSInteger> theQ = theBody->QIndentationLevel(localRowIndex))
			return theQ;
		}
	return null;
	}

bool SectionBody_Multi::ButtonTapped(UITVHandler_WithSections* iTVC,
	UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (theBody->ButtonTapped(iTVC, iTableView, iIndexPath, localRowIndex))
			return true;
		}
	return false;
	}

bool SectionBody_Multi::RowSelected(UITVHandler_WithSections* iTVC,
	UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (theBody->RowSelected(iTVC, iTableView, iIndexPath, localRowIndex))
			return true;
		}
	return false;
	}

ZQ<bool> SectionBody_Multi::CanSelect(bool iEditing, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<bool> theQ = theBody->CanSelect(iEditing, localRowIndex))
			return theQ;
		}
	return null;
	}

ZRef<SectionBody> SectionBody_Multi::pGetBodyAndRowIndex(size_t& oIndex, size_t iIndex)
	{ return this->pGetBodyAndRowIndex(oIndex, iIndex, nullptr); }

ZRef<SectionBody> SectionBody_Multi::pGetBodyAndRowIndex(size_t& oIndex, size_t iIndex, bool* oIsSucceeded)
	{
	oIndex = iIndex;
	foreachi (ii, fBodies)
		{
		ZRef<SectionBody> theBody = *ii;
		const size_t theCount = theBody->NumberOfRows();
		if (oIndex < theCount)
			{
			if (oIsSucceeded)
				{
				++ii;
				while (ii != fBodies.end())
					{
					if ((*ii)->NumberOfRows())
						{
						*oIsSucceeded = true;
						break;
						}
					++ii;
					}
				}
			return theBody;
			}
		oIndex -= theCount;
		}
	return null;
	}

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
// MARK: - UITVHandler_WithSections

using namespace ZooLib;
using namespace ZooLib::UIKit;

@interface UITVHandler_WithSections (Private)

- (ZRef<Section>)pGetSection:(size_t)iSectionIndex;
- (void)pDoUpdate1:(UITableView*)tableview;
- (void)pDoUpdate2:(UITableView*)tableview;
- (void)pDoUpdate3:(UITableView*)tableview;

@end // interface UITVHandler_WithSections

@implementation UITVHandler_WithSections

- (id)init
	{
	self = [super init];
	fTouchCount = 0;
	fDragging = false;
	fNeedsUpdate = false;
	fUpdateInFlight = false;
	fCheckForUpdateQueued = false;
	fShown = false;
	return self;
	}

- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
	{ return fSections_Shown.size(); }

- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<size_t> theQ = theSection->GetBody()->NumberOfRows())
			return *theQ;
		}
	return 0;
	}

- (void)tableView:(UITableView*)tableView
	commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
	forRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->GetBody()->CommitEditingStyle(editingStyle, indexPath.row);
	}

- (NSIndexPath*)tableView:(UITableView*)tableView
	willSelectRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->GetBody()->CanSelect([tableView isEditing], indexPath.row))
			{
			if (not *theQ)
				return nil;
			}
		}
	return indexPath;
	}

static void spApplyPosition(UITableViewCell* ioCell, bool iIsPreceded, bool iIsSucceeded);

 - (UITableViewCell*)tableView:(UITableView*)tableView
	cellForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		bool isPreceded = false, isSucceeded = false;
		if (ZRef<UITableViewCell> theCell =
			theSection->GetBody()->UITableViewCellForRow(tableView, indexPath.row, isPreceded, isSucceeded))
			{
			if ([tableView style] != UITableViewStylePlain)
				{
				if ([theCell respondsToSelector:@selector(setPosition:)])
					spApplyPosition(theCell, isPreceded, isSucceeded);
				}
			return [theCell.Orphan() autorelease];
			}
		}

	return nullptr;
	}

- (void)scrollViewWillBeginDragging:(UIScrollView *)scrollView
	{
	if (not sGetSet(fDragging, true))
		[self changeTouchState:YES forTableView:(UITableView*)scrollView];
	}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
	{
	if (sGetSet(fDragging, false))
		[self changeTouchState:NO forTableView:(UITableView*)scrollView];
	}

- (CGFloat)tableView:(UITableView*)tableView heightForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QHeaderHeight())
			return *theQ;
		}
	return tableView.sectionHeaderHeight;
	}

- (CGFloat)tableView:(UITableView*)tableView heightForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QFooterHeight())
			return *theQ;
		}
	return tableView.sectionFooterHeight;
	}

- (NSString*)tableView:(UITableView*)tableView titleForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QHeaderTitle())
			return ZUtil_NS::sString(*theQ);
		}
	return nullptr;
	}

- (NSString*)tableView:(UITableView*)tableView titleForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QFooterTitle())
			return ZUtil_NS::sString(*theQ);
		}
	return nullptr;
	}

- (UIView*)tableView:(UITableView*)tableView viewForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->QHeaderView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (UIView*)tableView:(UITableView*)tableView viewForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->QFooterView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (void)tableView:(UITableView*)tableView
	accessoryButtonTappedForRowWithIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->GetBody()->ButtonTapped(self, tableView, indexPath, indexPath.row);
	}

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->GetBody()->RowSelected(self, tableView, indexPath, indexPath.row);
	}

- (BOOL)tableView:(UITableView*)tableView
	shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->GetBody()->QShouldIndentWhileEditing(indexPath.row))
			return *theQ;
		}
	return true;
	}

- (void)tableView:(UITableView*)tableView willBeginEditingRowAtIndexPath:(NSIndexPath *)indexPath
	{
	[self changeTouchState:YES forTableView:tableView];
	}

- (void)tableView:(UITableView*)tableView didEndEditingRowAtIndexPath:(NSIndexPath *)indexPath
	{
	[self changeTouchState:NO forTableView:tableView];
	}

- (UITableViewCellEditingStyle)tableView:(UITableView*)tableView
	editingStyleForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theSection->GetBody()->QEditingStyle(indexPath.row))
			return *theQ;
		}
	return UITableViewCellEditingStyleNone;
	}

- (NSInteger)tableView:(UITableView*)tableView
	indentationLevelForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<NSInteger> theQ = theSection->GetBody()->QIndentationLevel(indexPath.row))
			return *theQ;
		}
	return 0;
	}

- (void)doUpdateIfPossible:(UITableView*)tableView
	{
	fNeedsUpdate = true;

	if (sGetSet(fUpdateInFlight, true))
		return;

	[self pDoUpdate1:tableView];
	}

-(void)pCheckForUpdate:(UITableView*)tableView
	{
	fCheckForUpdateQueued = false;

	if (fTouchCount)
		return;

	if (not fNeedsUpdate)
		return;

	if (sGetSet(fUpdateInFlight, true))
		return;

	[self pDoUpdate1:tableView];
	}

-(void)pEnqueueCheckForUpdate:(UITableView*)tableView
	{
	if (sGetSet(fCheckForUpdateQueued, true))
		return;

	[self
		performSelectorOnMainThread:@selector(pCheckForUpdate:)
		withObject:tableView
		waitUntilDone:NO];
	}

- (void)needsUpdate:(UITableView*)tableView
	{
	ZAssert(tableView);

	if (sGetSet(fNeedsUpdate, true))
		return;

	[self pEnqueueCheckForUpdate:tableView];
	}

- (NSIndexPath*)indexPathForSectionBody:(ZooLib::ZRef<ZooLib::UIKit::SectionBody>)iSB
	{
	for (size_t theSection = 0; theSection < fSections_Shown.size(); ++theSection)
		{
		size_t theRow = 0;
		if (fSections_Shown[theSection]->GetBody()->FindSectionBody(iSB, theRow))
			return [NSIndexPath indexPathForRow:theRow inSection:theSection];
		}
	return nil;
	}

- (void)tableViewWillAppear:(UITableView*)tableView
	{
	[tableView deselect];
	for (size_t xx = 0; xx < fSections_All.size(); ++xx)
		fSections_All[xx]->GetBody()->ViewWillAppear(tableView);
	}

- (void)tableViewDidAppear:(UITableView*)tableView
	{
	fShown = true;
	[tableView flashScrollIndicators];
	for (size_t xx = 0; xx < fSections_All.size(); ++xx)
		fSections_All[xx]->GetBody()->ViewDidAppear(tableView);
	}

- (void)tableViewWillDisappear:(UITableView*)tableView
	{
	fShown = false;
	for (size_t xx = 0; xx < fSections_All.size(); ++xx)
		fSections_All[xx]->GetBody()->ViewWillDisappear(tableView);
	}

- (void)tableViewDidDisappear:(UITableView*)tableView
	{
	[tableView deselect];
	for (size_t xx = 0; xx < fSections_All.size(); ++xx)
		fSections_All[xx]->GetBody()->ViewDidDisappear(tableView);
	}

- (void)changeTouchState:(BOOL)touchState forTableView:(UITableView*)tableView
	{
	if (touchState)
		{
		++fTouchCount;
		}
	else
		{
		if (0 == --fTouchCount)
			[self pEnqueueCheckForUpdate:tableView];
		}
	}

typedef enum 
	{
	UACellBackgroundViewPositionSingle = 0,
	UACellBackgroundViewPositionTop, 
	UACellBackgroundViewPositionBottom,
	UACellBackgroundViewPositionMiddle
	} UACellBackgroundViewPosition;

static void spApplyPosition(UITableViewCell* ioCell, bool iIsPreceded, bool iIsSucceeded)
	{
	UACellBackgroundViewPosition thePosition;
	if (iIsPreceded)
		{
		if (iIsSucceeded)
			thePosition = UACellBackgroundViewPositionMiddle;
		else
			thePosition = UACellBackgroundViewPositionBottom;
		}
	else if (iIsSucceeded)
		{
		thePosition = UACellBackgroundViewPositionTop;
		}
	else
		{
		thePosition = UACellBackgroundViewPositionSingle;
		}

	if ([ioCell respondsToSelector:@selector(applyPosition:)])
		[ioCell applyPosition:thePosition];
	}

- (void)pApplyPositionToVisibleCells:(UITableView*)tableView
	{
	if ([tableView style] == UITableViewStylePlain)
		return;

	[tableView visibleCells];
	const NSArray* paths = [tableView indexPathsForVisibleRows];

	size_t countInSection = 0;
	int priorSection = -1;
	for (size_t xx = 0, count = [paths count]; xx < count; ++xx)
		{
		NSIndexPath* thePath = [paths objectAtIndex:xx];
		UITableViewCell* cell = [tableView cellForRowAtIndexPath:thePath];
		if ([cell respondsToSelector:@selector(setPosition:)])
			{
			const int section = thePath.section;

			if (sQSet(priorSection, section))
				countInSection = [self tableView:tableView numberOfRowsInSection:section];

			const int row = thePath.row;
			spApplyPosition(cell, row > 0, row + 1 < countInSection);
			}
		}
	}

static void spInsertSections(UITableView* iTableView,
	size_t iBaseIndex,
	const ZRef<Section>* iSections,
	size_t iCount,
	set<ZRef<Section> >& ioSections_ToIgnore
	)
	{
	for (size_t xx = 0; xx < iCount; ++xx)
		{
		ZRef<Section> theSection = iSections[xx];
		theSection->GetBody()->Update_NOP();
		theSection->GetBody()->FinishUpdate();
		ioSections_ToIgnore.insert(theSection);
		[iTableView
			insertSections:sMakeIndexSet(iBaseIndex + xx)
			withRowAnimation:theSection->SectionAnimation_Insert()];
		}
	}

- (void)pDoUpdate1:(UITableView*)tableView
	{
	ZAssert(tableView);

	if (not sGetSet(fNeedsUpdate, false))
		return;

	ZAssert(fUpdateInFlight);

	fSections_ToIgnore.clear();
	fSections_Shown_Pending.clear();
	for (size_t xx = 0; xx < fSections_All.size(); ++xx)
		{
		ZRef<Section> theSection = fSections_All[xx];
		theSection->GetBody()->PreUpdate();
		if (not theSection->HideWhenEmpty() || not theSection->GetBody()->WillBeEmpty())
			fSections_Shown_Pending.push_back(theSection);
		}

	// We've done PreUpdate on every section, and fSections_Shown_Pending contains
	// those sections that will be visible.

	if (not fShown)
		{
		// We're not onscreen, so we can Update/Finish all sections, switch
		// to the new list of sections, reloadData, check for pending updates and return.
		for (size_t xx = 0; xx < fSections_All.size(); ++xx)
			{
			ZRef<Section> theSection = fSections_All[xx];
			theSection->GetBody()->Update_NOP();
			theSection->GetBody()->FinishUpdate();
			}
		fSections_Shown = fSections_Shown_Pending;
		[tableView reloadData];
		fUpdateInFlight = false;
		if (fNeedsUpdate)
			[self pEnqueueCheckForUpdate:tableView];
		return;
		}

	if (fSections_Shown == fSections_Shown_Pending)
		{
		// The list of sections hasn't changed, move directly on to pUpdate2
		[self pDoUpdate2:tableView];
		return;
		}

	// We need to insert and remove sections.
	[CATransaction begin];

	[CATransaction setCompletionBlock:^{ [self pDoUpdate2:tableView]; }];

	[tableView beginUpdates];

	const vector<ZRef<Section> > sectionsOld = fSections_Shown;
	fSections_Shown = fSections_Shown_Pending;

	const size_t endOld = sectionsOld.size();

	size_t iterNew = 0;
	const size_t endNew = fSections_Shown.size();

	for (size_t iterOld = 0; iterOld < endOld; ++iterOld)
		{
		const ZRef<Section> sectionOld = sectionsOld[iterOld];
		const size_t inNew = find(fSections_Shown.begin(), fSections_Shown.end(), sectionOld)
			- fSections_Shown.begin();
		if (inNew == endNew)
			{
			// sectionOld is no longer in fSections_Shown so must be deleted.
			[tableView
				deleteSections:sMakeIndexSet(iterOld)
				withRowAnimation:sectionOld->SectionAnimation_Delete()];
			// But it does need to be update/finished
			sectionOld->GetBody()->Update_NOP();
			sectionOld->GetBody()->FinishUpdate();
			}
		else
			{
			// sectionOld is still present. Don't do anything with it for now -- any cell
			// reload/insert/delete will happen later.
			if (size_t countToInsert = inNew - iterNew)
				{
				// There are sections to insert prior to sectionOld.
				spInsertSections(tableView,
					iterNew, &fSections_Shown[iterNew], countToInsert, fSections_ToIgnore);
				}
			iterNew = inNew + 1;
			}
		}

	// Insert remainder of pending.
	if (size_t countToInsert = endNew - iterNew)
		{
		spInsertSections(tableView,
			iterNew, &fSections_Shown[iterNew], countToInsert, fSections_ToIgnore);
		}

	[tableView endUpdates];

	[CATransaction commit];
	}

- (void)pDoUpdate2:(UITableView*)tableView
	{
	ZAssert(fUpdateInFlight);
	ZAssert(tableView);

	const bool isShown = fShown;

	std::vector<std::map<size_t, UITableViewRowAnimation> > theInserts(fSections_Shown.size());
	std::vector<std::map<size_t, UITableViewRowAnimation> > theDeletes(fSections_Shown.size());
	std::vector<std::map<size_t, UITableViewRowAnimation> > theReloads(fSections_Shown.size());

	bool anyChanges = false;
	for (size_t xx = 0; xx < fSections_Shown.size(); ++xx)
		{
		if (not ZUtil_STL::sContains(fSections_ToIgnore, fSections_Shown[xx]))
			{
			SectionBody::RowMeta theRowMeta_Old;
			SectionBody::RowMeta theRowMeta_New;
			SectionBody::RowUpdate theRowUpdate_Insert(theRowMeta_New, theInserts[xx]);
			SectionBody::RowUpdate theRowUpdate_Delete(theRowMeta_Old, theDeletes[xx]);
			SectionBody::RowUpdate theRowUpdate_Reload(theRowMeta_Old, theReloads[xx]);
			fSections_Shown[xx]->GetBody()->Update_Normal(theRowMeta_Old, theRowMeta_New,
				theRowUpdate_Insert, theRowUpdate_Delete, theRowUpdate_Reload);

			if (theDeletes[xx].size() || theInserts[xx].size() || theReloads[xx].size())
				anyChanges = true;
			}
		}
	fSections_ToIgnore.clear();

	if (spIsVersion4OrLater() && anyChanges)
		{
		[CATransaction begin];

		[CATransaction setCompletionBlock:^{ [self pDoUpdate3:tableView anyChanges:anyChanges]; }];

		[tableView beginUpdates];

		for (size_t xx = 0; xx < theReloads.size(); ++xx)
			{
			map<size_t, UITableViewRowAnimation>& theMap = theReloads[xx];
			foreachi (ii, theMap)
				{
				UITableViewRowAnimation theAnimation = UITableViewRowAnimationNone;
				if (fShown)
					theAnimation = ii->second;
				[tableView
					reloadRowsAtIndexPaths:sMakeNSIndexPathArray(xx, ii->first, 1)
					withRowAnimation:theAnimation];
				}
			}

		for (size_t xx = 0; xx < theDeletes.size(); ++xx)
			{
			map<size_t, UITableViewRowAnimation>& theMap = theDeletes[xx];
			foreachi (ii, theMap)
				{
				[tableView
					deleteRowsAtIndexPaths:sMakeNSIndexPathArray(xx, ii->first, 1)
					withRowAnimation:isShown ? ii->second : UITableViewRowAnimationNone];
				}
			}

		for (size_t xx = 0; xx < theInserts.size(); ++xx)
			{
			map<size_t, UITableViewRowAnimation>& theMap = theInserts[xx];
			foreachi (ii, theMap)
				{
				[tableView
					insertRowsAtIndexPaths:sMakeNSIndexPathArray(xx, ii->first, 1)
					withRowAnimation:isShown ? ii->second : UITableViewRowAnimationNone];
				}
			}

		for (size_t xx = 0; xx < fSections_All.size(); ++xx)
			fSections_All[xx]->GetBody()->FinishUpdate();

		[tableView endUpdates];

		[self pApplyPositionToVisibleCells:tableView];

		[CATransaction commit];
		}
	else
		{
		for (size_t xx = 0; xx < fSections_All.size(); ++xx)
			fSections_All[xx]->GetBody()->FinishUpdate();

		[self pDoUpdate3:tableView anyChanges:anyChanges];
		}
	}

- (void)pDoUpdate3:(UITableView*)tableView anyChanges:(bool)anyChanges
	{
	ZAssert(fUpdateInFlight);
	ZAssert(tableView);

	if (not spIsVersion4OrLater() && anyChanges)
		[tableView reloadData];

	ZAssert(fUpdateInFlight);
	ZAssert(tableView);
	fUpdateInFlight = false;

	[self pApplyPositionToVisibleCells:tableView];
	spUpdatePopovers();
	if (fNeedsUpdate)
		[self pEnqueueCheckForUpdate:tableView];
	}

-(ZRef<Section>)pGetSection:(size_t)iSectionIndex
	{
	if (iSectionIndex < fSections_Shown.size())
		return fSections_Shown[iSectionIndex];
	return null;
	}

@end // implementation UITVHandler_WithSections

// =================================================================================================
// MARK: - UITVHandler_WithSections_VariableRowHeight

@implementation UITVHandler_WithSections_VariableRowHeight

- (CGFloat)tableView:(UITableView*)tableView heightForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<CGFloat> theQ = theSection->GetBody()->QRowHeight(indexPath.row))
			return *theQ;
		}
	return tableView.rowHeight;
	}

@end // implementation UITVHandler_WithSections_VariableRowHeight

// =================================================================================================
// MARK: - UIGestureRecognizer_TableViewWithSections

@interface UIGestureRecognizer_TableViewWithSections : UIGestureRecognizer
	{
@public
	UITableView_WithSections* fTV;
	bool fCallEnd;
	}

@end // interface UIGestureRecognizer_TableViewWithSections

@implementation UIGestureRecognizer_TableViewWithSections

- (id)init
	{
	self = [super init];
	fCallEnd = false;
	self.cancelsTouchesInView = NO;
	self.delaysTouchesEnded = NO;
	return self;
	}

- (void)reset
	{
	[super reset];
	if (sGetSet(fCallEnd, false))
		[fTV pChangeTouchState:NO];
	}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesBegan:touches withEvent:event];
	if (not sGetSet(fCallEnd, true))
		[fTV pChangeTouchState:YES];
	}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesMoved:touches withEvent:event];
	}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesEnded:touches withEvent:event];
	self.state = UIGestureRecognizerStateFailed;
	if (sGetSet(fCallEnd, false))
		[fTV pChangeTouchState:NO];
	}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
	{
	[super touchesEnded:touches withEvent:event];
	self.state = UIGestureRecognizerStateFailed;
	if (sGetSet(fCallEnd, false))
		[fTV pChangeTouchState:NO];
	}

@end // UIGestureRecognizer_TableViewWithSections

// =================================================================================================
// MARK: - UITableView_WithSections

@implementation UITableView_WithSections

- (id)initWithFrame:(CGRect)frame style:(UITableViewStyle)style variableRowHeight:(BOOL)variableRowHeight
	{
	self = [super initWithFrame:frame style:style];

	fCallable_NeedsUpdate = sCallable<void()>(self, @selector(needsUpdate));

	if (variableRowHeight)
		fHandler = sAdopt& [[UITVHandler_WithSections_VariableRowHeight alloc] init];
	else
		fHandler = sAdopt& [[UITVHandler_WithSections alloc] init];
	[self setDelegate:fHandler];
	[self setDataSource:fHandler];
	ZRef<UIGestureRecognizer_TableViewWithSections> theGR = sAdopt&
		[[UIGestureRecognizer_TableViewWithSections alloc] init];
	theGR->fTV = self;
	[self addGestureRecognizer:theGR];
	return self;
	}

- (id)initWithFrame:(CGRect)frame style:(UITableViewStyle)style
	{ return [self initWithFrame:frame style:style variableRowHeight:NO]; }

- (void)doUpdateIfPossible
	{
	if (fHandler)
		[fHandler doUpdateIfPossible:self];
	}

- (void)needsUpdate
	{
	if (fHandler)
		[fHandler needsUpdate:self];
	}

- (void)appendSection:(ZooLib::ZRef<ZooLib::UIKit::Section>) iSection
	{
	fHandler->fSections_All.push_back(iSection);
	[self needsUpdate];
	}

- (NSIndexPath*)indexPathForSectionBody:(ZooLib::ZRef<ZooLib::UIKit::SectionBody>)iSB
	{ return [fHandler indexPathForSectionBody:iSB]; }

- (void)deselect
	{
	if (NSIndexPath* thePath = [self indexPathForSelectedRow])
		[self deselectRowAtIndexPath:thePath animated:YES];
	}

- (void)pChangeTouchState:(BOOL)touchState
	{
	[fHandler changeTouchState:touchState forTableView:self];
	}

@end // implementation UITableView_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)
