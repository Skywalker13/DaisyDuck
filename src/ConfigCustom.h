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

#ifndef CONFIGCUSTOM_H
#define CONFIGCUSTOM_H

#include "dialogConfigCustom.h"

class Config;

class ConfigCustom : public QDialog, private Ui::dialogConfigCustom
{
  Q_OBJECT

  public:
    ConfigCustom (QWidget *parent, Config *cfg);
    ~ConfigCustom (void);

    void showDialog (void);

  private slots:
    void writeConfig (void);

  private:
    Config *cfg;
};

#endif /* CONFIGCUSTOM_H */
