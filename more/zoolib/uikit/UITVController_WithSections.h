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

#if ZCONFIG_SPI_Enabled(iPhone)

#include "zoolib/ZCallable.h"
#include "zoolib/ZQ.h"
#include "zoolib/ZUnicodeString.h"

#import <UIKit/UITableView.h>

#include <map>
#include <set>
#include <vector>

@class UITVHandler_WithSections;

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

	ZRef<SectionBody> GetBody();

	bool fHideWhenEmpty;
	virtual bool HideWhenEmpty();

	ZQ<CGFloat> fHeaderHeight;
	virtual ZQ<CGFloat> QHeaderHeight();

	ZQ<CGFloat> fFooterHeight;
	virtual ZQ<CGFloat> QFooterHeight();

	ZQ<string8> fHeaderTitle;
	virtual ZQ<string8> QHeaderTitle();

	ZQ<string8> fFooterTitle;
	virtual ZQ<string8> QFooterTitle();

	ZRef<UIView> fHeaderView;
	virtual ZRef<UIView> QHeaderView();

	ZRef<UIView> fFooterView;
	virtual ZRef<UIView> QFooterView();

	ZQ<UITableViewRowAnimation> fSectionAnimation_Insert;
	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Insert();

	ZQ<UITableViewRowAnimation> fSectionAnimation_Delete;
	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Delete();

	ZQ<UITableViewRowAnimation> fSectionAnimation_Reload;
	virtual ZQ<UITableViewRowAnimation> QSectionAnimation_Reload();

// -----

	UITableViewRowAnimation SectionAnimation_Insert();
	UITableViewRowAnimation SectionAnimation_Delete();
	UITableViewRowAnimation SectionAnimation_Reload();

private:
	ZRef<SectionBody> fBody;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody

class SectionBody : public ZCounted
	{
public:
	class RowMeta;
	class RowUpdate;

// -----

	virtual size_t NumberOfRows() = 0;

	virtual void PreUpdate() = 0;
	virtual bool WillBeEmpty() = 0;
	virtual void Update_NOP() = 0;
	virtual void Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
		RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload) = 0;
	virtual void Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New) = 0;
	virtual void Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old) = 0;
	virtual void FinishUpdate() = 0;

// -----

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex) = 0;
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex) = 0;
	virtual bool CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex) = 0;
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex) = 0;
	virtual ZQ<CGFloat> QRowHeight(size_t iRowIndex) = 0;
	virtual ZQ<NSInteger> QIndentationLevel(size_t iRowIndex) = 0;

	virtual bool ButtonTapped(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex) = 0;
	virtual ZQ<bool> HasButton(bool iEditing, size_t iRowIndex) = 0;

	virtual bool RowSelected(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex) = 0;
	virtual ZQ<bool> CanSelect(bool iEditing, size_t iRowIndex) = 0;

	typedef ZCallable<ZRef<UITableViewCell>(UITableView*,size_t)> Callable_GetCell;

	typedef ZCallable<bool(UITVHandler_WithSections*,UITableView*,NSIndexPath*,ZRef<SectionBody>,size_t)> Callable_ButtonTapped;

	typedef ZCallable<bool(UITVHandler_WithSections*,UITableView*,NSIndexPath*,ZRef<SectionBody>,size_t)> Callable_RowSelected;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody::RowMeta

class SectionBody::RowMeta
	{
public:
	RowMeta();

	void UpdateCount(size_t iCount);

	size_t fBase;
	size_t fLimit;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody::RowUpdate

class SectionBody::RowUpdate
	{
public:
	RowUpdate(RowMeta& ioRowMeta, std::map<size_t, UITableViewRowAnimation>& ioMap);

	void Add(size_t iIndex, UITableViewRowAnimation iRowAnimation);
	void AddAll(UITableViewRowAnimation iRowAnimation);
	void AddRange(size_t iStart, size_t iCount, UITableViewRowAnimation iRowAnimation);

private:
	RowMeta& fRowMeta;
	std::map<size_t, UITableViewRowAnimation>& fMap;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_Concrete

class SectionBody_Concrete : public SectionBody
	{
public:
	SectionBody_Concrete();

// From SectionBody
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual bool CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual ZQ<CGFloat> QRowHeight(size_t iRowIndex);
	virtual ZQ<NSInteger> QIndentationLevel(size_t iRowIndex);

	virtual bool ButtonTapped(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex);
	virtual ZQ<bool> HasButton(bool iEditing, size_t iRowIndex);

	virtual bool RowSelected(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex);
	virtual ZQ<bool> CanSelect(bool iEditing, size_t iRowIndex);

//
	ZQ<UITableViewCellEditingStyle> fEditingStyle;
	ZQ<bool> fShouldIndentWhileEditing;
	ZQ<CGFloat> fRowHeight;
	ZQ<NSInteger> fIndentationLevel;

	ZRef<Callable_GetCell> fCallable_GetCell;

	ZRef<Callable_ButtonTapped> fCallable_ButtonTapped;
	ZRef<Callable_ButtonTapped> fCallable_ButtonTapped_Editing;

	ZRef<Callable_RowSelected> fCallable_RowSelected;
	ZRef<Callable_RowSelected> fCallable_RowSelected_Editing;

// Our protocol
	virtual UITableViewRowAnimation RowAnimation_Insert();
	virtual UITableViewRowAnimation RowAnimation_Delete();
	virtual UITableViewRowAnimation RowAnimation_Reload();

	UITableViewRowAnimation fRowAnimation_Insert;
	UITableViewRowAnimation fRowAnimation_Delete;
	UITableViewRowAnimation fRowAnimation_Reload;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_SingleRow

class SectionBody_SingleRow : public SectionBody_Concrete
	{
public:
	SectionBody_SingleRow(ZRef<UITableViewCell> iCell);

	virtual size_t NumberOfRows();

	virtual void PreUpdate();
	virtual bool WillBeEmpty();
	virtual void Update_NOP();
	virtual void Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
		RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload);
	virtual void Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New);
	virtual void Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old);
	virtual void FinishUpdate();

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);

	ZRef<UITableViewCell> fCell_Pending;

