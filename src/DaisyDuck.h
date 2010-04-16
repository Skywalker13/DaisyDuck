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

#ifndef DAISYDUCK_H
#define DAISYDUCK_H

#include <duck.h>
#include <vlc/vlc.h>

#include "OnlineBook.h"
#include "ConfigAccess.h"

#include "winMain.h"
#include "dialogAbout.h"

class Config;

class About : public QDialog, private Ui::dialogAbout
{
  Q_OBJECT

  public:
    About (QWidget *parent) : QDialog (parent)
    {
      this->setupUi (this);
    }
};

class DaisyDuck : public QMainWindow, private Ui::winMain
{
  Q_OBJECT

  signals:
    void playerEvEndReached (void);
    void playerEvTimeChanged (long time);
    void treeUpdate (void);

  public:
    DaisyDuck (void);
    ~DaisyDuck (void);

  private slots:
    /* Menu bar and buttons */
    void actionOpen_activated (void);
    void actionOnlineBooks_activated (void);
    void actionOnlineAccess_activated (void);
    void actionPlayer_activated (void);
    void actionHelp_activated (void);
    void actionAbout_activated (void);

    /* Tree smilnode */
    void treeClicked (QTreeWidgetItem *item, int column);
    void treeSelectionUpdate (void);

    /* Player controls */
    void playerAction (void);
    void playerSmilnodeNext (void);
    void playerSmilnodePrev (void);
    void playerNodeNext (void);
    void playerNodePrev (void);
    void playerPlay (void);
    void playerStop (void);
    void playerTimeChanged (long time);

    /* Online Books */
    void onlineBook (QString book, QString hash, QString summary);
    void showOnlineAccess (void);

  private:
    int daisySmilnodeWalk (int inc);
    int daisyNodeWalk (int inc);
    void daisyNodeSeek (void);

    void playerNodeNext (bool check_time);
    void playerNodePrev (bool check_time);
    void playerSeek (int smilpos, int smilnode);

    void selectionUpdate (void);
    void openBook (QString book, QString summary);

    static void vlc_event_cb (const libvlc_event_t *ev, void *data);

    Config       *cfg;
    QString       hash;
    OnlineBook   *winOnlineBook;
    ConfigAccess *dialogConfig;
    About        *dialogAbout;
    static const libvlc_event_type_t vlc_events_map[];

    enum {
      DAISY_IS_SLEEPING = 0,
      DAISY_IS_PLAYING,
    } daisyState;

    /* libduck */
    duck_t  *duck;
    QString  duck_path;

    /* libvlc */
    libvlc_instance_t     *vlc_core;
    libvlc_media_player_t *vlc_mp;
};

#endif /* DAISYDUCK_H */
