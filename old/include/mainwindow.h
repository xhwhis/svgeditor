#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStackedWidget>
#include <QToolBar>
#include <QToolButton>

#include "lcanvas.h"

using namespace lwscode;

class SVGCanvasView : public LCanvasView
{
	Q_OBJECT

public:
	SVGCanvasView(LCanvas *canvas, QWidget *parent = nullptr);
	~SVGCanvasView();

	void setItemType(int type)
	{
		m_itemType = type;
	}

public slots:
	void cutItem();
	void copyItem();
	void pasteItem();
	void deleteItem();
	void moveTopItem();
	void moveUpItem();
	void moveDownItem();
	void moveBottomItem();

protected:
	void contentsMousePressEvent(QMouseEvent *event);
	void contentsMouseMoveEvent(QMouseEvent *event);
	void contentsMouseReleaseEvent(QMouseEvent *event);
	void contentsContextMenuEvent(QContextMenuEvent *event);

private:
	int m_itemType;
	LCanvasItem *m_activeItem;
	LCanvasItemList *m_selectionItems;
	QPoint m_startPos;
	QPoint m_lastPos;
	QPoint m_endPos;
	int m_minZ;
	int m_maxZ;
	QPainter m_painter;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(LCanvas *canvas, QWidget *parent = nullptr);
	~MainWindow();

	void setCanvas(LCanvas *canvas)
	{
		m_canvas = canvas;
	}
	LCanvas *canvas()
	{
		return m_canvas;
	}
	void setView(SVGCanvasView *view)
	{
		m_view = view;
	}
	SVGCanvasView *view()
	{
		return m_view;
	}

public slots:
	void drawNone();
	void drawPath();
	void drawLine();
	void drawRect();
	void drawEllipse();
	void drawStar();
	void drawText();

private:
	void initUI();
	void initMenuBar();
	void initLeftToolBar();
	void initRightToolBar();
	void initCanvas();

private:
	QMenuBar *m_mainMenuBar;
	QMenu *m_fileMenu;
	QMenu *m_editMenu;
	QMenu *m_objectMenu;
	QMenu *m_viewMenu;

	QAction *m_newFileAction;
	QAction *m_openFileAction;
	QAction *m_saveFileAction;
	QAction *m_saveAsFileAction;

	QAction *m_undoEditAction;
	QAction *m_redoEditAction;
	QAction *m_cutEditAction;
	QAction *m_copyEditAction;
	QAction *m_pasteEditAction;
	QAction *m_deleteEditAction;

	QAction *m_moveTopObjectAction;
	QAction *m_moveUpObjectAction;
	QAction *m_moveDownObjectAction;
	QAction *m_moveBottomObjectAction;

	QAction *m_showRulesViewAction;

	QToolBar *m_leftToolBar;
	QToolButton *m_noneButton;
	QToolButton *m_pathButton;
	QToolButton *m_lineButton;
	QToolButton *m_rectButton;
	QToolButton *m_ellipseButton;
	QToolButton *m_starButton;

	QToolBar *m_rightToolBar;
	QStackedWidget *m_toolsPanelWidget;

	LCanvas *m_canvas;
	SVGCanvasView *m_view;
};

#endif // MAINWINDOW_H
