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

#ifndef __ZooLib_UITableViewController__
#define __ZooLib_UITableViewController__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZUnicodeString.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#import <UIKit/UITableView.h>

#include <vector>

namespace ZooLib {
namespace IPhone {

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section

class UITVC_Section : public ZCounted
	{
public:
	UITVC_Section();

	void SetHeaderFooterShownWhenEmpty(bool iShow);

	virtual ZQ<CGFloat> HeaderHeight();
	virtual ZQ<CGFloat> FooterHeight();

	virtual ZQ<string8> HeaderTitle();
	virtual ZQ<string8> FooterTitle();

	virtual ZRef<UIView> HeaderView();
	virtual ZRef<UIView> FooterView();

	virtual size_t NumberOfRows();
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iIndex);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle(size_t iIndex);
	virtual ZQ<bool> ShouldIndentWhileEditing(size_t iIndex);
	virtual ZQ<CGFloat> RowHeight(size_t iIndex);

	virtual void AccessoryButtonTapped(size_t iIndex);
	virtual void RowSelected(size_t iIndex);

// -----
	bool fHeaderFooterShownWhenEmpty;


	ZQ<CGFloat> fHeaderHeight;
	ZQ<CGFloat> fFooterHeight;

	ZQ<string8> fHeaderTitle;
	ZQ<string8> fFooterTitle;

	ZRef<UIView> fHeaderView;
	ZRef<UIView> fFooterView;

	ZRef<UITableViewCell> fTableViewCell;
	ZQ<UITableViewCellEditingStyle> fEditingStyle;
	ZQ<bool> fShouldIndentWhileEditing;
	ZQ<CGFloat> fRowHeight;
	ZRef<ZCallable<void(ZRef<UITVC_Section>,size_t)> > fCallable_AccessoryButtonTapped;
	ZRef<ZCallable<void(ZRef<UITVC_Section>,size_t)> > fCallable_RowSelected;
	};

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section_WithRow

class UITVC_Section_WithRow : public UITVC_Section
	{
public:
	class Row;
	void AddRow(ZRef<Row> iRow);

	virtual size_t NumberOfRows();
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iIndex);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle(size_t iIndex);
	virtual ZQ<bool> ShouldIndentWhileEditing(size_t iIndex);
	virtual ZQ<CGFloat> RowHeight(size_t iIndex);
	virtual void AccessoryButtonTapped(size_t iIndex);
	virtual void RowSelected(size_t iIndex);

	std::vector<ZRef<Row> > fRows;

	ZRef<Row> pGetRow(size_t iIndex);
	};

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section_WithRow::Row

class UITVC_Section_WithRow::Row : public ZCounted
	{
public:
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle();
	virtual ZQ<bool> ShouldIndentWhileEditing();
	virtual ZQ<CGFloat> RowHeight();
	virtual void AccessoryButtonTapped();
	virtual void RowSelected();

	ZRef<UITableViewCell> fTableViewCell;
	ZQ<UITableViewCellEditingStyle> fEditingStyle;
	ZQ<bool> fShouldIndentWhileEditing;
	ZQ<CGFloat> fRowHeight;
	ZRef<ZCallable<void(ZRef<Row>)> > fCallable_AccessoryButtonTapped;
	ZRef<ZCallable<void(ZRef<Row>)> > fCallable_RowSelected;
	};

} // namespace IPhone
} // namespace ZooLib

using namespace ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib_UITVC_WithSections

@interface ZooLib_UITVC_WithSections : UITableViewController
	{
	std::vector<ZRef<IPhone::UITVC_Section> > fSections;
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
- (void) addSection:(ZRef<IPhone::UITVC_Section>) iSection;

-(ZRef<IPhone::UITVC_Section>)pGetSection:(size_t)iIndex;

@end // ZooLib_UITableViewController

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __ZooLib_UITableViewController__
