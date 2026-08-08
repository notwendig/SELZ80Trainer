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


#include "SELZ80Trainer.h"
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <stdio.h>
#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#endif

// SEL has only
// 0000.....0FFF 1 4 kb Eprom mit Betriebsprogramm (Typ 2732)
// 1000.....17FF 2 2 kb Ram (Typ 2106) Speicher
// 1800.....1FFF 3 2 kb Ram/Eprom (2106/2716) - freie Fassung
// 2000.....27FF 5 2 kb Ram/Eprom (2106/2716) - freie Fassung
// 2800.....2FFF dekodiert auf BUS
// 3000.....3FFF 4 4 kb Eprom (Typ 2732) - freie Fassung
// 4000.....FFFF nicht verwendet

// Extend9	; JP 2808h if 0xC3 code exist
// ExtendA	; JP e000h if 0xC3 code exist
// ExtendB	; JP e003h if 0xC3 code exist

static uint8_t ram[0x10000];

MyZ80::MyZ80 (cputype_t type, QWidget*app)
	:Z80(type)
	,SIOcontrol_(0)
	,SIOstatus_(0)
	,control_(0)
	,display_{0,0,0,0,0,0,0,0}
	,changed_(false)
	,keycode_(0xFF)
	,reset_(0)
	,iswitch_(0)
{
#ifndef WIN32
	console_ = posix_openpt(O_RDWR|O_NOCTTY);

	if (console_ == -1
		|| grantpt (console_) == -1
		|| unlockpt (console_) == -1)
		perror("Console");

	 consolename_ = ptsname (console_);
	 cout << "Console at " << consolename_.toLocal8Bit().data() << endl;
#endif
}

MyZ80::~MyZ80 ()
{
	if(console_ != -1)
		close(console_);
};

char * MyZ80::getRam(void)
{
	return (char*)ram;
}

uint8_t MyZ80::rmem(uint16_t addr) const
{
	return ram[addr];
}

void MyZ80::setkey(unsigned code)
{
	if(code == KeyCode_RS)
	{
		reset_ = 1;
	}
	else
		keycode_ = code;
}

unsigned char MyZ80::inport(Z80Bus::address_t  port)
{
	unsigned char res = 0xFF;
	switch(port&0xFF)
	{
	case IC8279_Ctrl:
		res = (keycode_ != 0xFF) ? 1:0;
		break;
	case IC8279_Data:
		res = keycode_;
		keycode_ = 0xFF;
		break;
	case UserDisplay_1:
	case UserDisplay_2:
	case UserDisplay_3:
	case UserDisplay_4:
	case UserDisplay_5:
	case UserDisplay_6:
		break;
	case BinIO_PC:
		res = iswitch_;
		break;
	case SIO8251_Dataport:
#ifdef WIN32
		if(_kbhit())
			res = getchar();
#else
		int tmp;
		if( -1 != ioctl(console_, FIONREAD, &tmp) && tmp > 0)
			read (console_,&res,1);
#endif
		else
		{
			res = 0;
		}
		res = toupper(res);
		break;
	case SIO8251_CntrPort:
#ifdef WIN32
		if(_kbhit())
#else
		if( -1 != ioctl(console_, FIONREAD, &tmp) && tmp > 0)
#endif
			res = (1 << TxRDY) | (1 << RxRDY);
		else
		{
			res = 1 << TxRDY;
		}
		break;
	case PIO_8255_PortA:break;
	case PIO_8255_PortB:break;
		break;
	case PIO_8255_PortC:
		res = 0x10;
		break;
	case PIO_8255_CntrlStatus:
		break;
	}
	return res;
}

void MyZ80::outport(Z80Bus::address_t  port, Z80Bus::data_t data)
{

	switch(port&0xFF)
	{
	case IC8279_Ctrl:
		control_ = data;
		break;
	case IC8279_Data:
		if(control_ & 0x80)
		{

			unsigned char coded = (data >> 4) | (data << 4);
			display_[control_ & 0x7] = coded;

			changed_ = true;
		}
		else if (data == 0x0C2)
			keycode_ = 0xFF;
		break;
	case UserDisplay_1:
	case UserDisplay_2:
	case UserDisplay_3:
	case UserDisplay_4:
	case UserDisplay_5:
	case UserDisplay_6:
		display_[(port & 0x7) + 8] = data;
		changed_ = true;
		break;
	case BinIO_PC:
		leds_ = data;
		changed_ = true;
		break;
	case SIO8251_Dataport:
		if(-1 != console_)
			write(console_,&data,1);
		break;
	case SIO8251_CntrPort:
		break;
	case PIO_8255_PortA:break;
	case PIO_8255_PortB:
		if(-1 != console_)
			write(console_,&data,1);
		break;
	case PIO_8255_PortC:
		break;
	case PIO_8255_CntrlStatus:break;
	}
}

uint16_t MyZ80::bus(Z80Bus &iobus)
{
	if(reset_)
	{
		iobus.setPin(RESET,true);
		reset_ = 0;
	}
	// M1 opcode zycle
	if (iobus.check(RD|M1|MEMREQ))
	{
		static int dl = 0;
		dl++;
		if(!(dl%1000))
		{
			struct timespec tim;
			tim.tv_sec = 0;
			tim.tv_nsec = 17000;

		   nanosleep(&tim , 0);
		}

		if(!excmd_.isEmpty())
			iobus = excmd_.dequeue();
		else
			iobus = ram[(Z80Bus::address_t)iobus];
	}
	// Mem read zycle
	else if (iobus.check(RD|MEMREQ))
	{
		if(!excmd_.isEmpty())
			iobus = excmd_.dequeue();
		else
			iobus = ram[(Z80Bus::address_t)iobus];
	}
	// Mem write zycle
	else if (iobus.check(WR|MEMREQ))
	{
		if((Z80Bus::address_t)iobus >= 0x1000 &&
		   ((Z80Bus::address_t)iobus < 0x4000 || (Z80Bus::address_t)iobus > 0x5FFF)) // ROM
			ram[(Z80Bus::address_t)iobus] = iobus;
	}
	// interrupt ack cycle
	else if (iobus.check(RD|IOREQ|M1))
	{
		//intreq();
		ram[(Z80Bus::address_t) iobus] = iobus;
	}
	// input operation
	else if(iobus.check(IOREQ|RD))
		iobus = inport((Z80Bus::address_t)iobus & 0xFF);

	// output operation
	else if (iobus.check(IOREQ|WR))
		outport(iobus,iobus);

	// refresch cycle.
	else if (iobus.check(MEMREQ|REFRESH))
	{

	}

	return 0;
}

