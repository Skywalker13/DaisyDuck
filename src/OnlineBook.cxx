/*
 * DaisyDuck: tiny Daisy player for audio books.
 * Copyright (C) 2010 Mathieu Schroeter <mathieu@schroetersa.ch>
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

#include <QtWidgets/QtWidgets>
#include <QtNetwork/QtNetwork>
#include <QtXml/QDomDocument>
#include <QtCore/QCryptographicHash>

#include "common.h"
#include "Config.h"
#include "Input.h"
#include "OnlineBook.h"


OnlineBook::OnlineBook (QWidget *parent, Config *cfg) : QDialog (parent)
{
  this->setupUi (this);
  this->cfg = cfg;

  this->smilqty = 0;
  this->duck = NULL;
  this->rep = NULL;

  this->net      = new QNetworkAccessManager (this);
  this->net_ncc  = new QNetworkAccessManager (this);
  this->net_smil = new QNetworkAccessManager (this);

  this->progressDialog =
    new QProgressDialog (tr ("Downloading Daisy files ..."),
                         tr ("Cancel"), 0, 99, this);
  this->progressDialog->setAutoReset (true);
  this->progressDialog->setAutoClose (true);
  this->progressDialog->setWindowModality (Qt::WindowModal);
  this->progressDialog->findChildren <QTimer *> ()[0]->stop ();

  this->actionLoad->setEnabled (false);

  this->input = new Input (this, cfg);

  connect (progressDialog, SIGNAL (canceled ()),
           this, SLOT (progressCanceled ()));
  connect (this, SIGNAL (progressValue (int)),
           progressDialog, SLOT (setValue (int)));

  connect (actionRefresh, SIGNAL (clicked ()),
           this, SLOT (downloadList ()));
  connect (this->editUri, SIGNAL (returnPressed ()),
           this, SLOT (downloadList ()));

  connect (actionLoad, SIGNAL (clicked ()),
           this, SLOT (selectBook ()));
  connect (this, SIGNAL (actionLoad_end ()),
           this, SLOT (selectEnd ()));
  connect (actionCancel, SIGNAL (clicked ()),
           this, SLOT (selectCancel ()));
  connect (actionSettings, SIGNAL (clicked ()),
           this, SIGNAL (showSettings ()));

  connect (treeBooks, SIGNAL (itemDoubleClicked (QTreeWidgetItem *, int)),
           this, SLOT (selectBook (QTreeWidgetItem *, int)));

  connect (this, SIGNAL (downloadNcc ()),
           this, SLOT (downloadNccBegin ()));
  connect (this, SIGNAL (downloadSmil (char *, struct DaisyFiles *)),
           this, SLOT (downloadSmilBegin (char *, struct DaisyFiles *)));

  connect (this->net_ncc, SIGNAL (finished (QNetworkReply *)),
           this, SLOT (downloadNccFinished (QNetworkReply *)));
  connect (this->net_smil, SIGNAL (finished (QNetworkReply *)),
           this, SLOT (downloadSmilFinished (QNetworkReply *)));

  connect (this->input, SIGNAL (endInput ()),
           this, SLOT (inputEnded ()));
}

OnlineBook::~OnlineBook (void)
{
  this->flush ();

  delete this->net;
  delete this->net_ncc;
  delete this->net_smil;

  delete this->progressDialog;
  delete this->input;
}

void
OnlineBook::showWindow (void)
{
  this->editUri->setText (this->cfg->getUri ());
  this->list_args = this->cfg->getUriArgs ().toLatin1 ();
  this->show ();

  /*
   * We force the refresh only when the list of audio books is empty.
   * It will open the dialog box for the connection (if necessary).
   */
  if (!this->treeBooks->topLevelItemCount ()
      && !this->editUri->text ().isEmpty ())
    this->downloadList ();
}

/****************************************************************************/
/*                                                                          */
/*                              Private Slots                               */
/*                                                                          */
/****************************************************************************/

void
OnlineBook::inputEnded (void)
{
  this->list_args = this->cfg->getUriArgs ().toLatin1 ();
  this->downloadList ();
}

void
OnlineBook::downloadList (void)
{
  QUrl url (editUri->text ());
  QByteArray path;
  QNetworkRequest req (url);

  this->setEnabled (false);

  this->treeBooks->clear ();
  this->rep = this->net->post (req, this->list_args);

  connect (this->rep, SIGNAL (finished ()),
           this, SLOT (downloadFinished ()));

  this->winBar->showMessage
    (tr ("Daisy Duck is trying to load a list..."), 2000);
}

