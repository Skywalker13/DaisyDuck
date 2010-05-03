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

#include <cstdio>

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtCore/QFile>
#include <QtCore/QCryptographicHash>
#include <QtCore/QTime>

#include "common.h"
#include "Config.h"
#include "DaisyDuck.h"


const libvlc_event_type_t DaisyDuck::vlc_events_map[] = {
  libvlc_MediaPlayerEndReached,
  libvlc_MediaPlayerTimeChanged,
};

DaisyDuck::DaisyDuck (void)
{
  QString cfgfile, title;
  bool verbose = false;
  unsigned int vlc_argc = 0, i, ver;
  const char *vlc_argv[32];
  char useragent[256];
  libvlc_event_manager_t *ev;

  this->setupUi (this);
  this->daisyState = DAISY_IS_SLEEPING;
  this->vlc_rate = 100;
  this->labelSpeed->setText (tr ("Speed %1% :").arg (this->vlc_rate));

  title = this->windowTitle ();
  this->setWindowTitle (title + " - " DAISYDUCK_VERSION_STR);

  this->actionHelp_activated ();

  if (QCoreApplication::arguments ().contains ("-v"))
  {
    duck_verbosity (DUCK_MSG_VERBOSE);
    verbose = true;
  }
  else
    duck_verbosity (DUCK_MSG_ERROR);

  connect (actionOpen, SIGNAL (triggered ()),
           this, SLOT (actionOpen_activated ()));
  connect (actionOnlineBooks, SIGNAL (triggered ()),
           this, SLOT (actionOnlineBooks_activated ()));
  connect (actionOnlineAccess, SIGNAL (triggered ()),
           this, SLOT (actionOnlineAccess_activated ()));
  connect (actionConfigCustom, SIGNAL (triggered ()),
           this, SLOT (actionConfigCustom_activated ()));
  connect (actionPlayer, SIGNAL (triggered ()),
           this, SLOT (actionPlayer_activated ()));
  connect (actionHelp, SIGNAL (triggered ()),
           this, SLOT (actionHelp_activated ()));
  connect (actionAbout, SIGNAL (triggered ()),
           this, SLOT (actionAbout_activated ()));

  connect (treeSmilnode, SIGNAL (itemClicked (QTreeWidgetItem *, int)),
           this, SLOT (treeClicked (QTreeWidgetItem *, int)));
  connect (this, SIGNAL (treeUpdate ()),
           this, SLOT (treeSelectionUpdate ()));

  connect (pushAction, SIGNAL (clicked ()),
           this, SLOT (playerAction ()));
  connect (pushSmilnodeNext, SIGNAL (clicked ()),
           this, SLOT (playerSmilnodeNext ()));
  connect (pushSmilnodePrev, SIGNAL (clicked ()),
           this, SLOT (playerSmilnodePrev ()));
  connect (pushNodeNext, SIGNAL (clicked ()),
           this, SLOT (playerNodeNext ()));
  connect (pushNodePrev, SIGNAL (clicked ()),
           this, SLOT (playerNodePrev ()));
  connect (pushSpeedReset, SIGNAL (clicked ()),
           this, SLOT (playerSpeedReset ()));
  connect (pushSpeedUp, SIGNAL (clicked ()),
           this, SLOT (playerSpeedUp ()));
  connect (pushSpeedDown, SIGNAL (clicked ()),
           this, SLOT (playerSpeedDown ()));

  connect (this, SIGNAL (playerEvEndReached ()),
           this, SLOT (playerSmilnodeNext ()));
  connect (this, SIGNAL (playerEvTimeChanged (long)),
           this, SLOT (playerTimeChanged (long)));

  /* VideoLAN initialization */
  vlc_argv[vlc_argc++] = "--no-stats";
  vlc_argv[vlc_argc++] = "--verbose";
  if (verbose)
    vlc_argv[vlc_argc++] = "2";
  else
    vlc_argv[vlc_argc++] = "0";
  vlc_argv[vlc_argc++] = "--no-video-title-show" ;
  vlc_argv[vlc_argc++] = "--no-video";
  vlc_argv[vlc_argc++] = "--no-media-library";
  vlc_argv[vlc_argc++] = "--no-auto-preparse";

  /* ensure that audio pitch is preserved */
  vlc_argv[vlc_argc++] = "--audio-time-stretch";

  /* Network */
  vlc_argv[vlc_argc++] = "--http-reconnect";
  vlc_argv[vlc_argc++] = "--http-user-agent";

  ver = libduck_version ();
  snprintf (useragent, sizeof (useragent),
            "DaisyDuck/" DAISYDUCK_VERSION_STR " libduck/%u.%u.%u libvlc/%s",
            ver >> 16, (ver >> 8) & 0xF, ver & 0xF, libvlc_get_version ());
  vlc_argv[vlc_argc++] = useragent;

  vlc_argv[vlc_argc++] = "--ipv4-timeout";
  vlc_argv[vlc_argc++] = "500";
  vlc_argv[vlc_argc++] = "--mms-timeout";
  vlc_argv[vlc_argc++] = "500";

  this->vlc_core = libvlc_new (vlc_argc, vlc_argv);
  if (!this->vlc_core)
    exit (-1);

  this->vlc_mp = libvlc_media_player_new (this->vlc_core);
  if (!this->vlc_mp)
    exit (-1);

  ev = libvlc_media_player_event_manager (this->vlc_mp);
  if (!ev)
    exit (-1);

  for (i = 0; i < ARRAY_NB_ELEMENTS (vlc_events_map); i++)
    libvlc_event_attach (ev, vlc_events_map[i], DaisyDuck::vlc_event_cb, this);

  /* libduck initialization */
  this->duck = duck_init (NULL);
  if (!this->duck)
    exit (-1);

  /* Configuration file */
  cfgfile = Config::configPath () + Config::configDefault ();
  this->cfg = new Config (cfgfile);

  /* Online books */
  this->winOnlineBook = new OnlineBook (this, this->cfg);

  connect (this->winOnlineBook, SIGNAL (bookPath (QString, QString, QString)),
           this, SLOT (onlineBook (QString, QString, QString)));
  connect (this->winOnlineBook, SIGNAL (showSettings ()),
           this, SLOT (showOnlineAccess ()));

  /* Dialog to configure online access */
  this->dialogConfigAccess = new ConfigAccess (this, this->cfg);

  /* Dialog to configure custom informations */
  this->dialogConfigCustom = new ConfigCustom (this, this->cfg);

  /* Simple dialog box */
  this->dialogAbout = new About (this);

  this->customUpdate ();
}

