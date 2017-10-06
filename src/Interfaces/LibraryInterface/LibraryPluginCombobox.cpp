#include "LibraryPluginCombobox.h"
#include "LibraryPluginHandler.h"
#include "LibraryContainer/LibraryContainer.h"
#include "Helper/Settings/Settings.h"

#include <QList>
#include <QAction>
#include <QSize>

struct LibraryPluginCombobox::Private
{
	LibraryPluginHandler* lph=nullptr;
	QList<QAction*> actions;

	Private()
	{
		lph = LibraryPluginHandler::instance();
	}
};


LibraryPluginCombobox::LibraryPluginCombobox(const QString& text, QWidget* parent) :
	SayonaraComboBox(parent)
{
	m = Pimpl::make<Private>();

	this->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->setFrame(false);

	connect(m->lph, &LibraryPluginHandler::sig_initialized, this, &LibraryPluginCombobox::setup_actions);
	connect(m->lph, &LibraryPluginHandler::sig_libraries_changed, this, &LibraryPluginCombobox::setup_actions);
	connect(m->lph, &LibraryPluginHandler::sig_current_library_changed, this, &LibraryPluginCombobox::current_library_changed);

    Set::listen(Set::Player_Language, this, &LibraryPluginCombobox::setup_actions, false);

    setup_actions();
	setCurrentText(text);
}

LibraryPluginCombobox::~LibraryPluginCombobox() {}

void LibraryPluginCombobox::setup_actions()
{
	this->clear();

	QList<LibraryContainerInterface*> libraries = m->lph->get_libraries();

	for(const LibraryContainerInterface* container : libraries)
	{
        QPixmap pm = container->icon().scaled(
                    this->iconSize(),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
        );

        this->addItem(QIcon(pm), container->display_name(), container->name());
	}

	current_library_changed(m->lph->current_library()->name());
}

void LibraryPluginCombobox::action_triggered(bool b)
{
	if(!b){
		return;
	}

	QAction* action = static_cast<QAction*>(sender());
	QString name = action->data().toString();

	LibraryPluginHandler::instance()->set_current_library(name);
	for(QAction* library_action : m->actions)
	{
		if(library_action == action){
			continue;
		}

		library_action->setChecked(false);
	}
}

void LibraryPluginCombobox::current_library_changed(const QString& name)
{
	for(int i=0; i<this->count(); i++) {
		if(this->itemData(i).toString().compare(name) == 0){
			this->setCurrentIndex(i);
			break;
		}
	}
}
