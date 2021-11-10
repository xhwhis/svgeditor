#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "lcanvas.h"

using namespace lwscode;

class SVGCanvasView : public LCanvasView
{
	Q_OBJECT

public:
	SVGCanvasView(LCanvasScene *scene, QWidget *parent = nullptr);
	~SVGCanvasView();

	void setItemType(int type) { m_itemType = type; }

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
	LCanvasLine *m_paintingLine;
	LCanvasRect *m_paintingRect;
	LCanvasEllipse *m_paintingEllipse;
	LCanvasItemList *m_selectionItems;
	QPoint m_startPos;
	QPoint m_lastPos;
	QPoint m_endPos;
	bool m_bPaintingItem;
	bool m_bTranslatingItem;
	bool m_bScalingItem;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(LCanvasScene *scene, QWidget *parent = nullptr);
	~MainWindow();

	void setScene(LCanvasScene *scene) { m_scene = scene; }
	LCanvasScene *scene() const { return m_scene; }
	void setView(SVGCanvasView *view) { m_view = view; }
	SVGCanvasView *view() const { return m_view; }

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

	LCanvasScene *m_scene;
	SVGCanvasView *m_view;
};

#endif // MAINWINDOW_H
