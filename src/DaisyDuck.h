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

#ifndef DAISYDUCK_H
#define DAISYDUCK_H

#include <duck.h>
#include <vlc/vlc.h>

#include "OnlineBook.h"
#include "ConfigAccess.h"
#include "ConfigCustom.h"

#include "winMain.h"
#include "dialogAbout.h"

class Config;
class QShortcut;

class About : public QDialog, private Ui::dialogAbout
{
  Q_OBJECT

  public:
    About (QWidget *parent) : QDialog (parent)
    {
      unsigned int v;
      QString deps;

      this->setupUi (this);

      v = libduck_version ();
      deps  = QString ("   libduck-%1.%2.%3\n")
              .arg (v >> 16).arg ((v >> 8) & 0xF).arg (v & 0xF);
      deps += "   libvlc-" + QString (libvlc_get_version ()) + "\n";
      deps += "   libqt4-" + QString (qVersion ());
      this->labelDeps->setText (deps);
    }
};

class DaisyDuck : public QMainWindow, private Ui::winMain
{
  Q_OBJECT

  signals:
    void playerEvEndReached (void);
    void playerEvTimeChanged (long time);
    void playerEvError (void);
    void treeUpdate (void);

  public:
    DaisyDuck (void);
    ~DaisyDuck (void);

  private slots:
    /* Menu bar and buttons */
    void actionOpen_activated (void);
    void actionOnlineBooks_activated (void);
    void actionOnlineAccess_activated (void);
    void actionConfigCustom_activated (void);
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
    void playerSpeedReset (void);
    void playerSpeedUp (void);
    void playerSpeedDown (void);
    void playerVolume (int value);
    void playerError (void);

    /* Online Books */
    void onlineBook (const QString &book,
                     const QString &hash,
                     const QString &summary);
    void showOnlineAccess (void);

    /* Other */
    void actionVolumeUp (void);
    void actionVolumeDn (void);

  private:
    int daisySmilnodeWalk (int inc);
    int daisyNodeWalk (int inc);
    void daisyNodeSeek (void);

    void playerNodeNext (bool check_time);
    void playerNodePrev (bool check_time);
    void playerSeek (int smilpos, int smilnode);
    inline void playerSpeed (void);

    void selectionUpdate (void);
    void customUpdate (void);
    void openBook (const QString &book, const QString &summary);

    static void vlc_event_cb (const libvlc_event_t *ev, void *data);

    Config       *cfg;
    QString       hash;
    OnlineBook   *winOnlineBook;
    ConfigAccess *dialogConfigAccess;
    ConfigCustom *dialogConfigCustom;
    About        *dialogAbout;
    QShortcut    *shortcutVolumeUp;
    QShortcut    *shortcutVolumeDn;
    static const libvlc_event_type_t vlc_events_map[];

    enum {
      DAISY_IS_SLEEPING = 0,
      DAISY_IS_PLAYING,
    } daisyState;

    /* libduck */
    duck_t  *duck;
    QString  duck_path;
    int      duck_duration;

    /* libvlc */
    libvlc_instance_t     *vlc_core;
    libvlc_media_player_t *vlc_mp;
    int vlc_rate;
    int vlc_volume;
    static const int vlc_rate_inc = 10;
    static const int vlc_rate_min = 50;
};

#endif /* DAISYDUCK_H */