private:
	ZRef<UITableViewCell> fCell_New;
	ZRef<UITableViewCell> fCell_Current;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SectionBody_Multi

class SectionBody_Multi : public SectionBody
	{
public:
	virtual size_t NumberOfRows();

	virtual void PreUpdate();
	virtual bool WillBeEmpty();
	virtual void Update_NOP();
	virtual void Update_Normal(RowMeta& ioRowMeta_Old, RowMeta& ioRowMeta_New,
		RowUpdate& ioRowUpdate_Insert, RowUpdate& ioRowUpdate_Delete, RowUpdate& ioRowUpdate_Reload);
	virtual void Update_Insert(RowMeta& ioRowMeta_New, RowUpdate& ioRowUpdate_New);
	virtual void Update_Delete(RowMeta& ioRowMeta_Old, RowUpdate& ioRowUpdate_Old);
	virtual void FinishUpdate();

// -----

	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iRowIndex);
	virtual ZQ<UITableViewCellEditingStyle> QEditingStyle(size_t iRowIndex);
	virtual bool CommitEditingStyle(UITableViewCellEditingStyle iStyle, size_t iRowIndex);
	virtual ZQ<bool> QShouldIndentWhileEditing(size_t iRowIndex);
	virtual ZQ<CGFloat> QRowHeight(size_t iRowIndex);
	virtual ZQ<NSInteger> QIndentationLevel(size_t iRowIndex);

	virtual bool ButtonTapped(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex);
	virtual ZQ<bool> HasButton(bool iEditing, size_t iRowIndex);

	virtual bool RowSelected(UITVHandler_WithSections* iTVC,
		UITableView* iTableView, NSIndexPath* iIndexPath, size_t iRowIndex);
	virtual ZQ<bool> CanSelect(bool iEditing, size_t iRowIndex);

	std::vector<ZRef<SectionBody> > fBodies_Pending;

private:
	ZRef<SectionBody> pGetBodyAndRowIndex(size_t& oIndex, size_t iIndex);
	std::vector<ZRef<SectionBody> > fBodies;
	};

} // namespace UIKit
} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * UITVHandler_WithSections

@interface UITVHandler_WithSections : NSObject <UITableViewDelegate, UITableViewDataSource>
	{
	std::vector<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections_Shown;
	std::vector<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections_Shown_Pending;
	std::set<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections_ToIgnore;
	std::vector<std::map<size_t, UITableViewRowAnimation> > fInserts;
	std::vector<std::map<size_t, UITableViewRowAnimation> > fDeletes;
	std::vector<std::map<size_t, UITableViewRowAnimation> > fReloads;
	bool fNeedsUpdate;
	bool fUpdateInFlight;
@public
	bool fShown;
	std::vector<ZooLib::ZRef<ZooLib::UIKit::Section> > fSections_All;
	}

// From UITableViewDataSource
- (NSInteger)numberOfSectionsInTableView:(UITableView*)tableView;
- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section;
- (void)tableView:(UITableView*)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath*)indexPath;
 - (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath;

// From UITableViewDelegate
- (CGFloat)tableView:(UITableView*)tableView heightForHeaderInSection:(NSInteger)section;
- (CGFloat)tableView:(UITableView*)tableView heightForFooterInSection:(NSInteger)section;

- (NSString*)tableView:(UITableView*)tableView titleForHeaderInSection:(NSInteger)section;
- (NSString*)tableView:(UITableView*)tableView titleForFooterInSection:(NSInteger)section;

- (UIView*)tableView:(UITableView*)tableView viewForHeaderInSection:(NSInteger)section;
- (UIView*)tableView:(UITableView*)tableView viewForFooterInSection:(NSInteger)section;

- (void)tableView:(UITableView*)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath*)indexPath;
- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath;

- (UITableViewCellEditingStyle)tableView:(UITableView*)tableView editingStyleForRowAtIndexPath:(NSIndexPath*)indexPath;

// Our protocol
- (void)needsUpdate:(UITableView*)tableView;

@end // interface UITVController_WithSections

// =================================================================================================
#pragma mark -
#pragma mark * UITVHandler_WithSections_VariableRowHeight

@interface UITVHandler_WithSections_VariableRowHeight : UITVHandler_WithSections
	{}

// From UITableViewDelegate
- (CGFloat)tableView:(UITableView*)tableView heightForRowAtIndexPath:(NSIndexPath*)indexPath;

@end // interface UITVHandler_WithSections_VariableRowHeight

// =================================================================================================
#pragma mark -
#pragma mark * UITableView_WithSections

@interface UITableView_WithSections : UITableView
	{
@public
	ZooLib::ZRef<ZooLib::ZCallable<void()> > fCallable_NeedsUpdate;
	ZooLib::ZRef<UITVHandler_WithSections> fHandler;
	};

- (id)initWithFrame:(CGRect)frame style:(UITableViewStyle)style variableRowHeight:(BOOL)variableRowHeight;
- (id)initWithFrame:(CGRect)frame style:(UITableViewStyle)style;
- (void)appendSection:(ZooLib::ZRef<ZooLib::UIKit::Section>) iSection;
- (void)needsUpdate;
- (void)deselect;

@end // interface UITableView_WithSections

#endif // ZCONFIG_SPI_Enabled(iPhone)

#endif // __UITVC_WithSections__