DaisyDuck::~DaisyDuck (void)
{
  libvlc_media_t *media;

  if (this->winOnlineBook)
    delete this->winOnlineBook;
  if (this->dialogConfigAccess)
    delete this->dialogConfigAccess;
  if (this->dialogConfigCustom)
    delete this->dialogConfigCustom;
  if (this->dialogAbout)
    delete this->dialogAbout;

  if (this->cfg)
  {
    this->cfg->writeConfig ();
    delete this->cfg;
  }

  if (this->duck)
    duck_uninit (this->duck);

  if (this->vlc_mp)
  {
    media = libvlc_media_player_get_media (this->vlc_mp);
    if (media)
      libvlc_media_release (media);
    libvlc_media_player_release (this->vlc_mp);
  }

  if (this->vlc_core)
    libvlc_release (this->vlc_core);
}

/****************************************************************************/
/*                                                                          */
/*                              Private Slots                               */
/*                                                                          */
/****************************************************************************/

void
DaisyDuck::actionOpen_activated (void)
{
  QString file;
  QFile *fd;
  QByteArray data, hash;

  file = QFileDialog::getOpenFileName (this,
                                       tr ("Open File"), QDir::homePath (),
                                       tr ("Daisy NCC (ncc.html NCC.HTML)"));

  fd = new QFile (file);
  if (!fd->exists ())
    return;

  fd->open (QIODevice::ReadOnly);
  data = fd->readAll ();
  fd->close ();
  delete fd;

  hash = QCryptographicHash::hash (data, QCryptographicHash::Sha1).data ();
  this->hash = hash.toHex ();

  this->openBook (file, "");
}

