#ifndef URLHELPER_H
#define URLHELPER_H

#include <QString>

namespace Helper
{
	/**
	 * @brief Helper function for Url handling
	 * @ingroup Helper
	 */
	namespace Url {
		/**
		 * @brief get protocol string without ://
		 * @param url source url
		 * @return the first letters before ://
		 */
		QString get_protocol(const QString& url);

		/**
		 * @brief get url until the first /
		 * @param url source url
		 * @return the first letters before first /, url if there is no /
		 */
		QString get_base_url(const QString& url);

		/**
		 * @brief get everything behind the last /
		 * @param url source filename
		 * @return everything behind last /, or empty string, if there is no /
		 */
		QString get_filename(const QString& url);
	}
}

#endif // URLHELPER_H
