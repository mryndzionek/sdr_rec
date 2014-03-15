/*  =========================================================================
    Copyright (c) 2013 Mariusz Ryndzionek - mryndzionek@gmail.com

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.
    =========================================================================
 */

#ifndef __DEBUG_H_INCLUDED__
#define __DEBUG_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H_INCLUDED__ */