void
DaisyDuck::actionOnlineBooks_activated (void)
{
  this->winOnlineBook->showWindow ();
}

void
DaisyDuck::actionOnlineAccess_activated (void)
{
  this->dialogConfigAccess->showDialog ();
}

void
DaisyDuck::actionConfigCustom_activated (void)
{
  this->dialogConfigCustom->showDialog ();
  this->customUpdate ();
}

void
DaisyDuck::actionPlayer_activated (void)
{
  this->stackMain->setCurrentIndex (1);
}

void
DaisyDuck::actionHelp_activated (void)
{
  this->stackMain->setCurrentIndex (0);
}

void
DaisyDuck::actionAbout_activated (void)
{
  this->dialogAbout->exec ();
}

void
DaisyDuck::treeClicked (QTreeWidgetItem *item,
                        daisy_unused int column)
{
  int rc, id;

  if (!item)
    return;

  id = item->data (0, Qt::UserRole).toInt ();
  if (!id)
    return;

  this->playerStop ();

  rc = duck_walk (this->duck, id, 1);
  if (rc)
    return;

  this->playerPlay ();
  this->daisyNodeSeek ();
}

void
DaisyDuck::treeSelectionUpdate (void)
{
  int rc;
  duck_hx_t hx;
  int *it, i = DUCK_MAX_LEVEL;
  QTreeWidgetItem *item, *item_c;

  rc = duck_getheading (this->duck, &hx);
  if (rc || !hx.h1)
    return;

  item = this->treeSmilnode->topLevelItem (hx.h1 - 1);
  if (!item)
    return;

  item_c = this->treeSmilnode->currentItem ();

  /*
   * This part considers that the structure duck_hx_t is a list of int.
   * It selects the right item accordingly to the heading numbers.
   */
  it = reinterpret_cast<int *> (&hx);
  while (*++it && --i && item)
    item = item->child (*it - 1);

  if (item && item != item_c)
    this->treeSmilnode->setCurrentItem (item);
}

void
DaisyDuck::playerAction (void)
{
  int smilpos = 0, nodepos = 0;

  this->cfg->getBookmark (this->hash, &smilpos, &nodepos);

  switch (this->daisyState)
  {
  case DAISY_IS_SLEEPING:
    if (!smilpos || !nodepos)
      this->playerPlay ();
    else
      this->playerSeek (smilpos, nodepos);
    break;

  case DAISY_IS_PLAYING:
    this->playerStop ();
    break;
  }
}

void
DaisyDuck::playerSmilnodeNext (void)
{
  int rc;

  this->playerStop ();

  rc = this->daisySmilnodeWalk (1);
  if (rc)
  {
    /* We assume that the end of this book was reached. */
    this->cfg->delBookmark (this->hash);
    return;
  }

  this->playerPlay ();
  this->selectionUpdate ();
}

void
DaisyDuck::playerSmilnodePrev (void)
{
  int rc;

  this->playerStop ();

  rc = this->daisySmilnodeWalk (-1);
  if (rc)
    return;

  this->playerPlay ();
  this->selectionUpdate ();
}

void
DaisyDuck::playerNodeNext (void)
{
  this->playerNodeNext (true);

  if (this->daisyState == DAISY_IS_SLEEPING)
    this->playerPlay ();

  this->selectionUpdate ();
}

void
DaisyDuck::playerNodePrev (void)
{
  this->playerNodePrev (true);

  if (this->daisyState == DAISY_IS_SLEEPING)
    this->playerPlay ();

  this->selectionUpdate ();
}

