/* GUI_TagEdit.cpp */

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

#include "GUI_TagEdit.h"
#include "GUI/TagEdit/ui_GUI_TagEdit.h"

#include "TagLineEdit.h"

#include "Components/TagEdit/TagExpression.h"
#include "Components/Covers/CoverLocation.h"
#include "Components/TagEdit/TagEdit.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "GUI/Helper/Widgets/Completer.h"

#include "Helper/Message/Message.h"
#include "Helper/Tagging/Tagging.h"
#include "Helper/globals.h"
#include "Helper/Logger/Logger.h"
#include "Helper/MetaData/MetaDataList.h"
#include "Helper/MetaData/Album.h"
#include "Helper/MetaData/Artist.h"
#include "Helper/MetaData/Genre.h"
#include "Helper/Language.h"
#include "Database/DatabaseConnector.h"
#include "Database/LibraryDatabase.h"

#include <QDir>
#include <QDesktopServices>
#include <QRegExp>
#include <QUrl>

struct GUI_TagEdit::Private
{
	TagEdit*			tag_edit=nullptr;
	TagExpression		tag_expression;
	QMap<int, QString>	cover_path_map;
	int					cur_idx;
	/**
	 * @brief _tag_str_map, key = tag, val = replaced string
	 */
	QMap<Tag, ReplacedString> tag_str_map;
};

GUI_TagEdit::GUI_TagEdit(QWidget* parent) :
	Widget(parent)
{
	m = Pimpl::make<Private>();
	ui = new Ui::GUI_TagEdit();
	ui->setupUi(this);

	m->tag_edit = new TagEdit(this);

	ui->frame_tag_from_path->setVisible(ui->cb_tag_from_path->isChecked());

	connect(ui->btn_next, &QPushButton::clicked, this, &GUI_TagEdit::next_button_clicked);
	connect(ui->btn_prev, &QPushButton::clicked, this, &GUI_TagEdit::prev_button_clicked);
	connect(ui->btn_apply_tag, &QPushButton::clicked, this, &GUI_TagEdit::apply_tag_clicked);
	connect(ui->btn_apply_tag_all, &QPushButton::clicked, this, &GUI_TagEdit::apply_tag_all_clicked);
	connect(ui->rb_dont_replace, &QRadioButton::toggled, this, &GUI_TagEdit::rb_dont_replace_toggled);

	connect(ui->cb_album_all, &QCheckBox::toggled, this, &GUI_TagEdit::album_all_changed);
	connect(ui->cb_artist_all, &QCheckBox::toggled, this, &GUI_TagEdit::artist_all_changed);
	connect(ui->cb_album_artist_all, &QCheckBox::toggled, this, &GUI_TagEdit::album_artist_all_changed);
	connect(ui->cb_genre_all, &QCheckBox::toggled, this, &GUI_TagEdit::genre_all_changed);
	connect(ui->cb_year_all, &QCheckBox::toggled, this, &GUI_TagEdit::year_all_changed);
	connect(ui->cb_discnumber_all, &QCheckBox::toggled, this, &GUI_TagEdit::discnumber_all_changed);
	connect(ui->cb_rating_all, &QCheckBox::toggled, this, &GUI_TagEdit::rating_all_changed);
	connect(ui->cb_cover_all, &QCheckBox::toggled, this, &GUI_TagEdit::cover_all_changed);
	connect(ui->le_tag, &QLineEdit::textChanged, this, &GUI_TagEdit::tag_text_changed);

	connect(ui->btn_title, &QPushButton::toggled, this, &GUI_TagEdit::btn_title_checked);
	connect(ui->btn_artist, &QPushButton::toggled, this, &GUI_TagEdit::btn_artist_checked);
	connect(ui->btn_album, &QPushButton::toggled, this, &GUI_TagEdit::btn_album_checked);
	connect(ui->btn_track_nr, &QPushButton::toggled, this, &GUI_TagEdit::btn_track_nr_checked);
	connect(ui->btn_year, &QPushButton::toggled, this, &GUI_TagEdit::btn_year_checked);
	connect(ui->btn_disc_nr, &QPushButton::toggled, this, &GUI_TagEdit::btn_disc_nr_checked);
	connect(ui->btn_tag_help, &QPushButton::clicked, this, &GUI_TagEdit::btn_tag_help_clicked);

	connect(ui->btn_save, &QPushButton::clicked, this, &GUI_TagEdit::commit);
	connect(ui->btn_undo, &QPushButton::clicked, this, &GUI_TagEdit::undo_clicked);
	connect(ui->btn_undo_all, &QPushButton::clicked, this, &GUI_TagEdit::undo_all_clicked);
	connect(ui->btn_close, &QPushButton::clicked, this, &GUI_TagEdit::sig_cancelled);

	connect(m->tag_edit, &TagEdit::sig_progress, this, &GUI_TagEdit::progress_changed);
	connect(m->tag_edit, &TagEdit::sig_metadata_received, this, &GUI_TagEdit::metadata_changed);
	connect(m->tag_edit, &TagEdit::finished, this, &GUI_TagEdit::commit_finished);

	reset();
}


