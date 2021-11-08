#include "mainwindow.h"

SVGCanvasView::SVGCanvasView(LCanvasScene *scene, QWidget *parent)
	: LCanvasView(scene, parent)
{

}

SVGCanvasView::~SVGCanvasView()
{

}

void SVGCanvasView::cutItem()
{

}

void SVGCanvasView::copyItem()
{

}

void SVGCanvasView::pasteItem()
{

}

void SVGCanvasView::deleteItem()
{

}

void SVGCanvasView::moveTopItem()
{

}

void SVGCanvasView::moveUpItem()
{

}

void SVGCanvasView::moveDownItem()
{

}

void SVGCanvasView::moveBottomItem()
{

}

void SVGCanvasView::contentsMousePressEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_startPos = m_lastPos = m_endPos = pos;
	switch (m_itemType)
	{
	case LCanvasItem::Line:
	{
		m_paintingLine = new LCanvasLine(this->scene());
		m_paintingLine->setX(pos.x());
		m_paintingLine->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	case LCanvasItem::Rectangle:
	{
		m_paintingRect = new LCanvasRect(this->scene());
		m_paintingRect->setX(pos.x());
		m_paintingRect->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	case LCanvasItem::Ellipse:
	{
		m_paintingEllipse = new LCanvasEllipse(this->scene());
		m_paintingEllipse->setX(pos.x());
		m_paintingEllipse->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	default:
	{
		m_bPaintingItem = false;
		break;
	}
	}
}

void SVGCanvasView::contentsMouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;
	if (m_bPaintingItem)
	{
		switch (m_itemType)
		{
		case LCanvasItem::Line:
		{
			m_paintingLine->setPoints(m_startPos.x(), m_startPos.y(), m_endPos.x(), m_endPos.y());
			m_paintingLine->show();
			this->scene()->update();
			break;
		}
		case LCanvasItem::Rectangle:
		{
			m_paintingRect->setSize(qAbs(m_endPos.x() - m_startPos.x()),
									qAbs(m_endPos.y() - m_startPos.y()));
			m_paintingRect->show();
			this->scene()->update();
			break;
		}
		case LCanvasItem::Ellipse:
		{
			m_paintingEllipse->setSize(qAbs(m_endPos.x() - m_startPos.x()),
									   qAbs(m_endPos.y() - m_startPos.y()));
			m_paintingEllipse->show();
			this->scene()->update();
			break;
		}
		default:
		{
			break;
		}
		}
	}
	m_lastPos = pos;
}

void SVGCanvasView::contentsMouseReleaseEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;

	if (m_bPaintingItem)
	{
		m_paintingLine = nullptr;
		m_paintingRect = nullptr;
		m_paintingEllipse = nullptr;
	}

	m_startPos = QPoint();
	m_endPos = QPoint();
	m_lastPos = QPoint();
}

void SVGCanvasView::contentsContextMenuEvent(QContextMenuEvent *event)
{

}

