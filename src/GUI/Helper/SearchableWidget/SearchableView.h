#ifndef SEARCHABLEVIEW_H
#define SEARCHABLEVIEW_H

#include "GUI/Helper/SearchableWidget/SayonaraSelectionView.h"
#include "Helper/Pimpl.h"

#include <QKeyEvent>
#include <QAbstractItemView>

class QAbstractItemModel;
class QItemSelectionModel;
class MiniSearcher;
class SearchModelInterface;
class Settings;

class SearchViewInterface :
		public SayonaraSelectionView
{
	PIMPL(SearchViewInterface)

protected:
	enum class SearchDirection : unsigned char
	{
		First,
		Next,
		Prev
	};


public:
	explicit SearchViewInterface(QAbstractItemView* view);
	virtual ~SearchViewInterface();

	virtual void setSearchModel(SearchModelInterface* model) final;

	virtual QModelIndex get_index(int row, int col, const QModelIndex& parent=QModelIndex()) const override final;
	virtual int get_row_count(const QModelIndex& parent=QModelIndex()) const override final;
	virtual int get_column_count(const QModelIndex& parent=QModelIndex()) const override final;

	virtual QItemSelectionModel* get_selection_model() const override final;
	virtual void set_current_index(int idx) override final;


protected:
	Settings*					_settings=nullptr;


protected:
	virtual MiniSearcher*		mini_searcher() const=0;

	QModelIndex get_match_index(const QString& str, SearchDirection direction) const;
	void select_match(const QString& str, SearchDirection direction);
	void handleKeyPress(QKeyEvent* e);
};

#endif // SEARCHABLEVIEW_H
