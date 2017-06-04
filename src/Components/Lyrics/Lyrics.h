#ifndef LYRICS_H
#define LYRICS_H

#include "Helper/Pimpl.h"

#include <QObject>

class QString;
class QStringList;
class MetaData;

class Lyrics :
		public QObject
{
	Q_OBJECT
    PIMPL(Lyrics)

signals:
	void sig_lyrics_fetched();

public:
	explicit Lyrics(QObject* parent=nullptr);
	~Lyrics();

    QStringList servers() const;
    void set_metadata(const MetaData& md);
	bool fetch_lyrics(const QString& artist, const QString& title, int server_index);
	bool save_lyrics(const QString& plain_text);

    QString artist() const;
    QString title() const;
	QString lyric_header() const;
	QString local_lyric_header() const;
	QString lyrics() const;
	QString local_lyrics() const;

	bool is_lyric_valid() const;
	bool is_lyric_tag_available() const;
	bool is_lyric_tag_supported() const;

private slots:
	void lyrics_fetched();


};

#endif // LYRICS_H
