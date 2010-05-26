/*
 * DaisyDuck: tiny Daisy player for audio books.
 * Copyright (C) 2010 Mathieu Schroeter <mathieu.schroeter@gamesover.ch>
 *
 * This file is part of DaisyDuck.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COMMON_H
#define COMMON_H

#define DAISY_STRINGIFY(s) #s
#define DAISY_TOSTRING(s) DAISY_STRINGIFY(s)

#define DAISY_VERSION_INT(a, b, c) (a << 16 | b << 8 | c)
#define DAISY_VERSION_DOT(a, b, c) a ##.## b ##.## c
#define DAISY_VERSION(a, b, c) DAISY_VERSION_DOT(a, b, c)

#define DAISYDUCK_VERSION_MAJOR  1
#define DAISYDUCK_VERSION_MINOR  0
#define DAISYDUCK_VERSION_MICRO  0

#define DAISYDUCK_VERSION_INT DAISY_VERSION_INT(DAISYDUCK_VERSION_MAJOR, \
                                                DAISYDUCK_VERSION_MINOR, \
                                                DAISYDUCK_VERSION_MICRO)
#define DAISYDUCK_VERSION     DAISY_VERSION(DAISYDUCK_VERSION_MAJOR, \
                                            DAISYDUCK_VERSION_MINOR, \
                                            DAISYDUCK_VERSION_MICRO)
#define DAISYDUCK_VERSION_STR DAISY_TOSTRING(DAISYDUCK_VERSION)
#define DAISYDUCK_BUILD       DAISYDUCK_VERSION_INT


#ifndef daisy_unused
#if defined(__GNUC__)
#  define daisy_unused __attribute__ ((unused))
#else
#  define daisy_unused
#endif
#endif

#define ARRAY_NB_ELEMENTS(a) (sizeof (a) / sizeof (*a))

#endif /* COMMON_H */