void
OnlineBook::downloadFinished (void)
{
  QByteArray   data;
  QDomDocument doc;
  QDomElement  root;
  QDomNode     node;

  if (this->rep->error () != QNetworkReply::NoError)
  {
    this->winBar->clearMessage ();
    QMessageBox::warning (this, tr ("Network"),
                          tr ("The download of the books list has failed. "
                              "Check that the URI is correct and/or your "
                              "Internet access."));
    this->rep->deleteLater ();
    this->flush ();
    return;
  }

  data = this->rep->readAll ();
  this->rep->close ();
  this->rep->deleteLater ();

  doc.setContent (data);
  root = doc.documentElement ();
  node = root.firstChild ();

  for (; !node.isNull (); node = node.nextSibling ())
  {
    QDomElement  it;
    QDomNodeList array;
    QTreeWidgetItem *item;
    QStringList extra;

    it = node.toElement ();
    array = it.childNodes ();

    if (it.tagName () == "uriparam")
    {
      /* List of parameters */
      this->uri_args = "";
      for (auto i = 0; i < array.length (); i++)
      {
        QDomElement e = array.item (i).toElement ();
        this->uri_args += (!i ? "?" : "&")
                          + e.attribute ("name") + "=" + e.attribute ("value");
      }
      continue;
    }
    else if (it.tagName () != "audiobook")
      continue;

    item = new QTreeWidgetItem ();

    for (auto i = 0; i < array.length (); i++)
    {
      unsigned int col;
      QDomNode n = array.item (i);

      if (n.nodeName () == "number")
        col = 0;
      else if (n.nodeName () == "title")
        col = 1;
      else if (n.nodeName () == "creator")
        col = 2;
      else if (n.nodeName () == "category")
        col = 3;
      /*
       * The value is prepended by the name, for example:
       *  "uri:http://www.example.com/foobar"
       *  "summary:The summary of this Audio books."
       */
      else if (n.nodeName () == "uri" || n.nodeName () == "summary")
      {
        extra << n.nodeName () + ":" + n.firstChild ().toText ().data ();
        continue;
      }
      else
        continue;

      item->setText (col, n.firstChild ().toText ().data ());
    }

    item->setData (0, Qt::UserRole, extra);
    this->treeBooks->addTopLevelItem (item);
  }

  this->winBar->clearMessage ();
  this->setEnabled (true);

  if (!treeBooks->topLevelItemCount ())
  {
    int rc;

    this->actionLoad->setEnabled (false);
    rc = this->input->showDialog ();
    if (rc)
      QMessageBox::warning (this, tr ("Network"),
                            tr ("The list of books is not reachable. Please, "
                                "check the values in the \"Settings\" dialog "
                                "box."));
  }
  else
  {
    this->actionLoad->setEnabled (true);

    /* Sort the tree by categories and by creators in every category. */
    this->treeBooks->sortItems (2, Qt::AscendingOrder); /* creator */
    this->treeBooks->sortItems (3, Qt::AscendingOrder); /* category */
  }
}

void
OnlineBook::selectBook (void)
{
  QTreeWidgetItem *item;
  QTemporaryFile tmp;
  QStringList extra;
  QStringList::const_iterator it;

  item = treeBooks->currentItem ();
  if (!item)
    return;

  this->flush ();

  extra = item->data (0, Qt::UserRole).toStringList ();
  for (it = extra.constBegin (); it != extra.constEnd (); it++)
    if ((*it).startsWith ("uri:"))
    {
      this->uri = (*it).mid (4);
      break;
    }

  this->uri_base = this->uri;
  this->uri_base.truncate (this->uri.lastIndexOf ('/'));
  this->uri_base += "/";

  this->duck = duck_init (NULL);
  if (!this->duck)
    return;

  this->progressDialog->open ();

  emit downloadNcc ();
}

void
OnlineBook::selectBook (daisy_unused QTreeWidgetItem *item,
                        daisy_unused int column)
{
  this->selectBook ();
}

void
OnlineBook::selectEnd (void)
{
  struct DaisyFiles *f;
  QString summary;
  QTreeWidgetItem *item;
  QStringList extra;
  QStringList::const_iterator it;

  item = treeBooks->currentItem ();
  if (!item)
    return;

  this->hide ();

  duck_uninit (this->duck);
  this->duck = NULL;

  if (this->daisyFiles.isEmpty ())
  {
    this->flush ();
    return;
  }

  f = this->daisyFiles.first ();

  extra = item->data (0, Qt::UserRole).toStringList ();
  for (it = extra.constBegin (); it != extra.constEnd (); it++)
    if ((*it).startsWith ("summary:"))
    {
      summary = (*it).mid (8);
      break;
    }

  emit bookPath (f->smil->fileName (), this->hash, summary);

  this->progressDialog->reset ();
}

void
OnlineBook::selectCancel (void)
{
  this->hide ();
}

void
OnlineBook::downloadNccBegin (void)
{
  QNetworkRequest req (this->uri + this->uri_args);

  this->net_ncc->get (req);
}

