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

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtXml/QDomDocument>

#include "Config.h"


Config::Config (QString cfg)
{
  QDomDocument doc;
  QDomElement  root;
  QDomNode     node;
  QFile file (cfg);

  this->cfg = cfg;

  if (!file.open (QIODevice::ReadOnly))
  {
    this->writeConfig ();
    return;
  }

  if (!doc.setContent (&file))
  {
    file.close ();
    return;
  }
  file.close ();

  root = doc.documentElement ();
  node = root.firstChild ();

  for (; !node.isNull (); node = node.nextSibling ())
  {
    QDomElement it;

    it = node.toElement ();

    if (it.tagName () == "custom")
      this->parseCustom (&it);
    else if (it.tagName () == "onlinebook")
      this->parseOnlinebook (&it);
    else if (it.tagName () == "bookmark")
      this->parseBookmark (&it);
  }
}

Config::~Config (void)
{
  this->flushArguments ();

  while (!this->listBookmark.isEmpty ())
    delete this->listBookmark.takeFirst ();
}

QString
Config::getUri (void)
{
  return this->uri;
}

QString
Config::getUriArgs (void)
{
  QString args;
  QList<struct UriArgs *>::iterator it;

  for (it = this->uriArgs.begin (); it != this->uriArgs.end ();)
  {
    args += (*it)->name + "=" + (*it)->value;
    if (++it != this->uriArgs.end ())
      args += "&";
  }

  return args;
}

void
Config::getArgument (const QString **name,
                     const QString **value,
                     const QString **label)
{
  QList<struct UriArgs *>::iterator it;

  for (it = this->uriArgs.begin (); it != this->uriArgs.end (); it++)
  {
    if (!*name)
    {
      *name  = &(*it)->name;
      *value = &(*it)->value;
      *label = &(*it)->label;
      return;
    }

    /* return the next */
    if (*name == &(*it)->name)
    {
      *name = NULL;
      continue;
    }
  }

  *name  = NULL;
  *value = NULL;
  *label = NULL;
}

void
Config::flushArguments (void)
{
  while (!this->uriArgs.isEmpty ())
    delete this->uriArgs.takeFirst ();
}

void
Config::addArgument (QString name, QString value, QString label)
{
  struct UriArgs *args = new struct UriArgs;

  if (name.isEmpty ())
    return;

  args->name  = name;
  args->value = value;
  args->label = label;
  this->uriArgs << args;
}

void
Config::rememberArgsVal (bool remember)
{
  this->remember = remember;
}

void
Config::setUri (QString uri)
{
  this->uri = uri;
}

void
Config::setBookmark (QString hash, int smilpos, int nodepos)
{
  struct Bookmark *bm;
  QList<struct Bookmark *>::iterator it;

  for (it = this->listBookmark.begin (); it != this->listBookmark.end (); it++)
    if ((*it)->hash == hash)
    {
      (*it)->smilpos = smilpos;
      (*it)->nodepos = nodepos;
      return;
    }

  /* nothing found, then new bookmark */
  bm = new struct Bookmark;
  bm->hash    = hash;
  bm->smilpos = smilpos;
  bm->nodepos = nodepos;
  this->listBookmark << bm;
}

void
Config::getBookmark (QString hash, int *smilpos, int *nodepos)
{
  QList<struct Bookmark *>::iterator it;

  for (it = this->listBookmark.begin (); it != this->listBookmark.end (); it++)
    if ((*it)->hash == hash)
    {
      *smilpos = (*it)->smilpos;
      *nodepos = (*it)->nodepos;
      return;
    }

  *smilpos = 0;
  *nodepos = 0;
}

void
Config::delBookmark (QString hash)
{
  unsigned int i = 0;
  QList<struct Bookmark *>::iterator it;

  for (it  = this->listBookmark.begin ();
       it != this->listBookmark.end (); it++, i++)
    if ((*it)->hash == hash)
    {
      delete this->listBookmark.takeAt (i);
      break;
    }
}

void
Config::setCustom (QString title, QString text)
{
  this->title = title;
  this->text  = text;
}

void
Config::getCustom (const QString **title, const QString **text)
{
  *title = &this->title;
  *text  = &this->text;
}

