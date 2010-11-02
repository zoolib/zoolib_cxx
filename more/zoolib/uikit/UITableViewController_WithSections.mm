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

#include "zoolib/ZLog.h"
#include "zoolib/ZRef_NSObject.h"
#include "zoolib/ZUtil_NSObject.h"

namespace ZooLib {
namespace UIKit {

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
	return UITableViewRowAnimationRight;
	}

UITableViewRowAnimation Section::SectionAnimation_Delete()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Delete())
		return theQ.Get();
	return UITableViewRowAnimationLeft;
	}

UITableViewRowAnimation Section::SectionAnimation_Reload()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QSectionAnimation_Reload())
		return theQ.Get();
	return UITableViewRowAnimationFade;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody

ZRef<UITableViewCell> SectionBody::UITableViewCellForRow(UITableView* iView, size_t iRowIndex)
	{
	// Our subclass can override UITableViewCellForRow, or
	// fCallable_MakeTableViewCell must have been set.
	ZAssert(fCallable_MakeTableViewCell);
	return fCallable_MakeTableViewCell->Call(iView, iRowIndex);
	}

ZQ<UITableViewCellEditingStyle> SectionBody::QEditingStyle(size_t iRowIndex)
	{ return fEditingStyle; }

ZQ<bool> SectionBody::QShouldIndentWhileEditing(size_t iRowIndex)
	{ return fShouldIndentWhileEditing; }

ZQ<CGFloat> SectionBody::QRowHeight(size_t iRowIndex)
	{ return fRowHeight; }

ZQ<NSInteger> SectionBody::QIndentationLevel(size_t iRowIndex)
	{ return fIndentationLevel; }

ZQ<UITableViewRowAnimation> SectionBody::QRowAnimation_Insert()
	{ return fRowAnimation_Insert; }

ZQ<UITableViewRowAnimation> SectionBody::QRowAnimation_Delete()
	{ return fRowAnimation_Delete; }

ZQ<UITableViewRowAnimation> SectionBody::QRowAnimation_Reload()
	{ return fRowAnimation_Reload; }

bool SectionBody::AccessoryButtonTapped(size_t iRowIndex)
	{
	if (fCallable_AccessoryButtonTapped)
		return fCallable_AccessoryButtonTapped->Call(this, iRowIndex);
	return false;
	}

bool SectionBody::RowSelected(size_t iRowIndex)
	{
	if (fCallable_RowSelected)
		fCallable_RowSelected->Call(this, iRowIndex);
	return false;
	}

UITableViewRowAnimation SectionBody::RowAnimation_Insert()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Insert())
		return theQ.Get();
	return UITableViewRowAnimationTop;
	return UITableViewRowAnimationRight;
	}

UITableViewRowAnimation SectionBody::RowAnimation_Delete()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Delete())
		return theQ.Get();
	return UITableViewRowAnimationBottom;
	return UITableViewRowAnimationLeft;
	}

UITableViewRowAnimation SectionBody::RowAnimation_Reload()
	{
	if (ZQ<UITableViewRowAnimation> theQ = this->QRowAnimation_Reload())
		return theQ.Get();
	return UITableViewRowAnimationFade;
	}

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_SingleRow

SectionBody_SingleRow::SectionBody_SingleRow(ZRef<UITableViewCell> iCell)
:	fCell_Pending(iCell)
	{}

size_t SectionBody_SingleRow::NumberOfRows()
	{ return 1; }

void SectionBody_SingleRow::ApplyUpdates(UITableView* iTableView, size_t iSectionIndex,
	size_t iBaseOld, size_t iBaseNew)
	{
	if (fCell == fCell_Pending)
		return;

	fCell = fCell_Pending;
	
	[iTableView
		reloadRowsAtIndexPaths:sMakeNSIndexPathArray(iSectionIndex, iBaseOld, 1)
		withRowAnimation:this->RowAnimation_Reload()];
	}

ZRef<UITableViewCell> SectionBody_SingleRow::UITableViewCellForRow(UITableView* iView, size_t iRowIndex)
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

