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
	    quint8		rating_byte;	// value between 0 and 0xff
	    int			playcount;

	    Popularimeter();
	    Popularimeter(const QString& email_, quint8 rating_byte_, int playcount);
	    void set_sayonara_rating(quint8 max_5);
	    quint8 get_sayonara_rating() const;
	    QString to_string();
    };
}

#endif // POPULARIMETER_H