GUI_TagEdit::~GUI_TagEdit() {}


void GUI_TagEdit::language_changed()
{
	ui->retranslateUi(this);

	ui->btn_title->setText(Lang::get(Lang::Title));
	ui->btn_album->setText(Lang::get(Lang::Album));
	ui->btn_artist->setText(Lang::get(Lang::Artist));
	ui->btn_year->setText(Lang::get(Lang::Year));
	ui->btn_track_nr->setText(Lang::get(Lang::TrackNo));
	ui->btn_apply_tag->setText(Lang::get(Lang::Apply));
	ui->lab_track_title->setText(Lang::get(Lang::Title));
	ui->lab_album->setText(Lang::get(Lang::Album));
	ui->lab_artist->setText(Lang::get(Lang::Artist));
	ui->lab_year->setText(Lang::get(Lang::Year));
	ui->lab_genres->setText(Lang::get(Lang::Genres));
	ui->lab_rating_descr->setText(Lang::get(Lang::Rating));
	ui->lab_track_num->setText(Lang::get(Lang::TrackNo));

	ui->cb_album_all->setText(Lang::get(Lang::All));
	ui->cb_artist_all->setText(Lang::get(Lang::All));
	ui->cb_album_artist_all->setText(Lang::get(Lang::All));
	ui->cb_genre_all->setText(Lang::get(Lang::All));
	ui->cb_year_all->setText(Lang::get(Lang::All));
	ui->cb_discnumber_all->setText(Lang::get(Lang::All));
	ui->cb_rating_all->setText(Lang::get(Lang::All));
	ui->cb_cover_all->setText(Lang::get(Lang::All));

	ui->btn_undo->setText(Lang::get(Lang::Undo));
	ui->btn_close->setText(Lang::get(Lang::Close));
	ui->btn_save->setText(Lang::get(Lang::Save));
}


void GUI_TagEdit::commit_finished()
{
	ui->btn_save->setEnabled(true);
}


TagEdit* GUI_TagEdit::get_tag_edit() const
{
	return m->tag_edit;
}


void GUI_TagEdit::progress_changed(int val)
{
	ui->pb_progress->setVisible(val >= 0);

	if(val >= 0){
		ui->pb_progress->setValue(val);
	}

	if(val < 0){
		metadata_changed(m->tag_edit->get_all_metadata() );
	}
}

void GUI_TagEdit::metadata_changed(const MetaDataList& md)
{
	Q_UNUSED(md)

	reset();

	m->cur_idx = 0;
	track_idx_changed();
}


bool GUI_TagEdit::check_idx(int idx) const
{
	return between(idx,m->tag_edit->get_n_tracks());
}


void GUI_TagEdit::next_button_clicked()
{
	write_changes(m->cur_idx);

	m->cur_idx++;

	track_idx_changed();
}


void GUI_TagEdit::prev_button_clicked()
{
	write_changes(m->cur_idx);

	m->cur_idx--;

	track_idx_changed();
}

