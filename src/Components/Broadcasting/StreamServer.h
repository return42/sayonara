/* StreamServer.h */

/* Copyright (C) 2011-2017  Lucio Carreras
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

#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "StreamWriter.h"
#include "Utils/Settings/SayonaraClass.h"
#include "Utils/Pimpl.h"

#include <QThread>

class QTcpSocket;
/**
 * @brief The StreamServer class. This class is listening for new connections and holds and administrates current connections.
 * @ingroup Broadcasting
 */
class StreamServer :
		public QThread,
		protected SayonaraClass
{
	Q_OBJECT
	PIMPL(StreamServer)

	signals:
		void sig_new_connection(const QString& ip);
		void sig_connection_closed(const QString& ip);
		void sig_can_listen(bool);

	public:
		explicit StreamServer(QObject* parent=nullptr);
		~StreamServer();

	private:
		// create new server and listen
		void create_server();

		// listen for connection
		bool listen_for_connection();


	protected:
		void run() override;


	public slots:

		void accept_client(QTcpSocket* socket, const QString& ip);
		void reject_client(QTcpSocket* socket, const QString& ip);
		void dismiss(int idx);

		void disconnect(StreamWriterPtr sw);
		void disconnect_all();

		void server_close();
		void stop();
		void retry();


	private slots:
		void track_changed(const MetaData&);
		void server_destroyed();

		void new_client_request();
		void disconnected(StreamWriter* sw);
		void new_connection(const QString& ip);

        void s_active_changed();
        void s_port_changed();
        void s_mp3_enc_found();
};

#endif
