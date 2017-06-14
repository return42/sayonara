#include "GUI_IconPreferences.h"
#include "GUI/Preferences/ui_GUI_IconPreferences.h"
#include "Helper/Language.h"
#include "Helper/Logger/Logger.h"
#include "Helper/Settings/Settings.h"

#include <QStringList>
#include <QIcon>
#include <QDir>
#include <QCheckBox>
#include <QLabel>
#include <QRadioButton>

class IconRadioButton : public QRadioButton
{
private:
	QString _data;

public:
	using QRadioButton::QRadioButton;

	void set_data(const QString& data)
	{
		_data = data;
	}

	QString data() const
	{
		return _data;
	}
};

struct GUI_IconPreferences::Private
{
	QHash<QString, IconRadioButton*> rb_map;
	QString original_theme;

	Private()
	{
		original_theme = QIcon::themeName();
	}
};

GUI_IconPreferences::GUI_IconPreferences(QWidget* parent) :
	PreferenceWidgetInterface(parent)
{
	_m = Pimpl::make<Private>();
}

GUI_IconPreferences::~GUI_IconPreferences() {}

static
void add_line(QWidget* widget)
{
	QLayout* layout = widget->layout();
	QFrame* line = new QFrame(widget);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	layout->addWidget(line);
}


static
void add_header_entry(const QString& name, QWidget* widget)
{
	QLayout* layout = widget->layout();

	add_line(widget);

	QLabel* label = new QLabel(widget);
	label->setText(name);
	label->setStyleSheet("font-weight: bold;");
	layout->addWidget(label);

	add_line(widget);
}


static
IconRadioButton* add_radio_button(const QString& text, const QString& data, QWidget* widget)
{
	QLayout* layout = widget->layout();

	IconRadioButton* rb = new IconRadioButton(text, widget);
	rb->set_data(data);
	layout->addWidget(rb);

	return rb;
}

void GUI_IconPreferences::init_ui()
{
	if(is_ui_initialized()){
		return;
	}

	setup_parent(this, &ui);

	QWidget* widget = ui->scroll_area_widget;
	QLayout* layout = new QVBoxLayout(widget);
	widget->setLayout(layout);

	ui->scroll_area->setWidget(widget);

	add_line(widget);

	IconRadioButton* rb_automatic = add_radio_button(
					tr("Automatic") +
					" (" + Lang::get(Lang::Default) + ")",
					"",
					 widget
	);

	rb_automatic->setStyleSheet("font-weight: bold;");
	_m->rb_map[""] = rb_automatic;

	QStringList icon_paths = QIcon::themeSearchPaths();
	for(const QString& icon_path : icon_paths)
	{
		QDir d(icon_path);
		QStringList subdirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

		if(!subdirs.isEmpty()){
			add_header_entry(icon_path, widget);
		}

		for(const QString& subdir : subdirs) {

			QString abs_path = d.absoluteFilePath(subdir);
			IconRadioButton* rb = add_radio_button(subdir, subdir, widget);
			_m->rb_map[subdir] = rb;

			if(subdir.compare(QIcon::themeName()) == 0){
				rb->setStyleSheet("font-weight: bold;");
			}
		}
	}

	revert();
}

void GUI_IconPreferences::retranslate_ui()
{
	IconRadioButton* rb = _m->rb_map[""];
	if(rb){
		rb->setText(
					tr("Automatic") +
					" (" + Lang::get(Lang::Default) + ")"
		);
	}
}

QString GUI_IconPreferences::get_action_name() const
{
	return tr("Icons") + " (" + tr("Requires restart") + ")";
}

void GUI_IconPreferences::commit()
{
	for(const QString& key : _m->rb_map.keys())
	{
		IconRadioButton* rb = _m->rb_map[key];
		if(rb->isChecked()){
			_settings->set(Set::Icon_Theme, key);
			break;
		}
	}
}

void GUI_IconPreferences::revert()
{
	for(const QString& key : _m->rb_map.keys())
	{
		IconRadioButton* rb = _m->rb_map[key];
		rb->setChecked(key.compare(_m->original_theme) == 0);
	}
}