void GUI_TagEdit::track_idx_changed()
{
	bool valid;
	int n_tracks =m->tag_edit->get_n_tracks();

	ui->btn_next->setEnabled(m->cur_idx >= 0 && m->cur_idx < n_tracks - 1);
	ui->btn_prev->setEnabled(m->cur_idx > 0 && m->cur_idx < n_tracks);

	if(!check_idx(m->cur_idx)) return;

	MetaData md =m->tag_edit->get_metadata(m->cur_idx);

	if(ui->le_tag->text().isEmpty()){
		ui->le_tag->setText(md.filepath());
	}

	else if( !(	ui->btn_album->isChecked() ||
			ui->btn_artist->isChecked() ||
			ui->btn_title->isChecked() ||
			ui->btn_year->isChecked() ||
			ui->btn_disc_nr->isChecked() ||
			ui->btn_track_nr->isChecked()))
	{
		ui->le_tag->setText(md.filepath());
	}

	valid = m->tag_expression.update_tag(ui->le_tag->text(), md.filepath());
	set_tag_colors( valid );

	ui->le_title->setText(md.title);

	if(!ui->cb_album_all->isChecked()){
		ui->le_album->setText(md.album());
	}

	if(!ui->cb_artist_all->isChecked()){
		ui->le_artist->setText(md.artist());
	}

	if(!ui->cb_album_artist_all->isChecked()){
		ui->le_album_artist->setText(md.album_artist());
	}

	if(!ui->cb_genre_all->isChecked()){
		ui->le_genre->setText( md.genres_to_list().join(", ") );
	}

	if(!ui->cb_year_all->isChecked()){
		ui->sb_year->setValue(md.year);
	}

	if(!ui->cb_discnumber_all->isChecked()){
		ui->sb_discnumber->setValue(md.discnumber);
	}

	if(!ui->cb_rating_all->isChecked()){
		ui->lab_rating->set_rating(md.rating);
	}

	if(!ui->cb_cover_all->isChecked()){
		set_cover(md);

		if(m->tag_edit->has_cover_replacement(m->cur_idx)){
			ui->rb_replace->setChecked(true);
			ui->rb_dont_replace->setChecked(false);
		}
	
		else{
			ui->rb_dont_replace->setChecked(true);
			ui->rb_replace->setChecked(false);
		}
	}

	bool is_cover_supported = m->tag_edit->is_cover_supported(m->cur_idx);
	ui->frame_cover->setVisible(is_cover_supported);

	ui->sb_track_num->setValue(md.track_num);

	ui->lab_filepath->clear();

	ui->lab_filepath->setText( md.filepath());
	ui->lab_track_index->setText(
			Lang::get(Lang::Track).space() +
			QString::number(m->cur_idx+1 ) + "/" + QString::number( n_tracks )
	);

	ui->lab_tag_type->setText(tr("Tag") + ": " +
					Tagging::tag_type_to_string(
						 Tagging::get_tag_type(md.filepath())
					)
	);
}

void GUI_TagEdit::reset()
{
	m->cur_idx = -1;

	ui->cb_album_all->setChecked(false);
	ui->cb_artist_all->setChecked(false);
	ui->cb_album_artist_all->setChecked(false);
	ui->cb_genre_all->setChecked(false);
	ui->cb_discnumber_all->setChecked(false);
	ui->cb_rating_all->setChecked(false);
	ui->cb_year_all->setChecked(false);
	ui->cb_cover_all->setChecked(false);

	ui->lab_track_index ->setText(Lang::get(Lang::Track) + " 0/0");
	ui->btn_prev->setEnabled(false);
	ui->btn_next->setEnabled(false);

	ui->le_album->clear();
	ui->le_artist->clear();
	ui->le_album_artist->clear();
	ui->le_title->clear();
	ui->le_genre->clear();
	ui->le_tag->clear();
	ui->sb_year->setValue(0);
	ui->sb_discnumber->setValue(0);
	ui->lab_rating->set_rating(0);
	ui->sb_track_num->setValue(0);

	ui->le_album->setEnabled(true);
	ui->le_artist->setEnabled(true);
	ui->le_album_artist->setEnabled(true);
	ui->le_genre->setEnabled(true);
	ui->le_tag->setEnabled(true);
	ui->sb_year->setEnabled(true);
	ui->sb_discnumber->setEnabled(true);
	ui->lab_rating->setEnabled(true);

	ui->rb_dont_replace->setChecked(true);
	ui->rb_replace->setChecked(false);

	ui->btn_cover_replacement->setEnabled(true);
	show_replacement_field(false);

	QIcon icon(CoverLocation::getInvalidLocation().cover_path());
	ui->btn_cover_replacement->setIcon( icon );

	ui->lab_filepath->clear();
	ui->pb_progress->setVisible(false);

	ui->btn_album->setChecked(false);
	ui->btn_artist->setChecked(false);
	ui->btn_title->setChecked(false);
	ui->btn_year->setChecked(false);
	ui->btn_disc_nr->setChecked(false);
	ui->btn_track_nr->setChecked(false);

	m->cover_path_map.clear();
	init_completer();
}

