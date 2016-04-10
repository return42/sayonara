#ifndef ARTISTMATCH_H
#define ARTISTMATCH_H

#include <QMap>
#include <QString>
#include <QObject>


class ArtistMatch{

private:
	QMap<QString, double> _very_good;
	QMap<QString, double> _well;
	QMap<QString, double> _poor;

	QString _artist;


public:
	/**
	 * @brief The Quality enum used to access the bin of interest. See ArtistMatch::get(Quality q)
	 */
	enum class Quality : quint8 {
		Poor = 0,
		Well = 1,
		Very_Good = 2
	};


	ArtistMatch();
	ArtistMatch(const QString& artist_name);
	ArtistMatch(const ArtistMatch& other);

	/**
	 * @brief checks, if structure is valid.
	 * @return false, if all bins are empty. True else
	 */
	bool is_valid() const;

	/**
	 * @brief Compares two ArtistMatch structures
	 * @param am the other ArtistMatch
	 * @return true, if the artist string is the same. False else
	 */
	bool operator ==(const ArtistMatch& am) const;

	/**
	 * @brief adds an artist string to the corresponding bin
	 * @param artist artist string
	 * @param match the match value
	 */
	void add(const QString& artist, double match);

	/**
	 * @brief get bin by quality
	 * @param q quality. See ArtistMatch::Quality
	 * @return the desired bin
	 */
	QMap<QString, double> get(Quality q) const;

	/**
	 * @brief get the corresponding artist name of the ArtistMatch structure
	 * @return artist name
	 */
	QString get_artist_name() const;

	/**
	 * @brief converts the artist match to string
	 * @return string representation
	 */
	QString to_string() const;
};




#endif // ARTISTMATCH_H
