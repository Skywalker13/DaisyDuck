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

#include <QtGui/QtGui>

#include "Config.h"
#include "Input.h"

/* First line in the dialog gridLayout where are put the widgets. */
#define FIRST_LINE  2

class LineEdit : public QLineEdit
{
  public:
    LineEdit (QWidget *parent) : QLineEdit (parent) {};
    QString name;
};


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

  /* Delete all widgets dynamically added. */
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
      LineEdit  *wValue = new LineEdit (this);

      wLabel->setText (*label);
      wValue->setText (*value);
      wValue->name = *name;

      this->gridLayout->addWidget (wLabel, i, 0, 1, 1);
      this->gridLayout->addWidget (wValue, i, 1, 1, 1);

      if (i == FIRST_LINE)
        wValue->setFocus (Qt::OtherFocusReason);

      /* Resize the dialog to fit with the new widgets. */
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

/****************************************************************************/
/*                                                                          */
/*                              Private Slots                               */
/*                                                                          */
/****************************************************************************/

void
Input::writeConfig (void)
{
  int i;

  this->cfg->flushArguments ();
  for (i = FIRST_LINE; i < this->gridLayout->rowCount (); i++)
  {
    QLabel      *item1 = NULL;
    LineEdit    *item2 = NULL;
    QLayoutItem *il;

    il = this->gridLayout->itemAtPosition (i, 0);
    if (il)
      item1 = dynamic_cast<QLabel *> (il->widget ());

    il = this->gridLayout->itemAtPosition (i, 1);
    if (il)
      item2 = dynamic_cast<LineEdit *> (il->widget ());

    if (!item1 && !item2)
      continue;

    this->cfg->addArgument (item2 ? item2->name : QString (""),
                            item2 ? item2->text () : QString (""),
                            item1 ? item1->text () : QString (""));

    this->cfg->rememberArgsVal
      (this->checkValues->checkState () == Qt::Checked);
  }

  this->cfg->writeConfig ();

  emit endInput ();
}
