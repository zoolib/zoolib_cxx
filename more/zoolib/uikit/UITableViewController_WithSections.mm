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
#include "zoolib/uikit/UITableViewController_WithSections.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#include "zoolib/ZCallable_ObjC.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZRef_NSObject.h"
#include "zoolib/ZUtil_NSObject.h"
#include "zoolib/ZUtil_STL.h"

namespace ZooLib {
namespace UIKit {

using std::map;
using std::vector;

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
	ZAssert(iStart < fRowMeta.fLimit);
	ZAssert(iStart + iCount <= fRowMeta.fLimit);
	while (iCount--)
		ZUtil_STL::sInsertMustNotContain(0, fMap, fRowMeta.fBase + iStart++, iRowAnimation);
	}

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

// =================================================================================================
#pragma mark -
#pragma mark * Section

Section::Section(ZRef<SectionBody> iBody)
:	fBody(iBody)
	{}

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
#pragma mark * SectionBody_Concrete

ZRef<UITableViewCell> SectionBody_Concrete::UITableViewCellForRow(UITableView* iView, size_t iRowIndex)
	{
	// Our subclass can override UITableViewCellForRow, or
	// fCallable_MakeTableViewCell must have been set.
	ZAssert(fCallable_GetCell);
	return fCallable_GetCell->Call(iView, iRowIndex);
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

ZQ<UITableViewRowAnimation> SectionBody_Concrete::QRowAnimation_Insert()
	{ return fRowAnimation_Insert; }

ZQ<UITableViewRowAnimation> SectionBody_Concrete::QRowAnimation_Delete()
	{ return fRowAnimation_Delete; }

ZQ<UITableViewRowAnimation> SectionBody_Concrete::QRowAnimation_Reload()
	{ return fRowAnimation_Reload; }

bool SectionBody_Concrete::ButtonTapped(
	UITableViewController_WithSections* iTVC, UITableView* iTableView, size_t iRowIndex)
	{
	if (fCallable_ButtonTapped)
		return fCallable_ButtonTapped->Call(iTVC, iTableView, this, iRowIndex);
	return false;
	}

ZQ<bool> SectionBody_Concrete::HasButton(size_t iRowIndex)
	{
	if (fCallable_ButtonTapped)
		return true;
	return null;
	}

bool SectionBody_Concrete::RowSelected(
	UITableViewController_WithSections* iTVC, UITableView* iTableView, size_t iRowIndex)
	{
	if ([iTableView isEditing])
		{
		if (fCallable_RowSelected_Editing)
			return fCallable_RowSelected_Editing->Call(iTVC, iTableView, this, iRowIndex);
		}
	else
		{
		if (fCallable_RowSelected)
			return fCallable_RowSelected->Call(iTVC, iTableView, this, iRowIndex);
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
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Insert())
		return theQ.Get();
	return UITableViewRowAnimationRight;
//	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation SectionBody_Concrete::RowAnimation_Delete()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Delete())
		return theQ.Get();
	return UITableViewRowAnimationRight;
//	return UITableViewRowAnimationTop;
//	return UITableViewRowAnimationNone;
	}

UITableViewRowAnimation SectionBody_Concrete::RowAnimation_Reload()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Reload())
		return theQ.Get();
	return UITableViewRowAnimationFade;
//	return UITableViewRowAnimationNone;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_SingleRow

SectionBody_SingleRow::SectionBody_SingleRow(ZRef<UITableViewCell> iCell)
:	fCell(iCell)
,	fCell_Pending(iCell)
	{}

size_t SectionBody_SingleRow::NumberOfRows()
	{ return 1; }

void SectionBody_SingleRow::Update_NOP()
	{}

void SectionBody_SingleRow::Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
	RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload)
	{
	ioRowMeta_Old.UpdateCount(1);
	ioRowMeta_New.UpdateCount(1);
	if (fCell != fCell_Pending)
		ioRowUpdate_Reload.Add(0, this->RowAnimation_Reload());
	}

void SectionBody_SingleRow::Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New)
	{
	ioRowMeta_New.UpdateCount(1);
	ioRowUpdate_New.Add(0, this->RowAnimation_Insert());
	}

void SectionBody_SingleRow::Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old)
	{
	ioRowMeta_Old.UpdateCount(1);
	ioRowUpdate_Old.Add(0, this->RowAnimation_Delete());
	}

void SectionBody_SingleRow::FinishUpdate()
	{
	fCell = fCell_Pending;
	}

ZRef<UITableViewCell> SectionBody_SingleRow::UITableViewCellForRow(UITableView* iView,
	size_t iRowIndex)
	{ return fCell; }

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
		(*i)->Update_Insert(ioRowMeta_Old, ioRowUpdate_Old);
	}

