#ifndef __ZooLib_UITableViewController__
#define __ZooLib_UITableViewController__ 1

#include "zoolib/ZQ.h"
#include "zoolib/ZRef_Counted.h"

#import <UIKit/UITableView.h>

#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark *

namespace ZooLib {
namespace IPhone {

class UITVC_Section : public ZCounted
	{
public:
	virtual size_t NumberOfRows();
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iIndex);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle(size_t iIndex);

	virtual ZQ<CGFloat> RowHeight(size_t iIndex);
	virtual ZQ<CGFloat> HeaderHeight();
	virtual ZQ<CGFloat> FooterHeight();

	virtual ZRef<UIView> HeaderView();
	virtual ZRef<UIView> FooterView();

	virtual void AccessoryButtonTapped(size_t iIndex);
	};

class UITVC_Section_WithRow : public UITVC_Section
	{
public:
	class Row;
	void AddRow(ZRef<Row> iRow);

	virtual size_t NumberOfRows();
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView, size_t iIndex);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle(size_t iIndex);
	virtual void AccessoryButtonTapped(size_t iIndex);

	std::vector<ZRef<Row> > fRows;

	ZRef<Row> pGetRow(size_t iIndex);
	};

class UITVC_Section_WithRow::Row : public ZCounted
	{
public:
	virtual ZRef<UITableViewCell> UITableViewCellForRow(UITableView* iView);
	virtual ZQ<UITableViewCellEditingStyle> EditingStyle();
	virtual void AccessoryButtonTapped();
	};

} // namespace IPhone
} // namespace ZooLib

using namespace ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib_UITableViewController_WithSections

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

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section;
- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section;

- (void)tableView:(UITableView *)tableView accessoryButtonTappedForRowWithIndexPath:(NSIndexPath *)indexPath;

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath;

// Our protocol
- (void) addSection:(ZRef<IPhone::UITVC_Section>) iSection;

-(ZRef<IPhone::UITVC_Section>)pGetSection:(size_t)iIndex;

@end // ZooLib_UITableViewController

#endif // __ZooLib_UITableViewController__
