#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_mainMenuBar(nullptr)
	, m_leftToolBar(nullptr)
	, m_rightToolBar(nullptr)
	, m_centralWidget(nullptr)
	, m_canvas(nullptr)
	, m_canvasColorButton(nullptr)
	, m_canvasWidth(500)
	, m_canvasHeight(500)
{
	initUI();
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

	QMenu *fileMenu = new QMenu("File", m_mainMenuBar);
	QAction *newFileAction = new QAction("New", fileMenu);
	newFileAction->setShortcut(QKeySequence::New);
	fileMenu->addAction(newFileAction);
	QAction *openFileAction = new QAction("Open", fileMenu);
	openFileAction->setShortcut(QKeySequence::Open);
	fileMenu->addAction(openFileAction);
	QAction *saveFileAction = new QAction("Save", fileMenu);
	saveFileAction->setShortcut(QKeySequence::Save);
	fileMenu->addAction(saveFileAction);
	QAction *saveAsFileAction = new QAction("Save As", fileMenu);
	saveAsFileAction->setShortcut(QKeySequence::SaveAs);
	fileMenu->addAction(saveAsFileAction);
	m_mainMenuBar->addAction(fileMenu->menuAction());

	QMenu *editMenu = new QMenu("Edit", m_mainMenuBar);
	QAction *undoEditAction = new QAction("Undo", editMenu);
	undoEditAction->setShortcut(QKeySequence::Undo);
	editMenu->addAction(undoEditAction);
	QAction *redoEditAction = new QAction("Redo", editMenu);
	redoEditAction->setShortcut(QKeySequence::Redo);
	editMenu->addAction(redoEditAction);
	editMenu->addSeparator();
	QAction *cutEditAction = new QAction("Cut", editMenu);
	cutEditAction->setShortcut(QKeySequence::Cut);
	editMenu->addAction(cutEditAction);
	QAction *copyEditAction = new QAction("Copy", editMenu);
	copyEditAction->setShortcut(QKeySequence::Copy);
	editMenu->addAction(copyEditAction);
	QAction *pasteEditAction = new QAction("Paste", editMenu);
	pasteEditAction->setShortcut(QKeySequence::Paste);
	editMenu->addAction(pasteEditAction);
	QAction *deleteEditAction = new QAction("Delete", editMenu);
	deleteEditAction->setShortcut(QKeySequence::Delete);
	editMenu->addAction(deleteEditAction);
	m_mainMenuBar->addAction(editMenu->menuAction());

	QMenu *objectMenu = new QMenu("Object", m_mainMenuBar);
	QAction *moveTopObjectAction = new QAction("Move Top", objectMenu);
	objectMenu->addAction(moveTopObjectAction);
	QAction *moveUpObjectAction = new QAction("Move Up", objectMenu);
	objectMenu->addAction(moveUpObjectAction);
	QAction *moveDownObjectAction = new QAction("Move Down", objectMenu);
	objectMenu->addAction(moveDownObjectAction);
	QAction *moveBottomObjectAction = new QAction("Move Bottom", objectMenu);
	objectMenu->addAction(moveBottomObjectAction);
	m_mainMenuBar->addAction(objectMenu->menuAction());

	QMenu *viewMenu = new QMenu("View", m_mainMenuBar);
	QAction *showRulesViewAction = new QAction("Show Rules", viewMenu);
	showRulesViewAction->setCheckable(true);
	viewMenu->addAction(showRulesViewAction);
	m_mainMenuBar->addAction(viewMenu->menuAction());

	connect(newFileAction, SIGNAL(triggered()), this, SLOT(onNewFile()));
	connect(openFileAction, SIGNAL(triggered()), this, SLOT(onOpenFile()));
	connect(saveFileAction, SIGNAL(triggered()), this, SLOT(onSaveFile()));
}

