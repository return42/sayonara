#ifndef TEXTSELECTION_H
#define TEXTSELECTION_H

/**
 * @brief Holds start and size of a selection
 * @ingroup Tagging
 */
struct TextSelection
{
	int selection_start;
	int selection_size;

	TextSelection();

	/**
	 * @brief set selection_start to -1 and selection_size to 0
	 */
	void reset();
};

#endif // TEXTSELECTION_H
