#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	if (QApplication::primaryScreen()->size().width() > w.width() &&
		QApplication::primaryScreen()->size().height() > w.height())
	{
		w.show();
	}
	else
	{
		w.showMaximized();
	}

	QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

	return a.exec();
}
