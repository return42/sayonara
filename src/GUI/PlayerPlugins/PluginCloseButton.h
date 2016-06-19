#ifndef PLUGINCLOSEBUTTON_H
#define PLUGINCLOSEBUTTON_H

#include <QPushButton>
#include <QShowEvent>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QMenu>
#include "Helper/Settings/SayonaraClass.h"

/**
 * @brief The MenuButton class.\n
 * A button that sends a signal when clicked. \n
 * This Class is meant for inheritance like MenuToolButton does.
 * @ingroup GUIHelper
 */
class PluginCloseButton :
		public QPushButton,
		protected SayonaraClass
{

	Q_OBJECT

signals:
	void sig_triggered(QPoint);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void enterEvent(QEvent* e) override;
	virtual void leaveEvent(QEvent* e) override;

	/**
	 * @brief sets the icon depending on the skin
	 */
	void set_std_icon();

public:
	PluginCloseButton(QWidget* parent=nullptr);
	virtual ~PluginCloseButton();

protected slots:
	void _sl_skin_changed();
};

#endif // PLUGINCLOSEBUTTON_H
