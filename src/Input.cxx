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

#include <QtGui/QtGui>

#include "Config.h"
#include "Input.h"

#define FIRST_LINE  2


Input::Input (QWidget *parent, Config *cfg) : QDialog (parent)
{
  this->setupUi (this);
  this->cfg = cfg;

  connect (this, SIGNAL (accepted ()),
           this, SLOT (writeConfig ()));

  this->origSize = this->size ();
}

Input::~Input (void)
{
  this->flush ();
}

void
Input::flush (void)
{
  int i, j;

  for (i = FIRST_LINE; i < this->gridLayout->rowCount (); i++)
    for (j = 0; j < 2; j++)
    {
      QWidget *item;
      QLayoutItem *il = this->gridLayout->itemAtPosition (i, j);

      if (!il)
        continue;

      item = il->widget ();
      if (item)
        delete item;
    }

  this->resize (this->origSize);
}

int
Input::showDialog (void)
{
  int i;
  const QString *name = NULL, *value, *label;
  QSize size;

  this->flush ();

  size = this->origSize;

  i = FIRST_LINE;
  do
  {
    this->cfg->getArgument (&name, &value, &label);
    if (name)
    {
      QLabel    *wLabel = new QLabel (this);
      QLineEdit *wValue = new QLineEdit (this);

      wLabel->setText (*label);
      wValue->setText (*value);
      wValue->setAccessibleName (*name);

      this->gridLayout->addWidget (wLabel, i, 0, 1, 1);
      this->gridLayout->addWidget (wValue, i, 1, 1, 1);

      if (i == FIRST_LINE)
        wValue->setFocus (Qt::OtherFocusReason);

      size.setHeight (size.height () + wValue->height () + 5);
      this->resize (size);
      i++;
    }
  }
  while (name);

  if (i == FIRST_LINE)
    return -1;

  this->exec ();
  return 0;
}

void
Input::writeConfig (void)
{
  int i;

  this->cfg->flushArguments ();
  for (i = FIRST_LINE; i < this->gridLayout->rowCount (); i++)
  {
    QLabel      *item1 = NULL;
    QLineEdit   *item2 = NULL;
    QLayoutItem *il;

    il = this->gridLayout->itemAtPosition (i, 0);
    if (il)
      item1 = dynamic_cast<QLabel *> (il->widget ());

    il = this->gridLayout->itemAtPosition (i, 1);
    if (il)
      item2 = dynamic_cast<QLineEdit *> (il->widget ());

    if (!item1 && !item2)
      continue;

    this->cfg->addArgument (item2 ? item2->accessibleName () : QString (""),
                            item2 ? item2->text () : QString (""),
                            item1 ? item1->text () : QString (""));

    this->cfg->rememberArgsVal
      (this->checkValues->checkState () == Qt::Checked);
  }

  this->cfg->writeConfig ();

  this->emit endInput ();
}
