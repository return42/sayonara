#include "LibraryPluginCombobox.h"
#include "LibraryPluginHandler.h"
#include "LibraryContainer/LibraryContainer.h"
#include "GUI/Helper/Delegates/ComboBoxDelegate.h"
#include "Helper/Settings/Settings.h"

#include <QList>
#include <QAction>
#include <QFontMetrics>
#include <QEvent>
#include <QSize>
#include <QAbstractItemView>

struct LibraryPluginCombobox::Private
{
	LibraryPluginHandler* lph=nullptr;
	QList<QAction*> actions;

	Private()
	{
		lph = LibraryPluginHandler::getInstance();
	}
};


LibraryPluginCombobox::LibraryPluginCombobox(const QString& text, QWidget* parent) :
	QComboBox(parent),
	SayonaraClass()
{
	_m = Pimpl::make<Private>();

	this->setItemDelegate(new ComboBoxDelegate(this));
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	this->setFrame(false);

	connect(_m->lph, &LibraryPluginHandler::sig_initialized, this, &LibraryPluginCombobox::setup_actions);
	connect(_m->lph, &LibraryPluginHandler::sig_current_library_changed, this, &LibraryPluginCombobox::current_library_changed);

	REGISTER_LISTENER_NO_CALL(Set::Lib_AllLibraries, setup_actions);
	REGISTER_LISTENER_NO_CALL(Set::Player_Language, setup_actions);
	REGISTER_LISTENER(Set::Player_Style, style_changed);

	setup_actions();
	setCurrentText(text);


}

LibraryPluginCombobox::~LibraryPluginCombobox() {}

void LibraryPluginCombobox::setup_actions()
{
	this->clear();

	LibraryContainerInterface* current_library = _m->lph->current_library();
	QList<LibraryContainerInterface*> libraries = _m->lph->get_libraries();

	for(const LibraryContainerInterface* container : libraries)
	{
		this->addItem(QIcon(container->icon()), container->display_name(), container->name());
	}
}

void LibraryPluginCombobox::action_triggered(bool b)
{
	if(!b){
		return;
	}

	QAction* action = static_cast<QAction*>(sender());
	QString name = action->data().toString();

	LibraryPluginHandler::getInstance()->set_current_library(name);
	for(QAction* library_action : _m->actions)
	{
		if(library_action == action){
			continue;
		}

		library_action->setChecked(false);
	}
}

void LibraryPluginCombobox::current_library_changed(const QString& name)
{
	for(int i=0; i<this->count(); i++){
		if(this->itemData(i).toString().compare(name) == 0){
			this->setCurrentIndex(i);
			break;
		}
	}
}

void LibraryPluginCombobox::changeEvent(QEvent* event)
{
	QComboBox::changeEvent(event);

	if(event->type() == QEvent::StyleChange){

		QFontMetrics f(this->font());
		int h = (f.height() * 3) / 2;
		this->setIconSize(QSize(h, h));
	}
}

void LibraryPluginCombobox::style_changed()
{
	bool is_dark = (_settings->get(Set::Player_Style) == 1);
	if(is_dark){
		this->view()->parentWidget()->setStyleSheet("background: #3c3c3c; margin: -2px; padding: 0px; border: none;");
	}

	else {
		this->view()->parentWidget()->setStyleSheet("");
	}
}