void MainWindow::initLeftToolBar()
{
	m_leftToolBar = new QToolBar(this);
	m_leftToolBar->setMovable(false);
	this->addToolBar(Qt::LeftToolBarArea, m_leftToolBar);

	QToolButton *noneButton = new QToolButton(this);
	noneButton->setIcon(QIcon(":icons/none.svg"));
	noneButton->setIconSize(QSize(32, 32));
	noneButton->setText("None");
	noneButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* paletteButton = new QToolButton(this);
	paletteButton->setIcon(QIcon(":icons/palette.svg"));
	paletteButton->setIconSize(QSize(32, 32));
	paletteButton->setText("Palette");
	paletteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* pathButton = new QToolButton(this);
	pathButton->setIcon(QIcon(":icons/path.svg"));
	pathButton->setIconSize(QSize(32, 32));
	pathButton->setText("Path");
	pathButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* lineButton = new QToolButton(this);
	lineButton->setIcon(QIcon(":icons/line.svg"));
	lineButton->setIconSize(QSize(32, 32));
	lineButton->setText("Line");
	lineButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* rectButton = new QToolButton(this);
	rectButton->setIcon(QIcon(":icons/rect.svg"));
	rectButton->setIconSize(QSize(32, 32));
	rectButton->setText("Rect");
	rectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* ellipseButton = new QToolButton(this);
	ellipseButton->setIcon(QIcon(":icons/ellipse.svg"));
	ellipseButton->setIconSize(QSize(32, 32));
	ellipseButton->setText("Ellipse");
	ellipseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* triangleButton = new QToolButton(this);
	triangleButton->setIcon(QIcon(":icons/triangle.svg"));
	triangleButton->setIconSize(QSize(32, 32));
	triangleButton->setText("Triangle");
	triangleButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton* hexagonButton = new QToolButton(this);
	hexagonButton->setIcon(QIcon(":icons/hexagon.svg"));
	hexagonButton->setIconSize(QSize(32, 32));
	hexagonButton->setText("Hexagon");
	hexagonButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton *textButton = new QToolButton(this);
	textButton->setIcon(QIcon(":icons/text.svg"));
	textButton->setIconSize(QSize(32, 32));
	textButton->setText("Text");
	textButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	QToolButton *clearButton = new QToolButton(this);
	clearButton->setIcon(QIcon(":icons/clear.svg"));
	clearButton->setIconSize(QSize(32, 32));
	clearButton->setText("Clear");
	clearButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(32);

	m_leftToolBar->addWidget(noneButton);
	m_leftToolBar->addWidget(paletteButton);
	m_leftToolBar->addWidget(pathButton);
	m_leftToolBar->addWidget(lineButton);
	m_leftToolBar->addWidget(rectButton);
	m_leftToolBar->addWidget(ellipseButton);
	m_leftToolBar->addWidget(triangleButton);
	m_leftToolBar->addWidget(hexagonButton);
	m_leftToolBar->addWidget(textButton);
	m_leftToolBar->addWidget(clearButton);

	connect(noneButton, SIGNAL(clicked()), this, SLOT(onPaintNone()));
	connect(paletteButton, SIGNAL(clicked()), this, SLOT(setBrushColor()));
	connect(pathButton, SIGNAL(clicked()), this, SLOT(onPaintPath()));
	connect(lineButton, SIGNAL(clicked()), this, SLOT(onPaintLine()));
	connect(rectButton, SIGNAL(clicked()), this, SLOT(onPaintRect()));
	connect(ellipseButton, SIGNAL(clicked()), this, SLOT(onPaintEllipse()));
	connect(triangleButton, SIGNAL(clicked()), this, SLOT(onPaintTriangle()));
	connect(hexagonButton, SIGNAL(clicked()), this, SLOT(onPaintHexagon()));
	connect(textButton, SIGNAL(clicked()), this, SLOT(onPaintText()));
	connect(clearButton, SIGNAL(clicked()), this, SLOT(onNewFile()));
}

