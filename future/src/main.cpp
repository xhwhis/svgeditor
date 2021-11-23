#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	LCanvasScene canvas(800,600);
	MainWindow w(&canvas);
	w.resize(w.sizeHint());
	w.setWindowTitle("SVG Editor");

	if (QApplication::primaryScreen()->size().width() > w.width() &&
		QApplication::primaryScreen()->size().height() > w.height())
		w.show();
	else
		w.showMaximized();

	QObject::connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

	return a.exec();
}