void
OnlineBook::downloadNccFinished (QNetworkReply *rep)
{
  QByteArray data, hash;
  QString path;
  int rc, smils, i;
  struct DaisyFiles *tmp;

  if (rep->error () != QNetworkReply::NoError)
  {
    QMessageBox::warning (this, tr ("Network"), tr ("Book download failed"));
    rep->deleteLater ();
    this->flush ();
    return;
  }

  data = rep->readAll ();
  rep->close ();
  rep->deleteLater ();

  hash = QCryptographicHash::hash (data, QCryptographicHash::Sha1).data ();
  this->hash = hash.toHex ();

  tmp          = new struct DaisyFiles;
  tmp->smil    = new QTemporaryFile ();
  tmp->smilpos = 0;
  tmp->rep     = NULL;
  tmp->smil->open ();
  tmp->smil->write (data);
  tmp->smil->close ();

  this->daisyFiles << tmp;

  path = tmp->smil->fileName ();
  /*
   * FIXME: format should not be forced here, but the autodetection relies
   *        only on the file name.
   */
  duck_load (this->duck, path.toLatin1 (), DUCK_FORMAT_NCC);

  smils = duck_smilnode_number (this->duck);

  for (i = 1; i <= smils; i++)
  {
    duck_value_t res;
    struct DaisyFiles *df;
    QFileInfo *finfo;

    duck_walk (this->duck, i, 0);
    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_I_TYPE, &res);
    if (rc)
      break;

    if (res.i != DUCK_SMILNODE_HEADING)
      continue;

    rc = duck_smilnode_getinfo (this->duck, DUCK_SMILNODE_S_ANCHOR, &res);
    if (rc)
      continue;

    df = new struct DaisyFiles;
    df->smilpos = i;
    df->smil = new QTemporaryFile ();
    /* Ensure that the temporary file is created. */
    df->smil->open ();
    df->smil->close ();

    /* Set a temporary file name for every SMIL file. */
    finfo = new QFileInfo (df->smil->fileName ());
    data.replace (res.s, finfo->fileName ().toHtmlEscaped ().toLatin1 ());
    delete finfo;
    emit downloadSmil (res.s, df);

    this->daisyFiles << df;
  }

  this->progressDialog->setMaximum (this->daisyFiles.count () - 1);

  /* Rewrite NCC file with the path replacements. */
  tmp->smil->open ();
  tmp->smil->write (data);
  tmp->smil->close ();

  duck_load (this->duck, path.toLatin1 (), DUCK_FORMAT_NCC);
}

void
OnlineBook::downloadSmilBegin (char *anchor, struct DaisyFiles *df)
{
  QNetworkRequest *req;

  req =
    new QNetworkRequest (this->uri_base + QString (anchor) + this->uri_args);
  free (anchor);
  df->rep = this->net_smil->get (*req);
  delete req;
}

void
OnlineBook::downloadSmilFinished (QNetworkReply *rep)
{
  int found = 0, rc;
  duck_value_t res;
  QByteArray data;
  QString uri;
  QList<struct DaisyFiles *>::iterator it;

  if (rep->error () != QNetworkReply::NoError)
  {
    QMessageBox::warning (this, tr ("Network"), tr ("Book download failed"));
    rep->deleteLater ();
    this->flush ();
    return;
  }

  data = rep->readAll ();
  rep->close ();
  rep->deleteLater ();

  for (it = this->daisyFiles.begin (); it != this->daisyFiles.end (); it++)
    if ((*it)->rep == rep)
    {
      found = 1;
      break;
    }

  if (!found)
    return;

  (*it)->smil->open ();
  (*it)->smil->write (data);
  (*it)->smil->close ();

  duck_walk (this->duck, (*it)->smilpos, 1);
  rc = duck_node_getinfo (this->duck, DUCK_NODE_S_AUDIO_URI, &res);
  if (rc || !res.s)
    return;

  uri = this->uri_base + QString (res.s) + this->uri_args;
  data.replace (res.s, uri.toHtmlEscaped ().toLatin1 ());
  free (res.s);

  /* Rewrite the SMIL file with the URI replacements. */
  (*it)->smil->open ();
  (*it)->smil->write (data);
  (*it)->smil->close ();

  this->smilqty++;

  /* All SMIL files handled ? */
  if (this->smilqty == this->daisyFiles.count () - 1)
  {
    emit progressValue (this->smilqty);
    emit actionLoad_end ();
  }
  else if (!(this->smilqty % 4))
    emit progressValue (this->smilqty);
}

void
OnlineBook::progressCanceled (void)
{
  this->flush ();
}

/****************************************************************************/
/*                                                                          */
/*                               Private API                                */
/*                                                                          */
/****************************************************************************/

void
OnlineBook::flush (void)
{
  while (!this->daisyFiles.isEmpty ())
  {
    struct DaisyFiles *it = this->daisyFiles.takeFirst ();
    if (it->smil)
      delete it->smil;
    delete it;
  }

  if (this->duck)
  {
    duck_uninit (this->duck);
    this->duck = NULL;
  }

  this->smilqty = 0;
  this->progressDialog->reset ();
  this->setEnabled (true);
}