MainWindow::MainWindow(LCanvasScene *scene, QWidget *parent)
	: QMainWindow(parent)
	, m_scene(scene)
{
	initUI();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initUI()
{
	initMenuBar();
	initLeftToolBar();
	initRightToolBar();
	initCanvas();
}

void MainWindow::initMenuBar()
{
	m_mainMenuBar = new QMenuBar(this);
	this->setMenuBar(m_mainMenuBar);

	m_fileMenu = new QMenu("File", m_mainMenuBar);
	m_newFileAction = new QAction("New", m_fileMenu);
	m_newFileAction->setShortcut(QKeySequence::New);
	m_fileMenu->addAction(m_newFileAction);
	m_openFileAction = new QAction("Open", m_fileMenu);
	m_openFileAction->setShortcut(QKeySequence::Open);
	m_fileMenu->addAction(m_openFileAction);
	m_saveFileAction = new QAction("Save", m_fileMenu);
	m_saveFileAction->setShortcut(QKeySequence::Save);
	m_fileMenu->addAction(m_saveFileAction);
	m_saveAsFileAction = new QAction("Save As", m_fileMenu);
	m_saveAsFileAction->setShortcut(QKeySequence::SaveAs);
	m_fileMenu->addAction(m_saveAsFileAction);
	m_mainMenuBar->addAction(m_fileMenu->menuAction());

	m_editMenu = new QMenu("Edit", m_mainMenuBar);
	m_undoEditAction = new QAction("Undo", m_editMenu);
	m_undoEditAction->setShortcut(QKeySequence::Undo);
	m_editMenu->addAction(m_undoEditAction);
	m_redoEditAction = new QAction("Redo", m_editMenu);
	m_redoEditAction->setShortcut(QKeySequence::Redo);
	m_editMenu->addAction(m_redoEditAction);
	m_editMenu->addSeparator();
	m_cutEditAction = new QAction("Cut", m_editMenu);
	m_cutEditAction->setShortcut(QKeySequence::Cut);
	m_editMenu->addAction(m_cutEditAction);
	m_copyEditAction = new QAction("Copy", m_editMenu);
	m_copyEditAction->setShortcut(QKeySequence::Copy);
	m_editMenu->addAction(m_copyEditAction);
	m_pasteEditAction = new QAction("Paste", m_editMenu);
	m_pasteEditAction->setShortcut(QKeySequence::Paste);
	m_editMenu->addAction(m_pasteEditAction);
	m_deleteEditAction = new QAction("Delete", m_editMenu);
	m_deleteEditAction->setShortcut(QKeySequence::Delete);
	m_editMenu->addAction(m_deleteEditAction);
	m_mainMenuBar->addAction(m_editMenu->menuAction());

	m_objectMenu = new QMenu("Object", m_mainMenuBar);
	m_moveTopObjectAction = new QAction("Move Top", m_objectMenu);
	m_objectMenu->addAction(m_moveTopObjectAction);
	m_moveUpObjectAction = new QAction("Move Up", m_objectMenu);
	m_objectMenu->addAction(m_moveUpObjectAction);
	m_moveDownObjectAction = new QAction("Move Down", m_objectMenu);
	m_objectMenu->addAction(m_moveDownObjectAction);
	m_moveBottomObjectAction = new QAction("Move Bottom", m_objectMenu);
	m_objectMenu->addAction(m_moveBottomObjectAction);
	m_mainMenuBar->addAction(m_objectMenu->menuAction());

	m_viewMenu = new QMenu("View", m_mainMenuBar);
	m_showRulesViewAction = new QAction("Show Rules", m_viewMenu);
	m_showRulesViewAction->setCheckable(true);
	m_viewMenu->addAction(m_showRulesViewAction);
	m_mainMenuBar->addAction(m_viewMenu->menuAction());
}

void MainWindow::initLeftToolBar()
{
	m_leftToolBar = new QToolBar(this);
	m_leftToolBar->setMovable(false);
	this->addToolBar(Qt::LeftToolBarArea, m_leftToolBar);

	m_noneButton = new QToolButton(m_leftToolBar);
	m_noneButton->setIcon(QIcon(":icons/none.svg"));
	m_noneButton->setText("None");
	m_noneButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_noneButton->setFixedWidth(64);

	m_pathButton = new QToolButton(m_leftToolBar);
	m_pathButton->setIcon(QIcon(":icons/path.svg"));
	m_pathButton->setText("Path");
	m_pathButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_pathButton->setFixedWidth(64);

	m_lineButton = new QToolButton(m_leftToolBar);
	m_lineButton->setIcon(QIcon(":icons/line.svg"));
	m_lineButton->setText("Line");
	m_lineButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_lineButton->setFixedWidth(64);

	m_rectButton = new QToolButton(m_leftToolBar);
	m_rectButton->setIcon(QIcon(":icons/rect.svg"));
	m_rectButton->setText("Rect");
	m_rectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_rectButton->setFixedWidth(64);

	m_ellipseButton = new QToolButton(m_leftToolBar);
	m_ellipseButton->setIcon(QIcon(":icons/ellipse.svg"));
	m_ellipseButton->setText("Ellipse");
	m_ellipseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_ellipseButton->setFixedWidth(64);

	m_starButton = new QToolButton(m_leftToolBar);
	m_starButton->setIcon(QIcon(":icons/star.svg"));
	m_starButton->setText("Star");
	m_starButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_starButton->setFixedWidth(64);

	m_leftToolBar->addWidget(m_noneButton);
	m_leftToolBar->addWidget(m_pathButton);
	m_leftToolBar->addWidget(m_lineButton);
	m_leftToolBar->addWidget(m_rectButton);
	m_leftToolBar->addWidget(m_ellipseButton);
	m_leftToolBar->addWidget(m_starButton);

	connect(m_noneButton, SIGNAL(clicked()), this, SLOT(drawNone()));
	connect(m_pathButton, SIGNAL(clicked()), this, SLOT(drawPath()));
	connect(m_lineButton, SIGNAL(clicked()), this, SLOT(drawLine()));
	connect(m_rectButton, SIGNAL(clicked()), this, SLOT(drawRect()));
	connect(m_ellipseButton, SIGNAL(clicked()), this, SLOT(drawEllipse()));
	connect(m_starButton, SIGNAL(clicked()), this, SLOT(drawStar()));
}

void MainWindow::initRightToolBar()
{
	m_rightToolBar = new QToolBar(this);
	m_rightToolBar->setMovable(false);
	this->addToolBar(Qt::RightToolBarArea, m_rightToolBar);

	m_toolsPanelWidget = new QStackedWidget();
	m_rightToolBar->addWidget(m_toolsPanelWidget);
}

void MainWindow::initCanvas()
{
	m_view = new SVGCanvasView(m_scene, this);
	this->setCentralWidget(m_view);
}

void MainWindow::drawNone()
{
	m_view->setItemType(LCanvasItem::None);
}

void MainWindow::drawPath()
{
	m_view->setItemType(LCanvasItem::None);
}

void MainWindow::drawLine()
{
	m_view->setItemType(LCanvasItem::Line);
}

void MainWindow::drawRect()
{
	m_view->setItemType(LCanvasItem::Rectangle);
}

void MainWindow::drawEllipse()
{
	m_view->setItemType(LCanvasItem::Ellipse);
}

void MainWindow::drawStar()
{
	m_view->setItemType(LCanvasItem::None);
}

void MainWindow::drawText()
{
	m_view->setItemType(LCanvasItem::None);
}
