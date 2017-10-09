/* CoverView.h */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */



#ifndef ALBUMCOVERVIEW_H
#define ALBUMCOVERVIEW_H

#include "GUI/Library/Views/View.h"
#include "Utils/Pimpl.h"

namespace Library
{
	class CoverModel;
	class CoverView :
			public Library::View
	{
		Q_OBJECT
		PIMPL(CoverView)

	public:
		explicit CoverView(QWidget* parent=nullptr);
		virtual ~CoverView();

		int get_index_by_model_index(const QModelIndex& idx) const override;
		QModelIndex get_model_index_by_index(int idx) const override;

		void setModel(Library::CoverModel* model);
		void refresh();

	protected:
		void wheelEvent(QWheelEvent* e) override;
		void resizeEvent(QResizeEvent* e) override;
		void showEvent(QShowEvent* e) override;

		QStyleOptionViewItem viewOptions() const override;

	private:
		void change_zoom(int zoom=-1);

		void setModel(QAbstractItemModel* m) override;
		void setModel(ItemModel* m) override;

	private slots:
		void timed_out();
	};
}

#endif // ALBUMCOVERVIEW_H
