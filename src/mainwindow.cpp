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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QCheckBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    leds_(0xFF),
    z80_(Zilog80,this),
    instr_(0),
    mhz_(0)
{
    ui->setupUi(this);

    z80_.setPower(false);

    syslabel_[ 0] = ui->L0;
    syslabel_[ 1] = ui->L1;
    syslabel_[ 2] = ui->L2;
    syslabel_[ 3] = ui->L3;
    syslabel_[ 4] = ui->L4;
    syslabel_[ 5] = ui->L5;
    syslabel_[ 6] = ui->L6;
    syslabel_[ 7] = ui->L7;

    syslabel_[ 8] = ui->K0;
    syslabel_[ 9] = ui->K1;
    syslabel_[10] = ui->K2;
    syslabel_[11] = ui->K3;
    syslabel_[12] = ui->K4;
    syslabel_[13] = ui->K5;
    syslabel_[14] = ui->K6;
    syslabel_[15] = ui->K7;

    syslabel_[16] = ui->D0;
    syslabel_[17] = ui->D1;
    syslabel_[18] = ui->D2;
    syslabel_[19] = ui->D3;
    syslabel_[20] = ui->D4;
    syslabel_[21] = ui->D5;
    syslabel_[22] = ui->D6;
    syslabel_[23] = ui->D7;

    syslabel_[24] = ui->B0;
    syslabel_[25] = ui->B1;
    syslabel_[26] = ui->B2;
    syslabel_[27] = ui->B3;
    syslabel_[28] = ui->A0;
    syslabel_[29] = ui->A1;
    syslabel_[30] = ui->A2;
    syslabel_[31] = ui->A3;

    syslabel_[32] = ui->T0;
    syslabel_[33] = ui->T1;
    syslabel_[34] = ui->SYS;

    syssheet_[0] = new QPixmap(":/pictures/System0.png");
    syssheet_[1] = new QPixmap(":/pictures/System1.png");
    syssheet_[2] = new QPixmap(":/pictures/System2.png");
    syssheet_[3] = new QPixmap(":/pictures/System3.png");
    syssheet_[4] = new QPixmap(":/pictures/System4.png");;
    System(system_[4] ,4);

    QTimer *timer = new QTimer(this);

    connect(this, SIGNAL(setsegments0(unsigned char)), ui->Display7_0, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments1(unsigned char)), ui->Display7_1, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments2(unsigned char)), ui->Display7_2, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments3(unsigned char)), ui->Display7_3, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments4(unsigned char)), ui->Display7_4, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments5(unsigned char)), ui->Display7_5, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments6(unsigned char)), ui->Display7_6, SLOT(setsegments(unsigned char)));
    connect(this, SIGNAL(setsegments7(unsigned char)), ui->Display7_7, SLOT(setsegments(unsigned char)));

    connect(this, SIGNAL(enable0(bool)), ui->Display7_0, SLOT(setState(bool)));
    connect(this, SIGNAL(enable1(bool)), ui->Display7_1, SLOT(setState(bool)));
    connect(this, SIGNAL(enable2(bool)), ui->Display7_2, SLOT(setState(bool)));
    connect(this, SIGNAL(enable3(bool)), ui->Display7_3, SLOT(setState(bool)));
    connect(this, SIGNAL(enable4(bool)), ui->Display7_4, SLOT(setState(bool)));
    connect(this, SIGNAL(enable5(bool)), ui->Display7_5, SLOT(setState(bool)));
    connect(this, SIGNAL(enable6(bool)), ui->Display7_6, SLOT(setState(bool)));
    connect(this, SIGNAL(enable7(bool)), ui->Display7_7, SLOT(setState(bool)));


    connect(this, SIGNAL(setudigit0(unsigned char)), ui->Display7U_0, SLOT(setdigit(unsigned char)));
    connect(this, SIGNAL(setudigit1(unsigned char)), ui->Display7U_1, SLOT(setdigit(unsigned char)));
    connect(this, SIGNAL(setudigit2(unsigned char)), ui->Display7U_2, SLOT(setdigit(unsigned char)));
    connect(this, SIGNAL(setudigit3(unsigned char)), ui->Display7U_3, SLOT(setdigit(unsigned char)));
    connect(this, SIGNAL(setudigit4(unsigned char)), ui->Display7U_4, SLOT(setdigit(unsigned char)));
    connect(this, SIGNAL(setudigit5(unsigned char)), ui->Display7U_5, SLOT(setdigit(unsigned char)));

    connect(this, SIGNAL(enableu0(bool)), ui->Display7U_0, SLOT(setState(bool)));
    connect(this, SIGNAL(enableu1(bool)), ui->Display7U_1, SLOT(setState(bool)));
    connect(this, SIGNAL(enableu2(bool)), ui->Display7U_2, SLOT(setState(bool)));
    connect(this, SIGNAL(enableu3(bool)), ui->Display7U_3, SLOT(setState(bool)));
    connect(this, SIGNAL(enableu4(bool)), ui->Display7U_4, SLOT(setState(bool)));
    connect(this, SIGNAL(enableu5(bool)), ui->Display7U_5, SLOT(setState(bool)));


    connect(this, SIGNAL(setled0(bool)), ui->led_0, SLOT(setState(bool)));
    connect(this, SIGNAL(setled1(bool)), ui->led_1, SLOT(setState(bool)));
    connect(this, SIGNAL(setled2(bool)), ui->led_2, SLOT(setState(bool)));
    connect(this, SIGNAL(setled3(bool)), ui->led_3, SLOT(setState(bool)));
    connect(this, SIGNAL(setled4(bool)), ui->led_4, SLOT(setState(bool)));
    connect(this, SIGNAL(setled5(bool)), ui->led_5, SLOT(setState(bool)));
    connect(this, SIGNAL(setled6(bool)), ui->led_6, SLOT(setState(bool)));
    connect(this, SIGNAL(setled7(bool)), ui->led_7, SLOT(setState(bool)));

    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));


    QFile mon(":/rom/monitor.bin");
    if(!mon.open(QIODevice::ReadOnly))
        throw;

    mon.read(z80_.getRam(),mon.size());
    mon.close();

    QLabel *tty = new QLabel("Console:" + z80_.ttyname(),this);
    ui->statusbar->addPermanentWidget(tty);

    mhz_ = new QLabel("00.000 MHz",this);
    ui->statusbar->addPermanentWidget(mhz_);

    QCheckBox *power = new QCheckBox("Power",this);
    power->setStyleSheet( "QCheckBox::indicator { width: 20%; height: 20%;}\nQCheckBox::indicator:unchecked { image: url(:/pictures/PowerOff.png);}\nQCheckBox::indicator:checked { image: url(:/pictures/PowerOn.png);}");
    ui->statusbar->addPermanentWidget(power);
    connect(power,SIGNAL(stateChanged(int)),this,SLOT(on_powerStateChanged(int)));

    timer->setInterval(10);
    timer->start(10);
}

