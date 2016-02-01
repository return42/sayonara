/* StreamWriter.h */

/* Copyright (C) 2011-2016  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STREAM_WRITER_H
#define STREAM_WRITER_H

#include "StreamDataSender.h"
#include "StreamHttpParser.h"

#include "Helper/MetaData/MetaData.h"
#include "Helper/SayonaraClass.h"
#include "Interfaces/RawSoundReceiver/RawSoundReceiverInterface.h"

#include <QByteArray>
#include <QHostAddress>
#include <memory>

class EngineHandler;
class StreamWriter :
		public QObject,
		public RawSoundReceiverInterface,
		protected SayonaraClass

{
	Q_OBJECT

public:

	enum class Type : quint8 {
		Undefined,
		Standard,
		Invalid,
		Streaming
	};

	signals:
		void sig_new_connection(const QString& ip);
		void sig_disconnected(StreamWriter* sw);


	private:
		EngineHandler*		_engine=nullptr;
		StreamHttpParser*	_parser=nullptr;
		StreamDataSender*	_sender=nullptr;
		QTcpSocket*			_socket=nullptr;

		bool				_dismissed; // after that, only trash will be sent
		bool				_send_data; // after that, no data at all will be sent

		QString				_stream_title;
		QString				_ip;

		StreamWriter::Type	_type;

		void reset();


	public:

		StreamWriter(QTcpSocket* socket, const QString& ip, const MetaData& md);
		virtual ~StreamWriter();

		// get ip address
		QString get_ip() const;

		// get raw socket descriptor
		int get_sd() const;
		Type get_type() const;

		void change_track(const MetaData& md);


		bool send_playlist();
		bool send_favicon();
		bool send_metadata();
		bool send_bg();
		bool send_html5();
		bool send_header(bool reject);

		StreamHttpParser::HttpAnswer parse_message();

		void disconnect();
		void dismiss();

		void new_audio_data(const uchar* data, quint64 size) override;


	private slots:

		void socket_disconnected();
		void data_available();
};

typedef std::shared_ptr<StreamWriter> StreamWriterPtr;
#endif
