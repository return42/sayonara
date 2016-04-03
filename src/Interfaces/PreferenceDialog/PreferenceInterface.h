#ifndef PREFERENCEINTERFACE_H
#define PREFERENCEINTERFACE_H

#include <QAction>
#include <QByteArray>
#include <QLabel>
#include <QShowEvent>
#include <QCloseEvent>
#include <QString>

#include "GUI/Helper/SayonaraWidget.h"

class PreferenceAction : public QAction
{
	Q_OBJECT

public:
	PreferenceAction(const QString& name, QWidget* preference_interface) :
		QAction(nullptr)
	{
		this->setText(name + "...");
		connect(this, &QAction::triggered, preference_interface, &QWidget::show);
	}

};

template <typename T>
class PreferenceInterface : public T
{

private:
	PreferenceAction*	_action=nullptr;
	bool				_is_initialized;
	QByteArray			_geometry;

protected:
	/**
	 * @brief call setup_parent(this) here.\n
	 * initialize compoenents and connections here.\n
	 * After calling setup_parent(this), the preference Dialog is ready to use, language_changed() is called automatically
	 */
	virtual void init_ui()=0;

	template<typename W>
	/**
	 * @brief Sets up the Preference dialog. After this method, the dialog is "ready to use"\n
	 * This method should be the first to be called when calling init_ui()
	 * @param widget should always be "this"
	 */
	void setup_parent(W* widget) final {

		widget->setupUi(widget);
		//widget->setModal(true);

		QLabel* title_label = widget->get_title_label();
		if(title_label){
			title_label->setText(widget->get_action_name());
		}

		_is_initialized = true;

		widget->language_changed();
	}




	void language_changed()
	{
		translate_action();

		if(!is_ui_initialized()){
			return;
		}

		QString new_name = get_action_name();
		QLabel* label = get_title_label();
		if(label){
			label->setText(new_name);
		}

		this->setWindowTitle(new_name);
	}


	void translate_action()
	{
		QString new_name = this->get_action_name();
		this->get_action()->setText(new_name + "...");
	}



protected:

	void showEvent(QShowEvent* e) override
	{
		{
			if(!is_ui_initialized()){
				init_ui();
			}

			T::showEvent(e);

			if(!_geometry.isEmpty()){
				this->restoreGeometry(_geometry);
			}
		}

	}


	void closeEvent(QCloseEvent* e) override
	{
		//_geometry = saveGeometry();

		T::closeEvent(e);
	}


public:

	PreferenceInterface(QWidget* parent=nullptr) :
		T(parent)
	{
		_is_initialized = false;
	}


	/**
	 * @brief checks if ui has already been initialized.
	 * @return false, if the widget has never been activated before, true else
	 */
	virtual bool is_ui_initialized() const final
	{
		return _is_initialized;
	}


	/**
	 * @brief get action with translated text
	 * @return
	 */
	virtual QAction* get_action() final
	{
		// action has to be initialized here, because pure
		// virtual get_action_name should not be called from ctor
		QString name = get_action_name();
		if(!_action){
			_action = new PreferenceAction(name, this);
		}

		_action->setText(name + "...");
		return _action;
	}

	/**
	 * @brief has to be implemented and should return the translated action text
	 * @return translated action name
	 */
	virtual QString get_action_name() const=0;
	virtual QLabel* get_title_label()=0;

	virtual void commit()=0;
	virtual void revert()=0;

};

#endif // PREFERENCEINTERFACE_H
