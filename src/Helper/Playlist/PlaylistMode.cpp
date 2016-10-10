#include "PlaylistMode.h"
#include "Helper/Logger/Logger.h"
#include <QStringList>

Playlist::Mode::State
Playlist::Mode::set_state(bool active, bool enabled)
{
	quint8 ret = Playlist::Mode::Off;
	if(active){
		ret |= Playlist::Mode::On;
	}

	if(!enabled){
		ret |= Playlist::Mode::Disabled;
	}

	return (Playlist::Mode::State) ret;
}

Playlist::Mode::State Playlist::Mode::rep1() const { return _rep1; }
Playlist::Mode::State Playlist::Mode::repAll() const { return _repAll; }
Playlist::Mode::State Playlist::Mode::append() const { return _append; }
Playlist::Mode::State Playlist::Mode::shuffle() const { return _shuffle; }
Playlist::Mode::State Playlist::Mode::dynamic() const { return _dynamic; }
Playlist::Mode::State Playlist::Mode::gapless() const { return _gapless; }

void Playlist::Mode::setRep1(Playlist::Mode::State state){ _rep1 = state; }
void Playlist::Mode::setRepAll(Playlist::Mode::State state){ _repAll = state; }
void Playlist::Mode::setAppend(Playlist::Mode::State state){ _append = state; }
void Playlist::Mode::setShuffle(Playlist::Mode::State state){ _shuffle = state; }
void Playlist::Mode::setDynamic(Playlist::Mode::State state){ _dynamic = state; }
void Playlist::Mode::setGapless(Playlist::Mode::State state){ _gapless = state; }

void Playlist::Mode::setRep1(bool on, bool enabled){ _rep1 = set_state(on, enabled); }
void Playlist::Mode::setRepAll(bool on, bool enabled){ _repAll = set_state(on, enabled); }
void Playlist::Mode::setAppend(bool on, bool enabled){ _append = set_state(on, enabled); }
void Playlist::Mode::setShuffle(bool on, bool enabled){ _shuffle = set_state(on, enabled); }
void Playlist::Mode::setDynamic(bool on, bool enabled){ _dynamic = set_state(on, enabled); }
void Playlist::Mode::setGapless(bool on, bool enabled){ _gapless = set_state(on, enabled); }

bool Playlist::Mode::isActive(Playlist::Mode::State pl)
{
	quint8 ipl = (quint8) pl;
	return ( ipl & Playlist::Mode::On );
}

bool Playlist::Mode::isEnabled(Playlist::Mode::State pl){
	quint8 ipl = (quint8) pl;
	return (( ipl & Playlist::Mode::Disabled ) == 0);
}

bool Playlist::Mode::isActiveAndEnabled(Playlist::Mode::State pl)
{
	return (isEnabled(pl) && isActive(pl));
}

Playlist::Mode::Mode(){
	_rep1 = Playlist::Mode::Off;
	_repAll = Playlist::Mode::On;
	_append = Playlist::Mode::Off;

	_shuffle = Playlist::Mode::Off;
	_gapless = Playlist::Mode::Off;
	_dynamic = Playlist::Mode::Off;
}

void Playlist::Mode::print()
{
	sp_log(Log::Debug) << "rep1 = "   << (int) _rep1 << ", "
		<< "repAll = "  << (int) _repAll << ", "
		<< "append = "  << (int) _append <<", "
		<< "dynamic = " << (int) _dynamic << ","
		<< "gapless = " << (int) _gapless;

}

QString Playlist::Mode::toString() const {
	QString str;
	str += QString::number((int) _append)  + QString(",");
	str += QString::number((int) _repAll)  + QString(",");
	str += QString::number((int) _rep1)    + QString(",");
	str += "0,";
	str += QString::number((int) _shuffle) + QString(",");
	str += QString::number((int) _dynamic) + QString(",");
	str += QString::number((int) _gapless);

	return str;
}

Playlist::Mode Playlist::Mode::fromString(const QString& str){

	Playlist::Mode plm;
	QStringList list = str.split(',');

	if(list.size() < 6) return plm;

	plm.setAppend((Playlist::Mode::State) list[0].toInt());
	plm.setRepAll((Playlist::Mode::State) list[1].toInt());
	plm.setRep1((Playlist::Mode::State) list[2].toInt());
	plm.setShuffle((Playlist::Mode::State) list[4].toInt());
	plm.setDynamic((Playlist::Mode::State) list[5].toInt());

	if(list.size() > 6){
		plm.setGapless((Playlist::Mode::State) list[6].toInt());
	}

	return plm;
}

bool Playlist::Mode::operator==(const Playlist::Mode& pm) const {

	if(pm.append() != _append) return false;
	if(pm.repAll() != _repAll) return false;
	if(pm.rep1() != _rep1) return false;
	if(pm.shuffle() != _shuffle) return false;
	if(pm.dynamic() != _dynamic) return false;
	if(pm.gapless() != _gapless) return false;

	return true;
}
