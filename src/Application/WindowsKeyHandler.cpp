#include "WindowsKeyHandler.h"
#include "Components/PlayManager/PlayManager.h"


WindowsKeyHandler::WindowsKeyHandler(QObject* parent):
	QThread(parent)
{
	_may_run = true;
}


void WindowsKeyHandler::run()
{

	PlayManager* play_manager = PlayManager::getInstance();

	register_hotkey(MOD_NOREPEAT, VK_MEDIA_PLAY);
	register_hotkey(MOD_NOREPEAT, VK_MEDIA_PAUSE);
	register_hotkey(MOD_NOREPEAT, VK_MEDIA_PLAY_PAUSE);
	register_hotkey(MOD_NOREPEAT, VK_MEDIA_STOP);
	register_hotkey(MOD_NOREPEAT, VK_MEDIA_PREV_TRACK);
	register_hotkey(MOD_NOREPEAT, VK_MEDIA_NEXT_TRACK);

	MSG msg = {0};
	while (_may_run)
	{

		if( (GetMessage(&msg, NULL, 0, 0) != 0) && (msg.message == WM_HOTKEY) )
		{
			//UINT modifiers = msg.lParam;
			UINT key = msg.wParam;

			switch(key)
			{
				case VK_MEDIA_PLAY:
					play_manager->play();
					break;
				case VK_MEDIA_PAUSE:
					play_manager->pause();
					break;

				case VK_MEDIA_PLAY_PAUSE:
					play_manager->play_pause();
					break;

				case VK_MEDIA_STOP:
					play_manager->stop();
					break;
				case VK_MEDIA_PREV_TRACK:
					play_manager->previous();
					break;
				case VK_MEDIA_NEXT_TRACK:
					play_manager->next();
					break;
				default:
					break;
			}

		}

		if(!_may_run){
			return;
		}

		Helper::sleep_ms(100);
	}
}

WindowsKeyHandler::stop(){
	_may_run = false;
}

WindowsKeyHandler::register_hotkey(UINT modifiers, UINT key){

	bool success = register_hotkey(NULL, 1, modifiers, key);
	sp_log(Log::Debug) << "Try to register hotkey: " << key << success;
}