void MainWindow::on_powerStateChanged(int state)
{
    z80_.setPower(state);
    ui->action_Camel_Forth->setEnabled(state);

    emit enable0(state);
    emit enable1(state);
    emit enable2(state);
    emit enable3(state);
    emit enable4(state);
    emit enable5(state);
    emit enable6(state);
    emit enable7(state);

    emit enableu0(state);
    emit enableu1(state);
    emit enableu2(state);
    emit enableu3(state);
    emit enableu4(state);
    emit enableu5(state);
    if(!state)
    {
        emit setled0(state);
        emit setled1(state);
        emit setled2(state);
        emit setled3(state);
        emit setled4(state);
        emit setled5(state);
        emit setled6(state);
        emit setled7(state);
        instr_ = 0;
    }
}

void MainWindow::key(bool pressed)
{
    Q_UNUSED(pressed);

    SWITCH *sender = dynamic_cast<SWITCH*>(QObject::sender());
    z80_.setkey(sender->getKey());
}

void MainWindow::iswitch(bool pressed)
{
    SWITCH2 *sender = dynamic_cast<SWITCH2*>(QObject::sender());

    z80_.iswitch(sender->getKey(), pressed);
}

void MainWindow::tick()
{
    static int delay = 0;
    delay++;
    if(!(delay%100))
    {
            uint64_t instr = z80_.Clock();
            double mhz = (instr - instr_) * 0.0000001;
            instr_ = instr;
            QString fq;
            fq=QString("%1 Mhz").arg(mhz,6,'g',4);
            mhz_->setText(fq);
    }

    if(z80_.displaychanged())
    {
        emit setsegments0(z80_[0]);
        emit setsegments1(z80_[1]);
        emit setsegments2(z80_[2]);
        emit setsegments3(z80_[3]);
        emit setsegments4(z80_[4]);
        emit setsegments5(z80_[5]);
        emit setsegments6(z80_[6]);
        emit setsegments7(z80_[7]);

        emit enableu0(z80_[8] & 0x20);
        emit setudigit0(z80_[8]);
        emit enableu0(z80_[9] & 0x20);
        emit setudigit1(z80_[9]);
        emit enableu0(z80_[10] & 0x20);
        emit setudigit2(z80_[10]);
        emit enableu0(z80_[11] & 0x20);
        emit setudigit3(z80_[11]);
        emit enableu0(z80_[12] & 0x20);
        emit setudigit4(z80_[12]);
        emit enableu0(z80_[13] & 0x20);
        emit setudigit5(z80_[13]);

        unsigned leds = z80_.getLed();
        if(leds_ != leds)
        {
            leds_ = leds;
            emit setled0(leds & 0x01);
            emit setled1(leds & 0x02);
            emit setled2(leds & 0x04);
            emit setled3(leds & 0x08);
            emit setled4(leds & 0x10);
            emit setled5(leds & 0x20);
            emit setled6(leds & 0x40);
            emit setled7(leds & 0x80);
        }
    }
}