void GUI_TagEdit::init_completer()
{
	AlbumList albums;
	ArtistList artists;
	QStringList albumstr, artiststr;

	DatabaseConnector* db = DatabaseConnector::instance();
	LibraryDatabase* lib_db = db->library_db(-1, 0);

	lib_db->getAllAlbums(albums, true);
	lib_db->getAllArtists(artists, true);

	for(const Album& album : albums){
		albumstr << album.name();
	}

	for(const Artist& artist : artists){
		artiststr << artist.name();
	}

	Gui::Completer* album_completer = new Gui::Completer(albumstr, this);
	ui->le_album->setCompleter(album_completer);

	Gui::Completer* album_artist_completer = new Gui::Completer(artiststr, this);
	ui->le_album_artist->setCompleter(album_artist_completer);

	Gui::Completer* artist_completer = new Gui::Completer(artiststr, this);
	ui->le_artist->setCompleter(artist_completer);
}

void GUI_TagEdit::album_all_changed(bool b)
{
	ui->le_album->setEnabled(!b);
}

void GUI_TagEdit::artist_all_changed(bool b)
{
	ui->le_artist->setEnabled(!b);
}

void GUI_TagEdit::album_artist_all_changed(bool b)
{
	ui->le_album_artist->setEnabled(!b);
}

void GUI_TagEdit::genre_all_changed(bool b)
{
	ui->le_genre->setEnabled(!b);
}

void GUI_TagEdit::year_all_changed(bool b)
{
	ui->sb_year->setEnabled(!b);
}

void GUI_TagEdit::discnumber_all_changed(bool b)
{
	ui->sb_discnumber->setEnabled(!b);
}

void GUI_TagEdit::rating_all_changed(bool b)
{
	ui->lab_rating->setEnabled(!b);
}

void GUI_TagEdit::cover_all_changed(bool b)
{
	if(!b){
		if(between(m->cur_idx,m->tag_edit->get_n_tracks()) ){
			set_cover(m->tag_edit->get_metadata(m->cur_idx));
		}
	}

	ui->rb_dont_replace->setEnabled(!b);

	ui->btn_cover_replacement->setEnabled(!b);
}

void GUI_TagEdit::undo_clicked()
{
	m->tag_edit->undo(m->cur_idx);
	track_idx_changed();
}

void GUI_TagEdit::undo_all_clicked()
{
	m->tag_edit->undo_all();
	track_idx_changed();
}


void GUI_TagEdit::write_changes(int idx)
{
	if( !check_idx(idx) ) {
		return;
	}

	MetaData md =m->tag_edit->get_metadata(idx);

	md.title = ui->le_title->text();
	md.set_artist(ui->le_artist->text());
	md.set_album(ui->le_album->text());
	md.set_album_artist(ui->le_album_artist->text());
	md.set_genres(ui->le_genre->text().split(", "));
	md.discnumber = ui->sb_discnumber->value();
	md.year = ui->sb_year->value();
	md.track_num = ui->sb_track_num->value();
	md.rating = ui->lab_rating->get_rating();

	m->tag_edit->update_track(idx, md);

	if(is_cover_replacement_active()){
		update_cover(idx, m->cover_path_map[idx]);
	}
}

