#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_mainMenuBar(nullptr)
	, m_leftToolBar(nullptr)
	, m_rightToolBar(nullptr)
	, m_centralWidget(nullptr)
	, m_canvas(nullptr)
	, m_canvasColorButton(nullptr)
	, m_canvasWidth(400)
	, m_canvasHeight(400)
{
	this->setWindowTitle(QStringLiteral("SVG Editor"));
	this->setWindowIcon(QIcon(QStringLiteral(":icons/logo.svg")));
	this->setMinimumSize(800, 640);

	QFile file(QStringLiteral(":qss/light.qss"));
	file.open(QFile::ReadOnly);
	this->setStyleSheet(file.readAll());
	file.close();

	initUI();
}

void MainWindow::initUI()
{
	initCanvas();
	initMenuBar();
	initLeftToolBar();
	initRightToolBar();
}

void MainWindow::initMenuBar()
{
	m_mainMenuBar = new QMenuBar(this);
	this->setMenuBar(m_mainMenuBar);

	QMenu *fileMenu = new QMenu(QStringLiteral("File"), m_mainMenuBar);
	QAction *newFileAction = new QAction(QStringLiteral("New"), fileMenu);
	newFileAction->setIcon(QIcon(QStringLiteral(":icons/new.svg")));
	newFileAction->setShortcut(QKeySequence::New);
	fileMenu->addAction(newFileAction);
	QAction *openFileAction = new QAction(QStringLiteral("Open"), fileMenu);
	openFileAction->setIcon(QIcon(QStringLiteral(":icons/open.svg")));
	openFileAction->setShortcut(QKeySequence::Open);
	fileMenu->addAction(openFileAction);
	QAction *saveFileAction = new QAction(QStringLiteral("Save"), fileMenu);
	saveFileAction->setIcon(QIcon(QStringLiteral(":icons/save.svg")));
	saveFileAction->setShortcut(QKeySequence::Save);
	fileMenu->addAction(saveFileAction);
	QAction *saveAsFileAction = new QAction(QStringLiteral("Save As"), fileMenu);
	saveAsFileAction->setIcon(QIcon(QStringLiteral(":icons/save-as.svg")));
	saveAsFileAction->setShortcut(QKeySequence::SaveAs);
	fileMenu->addAction(saveAsFileAction);
	m_mainMenuBar->addAction(fileMenu->menuAction());

	QMenu *editMenu = new QMenu(QStringLiteral("Edit"), m_mainMenuBar);
	QAction *undoEditAction = new QAction(QStringLiteral("Undo"), editMenu);
	undoEditAction->setIcon(QIcon(QStringLiteral(":icons/undo.svg")));
	undoEditAction->setShortcut(QKeySequence::Undo);
	editMenu->addAction(undoEditAction);
	QAction *redoEditAction = new QAction(QStringLiteral("Redo"), editMenu);
	redoEditAction->setIcon(QIcon(QStringLiteral(":icons/redo.svg")));
	redoEditAction->setShortcut(QKeySequence::Redo);
	editMenu->addAction(redoEditAction);
	editMenu->addSeparator();
	QAction *cutEditAction = new QAction(QStringLiteral("Cut"), editMenu);
	cutEditAction->setIcon(QIcon(QStringLiteral(":icons/cut.svg")));
	cutEditAction->setShortcut(QKeySequence::Cut);
	editMenu->addAction(cutEditAction);
	QAction *copyEditAction = new QAction(QStringLiteral("Copy"), editMenu);
	copyEditAction->setIcon(QIcon(QStringLiteral(":icons/copy.svg")));
	copyEditAction->setShortcut(QKeySequence::Copy);
	editMenu->addAction(copyEditAction);
	QAction *pasteEditAction = new QAction(QStringLiteral("Paste"), editMenu);
	pasteEditAction->setIcon(QIcon(QStringLiteral(":icons/paste.svg")));
	pasteEditAction->setShortcut(QKeySequence::Paste);
	editMenu->addAction(pasteEditAction);
	QAction *deleteEditAction = new QAction(QStringLiteral("Delete"), editMenu);
	deleteEditAction->setIcon(QIcon(QStringLiteral(":icons/delete.svg")));
	deleteEditAction->setShortcut(QKeySequence::Delete);
	editMenu->addAction(deleteEditAction);
	m_mainMenuBar->addAction(editMenu->menuAction());

	QMenu *objectMenu = new QMenu(QStringLiteral("Object"), m_mainMenuBar);
	QAction *moveTopObjectAction = new QAction(QStringLiteral("Move Top"), objectMenu);
	objectMenu->addAction(moveTopObjectAction);
	QAction *moveUpObjectAction = new QAction(QStringLiteral("Move Up"), objectMenu);
	objectMenu->addAction(moveUpObjectAction);
	QAction *moveDownObjectAction = new QAction(QStringLiteral("Move Down"), objectMenu);
	objectMenu->addAction(moveDownObjectAction);
	QAction *moveBottomObjectAction = new QAction(QStringLiteral("Move Bottom"), objectMenu);
	objectMenu->addAction(moveBottomObjectAction);
	m_mainMenuBar->addAction(objectMenu->menuAction());

	QMenu *viewMenu = new QMenu(QStringLiteral("View"), m_mainMenuBar);
	QAction *showRulesViewAction = new QAction(QStringLiteral("Show Rules"), viewMenu);
	showRulesViewAction->setCheckable(true);
	viewMenu->addAction(showRulesViewAction);
	m_mainMenuBar->addAction(viewMenu->menuAction());

	connect(newFileAction, SIGNAL(triggered()), this, SLOT(onNewFile()));
	connect(openFileAction, SIGNAL(triggered()), this, SLOT(onOpenFile()));
	connect(saveFileAction, SIGNAL(triggered()), this, SLOT(onSaveFile()));

	connect(cutEditAction, SIGNAL(triggered()), m_canvas, SLOT(cutItem()));
	connect(copyEditAction, SIGNAL(triggered()), m_canvas, SLOT(copyItem()));
	connect(pasteEditAction, SIGNAL(triggered()), m_canvas, SLOT(pasteItem()));
	connect(deleteEditAction, SIGNAL(triggered()), m_canvas, SLOT(deleteItem()));
}

