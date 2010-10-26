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
#include "zoolib/uikit/UITVC_Section.h"

#if ZCONFIG_SPI_Enabled(iPhone)

#include "zoolib/ZRef_NSObject.h"
#include "zoolib/ZUtil_NSObject.h"

namespace ZooLib {
namespace UIKit {

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section

UITVC_Section::UITVC_Section()
:	fHeaderFooterShownWhenEmpty(true)
	{}

void UITVC_Section::SetHeaderFooterShownWhenEmpty(bool iShow)
	{ fHeaderFooterShownWhenEmpty = iShow; }

ZQ<CGFloat> UITVC_Section::HeaderHeight()
	{
	if (fHeaderFooterShownWhenEmpty || this->NumberOfRows())
		return fHeaderHeight;
	return null;
	}

ZQ<CGFloat> UITVC_Section::FooterHeight()
	{
	if (fHeaderFooterShownWhenEmpty || this->NumberOfRows())
		return fFooterHeight;
	return null;
	}

ZQ<string8> UITVC_Section::HeaderTitle()
	{ return fHeaderTitle; }

ZQ<string8> UITVC_Section::FooterTitle()
	{ return fFooterTitle; }

ZRef<UIView> UITVC_Section::HeaderView()
	{ return fHeaderView; }

ZRef<UIView> UITVC_Section::FooterView()
	{ return fFooterView; }

size_t UITVC_Section::NumberOfRows()
	{ return 0; }

ZRef<UITableViewCell> UITVC_Section::UITableViewCellForRow(UITableView* iView, size_t iIndex)
	{ return fTableViewCell; }

ZQ<UITableViewCellEditingStyle> UITVC_Section::EditingStyle(size_t iIndex)
	{ return fEditingStyle; }

ZQ<bool> UITVC_Section::ShouldIndentWhileEditing(size_t iIndex)
	{ return fShouldIndentWhileEditing; }

ZQ<CGFloat> UITVC_Section::RowHeight(size_t iIndex)
	{ return fRowHeight; }

void UITVC_Section::AccessoryButtonTapped(size_t iIndex)
	{
	if (fCallable_AccessoryButtonTapped)
		fCallable_AccessoryButtonTapped->Call(this, iIndex);
	}

void UITVC_Section::RowSelected(size_t iIndex)
	{
	if (fCallable_RowSelected)
		fCallable_RowSelected->Call(this, iIndex);
	}

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section_WithRow

void UITVC_Section_WithRow::AddRow(ZRef<Row> iRow)
	{
	fRows.push_back(iRow);
	}

size_t UITVC_Section_WithRow::NumberOfRows()
	{
	return fRows.size();
	}

ZRef<UITableViewCell> UITVC_Section_WithRow::UITableViewCellForRow(UITableView* iView, size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->UITableViewCellForRow(iView);
	return UITVC_Section::UITableViewCellForRow(iView, iIndex);
	}

ZQ<UITableViewCellEditingStyle> UITVC_Section_WithRow::EditingStyle(size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->EditingStyle();
	return UITVC_Section::EditingStyle(iIndex);
	}

void UITVC_Section_WithRow::AccessoryButtonTapped(size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->AccessoryButtonTapped();
	return UITVC_Section::AccessoryButtonTapped(iIndex);
	}

void UITVC_Section_WithRow::RowSelected(size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->RowSelected();
	return UITVC_Section::RowSelected(iIndex);
	}

ZQ<bool> UITVC_Section_WithRow::ShouldIndentWhileEditing(size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->ShouldIndentWhileEditing();
	return UITVC_Section::ShouldIndentWhileEditing(iIndex);
	}

ZQ<CGFloat> UITVC_Section_WithRow::RowHeight(size_t iIndex)
	{
	if (ZRef<Row> theRow = this->pGetRow(iIndex))
		return theRow->RowHeight();
	return UITVC_Section::RowHeight(iIndex);
	}

ZRef<UITVC_Section_WithRow::Row> UITVC_Section_WithRow::pGetRow(size_t iIndex)
	{
	if (iIndex < fRows.size())
		return fRows[iIndex];
	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark * UITVC_Section_WithRow::Row

ZRef<UITableViewCell> UITVC_Section_WithRow::Row::UITableViewCellForRow(UITableView* iView)
	{ return fTableViewCell; }

ZQ<UITableViewCellEditingStyle> UITVC_Section_WithRow::Row::EditingStyle()
	{ return fEditingStyle; }

ZQ<bool> UITVC_Section_WithRow::Row::ShouldIndentWhileEditing()
	{ return fShouldIndentWhileEditing; }

ZQ<CGFloat> UITVC_Section_WithRow::Row::RowHeight()
	{ return fRowHeight; }

void UITVC_Section_WithRow::Row::AccessoryButtonTapped()
	{
	if (fCallable_AccessoryButtonTapped)
		fCallable_AccessoryButtonTapped->Call(this);
	}

void UITVC_Section_WithRow::Row::RowSelected()
	{
	if (fCallable_RowSelected)
		fCallable_RowSelected->Call(this);
	}

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib_UITVC_WithSections

using ZooLib::UIKit::UITVC_Section;

@implementation ZooLib_UITVC_WithSections

- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView
	{ return fSections.size(); }

- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		return theSection->NumberOfRows();
	return 0;
	}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
	{
	// The fact that this method is implemented is what enables "swipe to delete".
	}
 
 - (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZRef<UITableViewCell> theCell = theSection->UITableViewCellForRow(tableView, indexPath.row))
			return [theCell.Orphan() autorelease];
		}
	return nullptr;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<CGFloat> theQ = theSection->RowHeight(indexPath.row))
			return theQ.Get();
		}
	return tableView.rowHeight;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->HeaderHeight())
			return theQ.Get();
		}
	return tableView.sectionHeaderHeight;
	}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZQ<CGFloat> theQ = theSection->FooterHeight())
			return theQ.Get();
		}
	return tableView.sectionFooterHeight;
	}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->HeaderTitle())
			return ZUtil_NSObject::sString(theQ.Get());
		}
	return nullptr;
	}

- (NSString *)tableView:(UITableView *)tableView titleForFooterInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZQ<string8> theQ = theSection->FooterTitle())
			return ZUtil_NSObject::sString(theQ.Get());
		}
	return nullptr;
	}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->HeaderView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:section])
		{
		if (ZRef<UIView> theView = theSection->FooterView())
			return [theView.Orphan() autorelease];
		}
	return nullptr;
	}

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		theSection->AccessoryButtonTapped(indexPath.row);
	}

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		theSection->RowSelected(indexPath.row);
	}

- (BOOL)tableView:(UITableView *)tableView shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<bool> theQ = theSection->ShouldIndentWhileEditing(indexPath.row))
			return theQ.Get();
		}
	return true;
	}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
	{
	if (ZRef<UITVC_Section> theSection = [self pGetSection:indexPath.section])
		{
		if (ZQ<UITableViewCellEditingStyle> theQ = theSection->EditingStyle(indexPath.row))
			return theQ.Get();
		}
	return UITableViewCellEditingStyleNone;
	}

- (void) addSection:(ZRef<UITVC_Section>) iSection
	{
	fSections.push_back(iSection);
	}

-(ZRef<UITVC_Section>)pGetSection:(size_t)iIndex
	{
	if (iIndex < fSections.size())
		return fSections[iIndex];
	return null;
	}

@end // implementation ZooLib_UITVC_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)
