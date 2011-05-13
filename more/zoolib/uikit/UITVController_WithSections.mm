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

#include "zoolib/ZCallable_ObjC.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZRef_NSObject.h"
#include "zoolib/ZUtil_NSObject.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"

#import "UIKit/UIGestureRecognizerSubclass.h"

namespace ZooLib {
namespace UIKit {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

NSArray* sMakeNSIndexPathArray(size_t iSectionIndex, size_t iBaseRowIndex, size_t iCount)
	{
	NSUInteger theIndices[2];
	theIndices[0] = iSectionIndex;
	NSMutableArray* theArray = [NSMutableArray arrayWithCapacity:iCount];
	for (size_t x = 0; x < iCount; ++x)
		{
		theIndices[1] = iBaseRowIndex + x;
		[theArray addObject:[NSIndexPath indexPathWithIndexes:&theIndices[0] length:2]];
		}
	return theArray;
	}

NSIndexSet* sMakeIndexSet(size_t iIndex)
	{ return [NSIndexSet indexSetWithIndex:iIndex]; }

static bool spIsVersion4OrLater()
	{
	static ZQ<bool> resultQ;
	if (!resultQ)
		resultQ = [[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0;
	return resultQ.Get();
	}
	
// =================================================================================================
#pragma mark -
#pragma mark * Section

Section::Section(ZRef<SectionBody> iBody)
:	fBody(iBody)
,	fHideWhenEmpty(false)
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
		return theQ.Get();
	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation Section::SectionAnimation_Delete()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Delete())
		return theQ.Get();
	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation Section::SectionAnimation_Reload()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Reload())
		return theQ.Get();
	return UITableViewRowAnimationNone;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody::RowMeta

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
#pragma mark -
#pragma mark * SectionBody::RowUpdate

SectionBody::RowUpdate::RowUpdate(RowMeta& ioRowMeta, std::map<size_t, UITableViewRowAnimation>& ioMap)
:	fRowMeta(ioRowMeta)
,	fMap(ioMap)
	{}

void SectionBody::RowUpdate::Add(size_t iIndex, UITableViewRowAnimation iRowAnimation)
	{ this->AddRange(iIndex, 1, iRowAnimation); }

void SectionBody::RowUpdate::AddAll(UITableViewRowAnimation iRowAnimation)
	{ this->AddRange(0, fRowMeta.fLimit, iRowAnimation); }

void SectionBody::RowUpdate::AddRange(size_t iStart, size_t iCount, UITableViewRowAnimation iRowAnimation)
	{
	ZAssert(iStart <= fRowMeta.fLimit);
	if (iCount)
		{
		ZAssert(iStart + iCount <= fRowMeta.fLimit);
		while (iCount--)
			ZUtil_STL::sInsertMustNotContain(0, fMap, fRowMeta.fBase + iStart++, iRowAnimation);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody

void SectionBody::ViewWillAppear(UITableView* iTV)
	{}

void SectionBody::ViewDidAppear(UITableView* iTV)
	{}

void SectionBody::ViewWillDisappear(UITableView* iTV)
	{}

void SectionBody::ViewDidDisappear(UITableView* iTV)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_Concrete

SectionBody_Concrete::SectionBody_Concrete()
:	fRowAnimation_Insert(UITableViewRowAnimationRight)
,	fRowAnimation_Delete(UITableViewRowAnimationRight)
,	fRowAnimation_Reload(UITableViewRowAnimationNone)
,	fApplyAccessory(true)
	{}

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
			return fCallable_ButtonTapped_Editing->Call(iTVC, iTableView, iIndexPath, this, iRowIndex);
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
			return fCallable_RowSelected_Editing->Call(iTVC, iTableView, iIndexPath, this, iRowIndex);
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
#pragma mark -
#pragma mark * SectionBody_SingleRow

SectionBody_SingleRow::SectionBody_SingleRow(ZRef<UITableViewCell> iCell)
:	fCell_Pending(iCell)
	{
	ZAssert(!fCell_Pending || ![fCell_Pending reuseIdentifier]);
	}

size_t SectionBody_SingleRow::NumberOfRows()
	{
	if (fCell_Current)
		return 1;
	return 0;
	}

void SectionBody_SingleRow::PreUpdate()
	{
	ZAssert(!fCell_Pending || ![fCell_Pending reuseIdentifier]);
	fCell_New = fCell_Pending;
	}

bool SectionBody_SingleRow::WillBeEmpty()
	{ return !fCell_New; }

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
#pragma mark -
#pragma mark * SectionBody_Multi

size_t SectionBody_Multi::NumberOfRows()
	{
	size_t count = 0;
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		count += (*i)->NumberOfRows();
	return count;
	}

void SectionBody_Multi::PreUpdate()
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies_Pending.begin(); i != fBodies_Pending.end(); ++i)
		(*i)->PreUpdate();
	}

bool SectionBody_Multi::WillBeEmpty()
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies_Pending.begin(); i != fBodies_Pending.end(); ++i)
		{
		if (!(*i)->WillBeEmpty())
			return false;
		}
	return true;
	}

