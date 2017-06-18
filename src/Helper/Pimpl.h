/* Pimpl.h */

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

#pragma once

#ifndef SAYONARA_PIMPL_H_
#define SAYONARA_PIMPL_H_

#include <memory>

#define PIMPL(Class) private: \
    struct Private; \
    friend struct Private; \
    std::unique_ptr< Class ::Private> _m; \
    private:

/**
 * ingroup @Helper
 */
namespace Pimpl
{
    template<typename T, typename... Args>
    /**
     * @brief make create an instance of the ::Private structure
     * @param args arguments, the ::Private constructor needs
     * @return Unique Pointer to the ::Private structure
     */
    std::unique_ptr<T> make(Args&&... args)
    {
		return std::unique_ptr<T>(
				new T(std::forward<Args>(args)...)
		);
    }
}

#endif // SAYONARA_PIMPL_H_
