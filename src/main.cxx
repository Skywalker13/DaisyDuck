/*
 * DaisyDuck: tiny Daisy player for audio books.
 * Copyright (C) 2010 Mathieu Schroeter <mathieu.schroeter@gamesover.ch>
 *
 * This file is part of DaisyDuck.
 *
 * DaisyDuck is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * DaisyDuck is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with DaisyDuck; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <iostream>

#include <QtGui/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QLibraryInfo>

#include "DaisyDuck.h"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#endif /* Q_WS_X11 */


int
main (int argc, char **argv)
{
  /*
   * This hack is needed to prevent races between Qt and libVLC. According
   * to the fact that Qt is initialized before VLC, and VLC uses XInitThreads
   * internally, it can have unexpected behaviours. With Qt 4.7.x,
   * XInitThreads() will be called by QApplication.
   */
#ifdef Q_WS_X11
  XInitThreads ();
#endif /* Q_WS_X11 */
  int rc;
  DaisyDuck *winMain;
  QApplication app (argc, argv);
  QString locale = QLocale::system ().name ();
  QTranslator ts1, ts2;

  if (QCoreApplication::arguments ().contains ("-h"))
  {
    std::cout << "DaisyDuck\n\n"
                 "Usage: daisyduck [options...]\n\n"
                 "Options:\n"
                 " -h     this help\n"
                 " -v     verbose mode\n"
                 "\n";
    return 0;
  }

  ts1.load (QString ("qt_") + locale,
                   QLibraryInfo::location (QLibraryInfo::TranslationsPath));
  app.installTranslator (&ts1);

  ts2.load (QString ("daisyduck_") + locale, DAISYDUCK_TS_PATH);
  app.installTranslator (&ts2);

  winMain = new DaisyDuck ();
  if (!winMain)
    return -1;

  winMain->show ();
  rc = app.exec ();

  delete winMain;
  return rc;
}
