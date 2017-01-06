/* MTP.h */

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



#ifndef MTP_H
#define MTP_H

#include "MTP_Device.h"
#include "MTP_File.h"
#include "MTP_Folder.h"
#include "MTP_RawDevice.h"
#include "MTP_Storage.h"
#include "MTP_Track.h"

#include <QThread>
#include <QList>

/**
 * @brief The MTP class
 * @ingroup MTP
 */
class MTP : public QThread
{
	Q_OBJECT

public:

	explicit MTP(QObject* parent=nullptr);
	virtual ~MTP();

	QList<MTP_RawDevicePtr>		get_raw_devices();

private:

	void run_raw_devices();

	void run() override;

	QList<MTP_RawDevicePtr>	_raw_devices;
	bool					_mtp_initialized;
};

#endif // MTP_H