void
DaisyDuck::playerPlay (void)
{
  int rc;
  libvlc_media_t *media;
  duck_value_t res;
  QString location;
  QKeySequence keys;

  rc = duck_node_getinfo (this->duck, DUCK_NODE_S_AUDIO_URI, &res);
  if (rc || !res.s)
    return;

  /*
   * The specifications of Daisy 2.02 is very limited with the audio file
   * formats (PCM, MPEG Audio and ADPCM2). See:
   *   http://www.daisy.org/z3986/specifications/daisy_202.html#audioformats
   *
   * Here we are playing with the specifications. All formats and stream
   * can be handled by DaisyDuck through libvlc (like web streaming).
   */
  if (!strstr (res.s, "://") || strstr (res.s, "file://"))
    location = this->duck_path + QString (res.s); /* local file */
  else
    location = QString (res.s); /* uri */

  free (res.s);
  media = libvlc_media_new_location (this->vlc_core, location.toAscii ());
  libvlc_media_player_set_media (this->vlc_mp, media);

  this->daisyState = DAISY_IS_PLAYING;
  this->winBar->showMessage (tr ("Daisy Duck is reading..."), 2000);
  this->pushNodeNext->setEnabled (true);
  this->pushNodePrev->setEnabled (true);
  this->pushSpeedReset->setEnabled (true);
  this->pushSpeedUp->setEnabled (true);
  this->pushSpeedDown->setEnabled (true);

  keys = this->pushAction->shortcut ();
  this->pushAction->setText (tr ("Stop"));
  this->pushAction->setShortcut (keys);

  libvlc_media_player_play (this->vlc_mp);
}

void
DaisyDuck::playerStop (void)
{
  libvlc_media_t *media;
  QKeySequence keys;

  libvlc_media_player_stop (this->vlc_mp);
  media = libvlc_media_player_get_media (this->vlc_mp);
  if (media)
    libvlc_media_release (media);
  this->daisyState = DAISY_IS_SLEEPING;
  this->winBar->showMessage (tr ("Daisy Duck is sleeping..."), 2000);
  this->pushNodeNext->setEnabled (false);
  this->pushNodePrev->setEnabled (false);
  this->pushSpeedReset->setEnabled (false);
  this->pushSpeedUp->setEnabled (false);
  this->pushSpeedDown->setEnabled (false);

  keys = this->pushAction->shortcut ();
  this->pushAction->setText (tr ("Play"));
  this->pushAction->setShortcut (keys);
}

void
DaisyDuck::playerTimeChanged (long time)
{
  int rc;
  duck_value_t res_start, res_stop;

  rc = duck_node_getinfo (this->duck, DUCK_NODE_I_AUDIO_POS_START, &res_start);
  if (rc)
    return;

  rc = duck_node_getinfo (this->duck, DUCK_NODE_I_AUDIO_POS_STOP, &res_stop);
  if (rc)
    return;

  if (time > res_stop.i)
    this->playerNodeNext (false);
  else if (time < res_start.i)
    this->playerNodePrev (false);

  this->selectionUpdate ();
}

void
DaisyDuck::playerSpeedReset (void)
{
  if (!this->vlc_mp)
    return;

  this->vlc_rate = 100;
  this->playerSpeed ();
}

void
DaisyDuck::playerSpeedUp (void)
{
  if (!this->vlc_mp)
    return;

  this->vlc_rate += this->vlc_rate_inc;
  this->playerSpeed ();
}

void
DaisyDuck::playerSpeedDown (void)
{
  if (!this->vlc_mp)
    return;

  this->vlc_rate -= this->vlc_rate_inc;
  if (this->vlc_rate < 0)
    this->vlc_rate = 0;
  this->playerSpeed ();
}

void
DaisyDuck::onlineBook (QString book, QString hash, QString summary)
{
  this->hash = hash;
  this->openBook (book, summary);
}

void
DaisyDuck::showOnlineAccess (void)
{
  this->winOnlineBook->hide ();
  this->dialogConfigAccess->showDialog ();
  this->winOnlineBook->showWindow ();
}

/****************************************************************************/
/*                                                                          */
/*                               Private API                                */
/*                                                                          */
/****************************************************************************/

int
DaisyDuck::daisySmilnodeWalk (int inc)
{
  int rc;
  int smilpos = 0;

  duck_getpos (this->duck, &smilpos, NULL);
  while (!(rc = duck_walk (this->duck, smilpos += inc, 1)))
  {
    duck_value_t res;

    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_I_TYPE, &res);
    if (rc || res.i != DUCK_SMILNODE_HEADING)
      continue;
    break;
  }

  return rc;
}