void SectionBody_Multi::Update_NOP()
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies_Pending.begin(); i != fBodies_Pending.end(); ++i)
		(*i)->Update_NOP();
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
		const size_t inNew =
			find(fBodies_Pending.begin(), fBodies_Pending.end(), bodyOld) - fBodies_Pending.begin();
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
	for (vector<ZRef<SectionBody> >::iterator i = fBodies_Pending.begin(); i != fBodies_Pending.end(); ++i)
		(*i)->Update_Insert(ioRowMeta_New, ioRowUpdate_New);
	}

void SectionBody_Multi::Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old)
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies_Pending.begin(); i != fBodies_Pending.end(); ++i)
		(*i)->Update_Delete(ioRowMeta_Old, ioRowUpdate_Old);
	}

void SectionBody_Multi::FinishUpdate()
	{
	fBodies = fBodies_Pending;
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->FinishUpdate();
	}

void SectionBody_Multi::ViewWillAppear(UITableView* iTV)
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->ViewWillAppear(iTV);
	}

void SectionBody_Multi::ViewDidAppear(UITableView* iTV)
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->ViewDidAppear(iTV);
	}

void SectionBody_Multi::ViewWillDisappear(UITableView* iTV)
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->ViewWillDisappear(iTV);
	}

void SectionBody_Multi::ViewDidDisappear(UITableView* iTV)
	{
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->ViewDidDisappear(iTV);
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
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		{
		ZRef<SectionBody> theBody = *i;
		const size_t theCount = theBody->NumberOfRows();
		if (oIndex < theCount)
			{
			if (oIsSucceeded)
				{
				++i;
				while (i != fBodies.end())
					{
					if ((*i)->NumberOfRows())
						{
						*oIsSucceeded = true;
						break;
						}
					++i;
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
#pragma mark -
#pragma mark * UITVHandler_WithSections

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
	[super init];
	fTouchCount = 0;
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
			return theQ.Get();
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
			if (!theQ.Get())
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
	[self changeTouchState:YES forTableView:(UITableView*)scrollView];
	}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate
	{
	[self changeTouchState:NO forTableView:(UITableView*)scrollView];
	}

- (CGFloat)tableView:(UITableView*)tableView heightForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QHeaderHeight())
			return theQ.Get();
		}
	return tableView.sectionHeaderHeight;
	}

- (CGFloat)tableView:(UITableView*)tableView heightForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QFooterHeight())
			return theQ.Get();
		}
	return tableView.sectionFooterHeight;
	}

- (NSString*)tableView:(UITableView*)tableView titleForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QHeaderTitle())
			return ZUtil_NSObject::sString(theQ.Get());
		}
	return nullptr;
	}

- (NSString*)tableView:(UITableView*)tableView titleForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QFooterTitle())
			return ZUtil_NSObject::sString(theQ.Get());
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
		{
		theSection->GetBody()->RowSelected(self, tableView, indexPath, indexPath.row);
//##		[tableView deselectRowAtIndexPath:indexPath animated:YES];
		}
	}

- (BOOL)tableView:(UITableView*)tableView
	shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->GetBody()->QShouldIndentWhileEditing(indexPath.row))
			return theQ.Get();
		}
	return true;
	}

- (UITableViewCellEditingStyle)tableView:(UITableView*)tableView
	editingStyleForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theSection->GetBody()->QEditingStyle(indexPath.row))
			return theQ.Get();
		}
	return UITableViewCellEditingStyleNone;
	}