void MainWindow::initLeftToolBar()
{
	m_leftToolBar = new QToolBar(this);
	m_leftToolBar->setMovable(false);
	this->addToolBar(Qt::LeftToolBarArea, m_leftToolBar);

	QToolButton *noneButton = new QToolButton(this);
	noneButton->setIcon(QIcon(QStringLiteral(":icons/none.svg")));
	noneButton->setText(QStringLiteral("None"));
	noneButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(64);

	QToolButton *pathButton = new QToolButton(this);
	pathButton->setIcon(QIcon(QStringLiteral(":icons/path.svg")));
	pathButton->setText(QStringLiteral("Path"));
	pathButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pathButton->setFixedWidth(64);

	QToolButton *lineButton = new QToolButton(this);
	lineButton->setIcon(QIcon(QStringLiteral(":icons/line.svg")));
	lineButton->setText(QStringLiteral("Line"));
	lineButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	lineButton->setFixedWidth(64);

	QToolButton *rectButton = new QToolButton(this);
	rectButton->setIcon(QIcon(QStringLiteral(":icons/rect.svg")));
	rectButton->setText(QStringLiteral("Rect"));
	rectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	rectButton->setFixedWidth(64);

	QToolButton *ellipseButton = new QToolButton(this);
	ellipseButton->setIcon(QIcon(QStringLiteral(":icons/ellipse.svg")));
	ellipseButton->setText(QStringLiteral("Ellipse"));
	ellipseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ellipseButton->setFixedWidth(64);

	QToolButton *triangleButton = new QToolButton(this);
	triangleButton->setIcon(QIcon(QStringLiteral(":icons/triangle.svg")));
	triangleButton->setText(QStringLiteral("Triangle"));
	triangleButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	triangleButton->setFixedWidth(64);

	QToolButton *hexagonButton = new QToolButton(this);
	hexagonButton->setIcon(QIcon(QStringLiteral(":icons/hexagon.svg")));
	hexagonButton->setText(QStringLiteral("Hexagon"));
	hexagonButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	hexagonButton->setFixedWidth(64);

	QToolButton *textButton = new QToolButton(this);
	textButton->setIcon(QIcon(QStringLiteral(":icons/text.svg")));
	textButton->setText(QStringLiteral("Text"));
	textButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	textButton->setFixedWidth(64);

	QToolButton *paletteButton = new QToolButton(this);
	paletteButton->setIcon(QIcon(QStringLiteral(":icons/palette.svg")));
	paletteButton->setText(QStringLiteral("Palette"));
	paletteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	paletteButton->setFixedWidth(64);

	QToolButton *clearButton = new QToolButton(this);
	clearButton->setIcon(QIcon(QStringLiteral(":icons/clear.svg")));
	clearButton->setText(QStringLiteral("Clear"));
	clearButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	clearButton->setFixedWidth(64);

	m_leftToolBar->addWidget(noneButton);
	m_leftToolBar->addWidget(pathButton);
	m_leftToolBar->addWidget(lineButton);
	m_leftToolBar->addWidget(rectButton);
	m_leftToolBar->addWidget(ellipseButton);
	m_leftToolBar->addWidget(triangleButton);
	m_leftToolBar->addWidget(hexagonButton);
	m_leftToolBar->addWidget(textButton);
	m_leftToolBar->addWidget(paletteButton);
	m_leftToolBar->addWidget(clearButton);

	connect(noneButton, SIGNAL(clicked()), this, SLOT(onPaintNone()));
	connect(pathButton, SIGNAL(clicked()), this, SLOT(onPaintPath()));
	connect(lineButton, SIGNAL(clicked()), this, SLOT(onPaintLine()));
	connect(rectButton, SIGNAL(clicked()), this, SLOT(onPaintRect()));
	connect(ellipseButton, SIGNAL(clicked()), this, SLOT(onPaintEllipse()));
	connect(triangleButton, SIGNAL(clicked()), this, SLOT(onPaintTriangle()));
	connect(hexagonButton, SIGNAL(clicked()), this, SLOT(onPaintHexagon()));
	connect(textButton, SIGNAL(clicked()), this, SLOT(onPaintText()));
	connect(paletteButton, SIGNAL(clicked()), this, SLOT(setStrokeColor()));
	connect(clearButton, SIGNAL(clicked()), this, SLOT(onNewFile()));
}

