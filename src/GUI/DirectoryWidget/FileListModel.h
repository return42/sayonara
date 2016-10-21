#ifndef FILE_LIST_MODEL_H
#define FILE_LIST_MODEL_H

#include "GUI/Helper/SearchableWidget/AbstractSearchModel.h"
#include "Helper/Settings/SayonaraClass.h"

#include <QStringList>
#include <QModelIndex>

class Settings;
class QVariant;

class FileListModel :
	public AbstractSearchListModel
{
	Q_OBJECT

	public:
		explicit FileListModel(QObject* parent=nullptr);
		virtual ~FileListModel();

		void set_parent_directory(const QString& dir);
		QStringList get_files() const;

		QModelIndex getFirstRowIndexOf(const QString& substr) override;
		QModelIndex getNextRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
		QModelIndex getPrevRowIndexOf(const QString& substr, int cur_row, const QModelIndex& parent=QModelIndex()) override;
		QMap<QChar, QString> getExtraTriggers() override;

		QVariant data(const QModelIndex &index, int role) const override;

		int rowCount(const QModelIndex &parent = QModelIndex()) const override;

		QMimeData* mimeData(const QModelIndexList &indexes) const override;
		Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
		QStringList _files;
};

#endif
