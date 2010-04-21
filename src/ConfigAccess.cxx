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

#include <QtGui/QDialog>

#include "Config.h"
#include "ConfigAccess.h"


ConfigAccess::ConfigAccess (QWidget *parent, Config *cfg) : QDialog (parent)
{
  this->setupUi (this);
  this->cfg = cfg;

  connect (this, SIGNAL (accepted ()),
           this, SLOT (writeConfig ()));
}

ConfigAccess::~ConfigAccess (void)
{
  /* NOP */
}

void
ConfigAccess::showDialog (void)
{
  unsigned int i = 0;
  const QString *name = NULL, *value, *label;

  this->editUri->setText (this->cfg->getUri ());

  this->tableVariables->clear ();
  this->tableVariables->setHorizontalHeaderLabels
    (QStringList () << tr ("Name") << tr ("Value") << tr ("Label"));

  do
  {
    this->cfg->getArgument (&name, &value, &label);
    if (name)
    {
      QTableWidgetItem *item1 = new QTableWidgetItem (*name);
      QTableWidgetItem *item2 = new QTableWidgetItem (*value);
      QTableWidgetItem *item3 = new QTableWidgetItem (*label);

      this->tableVariables->setItem (i, 0, item1);
      this->tableVariables->setItem (i, 1, item2);
      this->tableVariables->setItem (i, 2, item3);
      i++;
    }
  }
  while (name);

  this->exec ();
}

/****************************************************************************/
/*                                                                          */
/*                              Private Slots                               */
/*                                                                          */
/****************************************************************************/

void
ConfigAccess::writeConfig (void)
{
  int i;

  this->cfg->setUri (editUri->text ());

  this->cfg->flushArguments ();
  for (i = 0; i < tableVariables->rowCount (); i++)
  {
    const QTableWidgetItem *item1, *item2, *item3;

    item1 = tableVariables->item (i, 0);
    item2 = tableVariables->item (i, 1);
    item3 = tableVariables->item (i, 2);

    if (!item1)
      continue;

    this->cfg->addArgument (item1->text (),
                            item2 ? item2->text () : QString (""),
                            item3 ? item3->text () : QString (""));

    this->cfg->rememberArgsVal (true);
  }

  this->cfg->writeConfig ();
}