MainWindow::~MainWindow()
{
    z80_.setPower(false);
    delete ui;
}

void MainWindow::manual(bool checked)
{
}

void MainWindow::setSystem(bool checked)
{
    QAction *sender = dynamic_cast<QAction*>(QObject::sender());
    if(sender == ui->actionSystem_0)
        System(system_[0],0);
    else if(sender == ui->actionSystem_1)
        System(system_[1],1);
    else if (sender == ui->actionSystem_2)
            System(system_[2],2);
    else if (sender == ui->actionSystem_3)
        System(system_[3],3);
    else
        System(system_[4],4);
}

void MainWindow::System(const QStringList &sys, int n)
{
    for(int i = 0; i < 35; i++)
    {
        if(sys[i].size())
        {
            syslabel_[i]->setText(sys[i]);
            syslabel_[i]->setStyleSheet(sysstyle_[n]);
            //syslabel_[i]->setStyleSheet( i < 32 ? sysstyle_[n]:"");
            syslabel_[i]->show();
        }
        else
            syslabel_[i]->hide();
    }
    if(syssheet_[n])
        ui->sheet->setPixmap(*syssheet_[n]);

}

void MainWindow::setForth(bool checked)
{
    QFile forth(":/rom/forth.bin");
    if(!forth.open(QIODevice::ReadOnly))
        throw;

    z80_.setPower(false);
    forth.read(z80_.getRam()+SELNOTUSED,forth.size());
    forth.close();
    z80_.setPC(SELNOTUSED);
    z80_.setPower(true);
}

