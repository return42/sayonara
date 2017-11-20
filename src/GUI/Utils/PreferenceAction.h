#ifndef PREFERENCEACTION_H
#define PREFERENCEACTION_H

#include <QAction>
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"

class QString;
class QWidget;

class PreferenceAction :
		public QAction,
		public SayonaraClass
{
	Q_OBJECT
	PIMPL(PreferenceAction)

	public:
		PreferenceAction(const QString& display_name, const QString& identifier, QWidget* parent);
		virtual ~PreferenceAction();

		virtual QString label() const;
		virtual QString identifier() const=0;

	protected:
		virtual QString display_name() const=0;
		void language_changed();
};

class LibraryPreferenceAction :
	public PreferenceAction
{
	public:
		LibraryPreferenceAction(QWidget* parent);
		QString display_name() const override;
		QString identifier() const override;
};

class PlaylistPreferenceAction :
	public PreferenceAction
{
	public:
		PlaylistPreferenceAction(QWidget* parent);

		QString display_name() const override;
		QString identifier() const override;
};

class SearchPreferenceAction :
	public PreferenceAction
{
	public:
		SearchPreferenceAction(QWidget* parent);
		QString display_name() const override;
		QString identifier() const override;
};

class CoverPreferenceAction :
	public PreferenceAction
{
	public:
		CoverPreferenceAction(QWidget* parent);
		QString display_name() const override;
		QString identifier() const override;
};


class PlayerPreferencesAction :
	public PreferenceAction
{
	public:
		PlayerPreferencesAction(QWidget* parent);
		QString display_name() const override;
		QString identifier() const override;
};


#endif // PREFERENCEACTION_H
