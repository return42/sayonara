#ifndef INFO_DIALOG_CONTAINER_H_
#define INFO_DIALOG_CONTAINER_H_

#include "Helper/MetaData/MetaDataList.h"

class GUI_InfoDialog;

/**
 * @brief An interface used to abstract the usage of the info dialog.
 * An implementing class has to return the interpretation of a MetaDataList
 * and the MetaDataList itself. The implementing class may call the show functions
 * to open the info dialog at its specific tab.
 * @ingroup GUI
 * @ingroup Interfaces
 */
class InfoDialogContainer
{
	public:
		InfoDialogContainer();
		virtual ~InfoDialogContainer();

		/**
		 * @brief Show the Info dialogs' info tab
		 */
		void show_info();

		/**
		 * @brief Show the Info dialogs' lyrics tab
		 */
		void show_lyrics();

		/**
		 * @brief Show the tag editor
		 */
		void show_edit();

		/**
		 * @brief this function should not be called from outside.
		 * This function is triggered when the info dialog was closed.
		 */
		void info_dialog_closed();

	private:
		GUI_InfoDialog*		_info_dialog=nullptr;

	private:
		void check_info_dialog();
		bool init_dialog();

	protected:
		/**
		 * @brief get the interpretation for the metadata. Maybe a list of
		 * metadata should be intrepeted as albums while others should be
		 * considered as tracks
		 * @return interpretation of metadata
		 */
		virtual MetaDataList::Interpretation get_metadata_interpretation() const=0;

		/**
		 * @brief get the metadata that should be used for the info dialog
		 * So for lists, the selected tracks are used here
		 * @return MetaDataList
		 */
		virtual MetaDataList get_data_for_info_dialog() const=0;
};

#endif