- (NSInteger)tableView:(UITableView*)tableView
	indentationLevelForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<NSInteger> theQ = theSection->GetBody()->QIndentationLevel(indexPath.row))
			return theQ.Get();
		}
	return 0;
	}

- (void)doUpdateIfPossible:(UITableView*)tableView
	{
	fNeedsUpdate = true;

	if (fUpdateInFlight)
		return;

	fUpdateInFlight = true;
	[self pDoUpdate1:tableView];
	}

-(void)pCheckForUpdate:(UITableView*)tableView
	{
	fCheckForUpdateQueued = false;

	if (fTouchCount)
		return;

	if (!fNeedsUpdate)
		return;
	
	if (fUpdateInFlight)
		return;
	fUpdateInFlight = true;

	[self pDoUpdate1:tableView];
	}

-(void)pQueueCheckForUpdate:(UITableView*)tableView
	{
	if (fCheckForUpdateQueued)
		return;
	fCheckForUpdateQueued = true;

	[self
		performSelectorOnMainThread:@selector(pCheckForUpdate:)
		withObject:tableView
		waitUntilDone:NO];	
	}

- (void)needsUpdate:(UITableView*)tableView
	{
	ZAssert(tableView);

	if (fNeedsUpdate)
		return;
	fNeedsUpdate = true;

	[self pQueueCheckForUpdate:tableView];
	}

- (void)tableViewWillAppear:(UITableView*)tableView
	{
	[tableView deselect];
	for (size_t x = 0; x < fSections_All.size(); ++x)
		fSections_All[x]->GetBody()->ViewWillAppear(tableView);
	}

- (void)tableViewDidAppear:(UITableView*)tableView
	{
	fShown = true;
    [tableView flashScrollIndicators];
	for (size_t x = 0; x < fSections_All.size(); ++x)
		fSections_All[x]->GetBody()->ViewDidAppear(tableView);
	}

- (void)tableViewWillDisappear:(UITableView*)tableView
	{
	fShown = false;
	for (size_t x = 0; x < fSections_All.size(); ++x)
		fSections_All[x]->GetBody()->ViewWillDisappear(tableView);
	}

- (void)tableViewDidDisappear:(UITableView*)tableView
	{
	[tableView deselect];
	for (size_t x = 0; x < fSections_All.size(); ++x)
		fSections_All[x]->GetBody()->ViewDidDisappear(tableView);
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
			[self pQueueCheckForUpdate:tableView];
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

	[ioCell setPosition:thePosition];
	}

