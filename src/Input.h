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

#ifndef INPUT_H
#define INPUT_H

#include <QtGui/QDialog>

#include "dialogInput.h"

class Config;
class QSize;

class Input : public QDialog, private Ui::dialogInput
{
  Q_OBJECT

  public:
    Input (QWidget *parent, Config *cfg);
    ~Input (void);

    void showDialog (void);

  signals:
    void endInput (void);

  private slots:
    void writeConfig (void);

  private:
    Config *cfg;
    QSize   origSize;

    void flush (void);
};

#endif /* INPUT_H */
