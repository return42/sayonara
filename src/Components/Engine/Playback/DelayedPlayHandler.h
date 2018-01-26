#ifndef DELAYEDPLAYHANDLER_H
#define DELAYEDPLAYHANDLER_H

#include "Utils/Pimpl.h"
#include <QObject>

namespace Pipeline
{
	class DelayedPlayHandler
	{
		PIMPL(DelayedPlayHandler)

		public:
			DelayedPlayHandler();
			~DelayedPlayHandler();

		public:
			virtual void play()=0;

			void play_in(MilliSeconds ms);
			void abort_delayed_playing();
	};

	class Logic : public QObject
	{
		Q_OBJECT
		PIMPL(Logic)

		friend class DelayedPlayHandler;

		private:
			Logic(DelayedPlayHandler* dph);
			~Logic();

			void start_timer(MilliSeconds ms);
			void stop_timer();
	};
}


#endif // DELAYEDPLAYHANDLER_H