- (void)pApplyPositionToVisibleCells:(UITableView*)tableView
	{
	if ([tableView style] == UITableViewStylePlain)
		return;

	[tableView visibleCells];
	const NSArray* paths = [tableView indexPathsForVisibleRows];
	
	size_t countInSection;
	int lastSection = -1;
	for (size_t x = 0, count = [paths count]; x < count; ++x)
		{
		NSIndexPath* thePath = [paths objectAtIndex:x];
		UITableViewCell* cell = [tableView cellForRowAtIndexPath:thePath];
		if ([cell respondsToSelector:@selector(setPosition:)])
			{
			const int section = thePath.section; 
			if (lastSection != section)
				{
				lastSection = section;
				countInSection = [self tableView:tableView numberOfRowsInSection:section];
				}

			const int row = thePath.row;
			spApplyPosition(cell, row > 0, row < countInSection - 1);
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
	for (size_t x = 0; x < iCount; ++x)
		{
		ZRef<Section> theSection = iSections[x];
		theSection->GetBody()->Update_NOP();
		theSection->GetBody()->FinishUpdate();
		ioSections_ToIgnore.insert(theSection);
		[iTableView
			insertSections:sMakeIndexSet(iBaseIndex + x)
			withRowAnimation:theSection->SectionAnimation_Insert()];
		}
	}

- (void)pDoUpdate1:(UITableView*)tableView
	{
	ZAssert(tableView);

	if (!fNeedsUpdate)
		return;
	fNeedsUpdate = false;

	ZAssert(fUpdateInFlight);

	fSections_ToIgnore.clear();
	fSections_Shown_Pending.clear();
	for (size_t x = 0; x < fSections_All.size(); ++x)
		{
		ZRef<Section> theSection = fSections_All[x];
		theSection->GetBody()->PreUpdate();
		if (!theSection->HideWhenEmpty() || !theSection->GetBody()->WillBeEmpty())
			fSections_Shown_Pending.push_back(theSection);
		}

	// We've done PreUpdate on every section, and fSections_Shown_Pending contains
	// those sections that will be visible.

	if (!fShown)
		{
		// We're not onscreen, so we can Update/Finish all sections, switch
		// to the new list of sections, reloadData, check for pending updates and return.
		for (size_t x = 0; x < fSections_All.size(); ++x)
			{
			ZRef<Section> theSection = fSections_All[x];
			theSection->GetBody()->Update_NOP();
			theSection->GetBody()->FinishUpdate();
			}
		fSections_Shown = fSections_Shown_Pending;
		[tableView reloadData];
		fUpdateInFlight = false;
		if (fNeedsUpdate)
			[self pQueueCheckForUpdate:tableView];
		return;
		}

	if (fSections_Shown == fSections_Shown_Pending)
		{
		// The list of sections hasn't changed, move directly on to pUpdate2
		[self pDoUpdate2:tableView];
		return;
		}

	// We need to insert and remove sections.
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
	{ZLOGF(w, eDebug + 1); w << self;}

	[self performSelector:@selector(pDoUpdate2:)
		 withObject:tableView
		 afterDelay:0.35];
	}

- (void)pDoUpdate2:(UITableView*)tableView
	{
	ZAssert(fUpdateInFlight);
	ZAssert(tableView);

	const bool isShown = fShown;

	fInserts.clear();
	fInserts.resize(fSections_Shown.size());

	fDeletes.clear();
	fDeletes.resize(fSections_Shown.size());

	fReloads.clear();
	fReloads.resize(fSections_Shown.size());

	bool anyDeletes = false, anyInserts = false, anyReloads = false;
	for (size_t x = 0; x < fSections_Shown.size(); ++x)
		{
		if (!ZUtil_STL::sContains(fSections_ToIgnore, fSections_Shown[x]))
			{
			SectionBody::RowMeta theRowMeta_Old;
			SectionBody::RowMeta theRowMeta_New;
			SectionBody::RowUpdate theRowUpdate_Insert(theRowMeta_New, fInserts[x]);
			SectionBody::RowUpdate theRowUpdate_Delete(theRowMeta_Old, fDeletes[x]);
			SectionBody::RowUpdate theRowUpdate_Reload(theRowMeta_Old, fReloads[x]);
			fSections_Shown[x]->GetBody()->Update_Normal(theRowMeta_Old, theRowMeta_New,
				theRowUpdate_Insert, theRowUpdate_Delete, theRowUpdate_Reload);

			if (fDeletes[x].size())
				anyDeletes = true;
			if (fInserts[x].size())
				anyInserts = true;
			if (fReloads[x].size())
				anyReloads = true;
			}
		}
	fSections_ToIgnore.clear();

	if (anyDeletes || anyInserts || (anyReloads && spIsVersion4OrLater()))
		{
		[tableView beginUpdates];

		if (spIsVersion4OrLater())
			{
			for (size_t x = 0; x < fReloads.size(); ++x)
				{
				map<size_t, UITableViewRowAnimation>& theMap = fReloads[x];
				for (map<size_t, UITableViewRowAnimation>::iterator i = theMap.begin();
					i != theMap.end(); ++i)
					{
					UITableViewRowAnimation theAnimation = UITableViewRowAnimationNone;
					if (fShown)
						theAnimation = i->second;
					[tableView
						reloadRowsAtIndexPaths:sMakeNSIndexPathArray(x, i->first, 1)
						withRowAnimation:theAnimation];
					}
				}
			}

		for (size_t x = 0; x < fDeletes.size(); ++x)
			{
			map<size_t, UITableViewRowAnimation>& theMap = fDeletes[x];
			for (map<size_t, UITableViewRowAnimation>::iterator i = theMap.begin();
				i != theMap.end(); ++i)
				{
				[tableView
					deleteRowsAtIndexPaths:sMakeNSIndexPathArray(x, i->first, 1)
					withRowAnimation:isShown ? i->second : UITableViewRowAnimationNone];
				}
			}

		for (size_t x = 0; x < fInserts.size(); ++x)
			{
			map<size_t, UITableViewRowAnimation>& theMap = fInserts[x];
			for (map<size_t, UITableViewRowAnimation>::iterator i = theMap.begin();
				i != theMap.end(); ++i)
				{
				[tableView
					insertRowsAtIndexPaths:sMakeNSIndexPathArray(x, i->first, 1)
					withRowAnimation:isShown ? i->second : UITableViewRowAnimationNone];
				}
			}

		for (size_t x = 0; x < fSections_All.size(); ++x)
			fSections_All[x]->GetBody()->FinishUpdate();

		[tableView endUpdates];

		[self pApplyPositionToVisibleCells:tableView];

		[self
			performSelector:@selector(pDoUpdate3:)
			withObject:tableView
			afterDelay:0.35];
		}
	else
		{
		for (size_t x = 0; x < fSections_All.size(); ++x)
			fSections_All[x]->GetBody()->FinishUpdate();

		[self pDoUpdate3:tableView];
		}
	}

- (void)pDoUpdate3:(UITableView*)tableView
	{
	ZAssert(fUpdateInFlight);
	ZAssert(tableView);

	if (!spIsVersion4OrLater())
		{
		for (size_t x = 0; x < fSections_Shown.size(); ++x)
			{
			if (fReloads[x].size())
				{
				[tableView reloadData];
				break;
				}
			}
		}

	ZAssert(fUpdateInFlight);
	ZAssert(tableView);
	fUpdateInFlight = false;
	if (fNeedsUpdate)
		[self pQueueCheckForUpdate:tableView];
	}


-(ZRef<Section>)pGetSection:(size_t)iSectionIndex
	{
	if (iSectionIndex < fSections_Shown.size())
		return fSections_Shown[iSectionIndex];
	return null;
	}

@end // implementation UITVHandler_WithSections

// =================================================================================================
#pragma mark -
#pragma mark * UITVHandler_WithSections_VariableRowHeight

@implementation UITVHandler_WithSections_VariableRowHeight

- (CGFloat)tableView:(UITableView*)tableView heightForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<CGFloat> theQ = theSection->GetBody()->QRowHeight(indexPath.row))
			return theQ.Get();
		}
	return tableView.rowHeight;
	}

