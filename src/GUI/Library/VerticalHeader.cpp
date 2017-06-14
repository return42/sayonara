#include "VerticalHeader.h"

QSize VerticalHeader::sizeHint() const
{
	return this->sectionSizeFromContents(0);
}