static size_t spInsertBodies(UITableView* iTableView,
	size_t iSectionIndex,
	size_t iRowIndex,
	const ZRef<SectionBody>* iBodies,
	size_t iCount)
	{
	for (size_t x = 0; x < iCount; ++x)
		{
		ZRef<SectionBody> theBody = iBodies[x];
		theBody->ApplyUpdates(nil, 0, 0, 0);
		const UITableViewRowAnimation theAnimation = theBody->RowAnimation_Insert();
		for (size_t y = theBody->NumberOfRows(); y; --y)
			{
			[iTableView
				insertRowsAtIndexPaths:sMakeNSIndexPathArray(iSectionIndex, iRowIndex, 1)
				withRowAnimation:theAnimation];
			++iRowIndex;
			}
		}
	return iRowIndex;
	}

void SectionBody_Multi::ApplyUpdates(UITableView* iTableView, size_t iSectionIndex,
	size_t iBaseOld, size_t iBaseNew)
	{
	const vector<ZRef<SectionBody> > bodiesOld = fBodies;
	fBodies = fBodies_Pending;

	size_t iterOld = 0;
	const size_t endOld = bodiesOld.size();

	size_t iterNew = 0;
	const size_t endNew = fBodies.size();

	while (iterOld < endOld)
		{
		const ZRef<SectionBody> bodyOld = bodiesOld[iterOld];
		const size_t inNew = find(fBodies.begin(), fBodies.end(), bodyOld) - fBodies.begin();
		const size_t countOld = bodyOld->NumberOfRows();
		if (inNew == endNew)
			{
			// It's no longer in fBodies so delete its rows.
			[iTableView
				deleteRowsAtIndexPaths:sMakeNSIndexPathArray(iSectionIndex, iBaseOld, countOld)
				withRowAnimation:this->RowAnimation_Delete()];
			}
		else
			{
			if (size_t countToInsert = inNew - iterNew)
				iBaseNew = spInsertBodies(iTableView, iSectionIndex, iBaseNew, &fBodies[iterNew], countToInsert);

			// It's still in fBodies. Give it the opportunity to apply any changes.
			bodyOld->ApplyUpdates(iTableView, iSectionIndex, iBaseOld, iBaseNew);

			iBaseNew += bodyOld->NumberOfRows();
			iterNew = inNew + 1;
			}
		iBaseOld += countOld;
		++iterOld;
		}

	// Insert remainder of pending.
	if (size_t countToInsert = endNew - iterNew)
		spInsertBodies(iTableView, iSectionIndex, iBaseNew, &fBodies[iterNew], countToInsert);
	}

ZRef<UITableViewCell> SectionBody_Multi::UITableViewCellForRow(UITableView* iView, size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		{
		if (ZRef<UITableViewCell> result = theBody->UITableViewCellForRow(iView, iRowIndex))
			return result;
		}
	return SectionBody::UITableViewCellForRow(iView, iRowIndex);
	}

ZQ<UITableViewCellEditingStyle> SectionBody_Multi::QEditingStyle(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theBody->QEditingStyle(iRowIndex))
			return theQ;
		}
	return SectionBody::QEditingStyle(iRowIndex);
	}

ZQ<bool> SectionBody_Multi::QShouldIndentWhileEditing(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		{
		if (ZQ<bool> theQ = theBody->QShouldIndentWhileEditing(iRowIndex))
			return theQ;
		}
	return SectionBody::QShouldIndentWhileEditing(iRowIndex);
	}

ZQ<CGFloat> SectionBody_Multi::QRowHeight(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		{
		if (ZQ<CGFloat> theQ = theBody->QRowHeight(iRowIndex))
			return theQ;
		}
	return SectionBody::QRowHeight(iRowIndex);
	}

ZQ<NSInteger> SectionBody_Multi::QIndentationLevel(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		{
		if (ZQ<NSInteger> theQ = theBody->QIndentationLevel(iRowIndex))
			return theQ;
		}
	return SectionBody::QIndentationLevel(iRowIndex);
	}

bool SectionBody_Multi::AccessoryButtonTapped(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		return theBody->AccessoryButtonTapped(iRowIndex);
	return SectionBody::AccessoryButtonTapped(iRowIndex);
	}

bool SectionBody_Multi::RowSelected(size_t iRowIndex)
	{
	if (ZRef<SectionBody> theBody = this->pGetBody(iRowIndex))
		return theBody->RowSelected(iRowIndex);
	return SectionBody::RowSelected(iRowIndex);
	}

ZRef<SectionBody> SectionBody_Multi::pGetBody(size_t& ioIndex)
	{
	size_t theIndex = ioIndex;
	for (vector<ZRef<SectionBody> >::iterator i = fBodies.begin(); i != fBodies.end(); ++i)
		{
		ZRef<SectionBody> theBody = *i;
		const size_t theCount = theBody->NumberOfRows();
		if (theIndex < theCount)
			{
			ioIndex = theIndex;
			return theBody;
			}
		theIndex -= theCount;
		}
	return null;
	}

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * UITableViewController_WithSections

