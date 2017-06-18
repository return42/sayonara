/* SomaFMStationModel.h */

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


/* SomaFMStationModel.h */

#ifndef SomaFMStationModel_H
#define SomaFMStationModel_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "Helper/Pimpl.h"

#include <QMap>
#include <QList>
#include <QMimeData>

namespace SomaFM
{
	class Station;
	class StationModel :
			public AbstractSearchTableModel
	{
		Q_OBJECT
		PIMPL(StationModel)

	public:
		explicit StationModel(QObject *parent = nullptr);
		~StationModel();

	private:
		enum class Status : char
		{
			Waiting,
			Error,
			OK
		};

		// QAbstractItemModel interface
	public:
		int rowCount(const QModelIndex& parent=QModelIndex()) const override;
		int columnCount(const QModelIndex& parent=QModelIndex()) const override;
		QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const override;
		QMimeData* mimeData(const QModelIndexList &indexes) const override;
		Qt::ItemFlags flags(const QModelIndex &index) const override;


		// AbstractSearchModelInterface interface
	public:
		QModelIndex getFirstRowIndexOf(const QString& substr) override;
		QModelIndex getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent) override;
		QModelIndex getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent) override;
		QMap<QChar, QString> getExtraTriggers() override;

		void set_stations(const QList<SomaFM::Station>& stations);
		void replace_station(const SomaFM::Station& station);
		bool has_stations() const;
		void set_waiting();
	};
}

#endif // SomaFMStationModel_H
