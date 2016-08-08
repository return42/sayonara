/* GUI_Helper.h */

#ifndef GUI_HELPER_H
#define GUI_HELPER_H

#include <QIcon>
#include <QString>
#include <QPixmap>
#include <QSize>
#include <QImage>
#include <QWidget>

namespace GUI
{
	/**
	 * @brief fetch a icon from resources
	 * @param icon_name if icon_name ends with png the input string is not modified, else a .svg.png is appended
	 * @return icon
	 */
	QIcon get_icon(const QString& icon_name);


	/**
	 * @brief fetch a pixmap from resources
	 * @param icon_name if icon_name ends with png the input string is not modified, else a .svg.png is appended
	 * @param sz target size of pixmap
	 * @param keep_aspect if true, aspect ratio is kept
	 * @return pixmap
	 */
	QPixmap get_pixmap(const QString& icon_name, QSize sz=QSize(0, 0), bool keep_aspect=true);

}

#endif // GUI_HELPER_H
