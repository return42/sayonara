#ifndef QMLCOVERMODEL_H
#define QMLCOVERMODEL_H

#include <QString>

class QMLCoverModel :
	public QQuickItem
{
	Q_OBJECT

	signals:
		void titleChanged();
		void imgChanged();

	private:
		QString _title;

		Q_PROPERTY(QString _title READ title WRITE set_title NOTIFY titleChanged)
		Q_PROPERTY(QImage _img  READ img WRITE set_img NOTIFY imgChanged)

	public:
		Item(const QString& title, const QImage& img);
		virtual ~Item();

		QString title() const ;
		void set_title(const QString& title);
		QImage img() const;
		void set_img(const QImage& img);

};


#endif // QMLCOVERMODEL_H
