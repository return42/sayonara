#include "Discnumber.h"

Models::Discnumber::Discnumber()
{
	n_discs = 1;
	disc = 1;
}

Models::Discnumber::Discnumber(quint8 disc_, quint8 n_discs_)
{
	n_discs = n_discs_;
	disc = disc_;
}


QString Models::Discnumber::to_string()
{
	return QString("TPOS: ") + QString::number(disc) + "/" + QString::number(n_discs);
}
