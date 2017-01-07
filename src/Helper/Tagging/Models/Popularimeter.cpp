#include "Popularimeter.h"

Models::Popularimeter::Popularimeter()
{
    email = "sayonara player";
    rating_byte = 0x00;
    playcount = 0;
}

Models::Popularimeter::Popularimeter(const QString& email_, quint8 rating_byte_, int playcount_)
{
    playcount = playcount_;
    rating_byte = rating_byte_;
    email = email_;
}


void Models::Popularimeter::set_sayonara_rating(quint8 max_5)
{
    if(max_5 >= 5){
	rating_byte = 255;
    }
    else if(max_5 == 4){
	rating_byte =  196;
    }
    else if(max_5 == 3){
	rating_byte =  128;
    }
    else if(max_5 == 2){
	rating_byte =  64;
    }
    else if(max_5 == 1){
	rating_byte =  1;
    }
    else{
	rating_byte = 0;
    }
}

quint8 Models::Popularimeter::get_sayonara_rating() const
{
    if(rating_byte > 224){
	return 5;
    }
    else if(rating_byte > 160){
	return 4;
    }
    else if(rating_byte > 96){
	return 3;
    }
    else if(rating_byte > 32){
	return 2;
    }
    else if(rating_byte > 0){
	return 1;
    }
    return 0;
}

QString Models::Popularimeter::to_string()
{
    return QString("POPM: ") + email + " " + QString::number(rating_byte) + " " + QString::number(playcount);
}