int
DaisyDuck::daisyNodeWalk (int inc)
{
  int rc;
  int nodepos = 0;

  duck_getpos (this->duck, NULL, &nodepos);
  if (nodepos + inc < 1)
    return -1;

  rc = duck_walk (this->duck, 0, nodepos + inc);
  return rc;
}

void
DaisyDuck::daisyNodeSeek (void)
{
  int rc;
  duck_value_t res;

  rc = duck_node_getinfo (this->duck, DUCK_NODE_I_AUDIO_POS_START, &res);
  if (rc)
    return;

  libvlc_media_player_set_time (this->vlc_mp, res.i);
}

void
DaisyDuck::playerNodeNext (bool check_time)
{
  int rc;
  duck_value_t res;

  /* sync needed ? */
  if (check_time)
  {
    libvlc_time_t time;

    rc = duck_node_getinfo (this->duck, DUCK_NODE_I_AUDIO_POS_STOP, &res);
    if (rc)
      return;

    time = libvlc_media_player_get_time (this->vlc_mp);
    if (time > res.i)
      this->daisyNodeWalk (1); /* set the current node */
  }

  rc = this->daisyNodeWalk (1);
  if (rc) /* end node reached, next Smil */
    this->playerSmilnodeNext ();

  if (check_time)
    this->daisyNodeSeek ();
}

void
DaisyDuck::playerNodePrev (bool check_time)
{
  int rc;
  int smilpos, nodepos;
  duck_value_t res;

  /* sync needed ? */
  if (check_time)
  {
    libvlc_time_t time;

    rc = duck_node_getinfo (this->duck, DUCK_NODE_I_AUDIO_POS_START, &res);
    if (rc)
      return;

    time = libvlc_media_player_get_time (this->vlc_mp);
    if (time < res.i)
      this->daisyNodeWalk (-1); /* set the current node */
  }

  rc = duck_getpos (this->duck, &smilpos, &nodepos);
  if (rc)
    return;

  if (smilpos > 1 || (smilpos == 1 && nodepos > 1))
    rc = this->daisyNodeWalk (-1);

  if (rc) /* first node reached, prev Smil (last node) */
  {
    int i = 1;

    this->playerStop ();

    rc = this->daisySmilnodeWalk (-1);
    if (rc)
      return;

    while (!(rc = duck_walk (this->duck, 0, i)))
      i++;

    this->playerPlay ();
  }

  if (check_time)
    this->daisyNodeSeek ();
}

void
DaisyDuck::playerSeek (int smilpos, int nodepos)
{
  int rc;

  this->playerStop ();

  rc = duck_walk (this->duck, smilpos, nodepos);
  if (rc)
    return;

  this->playerPlay ();
  this->daisyNodeSeek ();
}

inline void
DaisyDuck::playerSpeed (void)
{
  this->labelSpeed->setText (tr ("Speed %1% :").arg (this->vlc_rate));
  libvlc_media_player_set_rate (this->vlc_mp, this->vlc_rate / 100.0);
}

void
DaisyDuck::selectionUpdate (void)
{
  int smilpos = 0, nodepos = 0;

  duck_getpos (this->duck, &smilpos, &nodepos);
  this->cfg->setBookmark (this->hash, smilpos, nodepos);

  emit this->treeUpdate ();
}

void
DaisyDuck::customUpdate (void)
{
  const QString *title, *text;

  this->cfg->getCustom (&title, &text);
  this->labelCustomTitle->setText (*title);
  this->labelCustomText->setText (*text);
}

#define BOOK_SETLABEL_INFO(l, e)                  \
  rc = duck_book_getinfo (this->duck, e, &res);   \
  if (!rc || !res.s)                              \
  {                                               \
    this->l->setText (res.s);                     \
    free (res.s);                                 \
  }                                               \
  else                                            \
    this->l->setText (tr ("Unknown"));