using namespace ZooLib;
using ZooLib::UIKit::Section;
//using ZooLib::UIKit::SectionBody;
using std::vector;

@interface UITableViewController_WithSections (Private)

- (ZRef<Section>)pGetSection:(size_t)iSectionIndex;
- (void)pDoUpdate;

@end // interface UITableViewController_WithSections

@implementation UITableViewController_WithSections

- (id)initWithStyle:(UITableViewStyle)style
	{
	[super initWithStyle:style];

	fNeedsUpdate = false;

	return self;
	}

- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
	{
	ZLOGTRACE(eDebug);
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

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
	{
	// The fact that this method is implemented is what enables "swipe to delete".
	}
 
 - (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZRef<UITableViewCell> theCell = theSection->fBody->UITableViewCellForRow(tableView, indexPath.row))
			return [theCell.Orphan() autorelease];
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
	return tableView.rowHeight;
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

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->fBody->AccessoryButtonTapped(indexPath.row);
	}

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		theSection->fBody->RowSelected(indexPath.row);
	}

- (BOOL)tableView:(UITableView *)tableView shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->fBody->QShouldIndentWhileEditing(indexPath.row))
			return theQ.Get();
		}
	return true;
	}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theSection->fBody->QEditingStyle(indexPath.row))
			return theQ.Get();
		}
	return UITableViewCellEditingStyleNone;
	}

- (NSInteger)tableView:(UITableView *)tableView indentationLevelForRowAtIndexPath:(NSIndexPath *)indexPath
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

static NSIndexSet* spAsIndexSet(size_t iIndex)
	{ return [NSIndexSet indexSetWithIndex:iIndex]; }

static void spInsertSections(UITableView* iTableView,
	size_t iBaseIndex,
	const ZRef<Section>* iSections,
	size_t iCount)
	{
	for (size_t x = 0; x < iCount; ++x)
		{
		ZRef<Section> theSection = iSections[x];
		theSection->fBody->ApplyUpdates(nil, 0, 0, 0);
		[iTableView
			insertSections:spAsIndexSet(iBaseIndex + x)
			withRowAnimation:theSection->SectionAnimation_Insert()];
		}
	}

- (void)pDoUpdate
	{
	if (!fNeedsUpdate)
		return;
	fNeedsUpdate = false;

	[self.tableView beginUpdates];

	const vector<ZRef<Section> > sectionsOld = fSections;
	fSections = fSections_Pending;

	size_t iterOld = 0;
	const size_t endOld = sectionsOld.size();

	size_t iterNew = 0;
	const size_t endNew = fSections.size();

	while (iterOld < endOld)
		{
		const ZRef<Section> sectionOld = sectionsOld[iterOld];
		const size_t inNew = find(fSections.begin(), fSections.end(), sectionOld) - fSections.begin();
		if (inNew == endNew)
			{
			// It's no longer in fSections, and must be deleted.
			[self.tableView
				deleteSections:spAsIndexSet(iterOld)
				withRowAnimation:sectionOld->SectionAnimation_Delete()];
			++iterOld;
			}
		else
			{
			// It's (still) in fSections. Give it the opportunity to apply any changes.
			sectionOld->fBody->ApplyUpdates(self.tableView, iterNew, 0, 0);
			
			if (size_t countToInsert = inNew - iterNew)
				{
				// There are sections to insert prior to sectionOld.
				spInsertSections(self.tableView, iterNew, &fSections[iterNew], countToInsert);
				}

			++iterOld;
			iterNew = inNew + 1;
			}
		}
			
	// Insert remainder of pending.
	if (size_t countToInsert = endNew - iterNew)
		spInsertSections(self.tableView, iterNew, &fSections[iterNew], countToInsert);

	[self.tableView endUpdates];
	}

- (void)needsUpdate
	{
	if (fNeedsUpdate)
		return;
	fNeedsUpdate = true;
	[self performSelectorOnMainThread:@selector(pDoUpdate) withObject:nil waitUntilDone:NO];
	}

-(ZRef<Section>)pGetSection:(size_t)iSectionIndex
	{
	if (iSectionIndex < fSections.size())
		return fSections[iSectionIndex];
	return null;
	}

@end // implementation UITableViewController_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)
