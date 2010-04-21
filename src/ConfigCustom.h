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
