#ifndef GUI_LYRICS_H
#define GUI_LYRICS_H

#include <QWidget>
#include "Helper/Pimpl.h"
#include "GUI/Helper/SayonaraWidget/SayonaraWidget.h"

class MetaData;
namespace Ui {
	class GUI_Lyrics;
}

class GUI_Lyrics :
		public SayonaraWidget
{
	Q_OBJECT

signals:
	void sig_closed();

public:
	explicit GUI_Lyrics(QWidget *parent = nullptr);
	~GUI_Lyrics();

	void set_metadata(const MetaData& md);

private:
	PIMPL(GUI_Lyrics)
	Ui::GUI_Lyrics *ui=nullptr;

	void guess_artist_and_title(const MetaData& md);
	void init();

	void zoom(qreal font_size);

private slots:
	void zoom_in();
	void zoom_out();

	void lyrics_fetched();
	void lyric_server_changed(int idx);

	void lyric_search_button_pressed();
	void switch_pressed();
	void prepare_lyrics();

	void language_changed() override;

protected:
	void showEvent(QShowEvent* e);
	void wheelEvent(QWheelEvent* e);
	void keyPressEvent(QKeyEvent* e);
};


#endif // GUI_LYRICS_H