@end // implementation UITVHandler_WithSections_VariableRowHeight

// =================================================================================================
#pragma mark -
#pragma mark * UIGestureRecognizer_TableViewWithSections

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
	ZLOGTRACE(eDebug + 1);
	[super init];
	fCallEnd = false;
	self.delaysTouchesEnded = NO;
	return self;
	}

- (void)reset
	{
	ZLOGTRACE(eDebug + 1);
	[super reset];
	if (fCallEnd)
		{
		fCallEnd = false;
		[fTV pChangeTouchState:NO];
		}
	}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
	{
	ZLOGTRACE(eDebug + 1);
	[super touchesBegan:touches withEvent:event];
	fCallEnd = true;
	[fTV pChangeTouchState:YES];
	}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
	{
	ZLOGTRACE(eDebug + 1);
	[super touchesMoved:touches withEvent:event];
	}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
	{
	ZLOGTRACE(eDebug + 1);
	[super touchesEnded:touches withEvent:event];
	self.state = UIGestureRecognizerStateFailed;
	if (fCallEnd)
		{
		fCallEnd = false;
		[fTV pChangeTouchState:NO];
		}
	}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
	{
	ZLOGTRACE(eDebug + 1);
	[super touchesEnded:touches withEvent:event];
	self.state = UIGestureRecognizerStateFailed;
	if (fCallEnd)
		{
		fCallEnd = false;
		[fTV pChangeTouchState:NO];
		}
	}

@end // UIGestureRecognizer_TableViewWithSections

// =================================================================================================
#pragma mark -
#pragma mark * UITableView_WithSections

@implementation UITableView_WithSections

- (id)initWithFrame:(CGRect)frame style:(UITableViewStyle)style variableRowHeight:(BOOL)variableRowHeight
	{
	[super initWithFrame:frame style:style];
	fCallable_NeedsUpdate = MakeCallable<void()>(self, @selector(needsUpdate));
	if (variableRowHeight)
		fHandler = Adopt& [[UITVHandler_WithSections_VariableRowHeight alloc] init];
	else
		fHandler = Adopt& [[UITVHandler_WithSections alloc] init];
	[self setDelegate:fHandler];
	[self setDataSource:fHandler];
	ZRef<UIGestureRecognizer_TableViewWithSections> theGR = Adopt&
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
