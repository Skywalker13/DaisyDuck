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

#include <QtWidgets/QDialog>

#include "Config.h"
#include "ConfigCustom.h"


ConfigCustom::ConfigCustom (QWidget *parent, Config *cfg) : QDialog (parent)
{
  this->setupUi (this);
  this->cfg = cfg;

  connect (this, SIGNAL (accepted ()),
           this, SLOT (writeConfig ()));
}

ConfigCustom::~ConfigCustom (void)
{
  /* NOP */
}

void
ConfigCustom::showDialog (void)
{
  const QString *title, *text;

  this->cfg->getCustom (&title, &text);
  this->editTitle->setText (*title);
  this->editText->setPlainText (*text);

  this->exec ();
}

/****************************************************************************/
/*                                                                          */
/*                              Private Slots                               */
/*                                                                          */
/****************************************************************************/

void
ConfigCustom::writeConfig (void)
{
  this->cfg->setCustom (this->editTitle->text (),
                        this->editText->toPlainText ());
  this->cfg->writeConfig ();
}