void SectionBody_Multi::FinishUpdate()
	{
	fBodies = fBodies_Pending;
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		(*i)->FinishUpdate();
	}

ZRef<UITableViewCell> SectionBody_Multi::UITableViewCellForRow(UITableView* iView, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZRef<UITableViewCell> result = theBody->UITableViewCellForRow(iView, localRowIndex))
			return result;
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

bool SectionBody_Multi::ButtonTapped(UITableViewController_WithSections* iTVC, UITableView* iTableView, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (theBody->ButtonTapped(iTVC, iTableView, localRowIndex))
			return true;
		}
	return false;
	}

ZQ<bool> SectionBody_Multi::HasButton(size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (ZQ<bool> theQ = theBody->HasButton(localRowIndex))
			return theQ;
		}
	return null;
	}

bool SectionBody_Multi::RowSelected(UITableViewController_WithSections* iTVC,
	UITableView* iTableView, size_t iRowIndex)
	{
	size_t localRowIndex;
	if (ZRef<SectionBody> theBody = this->pGetBodyAndRowIndex(localRowIndex, iRowIndex))
		{
		if (theBody->RowSelected(iTVC, iTableView, localRowIndex))
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
	{
	oIndex = iIndex;
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		{
		ZRef<SectionBody> theBody = *i;
		const size_t theCount = theBody->NumberOfRows();
		if (oIndex < theCount)
			return theBody;
		oIndex -= theCount;
		}
	return null;
	}

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * UITableViewController_WithSections

using namespace ZooLib;
using namespace ZooLib::UIKit;

@interface UITableViewController_WithSections (Private)

- (ZRef<Section>)pGetSection:(size_t)iSectionIndex;
- (void)pDoUpdate1;
- (void)pDoUpdate2;
- (void)pDoUpdate3;
- (void)pDoUpdate4;

@end // interface UITableViewController_WithSections

@implementation UITableViewController_WithSections

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
	{
	[super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
	fCallable_NeedsUpdate = MakeCallable<void()>(self, @selector(needsUpdate));
	fNeedsUpdate = false;
	fUpdateInFlight = false;
	fShown = false;
	return self;
	}

- (void)viewWillAppear:(BOOL)animated
	{
	fShown = true;
	[super viewWillAppear:animated];
	}

- (void)viewDidDisappear:(BOOL)animated
	{
	[super viewDidDisappear:animated];
	fShown = false;
	}

- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
	{
	return fSections.size();
	}

- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<size_t> theQ = theSection->fBody->NumberOfRows())
			return theQ.Get();
		}
	return 0;
	}

- (void)tableView:(UITableView *)tableView
	commitEditingStyle:(UITableViewCellEditingStyle)editingStyle
	forRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->fBody->CommitEditingStyle(editingStyle, indexPath.row);
	}
 
- (NSIndexPath *)tableView:(UITableView *)tableView
	willSelectRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->fBody->CanSelect([tableView isEditing], indexPath.row))
			{
			if (!theQ.Get())
				return nil;
			}
		}
	return indexPath;
	}

 - (UITableViewCell*)tableView:(UITableView*)tableView
	cellForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		const size_t theRowIndex = indexPath.row;
		if (ZRef<UITableViewCell> theCell =
			theSection->fBody->UITableViewCellForRow(tableView, theRowIndex))
			{
			if (ZQ<bool> theQ = theSection->fBody->HasButton(theRowIndex))
				{
				if (theQ.Get())
					[theCell setAccessoryType:UITableViewCellAccessoryDetailDisclosureButton];
				else
					[theCell setEditingAccessoryType:UITableViewCellAccessoryNone];
				}
			else
				{
				if (ZQ<bool> theQ = theSection->fBody->CanSelect(false, theRowIndex))
					{
					if (theQ.Get())
						[theCell setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
					else
						[theCell setAccessoryType:UITableViewCellAccessoryNone];
					}

				if (ZQ<bool> theQ = theSection->fBody->CanSelect(true, theRowIndex))
					{
					if (theQ.Get())
						[theCell setEditingAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
					else
						[theCell setEditingAccessoryType:UITableViewCellAccessoryNone];
					}
				}

			return [theCell.Orphan() autorelease];
			}
		}
	return nullptr;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<CGFloat> theQ = theSection->fBody->QRowHeight(indexPath.row))
			return theQ.Get();
		}
	CGFloat result = tableView.rowHeight;
	return result;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QHeaderHeight())
			return theQ.Get();
		}
	return tableView.sectionHeaderHeight;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->QFooterHeight())
			return theQ.Get();
		}
	return tableView.sectionFooterHeight;
	}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QHeaderTitle())
			return ZUtil_NSObject::sString(theQ.Get());
		}
	return nullptr;
	}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->QFooterTitle())
			return ZUtil_NSObject::sString(theQ.Get());
		}
	return nullptr;
	}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->QHeaderView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
	{
	if (ZRef<Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->QFooterView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (void)tableView:(UITableView *)tableView
	accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->fBody->ButtonTapped(self, tableView, indexPath.row);
	}

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->fBody->RowSelected(self, tableView, indexPath.row);
	}

