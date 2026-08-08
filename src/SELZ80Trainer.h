#ifndef SELZ80_D
#define SELZ80_D
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

#include <stdint.h>
#include "Z80.h"
#include "SELRom.h"
#include <QObject>
#include <QQueue>

using namespace std;


// And now declare your own cpu class inheritances from the Z80 class

class MyZ80:public QObject, public Z80
{
	Q_OBJECT
public:
  MyZ80 (cputype_t type, QWidget* app);

  virtual ~MyZ80 ();
	bool displaychanged(void) const { return changed_;};
	unsigned char operator [] (unsigned idx) const { return display_[idx%14];}
	char* getRam(void);
	void setkey(unsigned code);
	unsigned getLed(void) const { return leds_;}
	void iswitch(int bit, bool val)
	{
		if(val)
		   iswitch_ |= (1 << bit);
		else
			iswitch_ &= ~(1 << bit);
		cout << bit << hex << (unsigned) iswitch_ << endl;
	}

	const QString& ttyname() const { return consolename_;}

	void setCmd(const QQueue<uint8_t> &cmd)
	{
		setPower(false);
		excmd_ = cmd;
	}
	void setPC(unsigned short pc)
	{
		QQueue<uint8_t> pcq;
		pcq.enqueue(0xc3);	// JP xxxx
		pcq.enqueue(pc);
		pcq.enqueue(pc>>8);
		setCmd(pcq);
	}

signals:
public slots:

protected:
	QQueue<uint8_t> excmd_;

  void outport(Z80Bus::address_t  addr, Z80Bus::data_t data);
  unsigned char inport(Z80Bus::address_t  port);
  // You need to implement the pure virtual bus interface function
  uint16_t bus(Z80Bus &iobus);

  // And if you have compiled including the disassembler then also
  uint8_t rmem(uint16_t addr) const;

	uint8_t SIOcontrol_;
	uint8_t SIOstatus_;
	uint8_t control_;

	unsigned char display_[14];
	bool changed_;
	unsigned keycode_;
	int reset_;
	unsigned leds_;
	unsigned char iswitch_;

	QString consolename_;
	int console_;
};

#endif // SELZ80_D
