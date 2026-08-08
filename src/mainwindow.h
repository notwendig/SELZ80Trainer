#ifndef MAINWINDOW_H
#define MAINWINDOW_H
/*
 * Declaration file for the instruction Z80 opcode emulator.
 * Copyright (C) 2014  Juergen Willi Sievers @notwendiger.
 *                     <notwendig@augenpunkte.de>
 * Includes possible all documented and and undocumented opcodes.
 *
 *
 * This file is part of libZ80 - Zilog's Z80 instruction set emulator.
 *
 * SEL Z80 Trainer simulation
 * libZ80 is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * If you got some questions or find  bugs or parts that could be made better
 * don't hesitate to drop a line by email: <notwendig@augenpunkte.de> or also
 * by Twitter @notwendiger
 *
 * MANY THANKS TO Cristian Dinu thanks a lot your document has inspired me to go this way
 * by developing my own z80 instruction set simulator.
 *
 * An document by Cristian Dinu, compiled by using various sources of information
 * (see the acknowledgements section at http://www.z80.info/decoding.htm).
*/

#include <QMainWindow>
#include <QLabel>

#include "SELZ80Trainer.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void tick();
	void key(bool pressed);
	void iswitch(bool pressed);
	void manual(bool checked);
	void setSystem(bool checked);
	void setForth(bool checked);

signals:
	void setsegments0(unsigned char);
	void setsegments1(unsigned char);
	void setsegments2(unsigned char);
	void setsegments3(unsigned char);
	void setsegments4(unsigned char);
	void setsegments5(unsigned char);
	void setsegments6(unsigned char);
	void setsegments7(unsigned char);

	void enable0(bool);
	void enable1(bool);
	void enable2(bool);
	void enable3(bool);
	void enable4(bool);
	void enable5(bool);
	void enable6(bool);
	void enable7(bool);

	void setudigit0(unsigned char);
	void setudigit1(unsigned char);
	void setudigit2(unsigned char);
	void setudigit3(unsigned char);
	void setudigit4(unsigned char);
	void setudigit5(unsigned char);

	void enableu0(bool);
	void enableu1(bool);
	void enableu2(bool);
	void enableu3(bool);
	void enableu4(bool);
	void enableu5(bool);

	void setled0(bool);
	void setled1(bool);
	void setled2(bool);
	void setled3(bool);
	void setled4(bool);
	void setled5(bool);
	void setled6(bool);
	void setled7(bool);

protected:

	void System(const QStringList &sys, int n);

private slots:
	void on_MainWindow_iconSizeChanged(const QSize &iconSize);
	void on_powerStateChanged(int state);

private:
	Ui::MainWindow *ui;
	unsigned leds_;
	MyZ80 z80_;
	uint64_t instr_;
	QLabel *mhz_;
	static const QStringList system_[5];
	QPixmap* syssheet_[5];
	QLabel* syslabel_[35];
	static const QStringList sysstyle_;
};

#endif // MAINWINDOW_H