- (BOOL)tableView:(UITableView *)tableView
	shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->fBody->QShouldIndentWhileEditing(indexPath.row))
			return theQ.Get();
		}
	return true;
	}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView
	editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theSection->fBody->QEditingStyle(indexPath.row))
			return theQ.Get();
		}
	return UITableViewCellEditingStyleNone;
	}

- (NSInteger)tableView:(UITableView *)tableView
	indentationLevelForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<NSInteger> theQ = theSection->fBody->QIndentationLevel(indexPath.row))
			return theQ.Get();
		}
	return 0;
	}

- (void)appendSection:(ZRef<Section>) iSection
	{
	fSections_Pending.push_back(iSection);
	[self needsUpdate];
	}

- (void)needsUpdate
	{
	if (fNeedsUpdate)
		return;
	fNeedsUpdate = true;
	if (!fUpdateInFlight)
		[self performSelectorOnMainThread:@selector(pDoUpdate1) withObject:nil waitUntilDone:NO];
	}

static void spInsertSections(UITableView* iTableView, bool iShown,
	size_t iBaseIndex,
	const ZRef<Section>* iSections,
	size_t iCount)
	{
	for (size_t x = 0; x < iCount; ++x)
		{
		ZRef<Section> theSection = iSections[x];
		theSection->fBody->Update_NOP();
		theSection->fBody->FinishUpdate();
		[iTableView
			insertSections:sMakeIndexSet(iBaseIndex + x)
			withRowAnimation:iShown ? theSection->SectionAnimation_Insert() : UITableViewRowAnimationNone];
		}
	}

- (void)pDoUpdate1
	{
	ZAssert(fNeedsUpdate);
	ZAssert(!fUpdateInFlight);

	fNeedsUpdate = false;
	fUpdateInFlight = true;

	if (!fShown)
		{
		ZLOGTRACE(eDebug);
		fSections = fSections_Pending;
		for (size_t x = 0; x < fSections.size(); ++x)
			{
			fSections[x]->fBody->Update_NOP();
			fSections[x]->fBody->FinishUpdate();
			}
		[self.tableView reloadData];
		[self pDoUpdate4];
		return;
		}

	// Insert and delete sections first, this will be rare.
	fSections_ToIgnore.clear();
	if (fSections == fSections_Pending)
		{
		[self pDoUpdate2];
		return;
		}

	const bool isShown = fShown;
	[self.tableView beginUpdates];
	
	const vector<ZRef<Section> > sectionsOld = fSections;
	fSections = fSections_Pending;

	const size_t endOld = sectionsOld.size();

	size_t iterNew = 0;
	const size_t endNew = fSections.size();

	for (size_t iterOld = 0; iterOld < endOld; ++iterOld)
		{
		const ZRef<Section> sectionOld = sectionsOld[iterOld];
		const size_t inNew =
			find(fSections.begin(), fSections.end(), sectionOld) - fSections.begin();
		if (inNew == endNew)
			{
			// It's no longer in fSections, and must be deleted.
			[self.tableView
				deleteSections:sMakeIndexSet(iterOld)
				withRowAnimation:isShown ? sectionOld->SectionAnimation_Delete() : UITableViewRowAnimationNone];
			}
		else
			{
			if (size_t countToInsert = inNew - iterNew)
				{
				// There are sections to insert prior to sectionOld.
				spInsertSections(self.tableView, isShown, iterNew, &fSections[iterNew], countToInsert);
				fSections_ToIgnore.insert(&fSections[iterNew], &fSections[iterNew + countToInsert]);
				}

			fSections_ToIgnore.insert(sectionOld);
			iterNew = inNew + 1;
			}
		}
			
	// Insert remainder of pending.
	if (size_t countToInsert = endNew - iterNew)
		{
		spInsertSections(self.tableView, isShown, iterNew, &fSections[iterNew], countToInsert);
		fSections_ToIgnore.insert(&fSections[iterNew], &fSections[iterNew + countToInsert]);
		}

	[self.tableView endUpdates];

	if (isShown)
		{
		ZLOGTRACE(eDebug);
		[self performSelector:@selector(pDoUpdate2)
			 withObject:nil
			 afterDelay:0.35];
		 }
	else
		{
		[self pDoUpdate2];
		}
	}