#define APPEND_DOMTEXT(s)                         \
  if (!this->s.isEmpty ())                        \
  {                                               \
    QDomText text = doc.createTextNode (this->s); \
    stag = doc.createElement (#s);                \
    stag.appendChild (text);                      \
    tag.appendChild (stag);                       \
  }

void
Config::writeConfig (void)
{
  QDomDocument doc ("daisyduckcfg");
  QDomElement root;
  QDomElement tag, stag;
  QFile file (this->cfg);
  QList<struct UriArgs *>::iterator it;

  root = doc.createElement ("daisyduckcfg");
  doc.appendChild (root);

  tag = doc.createElement ("custom");
  root.appendChild (tag);

  APPEND_DOMTEXT (title)
  APPEND_DOMTEXT (text)

  tag = doc.createElement ("onlinebook");
  root.appendChild (tag);

  APPEND_DOMTEXT (uri)

  stag = doc.createElement ("uriparam");
  for (it = this->uriArgs.begin (); it != this->uriArgs.end (); it++)
  {
    QDomElement t;

    if ((*it)->name.isEmpty ())
      continue;

    t = doc.createElement ("param");
    t.setAttribute ("name", (*it)->name);
    t.setAttribute ("value", this->remember ? (*it)->value : "");
    t.setAttribute ("label", (*it)->label);
    stag.appendChild (t);
  }
  tag.appendChild (stag);

  /* Bookmarks */
  if (!this->listBookmark.isEmpty ())
  {
    QList<struct Bookmark *>::iterator it;

    for (it  = this->listBookmark.begin ();
         it != this->listBookmark.end (); it++)
    {
      QDomText text;

      if ((*it)->smilpos == 1 && (*it)->nodepos == 1)
        continue;

      tag = doc.createElement ("bookmark");
      text = doc.createTextNode ((*it)->hash);

      stag = doc.createElement ("hash");
      stag.appendChild (text);
      tag.appendChild (stag);

      stag = doc.createElement ("position");
      stag.setAttribute ("smil", (*it)->smilpos);
      stag.setAttribute ("node", (*it)->nodepos);
      tag.appendChild (stag);

      root.appendChild (tag);
    }
  }

  file.open (QIODevice::WriteOnly);
  file.write (doc.toByteArray ());
  file.close ();
}

QString
Config::configPath (void)
{
  QString path;
  QDir dir;
#ifdef _WIN32
  const char *appdata = getenv ("APPDATA");

  if (!appdata || !dir.cd (appdata))
    return ".\\";

  path = "daisyduck";
  if (!dir.exists (path)
      && !dir.mkpath (path))
    return ".\\";

  path = QString (appdata) + "\\" + path + "\\";
#else
  if (!dir.cd (QDir::homePath ()))
    return "./";

  path = ".config/daisyduck";
  if (!dir.exists (path)
      && !dir.mkpath (path))
    return "./";

  path = QDir::homePath () + "/" + path + "/";
#endif /* !_WIN32 */
  return path;
}

QString
Config::configDefault (void)
{
  return QString ("daisyduck.conf");
}

/****************************************************************************/
/*                                                                          */
/*                               Private API                                */
/*                                                                          */
/****************************************************************************/

void
Config::parseCustom (const QDomElement *item)
{
  bool title_done = false, text_done = false;
  QDomNode node;

  node = item->firstChild ();

  for (; !node.isNull (); node = node.nextSibling ())
  {
    QDomElement it;

    it = node.toElement ();

    if (!title_done && it.tagName () == "title")
    {
      this->title = it.text ();
      title_done = true;
    }
    else if (!text_done && it.tagName () == "text")
    {
      this->text = it.text ();
      text_done = true;
    }

    if (title_done && text_done)
      break;
  }
}

void
Config::parseOnlinebook (const QDomElement *item)
{
  bool uri_done = false, uriparam_done = false;
  QDomNode node;

  node = item->firstChild ();

  for (; !node.isNull (); node = node.nextSibling ())
  {
    QDomElement it;

    it = node.toElement ();

    if (!uriparam_done && it.tagName () == "uriparam")
    {
      unsigned int i;
      QDomNodeList array;

      array = it.childNodes ();

      for (i = 0; i < array.length (); i++)
      {
        QDomNode n = array.item (i);
        QDomElement item = n.toElement ();
        this->addArgument (item.attribute ("name"),
                           item.attribute ("value"),
                           item.attribute ("label"));
      }

      uriparam_done = true;
    }
    else if (!uri_done && it.tagName () == "uri")
    {
      this->uri = it.text ();
      uri_done = true;
    }

    if (uriparam_done && uri_done)
      break;
  }
}

void
Config::parseBookmark (const QDomElement *item)
{
  QDomNode node;
  QString hash;
  int smilpos = 0, nodepos = 0;

  node = item->firstChild ();

  for (; !node.isNull (); node = node.nextSibling ())
  {
    QDomElement it;

    it = node.toElement ();

    if (it.tagName () == "position")
    {
      smilpos = it.attribute ("smil").toInt ();
      nodepos = it.attribute ("node").toInt ();
    }
    else if (it.tagName () == "hash")
      hash = it.text ();
  }

  if (!smilpos || !nodepos)
    return;

  this->setBookmark (hash, smilpos, nodepos);
}
