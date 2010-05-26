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
  bool res = false;
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

#if _WIN32
  res = ts2.load (QString ("daisyduck_") + locale,
                  QCoreApplication::applicationDirPath ()
                  + "/../usr/share/daisyduck/ts");
#else
#ifdef DAISYDUCK_TS_PATH
  res = ts2.load (QString ("daisyduck_") + locale, QString (DAISYDUCK_TS_PATH));
#endif /* DAISYDUCK_TS_PATH */
  if (!res)
    res = ts2.load (QString ("daisyduck_") + locale,
                    QString ("/usr/local/share/daisyduck/ts"));
  if (!res)
    res = ts2.load (QString ("daisyduck_") + locale,
                    QString ("/usr/share/daisyduck/ts"));
#endif /* !_WIN32 */

  if (res)
  app.installTranslator (&ts2);

  DaisyDuck winMain;

  winMain.show ();
  return app.exec ();
}
