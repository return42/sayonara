#include "CoverChangeNotifier.h"

using Cover::ChangeNotfier;

ChangeNotfier::ChangeNotfier() : QObject(nullptr) {}

ChangeNotfier::~ChangeNotfier() {}

void ChangeNotfier::shout()
{
	emit sig_covers_changed();
}