void MainWindow::initRightToolBar()
{
	m_rightToolBar = new QToolBar(this);
	m_rightToolBar->setMovable(false);
	this->addToolBar(Qt::RightToolBarArea, m_rightToolBar);

	QWidget *toolsPanelWidget = new QWidget(m_rightToolBar);
	m_rightToolBar->addWidget(toolsPanelWidget);

	QVBoxLayout *mainLayout = new QVBoxLayout(toolsPanelWidget);

	QLabel *canvasColorLabel = new QLabel(QStringLiteral("Canvas Color"), toolsPanelWidget);
	m_canvasColorButton = new QPushButton(toolsPanelWidget);
	m_canvasColorButton->setFixedSize(64, 32);
	m_canvasColorButton->setStyleSheet(QStringLiteral("background-color: white; border-radius: 8px"));
	QHBoxLayout *canvasColorLayout = new QHBoxLayout();
	canvasColorLayout->addWidget(canvasColorLabel);
	canvasColorLayout->addWidget(m_canvasColorButton);
	mainLayout->addLayout(canvasColorLayout);

	QLabel *canvasWidthLabel = new QLabel(QStringLiteral("Canvas Width"), toolsPanelWidget);
	QSpinBox *canvasWidthSpinBox = new QSpinBox(toolsPanelWidget);
	canvasWidthSpinBox->setFixedSize(64, 32);
	canvasWidthSpinBox->setRange(100, 1600);
	canvasWidthSpinBox->setSingleStep(10);
	canvasWidthSpinBox->setValue(400);
	QHBoxLayout *canvasWidthLayout = new QHBoxLayout();
	canvasWidthLayout->addWidget(canvasWidthLabel);
	canvasWidthLayout->addWidget(canvasWidthSpinBox);
	mainLayout->addLayout(canvasWidthLayout);

	QLabel *canvasHeightLabel = new QLabel(QStringLiteral("Canvas Height"), toolsPanelWidget);
	QSpinBox *canvasHeightSpinBox = new QSpinBox(toolsPanelWidget);
	canvasHeightSpinBox->setFixedSize(64, 32);
	canvasHeightSpinBox->setRange(100, 1600);
	canvasHeightSpinBox->setSingleStep(10);
	canvasHeightSpinBox->setValue(400);
	QHBoxLayout *canvasHeightLayout = new QHBoxLayout();
	canvasHeightLayout->addWidget(canvasHeightLabel);
	canvasHeightLayout->addWidget(canvasHeightSpinBox);
	mainLayout->addLayout(canvasHeightLayout);

	QLabel *strokeWidthLabel = new QLabel(QStringLiteral("Stroke Width"), toolsPanelWidget);
	QSpinBox *strokeWidthSpinBox = new QSpinBox(toolsPanelWidget);
	strokeWidthSpinBox->setFixedSize(64, 32);
	strokeWidthSpinBox->setRange(1, 10);
	QHBoxLayout *strokeWidthLayout = new QHBoxLayout();
	strokeWidthLayout->addWidget(strokeWidthLabel);
	strokeWidthLayout->addWidget(strokeWidthSpinBox);
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
	m_canvas->resize(400,400);
	m_canvas->setMinimumSize(QSize(100, 100));
	m_canvas->setMaximumSize(QSize(1600, 1600));
	m_centralWidget->setWidget(m_canvas);

	if (!m_canvasColor.isValid())
	{
		m_canvas->setStyleSheet(QStringLiteral("background-color: white") +
								QStringLiteral("; border-radius: 8px"));
	}
	else
	{
		m_canvas->setStyleSheet(QStringLiteral("background-color: ") + m_canvasColor.name() +
								QStringLiteral("; border-radius: 8px"));
	}

	connect(this, SIGNAL(changeItemType(LCanvasItem::ItemType)), m_canvas, SLOT(setItemType(LCanvasItem::ItemType)));
	connect(this, SIGNAL(sigReadItemsFromFile(const QString &)), m_canvas, SLOT(readItemsFromFile(const QString &)));
	connect(this, SIGNAL(sigWriteItemsToFile(const QString &)), m_canvas, SLOT(writeItemsToFile(const QString &)));
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
	if (!m_canvas)
		return;

	if (m_canvas->noItems())
	{
		int result = QMessageBox::warning(
					this, tr("New File"), tr("Do you want to clear the canvas?"),
					QMessageBox::Cancel | QMessageBox::Apply, QMessageBox::Cancel);

		if (result == QMessageBox::Cancel)
			return;
	}

	m_canvas->clearCanvas();
}

void MainWindow::onOpenFile()
{
	QString filePath = QFileDialog::getOpenFileName(
				this, QStringLiteral("Open File"), QStringLiteral(), QStringLiteral("SVG FILES(*.svg)"));

	if (!filePath.isEmpty())
		emit sigReadItemsFromFile(filePath);
}

void MainWindow::onSaveFile()
{
	QString filePath = QFileDialog::getSaveFileName(
				this, QStringLiteral("Save File"), QStringLiteral(), QStringLiteral("SVG FILES(*.svg)"));

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
		m_canvasColorButton->setStyleSheet(QStringLiteral("background-color: ") + getColor.name() +
										   QStringLiteral("border-radius: 8px"));
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
