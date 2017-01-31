#include "Popularimeter.h"

Models::Popularimeter::Popularimeter()
{
    email = "sayonara player";
	rating = 0;
    playcount = 0;
}

Models::Popularimeter::Popularimeter(const QString& email_, quint8 rating_, int playcount_)
{
    playcount = playcount_;
	rating = rating_;
    email = email_;
}


void Models::Popularimeter::set_rating(quint8 max_5)
{
	rating = max_5;
}

void Models::Popularimeter::set_rating_byte(quint8 byte)
{
	if(byte == 0x00){
		rating = 0;
	}
	else if(byte < 0x30){		//48
		rating = 1;
	}
	else if(byte < 0x60){	// 92
		rating = 2;
	}
	else if(byte < 0xA0){	// 160
		rating = 3;
	}
	else if(byte < 0xD8){	// 216
		rating = 4;
	}
	else{
		rating = 5;			// 255
	}
}

quint8 Models::Popularimeter::get_rating() const
{
	return rating;
}

quint8 Models::Popularimeter::get_rating_byte() const
{
	quint8 rating_byte;

	switch(rating)
	{
		case 0:
			rating_byte = 0x00;
			break;
		case 1:
			rating_byte = 0x01; // 1
			break;
		case 2:
			rating_byte = 0x40; // 64
			break;
		case 3:
			rating_byte = 0x7F; // 128
			break;
		case 4:
			rating_byte = 0xC0; // 192
			break;
		default:
			rating_byte = 0xFF; // 255
	}

	return rating_byte;
}

QString Models::Popularimeter::to_string()
{
	return QString("POPM: ") + email + " " + QString::number(get_rating_byte()) + " " + QString::number(playcount);
}