void GUI_TagEdit::commit()
{
	if(!ui->btn_save->isEnabled()){
		return;
	}

	ui->btn_save->setEnabled(false);
	ui->btn_undo->setEnabled(false);
	ui->btn_undo_all->setEnabled(false);

	write_changes(m->cur_idx);

    for(int i=0; i<m->tag_edit->get_n_tracks(); i++)
    {
		if(i ==m->cur_idx) continue;

		MetaData md =m->tag_edit->get_metadata(i);

		if( ui->cb_album_all->isChecked()){
			md.set_album(ui->le_album->text());
		}
		if( ui->cb_artist_all->isChecked()){
			md.set_artist(ui->le_artist->text());
		}
		if( ui->cb_album_artist_all->isChecked()){
			md.set_album_artist(ui->le_album_artist->text());
		}
        if( ui->cb_genre_all->isChecked())
        {
			QStringList genres = ui->le_genre->text().split(", ");
            md.set_genres(genres);
		}

		if( ui->cb_discnumber_all->isChecked() ){
			md.discnumber = ui->sb_discnumber->value();
		}

		if( ui->cb_rating_all->isChecked()){
			md.rating = ui->lab_rating->get_rating();
		}

		if( ui->cb_year_all->isChecked()){
			md.year = ui->sb_year->value();
		}

		if( ui->cb_cover_all->isChecked() ){
			update_cover(i, m->cover_path_map[m->cur_idx]);
		}

		m->tag_edit->update_track(i, md);
	}

	m->tag_edit->commit();
}

void GUI_TagEdit::show_close_button(bool show)
{
	ui->btn_close->setVisible(show);
}


void GUI_TagEdit::show_replacement_field(bool b)
{
	ui->lab_replacement->setVisible(b);
	ui->btn_cover_replacement->setVisible(b);
	ui->cb_cover_all->setVisible(b);
	ui->cb_cover_all->setChecked(false);
}

bool GUI_TagEdit::is_cover_replacement_active() const
{
	return (ui->rb_replace->isChecked() &&
			ui->frame_cover->isVisible() &&
			ui->btn_cover_replacement->isVisible());
}

void GUI_TagEdit::set_cover(const MetaData& md)
{
	QByteArray img_data;
	QString mime_type;
	bool has_cover = Tagging::extract_cover(md, img_data, mime_type);

	if(!has_cover){
		ui->btn_cover_original->setIcon(QIcon());
		ui->btn_cover_original->setText(Lang::get(Lang::None));
	}

	else{
		QImage img = QImage::fromData(img_data, mime_type.toLocal8Bit().data());
		QPixmap pm = QPixmap::fromImage(img);
		QIcon icon;
		icon.addPixmap(pm);
		ui->btn_cover_original->setIcon(icon);
		ui->btn_cover_original->setText(QString());
	}

	CoverLocation cl = CoverLocation::get_cover_location(md);
	ui->btn_cover_replacement->set_cover_location(cl);

	ui->cb_cover_all->setEnabled(cl.valid());
	ui->btn_cover_replacement->setEnabled(cl.valid() && !ui->cb_cover_all->isChecked());

	if(cl.valid()){
		m->cover_path_map[m->cur_idx] = cl.cover_path();
	}
}

void GUI_TagEdit::update_cover(int idx, const QString& cover_path)
{
	QImage img(cover_path);
	m->tag_edit->update_cover(idx, img);
}


void GUI_TagEdit::rb_dont_replace_toggled(bool b)
{
	show_replacement_field(!b);
}


/*** TAG ***/

void GUI_TagEdit::set_tag_colors(bool valid)
{
	if( !valid ){
		ui->le_tag->setStyleSheet("color: red;");
	}

	else{
		ui->le_tag->setStyleSheet("");
	}

	ui->btn_apply_tag->setEnabled(valid);
	ui->btn_apply_tag_all->setEnabled(valid);
}


void GUI_TagEdit::tag_text_changed(const QString& str)
{
	if( !check_idx(m->cur_idx) ) {
		return;
	}

	bool valid;
	MetaData md =m->tag_edit->get_metadata(m->cur_idx);

	valid = m->tag_expression.update_tag(str, md.filepath() );

	set_tag_colors( valid );
}


