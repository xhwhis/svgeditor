#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lcanvasview.h"
#include "lcanvasitem.h"

using namespace lwscode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow() {}

signals:
	void changeItemType(ItemType itemType);
	void sigReadItemsFromFile(const QString &filePath);
	void sigWriteItemsToFile(const QString &filePath);

protected slots:
	void onPaintNone();
	void onPaintPath();
	void onPaintLine();
	void onPaintRect();
	void onPaintTriangle();
	void onPaintHexagon();
	void onPaintEllipse();
	void onPaintText();
	void onNewFile();
	void onOpenFile();
	void onSaveFile();
	void setCanvasColor();
	void setCanvasWidth(int width);
	void setCanvasHeight(int height);
	void setFillColor();
	void setStrokeColor();
	void setStrokeWidth(int width);

private:
	void initUI();
	void initCanvas();
	void initMenuBar();
	void initLeftToolBar();
	void initRightToolBar();
	void initBottomToolBar();

private:
	QMenuBar *m_mainMenuBar;
	QToolBar *m_leftToolBar;
	QToolBar *m_rightToolBar;
	QToolBar *m_bottomToolBar;
	QScrollArea *m_centralWidget;
	LCanvasView *m_canvas;
	QPushButton *m_canvasColorButton;
	QColor m_canvasColor;
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif // MAINWINDOW_H
