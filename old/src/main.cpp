#include "mainwindow.h"

#include <QApplication>
#include <QGuiApplication>

using namespace lwscode;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	LCanvas canvas(800,600);
	MainWindow w(&canvas);
	w.resize(w.sizeHint());
	w.setWindowTitle("SVG Editor");

	if (QGuiApplication::primaryScreen()->size().width() > w.width() &&
		QGuiApplication::primaryScreen()->size().height() > w.height())
		w.show();
	else
		w.showMaximized();

	QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

	return a.exec();
}