- (void)pDoUpdate2
	{
	ZAssert(fUpdateInFlight);

	UITableView* theTV = self.tableView;

	fInserts.clear();
	fInserts.resize(fSections.size());

	fDeletes.clear();
	fDeletes.resize(fSections.size());

	fReloads.clear();
	fReloads.resize(fSections.size());

	bool anyReloads = false;
	for (size_t x = 0; x < fSections.size(); ++x)
		{
		if (!ZUtil_STL::sContains(fSections_ToIgnore, fSections[x]))
			{
			SectionBody::RowMeta theRowMeta_Old;
			SectionBody::RowMeta theRowMeta_New;
			SectionBody::RowUpdate theRowUpdate_Insert(theRowMeta_New, fInserts[x]);
			SectionBody::RowUpdate theRowUpdate_Delete(theRowMeta_Old, fDeletes[x]);
			SectionBody::RowUpdate theRowUpdate_Reload(theRowMeta_Old, fReloads[x]);
			fSections[x]->fBody->Update_Normal(theRowMeta_Old, theRowMeta_New,
				theRowUpdate_Insert, theRowUpdate_Delete, theRowUpdate_Reload);
			if (fReloads[x].size())
				anyReloads = true;
			}
		}
	
	bool anyAnimatedReloads = anyReloads;
	if (anyReloads)
		{
		const bool isShown = fShown;
		[self.tableView beginUpdates];
		for (size_t x = 0; x < fReloads.size(); ++x)
			{
			map<size_t, UITableViewRowAnimation>& theMap = fReloads[x];
			for (map<size_t, UITableViewRowAnimation>::iterator i = theMap.begin();
				i != theMap.end(); ++i)
				{
				UITableViewRowAnimation theAnimation = UITableViewRowAnimationNone;
				if (isShown)
					theAnimation = i->second;
				if (UITableViewRowAnimationNone != theAnimation)
					anyAnimatedReloads = true;
				[theTV
					reloadRowsAtIndexPaths:sMakeNSIndexPathArray(x, i->first, 1)
					withRowAnimation:theAnimation];
				}
			}
		[self.tableView endUpdates];
		}

	if (anyAnimatedReloads)
		{
		ZLOGTRACE(eDebug);
		[self performSelector:@selector(pDoUpdate3)
			 withObject:nil
			 afterDelay:0.2];
		}
	else
		{
		[self pDoUpdate3];
		}
	}

- (void)pDoUpdate3
	{
	ZAssert(fUpdateInFlight);

	UITableView* theTV = self.tableView;

	const bool isShown = fShown;

	bool anyChanges = false;
	for (size_t x = 0; x < fDeletes.size(); ++x)
		{
		if (fDeletes[x].size())
			anyChanges = true;
		}

	for (size_t x = 0; x < fInserts.size(); ++x)
		{
		if (fInserts[x].size())
			anyChanges = true;
		}

	if (!anyChanges)
		{
		for (size_t x = 0; x < fSections.size(); ++x)
			fSections[x]->fBody->FinishUpdate();
		}
	else
		{
		[self.tableView beginUpdates];
		for (size_t x = 0; x < fDeletes.size(); ++x)
			{
			map<size_t, UITableViewRowAnimation>& theMap = fDeletes[x];
			for (map<size_t, UITableViewRowAnimation>::iterator i = theMap.begin();
				i != theMap.end(); ++i)
				{
				[theTV
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
				[theTV
					insertRowsAtIndexPaths:sMakeNSIndexPathArray(x, i->first, 1)
					withRowAnimation:isShown ? i->second : UITableViewRowAnimationNone];
				}
			}
		for (size_t x = 0; x < fSections.size(); ++x)
			fSections[x]->fBody->FinishUpdate();
		[self.tableView endUpdates];
		}

	if (isShown && anyChanges)
		{
		ZLOGTRACE(eDebug);
		[self performSelector:@selector(pDoUpdate4)
			 withObject:nil
			 afterDelay:0.35];
		}
	else
		{
		[self pDoUpdate4];
		}
	}

- (void)pDoUpdate4
	{
	ZAssert(fUpdateInFlight);
	fUpdateInFlight = false;
	if (fNeedsUpdate)
		[self performSelectorOnMainThread:@selector(pDoUpdate1) withObject:nil waitUntilDone:NO];
	}

-(ZRef<Section>)pGetSection:(size_t)iSectionIndex
	{
	if (iSectionIndex < fSections.size())
		return fSections[iSectionIndex];
	return null;
	}

@end // implementation UITableViewController_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)
