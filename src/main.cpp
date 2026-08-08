#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSizePolicy qsp(QSizePolicy::Preferred,QSizePolicy::Preferred);
	qsp.setHeightForWidth(true);

	MainWindow w;

	w.setSizePolicy(qsp);
	QIcon icon(":/pictures/icone.png");
	w.setWindowIcon(icon);
	w.show();

	return a.exec();
}
