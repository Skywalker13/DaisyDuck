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

#ifndef CONFIG_H
#define CONFIG_H

#include <QtCore/QList>
#include <QtCore/QString>

class QDomElement;

struct UriArgs {
  QString name;
  QString value;
  QString label;
};

struct Bookmark {
  QString hash;   /* SHA1 of the NCC file  */
  int smilpos;
  int nodepos;
};

class Config
{
  public:
    Config (QString cfg);
    ~Config (void);

    /* URI + Arguments handling */
    QString getUri (void);
    QString getUriArgs (void);
    void getArgument (const QString **name,
                      const QString **value,
                      const QString **label);
    void flushArguments (void);
    void addArgument (QString name, QString value, QString label);
    void rememberArgsVal (bool remember);
    void setUri (QString uri);

    /* Bookmarks handling */
    void setBookmark (QString *hash, int smilpos, int nodepos);
    void getBookmark (QString *hash, int *smilpos, int *nodepos);
    void delBookmark (QString *hash);

    /* Custom items */
    void setCustom (QString title, QString text);
    void getCustom (const QString **title, const QString **text);

    void writeConfig (void);

    static QString configPath (void);
    static QString configDefault (void);

  private:
    void parseCustom (const QDomElement *item);
    void parseOnlinebook (const QDomElement *item);
    void parseBookmark (const QDomElement *item);

    bool remember;
    QString cfg;
    QString uri;
    QString title;
    QString text;
    QList<struct UriArgs *> uriArgs;
    QList<struct Bookmark *> listBookmark;
};

#endif /* CONFIG_H */
