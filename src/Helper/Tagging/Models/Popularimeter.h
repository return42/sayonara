#ifndef POPULARIMETER_MODELS_H_
#define POPULARIMETER_MODELS_H_

#include <QString>

namespace Models
{
    /**
     * @brief The Popularimeter class
     * @ingroup Tagging
     */
    class Popularimeter
    {
	public:

	    QString		email;
		int			rating;
		int			playcount;

	    Popularimeter();
		Popularimeter(const QString& email_, quint8 rating_byte_, int playcount);
		void set_rating(quint8 max_5);
		void set_rating_byte(quint8 byte);
		quint8 get_rating() const;
		quint8 get_rating_byte() const;
	    QString to_string();
    };
}

#endif // POPULARIMETER_H