void MainWindow::initRightToolBar()
{
	m_rightToolBar = new QToolBar(this);
	m_rightToolBar->setMovable(false);
	m_rightToolBar->setFixedWidth(128);
	this->addToolBar(Qt::RightToolBarArea, m_rightToolBar);

	QWidget *toolsPanelWidget = new QWidget(m_rightToolBar);
	m_rightToolBar->addWidget(toolsPanelWidget);
	QVBoxLayout* mainLayout = new QVBoxLayout(toolsPanelWidget);

	QLabel *canvasColorLabel = new QLabel("Canvas Color", toolsPanelWidget);
	m_canvasColorButton = new QPushButton(toolsPanelWidget);
	m_canvasColorButton->setFixedSize(60, 30);
	m_canvasColorButton->setStyleSheet("background-color:white; border-radius:10px");
	QHBoxLayout *canvasColorLayout = new QHBoxLayout(toolsPanelWidget);
	canvasColorLayout->addWidget(canvasColorLabel);
	canvasColorLayout->addWidget(m_canvasColorButton);

	QLabel *canvasWidthLabel = new QLabel("Canvas Width", toolsPanelWidget);
	QSpinBox *canvasWidthSpinBox = new QSpinBox(toolsPanelWidget);
	canvasWidthSpinBox->setFixedSize(60, 30);
	canvasWidthSpinBox->setRange(100, 2000);
	canvasWidthSpinBox->setSingleStep(10);
	canvasWidthSpinBox->setValue(500);
	canvasWidthSpinBox->setStyleSheet("QSpinBox{font-size:24px; border-radius:10px}"
									  "QSpinBox::up-button{width:0px}"
									  "QSpinBox::down-button{width:0px}");
	QHBoxLayout *canvasWidthLayout = new QHBoxLayout(toolsPanelWidget);
	canvasWidthLayout->addWidget(canvasWidthLabel);
	canvasWidthLayout->addWidget(canvasWidthSpinBox);

	QLabel *canvasHeightLabel = new QLabel("canvas Height", toolsPanelWidget);
	QSpinBox *canvasHeightSpinBox = new QSpinBox(toolsPanelWidget);
	canvasHeightSpinBox->setFixedSize(60, 30);
	canvasHeightSpinBox->setRange(100, 2000);
	canvasHeightSpinBox->setSingleStep(10);
	canvasHeightSpinBox->setValue(500);
	canvasHeightSpinBox->setStyleSheet("QSpinBox{font-size:24px; border-radius:10px}"
									   "QSpinBox::up-button{width:0px}"
									   "QSpinBox::down-button{width:0px}");
	QHBoxLayout *canvasHeightLayout = new QHBoxLayout(toolsPanelWidget);
	canvasHeightLayout->addWidget(canvasHeightLabel);
	canvasHeightLayout->addWidget(canvasHeightSpinBox);

	QLabel *strokeWidthLabel = new QLabel("Stroke Width", toolsPanelWidget);
	QSpinBox *strokeWidthSpinBox = new QSpinBox(toolsPanelWidget);
	strokeWidthSpinBox->setFixedSize(60, 30);
	strokeWidthSpinBox->setRange(1, 10);
	strokeWidthSpinBox->setStyleSheet("QSpinBox{font-size:24px; border-radius:10px}"
									  "QSpinBox::up-button{width:0px}"
									  "QSpinBox::down-button{width:0px}");
	QHBoxLayout *strokeWidthLayout = new QHBoxLayout(toolsPanelWidget);
	strokeWidthLayout->addWidget(strokeWidthLabel);
	strokeWidthLayout->addWidget(strokeWidthSpinBox);

	mainLayout->addLayout(canvasColorLayout);
	mainLayout->addLayout(canvasHeightLayout);
	mainLayout->addLayout(canvasWidthLayout);
	mainLayout->addLayout(strokeWidthLayout);

	connect(m_canvasColorButton, SIGNAL(clicked()), this, SLOT(setCanvasColor()));
	connect(canvasWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCanvasWidth(int)));
	connect(canvasHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCanvasHeight(int)));
	connect(strokeWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setStrokeWidth(int)));
}

