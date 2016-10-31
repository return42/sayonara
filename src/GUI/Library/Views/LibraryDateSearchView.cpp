#include "LibraryDateSearchView.h"
#include "GUI/Library/Models/DateSearchModel.h"
#include "GUI/Library/GUI_DateSearchConfig.h"
#include "GUI/Helper/ContextMenu/ContextMenu.h"
#include "GUI/Helper/Delegates/ListDelegate.h"
#include "Helper/Language.h"
#include "Helper/Library/DateFilter.h"

struct LibraryDateSearchView::Private
{
	ContextMenu*			rc_menu=nullptr;
	GUI_DateSearchConfig*	dsc=nullptr;
	DateSearchModel*		model=nullptr;

	void check_dsc(QWidget* parent){
		if(!dsc){
			dsc = new GUI_DateSearchConfig(parent);
		}
	}
};

LibraryDateSearchView::LibraryDateSearchView(QWidget* parent) :
	SearchableListView(parent)
{
	_m = Pimpl::make<LibraryDateSearchView::Private>();
	_m->model = new DateSearchModel(this);

	this->setModel(_m->model);
	this->setSearchModel(_m->model);
	this->setItemDelegate(new ListDelegate(this));
}

LibraryDateSearchView::~LibraryDateSearchView() {}


Library::DateFilter LibraryDateSearchView::get_filter(int row) const
{
	return _m->model->get_filter(row);
}


void LibraryDateSearchView::contextMenuEvent(QContextMenuEvent* e)
{
	if(!_m->rc_menu){
		_m->rc_menu = new ContextMenu(this);
		_m->rc_menu->show_actions(
			ContextMenu::EntryNew |
			ContextMenu::EntryEdit |
			ContextMenu::EntryDelete
		);

		connect(_m->rc_menu, &ContextMenu::sig_new, this, &LibraryDateSearchView::new_clicked);
		connect(_m->rc_menu, &ContextMenu::sig_edit, this, &LibraryDateSearchView::edit_clicked);
		connect(_m->rc_menu, &ContextMenu::sig_delete, this, &LibraryDateSearchView::delete_clicked);
	}

	QPoint pos = e->globalPos();
	_m->rc_menu->exec(pos);

	SearchableListView::contextMenuEvent(e);
}


void LibraryDateSearchView::new_clicked()
{
	_m->check_dsc(this);
	_m->dsc->set_filter(Library::DateFilter());
	_m->dsc->exec();

	Library::DateFilter filter = _m->dsc->get_edited_filter();
	if(filter.valid()){
		_m->model->add_data(filter);
	}
}


void LibraryDateSearchView::edit_clicked()
{
	QModelIndex cur_idx = this->currentIndex();
	Library::DateFilter filter = _m->model->get_filter(cur_idx.row());

	_m->check_dsc(this);
	_m->dsc->set_filter(filter);
	_m->dsc->exec();

	Library::DateFilter edited_filter = _m->dsc->get_edited_filter();
	if(edited_filter.valid()){
		_m->model->set_data(edited_filter, this->currentIndex().row());
	}
}


void LibraryDateSearchView::delete_clicked()
{
	QModelIndex cur_idx = this->currentIndex();
	_m->model->remove(cur_idx.row());
}