void GUI_TagEdit::apply_tag(int idx)
{
	if(!check_idx(idx)) {
		return;
	}

	QMap<Tag, ReplacedString> tag_cap_map = m->tag_expression.get_tag_val_map();
	MetaData md =m->tag_edit->get_metadata(idx);

	for(const QString& tag : tag_cap_map.keys()){
		ReplacedString cap = tag_cap_map[tag];

		if(tag.compare(TAG_TITLE) == 0){
			md.title = cap;
		}

		else if(tag.compare(TAG_ALBUM) == 0){
			md.set_album(cap);
		}

		else if(tag.compare( TAG_ARTIST) == 0){
			md.set_artist(cap);
		}

		else if(tag.compare(TAG_TRACK_NUM) == 0){
			md.track_num = cap.toInt();
		}

		else if(tag.compare(TAG_YEAR) == 0){
			md.year = cap.toInt();
		}

		else if(tag.compare(TAG_DISC) == 0){
			md.discnumber = cap.toInt();
		}
	}

	m->tag_edit->update_track(idx, md);

	if(idx ==m->cur_idx){
		// force gui update
		track_idx_changed();
	}
}

void GUI_TagEdit::apply_tag_clicked()
{
	apply_tag(m->cur_idx);
}

void GUI_TagEdit::apply_tag_all_clicked()
{
	IdxList not_valid;

	QString not_valid_str = tr("Cannot apply tag for") + "<br /><br /> ";
	MetaDataList v_md = m->tag_edit->get_all_metadata();
	int n_tracks = v_md.size();

	for(int i=0; i<n_tracks; i++){
		bool valid;

		valid = m->tag_expression.update_tag(ui->le_tag->text(), v_md[i].filepath() );

		if(! valid ){
			not_valid << i;
			not_valid_str +=
					QString::number(i+1) + "/" +
					QString::number(n_tracks) + " " +
					v_md[i].title + " " + Lang::get(Lang::By).space() +
					v_md[i].artist() + "<br />";
		}
	}

	not_valid_str += "<br />" + tr("Ignore these tracks?");

	GlobalMessage::Answer answer = GlobalMessage::Answer::Yes;
	if(! not_valid.isEmpty() ){
		answer = Message::question_yn(not_valid_str);
	}

	if(answer == GlobalMessage::Answer::Yes){
		for(int i=0; i<n_tracks; i++){
			if(not_valid.contains(i)) continue;

			m->tag_expression.update_tag(ui->le_tag->text(), v_md[i].filepath() );
			apply_tag(i);
		}
	}
}


bool GUI_TagEdit::replace_selected_tag_text(Tag t, bool b)
{
	TextSelection ts = ui->le_tag->get_text_selection();

	if(ts.selection_start < 0 && b) {
		sp_log(Log::Debug, this) << "Nothing selected...";
		return false;
	}

	QString text = ui->le_tag->text();

	// replace the string by a tag
	if(b){
		ReplacedString selected_text = text.mid( ts.selection_start, ts.selection_size );
		if(!m->tag_expression.check_tag(t, selected_text)) return false;

		text.replace( ts.selection_start, ts.selection_size, t );
		ui->le_tag->setText(text);

		m->tag_str_map[t] = selected_text;
	}

	// replace tag by the original string
	else{
		text.replace(t, m->tag_str_map[t]);
		ui->le_tag->setText(text);

		m->tag_str_map.remove(t);
	}


	if(check_idx(m->cur_idx)){
		// fetch corresponding filepath and update the tag expression
		MetaData md =m->tag_edit->get_metadata(m->cur_idx);
		bool valid = m->tag_expression.update_tag(text, md.filepath() );

		set_tag_colors( valid );
	}

	return true;
}

void GUI_TagEdit::btn_title_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_TITLE, b)){
		ui->btn_title->setChecked(false);
	}
}

void GUI_TagEdit::btn_artist_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_ARTIST, b)){
		ui->btn_artist->setChecked(false);
	}
}

void GUI_TagEdit::btn_album_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_ALBUM, b)){
		ui->btn_album->setChecked(false);
	}
}

void GUI_TagEdit::btn_track_nr_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_TRACK_NUM, b)){
		ui->btn_track_nr->setChecked(false);
	}
}

void GUI_TagEdit::btn_disc_nr_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_DISC, b)){
		ui->btn_disc_nr->setChecked(false);
	}
}

void GUI_TagEdit::btn_year_checked(bool b)
{
	if(!replace_selected_tag_text(TAG_YEAR, b)){
		ui->btn_year->setChecked(false);
	}
}

void GUI_TagEdit::btn_tag_help_clicked()
{
	QUrl url(QString("http://sayonara-player.com/faq.php#tag-edit"));
	QDesktopServices::openUrl(url);
}
