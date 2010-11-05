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

#ifndef __UITVC_WithSections__
#define __UITVC_WithSections__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZUnicodeString.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#import <UIKit/UITableView.h>

#include <vector>

namespace ZooLib {
namespace UIKit {

NSArray* sMakeNSIndexPathArray(size_t iSectionIndex, size_t iBaseRowIndex, size_t iCount);
NSIndexSet* sMakeIndexSet(size_t iIndex);

class SectionBody;

// =================================================================================================
#pragma mark -
#pragma mark * Section

class Section : public ZCounted
	{
public:
	Section(ZRef<SectionBody> iBody);

	virtual ZQ<CGFloat> QHeaderHeight();
	virtual ZQ<CGFloat> QFooterHeight();

	virtual ZQ<string8> QHeaderTitle();
	virtual ZQ<string8> QFooterTitle();

	virtual ZRef<UIView> QHeaderView();
	virtual ZRef<UIView> QFooterView();

	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Insert();
	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Delete();
	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Reload();

//##	bool fHeaderShownWhenEmpty;
//##	bool fFooterShownWhenEmpty;

	ZQ<CGFloat> fHeaderHeight;
	ZQ<CGFloat> fFooterHeight;

	ZQ<string8> fHeaderTitle;
	ZQ<string8> fFooterTitle;

	ZRef<UIView> fHeaderView;
	ZRef<UIView> fFooterView;

	ZQ<UITableViewRowAnimation> fSectionAnimation_Insert;
	ZQ<UITableViewRowAnimation> fSectionAnimation_Delete;
	ZQ<UITableViewRowAnimation> fSectionAnimation_Reload;

	ZRef<SectionBody> fBody;

// -----

	UITableViewRowAnimation SectionAnimation_Insert();
	UITableViewRowAnimation SectionAnimation_Delete();
	UITableViewRowAnimation SectionAnimation_Reload();
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody

class SectionBody : public ZCounted
	{
public:
	virtual size_t NumberOfRows() = 0;
	virtual void ApplyUpdates(UITableView* iTableView, size_t iSectionIndex,
		size_t iBaseOld, size_t iBaseNew) = 0;

// -----

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual ZQ<CGFloat> QRowHeight(size_t iRowIndex);
	virtual ZQ<NSInteger> QIndentationLevel(size_t iRowIndex);

	virtual ZQ<UITableViewRowAnimation> QRowAnimation_Insert();
	virtual ZQ<UITableViewRowAnimation> QRowAnimation_Delete();
	virtual ZQ<UITableViewRowAnimation> QRowAnimation_Reload();

	ZQ<UITableViewRowAnimation> fRowAnimation_Insert;
	ZQ<UITableViewRowAnimation> fRowAnimation_Delete;
	ZQ<UITableViewRowAnimation> fRowAnimation_Reload;

	virtual bool AccessoryButtonTapped(size_t iRowIndex);
	virtual bool RowSelected(size_t iRowIndex);

	ZQ<UITableViewCellEditingStyle> fEditingStyle;
	ZQ<bool> fShouldIndentWhileEditing;
	ZQ<CGFloat> fRowHeight;
	ZQ<NSInteger> fIndentationLevel;

	ZRef<ZCallable<ZRef<UITableViewCell>(UITableView*,size_t)> > fCallable_MakeTableViewCell;
	ZRef<ZCallable<bool(ZRef<SectionBody>,size_t)> > fCallable_AccessoryButtonTapped;
	ZRef<ZCallable<bool(ZRef<SectionBody>,size_t)> > fCallable_RowSelected;

// -----

	UITableViewRowAnimation RowAnimation_Insert();
	UITableViewRowAnimation RowAnimation_Delete();
	UITableViewRowAnimation RowAnimation_Reload();
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_SingleRow

class SectionBody_SingleRow : public SectionBody
	{
public:
	SectionBody_SingleRow(ZRef<UITableViewCell> iCell);

	virtual size_t NumberOfRows();
	virtual void ApplyUpdates(UITableView* iTableView, size_t iSectionIndex,
		size_t iBaseOld, size_t iBaseNew);

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);

	ZRef<UITableViewCell> fCell;
	ZRef<UITableViewCell> fCell_Pending;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_Multi

class SectionBody_Multi : public SectionBody
	{
public:
	virtual size_t NumberOfRows();
	virtual void ApplyUpdates(UITableView* iTableView, size_t iSectionIndex,
		size_t iBaseOld, size_t iBaseNew);

// -----

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual ZQ<CGFloat> QRowHeight(size_t iRowIndex);
	virtual ZQ<NSInteger> QIndentationLevel(size_t iRowIndex);

	virtual bool AccessoryButtonTapped(size_t iRowIndex);
	virtual bool RowSelected(size_t iRowIndex);

	std::vector<ZRef<SectionBody> > fBodies;
	std::vector<ZRef<SectionBody> > fBodies_Pending;

	ZRef<SectionBody> pGetBody(size_t& ioIndex);
	};

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * UITableViewController_WithSections

@interface UITableViewController_WithSections : UITableViewController
	{
	std::vector<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections;
	std::vector<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections_Pending;
	bool fNeedsUpdate;
	ZooLib::ZRef<ZooLib::ZCallable<void()> > fCallable_NeedsUpdate;
	}

// From UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView;
- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section;
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath;
 - (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath;

// From UITableViewDelegate
- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath;
- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section;
- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section;

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section;
- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section;

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section;
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section;

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath;
- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath;

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath;

// Our protocol
- (void)appendSection:(ZooLib::ZRef<ZooLib::UIKit::Section>) iSection;
- (void)needsUpdate;

@end // interface UITableViewController_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __UITVC_WithSections__
