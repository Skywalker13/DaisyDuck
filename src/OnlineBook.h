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

#ifndef ONLINEBOOK_H
#define ONLINEBOOK_H

#include <duck.h>

#include "dialogOnlineBook.h"

class QNetworkAccessManager;
class QNetworkReply;
class QTemporaryFile;
class QProgressDialog;
class Config;
class Input;

struct DaisyFiles {
  QNetworkReply  *rep;
  QTemporaryFile *smil;
  int             smilpos;
};

class OnlineBook : public QDialog, private Ui::dialogOnlineBook
{
  Q_OBJECT

  signals:
    void showSettings (void);
    void bookPath (QString path, QString hash, QString summary);
    void actionLoad_end (void);
    void downloadNcc (void);
    void downloadSmil (char *anchor, struct DaisyFiles *df);
    void progressValue (int value);

  public:
    OnlineBook (QWidget *parent, Config *cfg);
    ~OnlineBook (void);

    void showWindow (void);

  private slots:
    void inputEnded (void);

    void downloadList (void);
    void downloadFinished (void);
    void selectBook (void);
    void selectBook (QTreeWidgetItem *item, int column);
    void selectEnd (void);
    void selectCancel (void);

    void downloadNccBegin (void);
    void downloadNccFinished (QNetworkReply *rep);
    void downloadSmilBegin (char *anchor, struct DaisyFiles *df);
    void downloadSmilFinished (QNetworkReply *rep);

    void progressCanceled (void);

  private:
    void flush (void);

    Config *cfg;
    Input *input;

    QProgressDialog *progressDialog;

    QNetworkAccessManager *net;
    QNetworkAccessManager *net_ncc;
    QNetworkAccessManager *net_smil;
    QNetworkReply *rep;

    QList<struct DaisyFiles *> daisyFiles;
    int smilqty;

    /* Current selected book */
    QString uri;        /* URI on the NCC file          */
    QString uri_base;   /* Base of the NCC uri          */
    QString uri_args;   /* Arguments for POST           */
    QString hash;       /* SHA1 of the NCC file         */

    QByteArray list_args; /* Arguments for the daisyduck book list. */

    /*
     * The use of libduck is done for the NCC parsing, in order to retrieve
     * the list of all SMIL files.
     */
    duck_t *duck;
};

#endif /* ONLINEBOOK_H */
