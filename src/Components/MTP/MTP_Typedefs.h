
/* Copyright (C) 2011-2015  Lucio Carreras
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




#ifndef MTP_TYPEDEFS_H
#define MTP_TYPEDEFS_H

#include <memory>

#include <libmtp.h>
#include <QString>
#include <QList>
#include "Helper/Logger/Logger.h"


class MTP_Device;
class MTP_File;
class MTP_Folder;
class MTP_RawDevice;
class MTP_Storage;
class MTP_Track;

typedef LIBMTP_device_entry_t			MTPIntern_DeviceEntry;
typedef LIBMTP_error_number_t			MTPIntern_ErrorNumber;
typedef LIBMTP_error_t					MTPIntern_Error;

typedef LIBMTP_mtpdevice_t				MTPIntern_Device;
typedef std::shared_ptr<MTP_Device>		MTP_DevicePtr;

typedef LIBMTP_file_t					MTPIntern_File;
typedef std::shared_ptr<MTP_File>		MTP_FilePtr;

typedef LIBMTP_folder_t					MTPIntern_Folder;
typedef std::shared_ptr<MTP_Folder>		MTP_FolderPtr;


typedef LIBMTP_raw_device_t				MTPIntern_RawDevice;
typedef std::shared_ptr<MTP_RawDevice>	MTP_RawDevicePtr;

typedef LIBMTP_devicestorage_t			MTPIntern_Storage;
typedef std::shared_ptr<MTP_Storage>	MTP_StoragePtr;

typedef LIBMTP_track_t					MTPIntern_Track;
typedef std::shared_ptr<MTP_Track>		MTP_TrackPtr;









#endif // MTP_TYPEDEFS_H
