#include "EqualizerPresets.h"
#include "Helper/Logger/Logger.h"
#include <QStringList>

EQ_Setting::EQ_Setting(QString n){
	name = n;

	for(int i=0; i<10; i++){
		values.push_back(0);
	}
}


EQ_Setting::EQ_Setting(const EQ_Setting& s){
	values = s.values;
	name = s.name;
}

EQ_Setting::~EQ_Setting()
{

}


EQ_Setting EQ_Setting::fromString(const QString& str){

	EQ_Setting eq;
	QStringList list = str.split(':');
	if(list.size() < 11) {
		sp_log(Log::Debug) << "no valid eq string: " << str;
		return eq;
	}

	eq.name = list.at(0);
	list.pop_front();

	for(int i=0; i<list.size(); i++){
		if( i == eq.values.size() ){
			break;
		}

		eq.values[i] = list[i].toInt();
	}

	return eq;
}


QString EQ_Setting::toString() const {

	QString str = name;

	for(int i=0; i<values.size(); i++){
		str += QString(":") + QString::number(values[i]);
	}

	return str;
}


bool EQ_Setting::operator==(const EQ_Setting& s){
	QString str = toString();
	QString other = s.toString();
	return ( str.compare(other) == 0 );
}


QList<EQ_Setting> EQ_Setting::get_defaults(){
	QList<EQ_Setting> defaults;

	defaults << fromString(QString(":0:0:0:0:0:0:0:0:0:0"));
	defaults << fromString(QString("Flat:0:0:0:0:0:0:0:0:0:0"));
	defaults << fromString(QString("Rock:2:4:8:3:1:3:7:10:14:14"));
	defaults << fromString(QString("Light Rock:1:1:2:1:-2:-3:-1:3:5:8"));
	defaults << fromString(QString("Treble:0:0:-3:-5:-3:2:8:15:17:13"));
	defaults << fromString(QString("Bass:13:17:15:8:2:-3:-5:-3:0:0"));
	defaults << fromString(QString("Mid:0:0:5:9:15:15:12:7:2:0"));
	return defaults;
}
