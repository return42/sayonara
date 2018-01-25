#include "TextSelection.h"

TextSelection::TextSelection()
{
	reset();
}

void TextSelection::reset()
{
	selection_start = -1;
	selection_size = 0;
}

