/* SayonaraLoadingBar.cpp */

#include "SayonaraLoadingBar.h"

SayonaraLoadingBar::SayonaraLoadingBar(QWidget* parent) :
	QProgressBar(parent)
{
	_parent = parent;

	_fixed_height = 5;

	this->setEnabled(false);
	this->setObjectName("loading_bar");

	this->setMinimum(0);
	this->setMaximum(0);
	this->setVisible(false);
}

void SayonaraLoadingBar::showEvent(QShowEvent* e)
{
	this->setGeometry(0,
					  _parent->height() - _fixed_height,
					  _parent->width(),
					  _fixed_height);

	QProgressBar::showEvent(e);
}