void MainWindow::initCanvas()
{
	m_centralWidget = new QScrollArea(this);
	m_centralWidget->setAlignment(Qt::AlignCenter);
	this->setCentralWidget(m_centralWidget);

	m_canvas = new LCanvasView(m_centralWidget);
	m_canvas->resize(500,500);
	m_canvas->setMinimumSize(QSize(100, 100));
	m_canvas->setMaximumSize(QSize(2000, 2000));
	m_centralWidget->setWidget(m_canvas);

	if (!m_canvasColor.isValid())
	{
		m_canvas->setStyleSheet("background-color:white; border-radius:10px");
	}
	else
	{
		m_canvas->setStyleSheet("background-color:" + m_canvasColor.name() + "; border-radius:10px");
	}

	connect(this, SIGNAL(changeItemType(LCanvasItem::ItemType)), m_canvas, SLOT(setItemType(LCanvasItem::ItemType)));
	connect(this, SIGNAL(sigWriteItemsToFile(QString &)), m_canvas, SLOT(writeItemsToFile(QString &)));
	connect(this, SIGNAL(sigReadItemsFromFile(QString &)), m_canvas, SLOT(readItemsFromFile(QString &)));
}

void MainWindow::onPaintNone()
{
	emit changeItemType(LCanvasItem::None);
}

void MainWindow::onPaintPath()
{
	emit changeItemType(LCanvasItem::Path);
}

void MainWindow::onPaintLine()
{
	emit changeItemType(LCanvasItem::Line);
}

void MainWindow::onPaintRect()
{
	emit changeItemType(LCanvasItem::Rect);
}

void MainWindow::onPaintEllipse()
{
	emit changeItemType(LCanvasItem::Ellipse);
}

void MainWindow::onPaintTriangle()
{
	emit changeItemType(LCanvasItem::Triangle);
}

void MainWindow::onPaintHexagon()
{
	emit changeItemType(LCanvasItem::Hexagon);
}

void MainWindow::onPaintText()
{
	emit changeItemType(LCanvasItem::Text);
}

void MainWindow::onNewFile()
{
	if (m_canvas != nullptr && m_canvas->noItems())
	{
		int result = QMessageBox::warning(
					this, "New File", "Do you want to clear the canvas?",
					QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);

		if (result == QMessageBox::Cancel)
			return;
	}

	m_canvas->clearCanvas();
}

void MainWindow::onOpenFile()
{
	QString filePath = QFileDialog::getOpenFileName(
				this, QLatin1String("Open File"), QLatin1String(), QLatin1String("SVG FILES(*.svg)"));

	if (!filePath.isEmpty())
		emit sigReadItemsFromFile(filePath);
}

void MainWindow::onSaveFile()
{
	QString filePath = QFileDialog::getSaveFileName(
				this, QLatin1String("Save File"), QLatin1String(), QLatin1String("SVG FILES(*.svg)"));

	if (!filePath.isEmpty())
		emit sigWriteItemsToFile(filePath);
}

void MainWindow::setCanvasColor()
{
	QColorDialog color;
	QColor getColor = color.getColor();
	if (getColor.isValid())
	{
		m_canvasColor = getColor;
		m_canvasColorButton->setStyleSheet("background-color:" + getColor.name() + "; border-radius:10px");
		m_canvas->setCanvasColor(getColor);
	}
}

void MainWindow::setCanvasWidth(int width)
{
	m_canvasHeight = m_canvas->height();
	m_canvas->resize(QSize(width, m_canvasHeight));
}

void MainWindow::setCanvasHeight(int height)
{
	m_canvasWidth = m_canvas->width();
	m_canvas->resize(QSize(m_canvasWidth, height));
}

void MainWindow::setStrokeColor()
{
	QColor color = QColorDialog::getColor();
	if (color.isValid())
		m_canvas->setStrokeColor(color);
}

void MainWindow::setStrokeWidth(int size)
{
	m_canvas->setStrokeWidth(size);
}