const QStringList MainWindow::system_[5] = {
    {
    "S0", // L0
    "S1", // L1
    "S2", // L2
    "S3", // L3
    "S4", // L4
    "", // L5
    "", // L6
    "0", // L7

    "S0\nR+1\n(incr.", // K0
    "S1\nB=\n!B", // K1
    "S2\nA=\n!A", // K2
    "S3\nB=\nact.", // K3
    "S4\nA=\nact.", // K4
    "", // K5
    "END", // K6
    "", // K7

    "R0", // D0
    "R1", // D1
    "R2", // D2
    "R3", // D3
    "CY", // D4
    "", // D5
    "", // D6
    "", // D7

    "B0", // B0
    "B1", // B1
    "B2", // B2
    "B3", // B3
    "A0", // A0
    "A1", // A1
    "A2", // A2
    "A3", // A3

    "S4 S3 S2 S1 S0 | R\n" // T0
    "---------------|--\n"
    " 1  0  0  0  0 | A\n"
    " 1  0  1  0  0 |!A\n"
    " 1  0  0  1  1 |-A\n"
    " 1  0  0  0  1 |A+1\n"
    " 1  0  0  1  0 |A-1\n"
    " 1  1  0  0  0 |A+B\n"
    " 1  1  0  0  1 |A-B\n"
    " 1  1  1  0  1 |B-A"
    ,
    "S4 S3 S2 S1 S0 | R\n" // T1
    "---------------|--\n"
    " 0  0  0  0  0 | 0\n"
    " 0  0  0  0  1 | 1\n"
    " 0  0  1  1  1 |-1\n"
    " 0  1  0  0  0 | B\n"
    " 0  1  0  1  0 |!B\n"
    " 0  1  0  1  1 |-B\n"
    " 0  1  0  0  1 |B+1\n"
    " 0  1  1  0  0 |B-1"
    ,
    "SYSTEM 0\n4-Bit-Adder."
},
{
    "U0", // L0
    "U1", // L1
    "U2", // L2
    "U3", // L3
    "", // L4
    "", // L5
    "", // L6
    "1", // L7

    "U0\nFunc.", // K0
    "U1\nFunc.", // K1
    "U2\nFunc.", // K2
    "U3\nFunc.", // K3
    "", // K4
    "", // K5
    "END", // K6
    "", // K7

    "R0", // D0
    "R1", // D1
    "R2", // D2
    "R3", // D3
    "CY", // D4
    "", // D5
    "", // D6
    "", // D7

    "B0", // B0
    "B1", // B1
    "B2", // B2
    "B3", // B3
    "A0", // A0
    "A1", // A1
    "A2", // A2
    "A3", // A3

    "U3 U2 U1 U0 | R\n" // T0
    "------------|---\n"
    " 1  0  0  0 |A-B\n"
    " 1  0  0  1 |A&B\n"
    " 1  0  1  0 |A|B\n"
    " 1  0  1  1 |A^B\n"
    " 1  1  0  0 | -1\n"
    " 1  1  0  1 |  ?\n"
    " 1  1  1  0 |  ?\n"
    " 1  1  1  1 |  ?"
    ,
    "U3 U2 U1 U0 | R\n" // T1
    "------------|---\n"
    " 0  0  0  0 |  0\n"
    " 0  0  0  1 |  1\n"
    " 0  0  1  0 | -1\n"
    " 0  0  1  1 |  B\n"
    " 0  1  0  0 | !B\n"
    " 0  1  0  1 | -B\n"
    " 0  1  1  0 |B+1\n"
    " 0  1  1  1 |B-1"
    ,
    "SYSTEM 1\n4-Bit-ALU\nROM-Decoder"
},
{
    "U0", // L0
    "U1", // L1
    "U2", // L2
    "U3", // L3
    "", // L4
    "", // L5
    "", // L6
    "2", // L7

    "U0", // K0
    "U1", // K1
    "U2", // K2
    "U3", // K3
    "", // K4
    "", // K5
    "", // K6
    "", // K7

    "R0", // D0
    "R1", // D1
    "R2", // D2
    "R3", // D3
    "C", // D4
    "P/V", // D5
    "D", // D6
    "Z", // D7

    "B0", // B0
    "B1", // B1
    "B2", // B2
    "B3", // B3
    "", // A0
    "", // A1
    "", // A2
    "", // A3

    "U3 U2 U1 U0 | R      \n" // T0
    "------------+--------\n"
    " 1  0  0  0 | SUB B  \n"
    " 1  0  0  1 | AND B  \n"
    " 1  0  1  0 | OR B   \n"
    " 1  0  1  1 | XOR B  \n"
    " 1  1  0  0 | LD A,-1\n"
    " 1  1  0  1 | ---    \n"
    " 1  1  1  0 | ---    \n"
    " 1  1  1  1 | ---    "
        ,

    "U3 U2 U1 U0 | R      \n" // T1
    "------------+--------\n"
    " 0  0  0  0 | NOP    \n"
    " 0  0  0  1 | LD A,1 \n"
    " 0  0  1  0 | CPL    \n"
    " 0  0  1  1 | IN A,B \n"
    " 0  1  0  0 | LD A,0 \n"
    " 0  1  0  1 | INC A  \n"
    " 0  1  1  0 | DEC A  \n"
    " 0  1  1  1 | ADD A,B"
        ,
    "System 2\n4 Bit Akkumulator."  // SYS
},
    {
        "U0", // L0
        "U1", // L1
        "U2", // L2
        "U3", // L3
        "", // L4
        "", // L5
        "", // L6
        "3", // L7

        "U0", // K0
        "U1", // K1
        "U2", // K2
        "U3", // K3
        "", // K4
        "", // K5
        "", // K6
        "", // K7

        "R0", // D0
        "R1", // D1
        "R2", // D2
        "R3", // D3
        "C", // D4
        "P/V", // D5
        "D", // D6
        "Z", // D7

        "B0", // B0
        "B1", // B1
        "B2", // B2
        "B3", // B3
        "", // A0
        "", // A1
        "", // A2
        "", // A3

        "U3 U2 U1 U0 | R      \n" // T0
        "------------+--------\n"
        " 1  0  0  0 | SUB A,(a)\n"
        " 1  0  0  1 | AND A,(a)\n"
        " 1  0  1  0 | OR A,(a)\n"
        " 1  0  1  1 | XOR A,(a)\n"
        " 1  1  0  0 | LD A,-1\n"
        " 1  1  0  1 | IN A,B\n"
        " 1  1  1  0 | LD (a),A\n"
        " 1  1  1  1 | ---    "
            ,

        "U3 U2 U1 U0 | R      \n" // T1
        "------------+--------\n"
        " 0  0  0  0 | NOP    \n"
        " 0  0  0  1 | LD A,1 \n"
        " 0  0  1  0 | CPL    \n"
        " 0  0  1  1 | IN A,(a)\n"
        " 0  1  0  0 | LD A,0 \n"
        " 0  1  0  1 | INC A  \n"
        " 0  1  1  0 | DEC A  \n"
        " 0  1  1  1 | ADD A,(a)"
            ,
        "System 3\n4 Bit Akkumulator.\nmit Speicher"  // SYS
    },
{

    " ", // L0
    " ", // L1
    " ", // L2
    " ", // L3
    " ", // L4
    " ", // L5
    " ", // L6
    " ", // L7

    " ", // K0
    " ", // K1
    " ", // K2
    " ", // K3
    " ", // K4
    " ", // K5
    " ", // K6
    " ", // K7

    " ", // D0
    " ", // D1
    " ", // D2
    " ", // D3
    " ", // D4
    " ", // D5
    " ", // D6
    " ", // D7

    " ", // B0
    " ", // B1
    " ", // B2
    " ", // B3
    " ", // A0
    " ", // A1
    " ", // A2
    " ", // A3

    " ", // T0
    " ", // T1
    "JSievers@NadiSoft.de"  // SYS
}};



const QStringList MainWindow::sysstyle_ = {
    "QLabel{\n	 background: #699BCD;\n }",
    "QLabel{\n	 background: #94C494;\n }",
    "QLabel{\n	 background: #E58645;\n }",
    "QLabel{\n	 background: #FFE604;\n }",
    "QLabel{\n	 background: #216480;\n }"
};

void MainWindow::on_MainWindow_iconSizeChanged(const QSize &iconSize)
{

}