void
DaisyDuck::openBook (QString book, QString summary)
{
  int first = 1, level_p = 1;
  int rc, i, smils, child;
  int smilpos = 0, nodepos = 0;
  duck_value_t res;
  QFileInfo *finfo;
  QTreeWidgetItem *item[DUCK_MAX_LEVEL] = { NULL };

  if (book.isNull ())
    return;

  finfo = new QFileInfo (book);
  if (!finfo)
    return;

  this->duck_path = finfo->absolutePath () + "/";
  delete finfo;

  if (this->daisyState == DAISY_IS_PLAYING)
    this->playerStop ();

  rc = duck_load (this->duck, book.toAscii (), DUCK_FORMAT_NCC);
  if (rc)
  {
    if (rc < 0)
    {
      QMessageBox::critical (this, tr ("Parsing"),
                             tr ("This audio book can not be loaded by "
                                 "DaisyDuck."));
      return;
    }

    QMessageBox::warning (this, tr ("Parsing"),
                          tr ("The parsing has failed somewhere! Probably that "
                              "this audio book will not be fully readable."));
  }

  /* flush */
  treeSmilnode->clear ();

  BOOK_SETLABEL_INFO (labelBook,      DUCK_BOOK_S_TITLE)
  BOOK_SETLABEL_INFO (labelAuthor,    DUCK_BOOK_S_AUTHOR)
  BOOK_SETLABEL_INFO (labelNarrator,  DUCK_BOOK_S_NARRATOR)

  rc = duck_book_getinfo (this->duck, DUCK_BOOK_I_DURATION, &res);
  if (!rc)
  {
    QTime time = QTime (0, 0, 0, 0);
    time = time.addSecs (res.i);
    this->labelDuration->setText
      (time.toString (tr ("H 'hours' m 'minutes' s 'seconds'")));
  }
  else
    this->labelDuration->setText (tr ("Unknown"));

  if (summary.isEmpty ())
    summary = tr ("Not available");

  this->labelSummary->setText (summary);

  /* fill tree */
  child = 0;
  smils = duck_smilnode_number (this->duck);
  for (i = 1; i <= smils; i++)
  {
    int level, id;

    duck_walk (this->duck, i, 0);
    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_I_TYPE, &res);
    if (rc)
      return;

    if (res.i != DUCK_SMILNODE_HEADING)
      continue;

    if (!first)
      first = i;

    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_I_LEVEL, &res);
    if (rc || res.i > DUCK_MAX_LEVEL)
      return;
    level = res.i;

    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_I_INDEX, &res);
    if (rc)
      continue;
    id = res.i;

    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_S_HEADER, &res);
    if (rc)
      continue;

    if (!res.s)
      res.s = strdup (tr ("Undefined").toAscii ());

    if (level == 1)
    {
      item[0] = new QTreeWidgetItem ();
      item[0]->setText (0, QString::fromUtf8 (res.s, -1));
      item[0]->setData (0, Qt::UserRole, id);
      treeSmilnode->addTopLevelItem (item[0]);
      child = 0;
    }
    else
    {
      QTreeWidgetItem *itemc = new QTreeWidgetItem ();
      itemc->setText (0, QString::fromUtf8 (res.s, -1));
      itemc->setData (0, Qt::UserRole, id);
      item[level - 2]->addChild (itemc);

      if (!item[level - 1] || level_p >= level)
        item[level - 1] = itemc;
    }

    free (res.s);
    level_p = level;
  }

  /* got to the first heading */
  duck_walk (this->duck, first, 1);

  this->cfg->getBookmark (this->hash, &smilpos, &nodepos);
  if ((smilpos > 1 && nodepos) || (smilpos == 1 && nodepos > 1))
    this->winBar->showMessage (tr ("This book was interrupted, press "
                                   "\"Play\" to resume."));

  this->actionPlayer_activated ();
  this->playerSpeedReset ();
}

void
DaisyDuck::vlc_event_cb (const libvlc_event_t *ev, void *data)
{
  libvlc_event_type_t type;
  DaisyDuck *daisy = reinterpret_cast<DaisyDuck *> (data);

  if (!ev || !daisy)
    return;

  type = ev->type;
  switch (type)
  {
  case libvlc_MediaPlayerEndReached:
    daisy->emit playerEvEndReached ();
    break;

  case libvlc_MediaPlayerTimeChanged:
    daisy->emit playerEvTimeChanged (ev->u.media_player_time_changed.new_time);
    break;

  default:
    break;
  }
}
