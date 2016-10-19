#include "Dragable.h"
#include "GUI/Helper/GUI_Helper.h"
#include "Helper/FileHelper.h"

#include <QPoint>
#include <QMimeData>
#include <QPixmap>
#include <QDrag>
#include <QPainter>
#include <QApplication>
#include <QWidget>
#include <QUrl>
#include <QFontMetrics>

struct Dragable::Private
{
	QPoint		start_drag_pos;
	bool		dragging=false;
	QWidget*	parent=nullptr;
	QDrag*		drag=nullptr;

	Private()
	{
		start_drag_pos = QPoint();
		dragging = false;
	}

	QStringList get_strings(const QMimeData* data)
	{
		QStringList ret;
		int playlists, dirs, tracks;
		playlists = dirs = tracks = 0;

		QList<QUrl> urls = data->urls();

		for(const QUrl& url : urls)
		{
			QString filename = url.toLocalFile();
			if(Helper::File::is_playlistfile(filename)){
				playlists++;
			}

			else if(Helper::File::is_soundfile(filename)){
				tracks++;
			}

			else if(Helper::File::is_dir(filename)){
				dirs++;
			}
		}

		if(tracks > 0){
			ret << QString::number(tracks) + " " + "tracks";
		}
		if(playlists > 0){
			ret << QString::number(playlists) + " " + "playlists";
		}
		if(dirs >0){
			ret << QString::number(dirs) + " " + "directories";
		}

		return ret;
	}
};


Dragable::Dragable(QWidget* parent)
{
	_m = Pimpl::make<Dragable::Private>();
	_m->parent = parent;
}

Dragable::~Dragable() {}

void Dragable::drag_pressed(const QPoint& p)
{
	_m->dragging = false;
	_m->start_drag_pos = p;
}


QDrag* Dragable::drag_moving(const QPoint& p)
{
	int distance = (p - _m->start_drag_pos).manhattanLength();

	if( distance < QApplication::startDragDistance())
	{
		return _m->drag;
	}

	if(_m->dragging)
	{
		return _m->drag;
	}

	if(_m->drag){
		delete _m->drag;
	}

	_m->dragging = true;
	_m->start_drag_pos = QPoint();
	_m->drag = new QDrag(_m->parent);

	QMimeData* data = get_mimedata();
	if(data == nullptr)
	{
		return _m->drag;
	}

	QStringList strings = _m->get_strings(data);

	QFontMetrics fm(QApplication::font());
	const int logo_height = 24;
	const int logo_width = logo_height;
	const QSize logo_size(logo_width, logo_height);
	const int left_offset = 4;
	const int font_height = fm.ascent();
	const int text_height = strings.size() * (font_height + 2);
	const int pm_height = std::max(30, 30 + (strings.size() - 1) * font_height + 4);
	const int font_padding = (pm_height - text_height) / 2 + 1;

	int pm_width = logo_height + 4;

	for(const QString& str : strings){

		pm_width = std::max( pm_width, fm.width(str) );
	}

	pm_width += logo_width + 22;


	QPixmap cover = get_pixmap();
	if(cover.isNull()){
		cover = GUI::get_pixmap("logo.png", logo_size, true);
	}

	QPixmap pm(pm_width, pm_height);
	QPainter painter(&pm);

	painter.fillRect(pm.rect(), QColor(64, 64, 64));
	painter.setPen(QColor("#424e72"));
	painter.drawRect(0, 0, pm_width - 1, pm_height - 1);
	painter.drawPixmap(left_offset, (pm_height - logo_height) / 2, logo_height, logo_height, cover);
	painter.setPen(QColor(255, 255, 255));
	painter.translate(logo_width + 15, font_padding + font_height - 2);

	for(const QString& str : strings){
		painter.drawText(0, 0, str);
		painter.translate(0, font_height + 2);
	}

	_m->drag->setMimeData(data);
	_m->drag->setPixmap(pm);
	_m->drag->exec(Qt::CopyAction);

	return _m->drag;
}


void Dragable::drag_released(Dragable::ReleaseReason reason)
{
	if(!_m){
		return;
	}

	if(reason == Dragable::ReleaseReason::Destroyed)
	{
		_m->drag = nullptr;
	}

	else if(_m->drag){
		delete _m->drag; _m->drag = nullptr;
	}

	_m->dragging = false;
	_m->start_drag_pos = QPoint();
}


QPixmap Dragable::get_pixmap() const
{
	return QPixmap();
}
