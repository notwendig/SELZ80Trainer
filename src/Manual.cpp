#include "Manual.h"
#include "ui_Manual.h"

Manual::Manual(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Manual)
{
	ui->setupUi(this);
}

Manual::~Manual()
{
	delete ui;
}
