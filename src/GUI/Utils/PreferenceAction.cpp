#include "PreferenceAction.h"
#include "Utils/Settings/Settings.h"
#include "Utils/Language.h"
#include "Components/Preferences/PreferenceRegistry.h"

#include <QFont>

struct PreferenceAction::Private
{
	QString identifier;

	Private(const QString& identifier) :
		identifier(identifier)
	{}
};

PreferenceAction::PreferenceAction(const QString& text, const QString& identifier, QWidget* parent) :
	QAction(QString(Lang::get(Lang::Preferences) + ": " + text), parent)
{
	m = Pimpl::make<Private>(identifier);

	connect(this, &QAction::triggered, [=](){
		PreferenceRegistry::instance()->show_preference(this->identifier());
	});

	Set::listen(Set::Player_Language, this, &PreferenceAction::language_changed, false);
}

PreferenceAction::~PreferenceAction() {}

QString PreferenceAction::label() const
{
	return Lang::get(Lang::Preferences) + ": " + display_name();
}

QString PreferenceAction::identifier() const
{
	return m->identifier;
}

void PreferenceAction::language_changed()
{
	this->setText(this->label());
}

LibraryPreferenceAction::LibraryPreferenceAction(QWidget* parent) :
	PreferenceAction(Lang::get(Lang::Library), identifier(), parent)
{}

QString LibraryPreferenceAction::display_name() const
{
	return Lang::get(Lang::Library);
}

QString LibraryPreferenceAction::identifier() const
{
	return "library";
}

PlaylistPreferenceAction::PlaylistPreferenceAction(QWidget* parent) :
	PreferenceAction(Lang::get(Lang::Playlist), identifier(), parent)
{}


QString PlaylistPreferenceAction::display_name() const
{
	return Lang::get(Lang::Playlist);
}

QString PlaylistPreferenceAction::identifier() const
{
	return "playlist";
}

SearchPreferenceAction::SearchPreferenceAction(QWidget* parent) :
	PreferenceAction(Lang::get(Lang::SearchNoun), identifier(), parent)
{}

QString SearchPreferenceAction::display_name() const
{
	return Lang::get(Lang::SearchNoun);
}

QString SearchPreferenceAction::identifier() const
{
	return "search";
}

CoverPreferenceAction::CoverPreferenceAction(QWidget* parent) :
	   PreferenceAction(Lang::get(Lang::Covers), identifier(), parent)
{}

QString CoverPreferenceAction::display_name() const
{
	return Lang::get(Lang::Covers);
}

QString CoverPreferenceAction::identifier() const
{
	return "covers";
}

PlayerPreferencesAction::PlayerPreferencesAction(QWidget* parent) :
	PreferenceAction(Lang::get(Lang::Application), identifier(), parent)
{

}

QString PlayerPreferencesAction::display_name() const
{
	return Lang::get(Lang::Application);
}

QString PlayerPreferencesAction::identifier() const
{
	return "player";
}
