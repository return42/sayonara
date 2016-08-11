#ifndef COVER_BUTTON_H
#define COVER_BUTTON_H

#include <QPushButton>
#include "Components/CoverLookup/CoverLocation.h"

class GUI_AlternativeCovers;
class CoverLookup;

class CoverButton : public QPushButton
{
	Q_OBJECT

	signals:
	void sig_cover_replaced();
	void sig_cover_found();

	public:
	CoverButton(QWidget* parent=nullptr);
	virtual ~CoverButton();


	bool has_valid_cover() const;
	CoverLocation get_found_cover() const;
	void set_cover_location(const CoverLocation& cl);

	private:
	GUI_AlternativeCovers* 	_alternative_covers=nullptr;
	CoverLookup*			_cover_lookup=nullptr;
	CoverLocation 			_found_cover_location;
	CoverLocation 			_search_cover_location;
	QString					_text;

	private slots:
	void cover_button_clicked();
	void alternative_cover_fetched(const CoverLocation& cl);
	void cover_found(const CoverLocation& cl);
	void set_cover_image(const QString& cover_path);
};


#endif
