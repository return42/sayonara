#ifndef DISCNUMBER_MODELS_H_
#define DISCNUMBER_MODELS_H_

#include <QString>

namespace Models
{
	/**
	 * @brief The Discnumber class
	 * @ingroup Tagging
	 */
	class Discnumber
	{
	public:

		quint8		disc;
		quint8		n_discs;

		Discnumber();
		Discnumber(quint8 disc, quint8 n_discs=1);
		QString to_string();
	};
}

#endif // DISCNUMBER_H
