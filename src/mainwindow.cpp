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
	this->setWindowTitle(tr("SVG Editor"));
	this->setWindowIcon(QIcon(QStringLiteral(":icons/logo.svg")));
	this->setMinimumSize(1000, 750);
	this->setContextMenuPolicy(Qt::NoContextMenu);

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
	initBottomToolBar();
}

void MainWindow::initCanvas()
{
	m_centralWidget = new QScrollArea(this);
	m_centralWidget->setAlignment(Qt::AlignCenter);
	this->setCentralWidget(m_centralWidget);

	m_canvas = new LCanvasView(m_centralWidget);
	m_centralWidget->setWidget(m_canvas);

	connect(this, SIGNAL(changeItemType(ItemType)), m_canvas, SLOT(setItemType(ItemType)));
	connect(this, SIGNAL(sigReadItemsFromFile(QString)), m_canvas, SLOT(readItemsFromFile(QString)));
	connect(this, SIGNAL(sigWriteItemsToFile(QString)), m_canvas, SLOT(writeItemsToFile(QString)));
}

void MainWindow::initMenuBar()
{
	m_mainMenuBar = new QMenuBar(this);
	this->setMenuBar(m_mainMenuBar);

	// file menu
	QMenu *fileMenu = new QMenu(tr("File"), m_mainMenuBar);

	QAction *newFileAction = new QAction(tr("New"), fileMenu);
	newFileAction->setIcon(QIcon(QStringLiteral(":icons/new.svg")));
	newFileAction->setShortcut(QKeySequence::New);

	QAction *openFileAction = new QAction(tr("Open"), fileMenu);
	openFileAction->setIcon(QIcon(QStringLiteral(":icons/open.svg")));
	openFileAction->setShortcut(QKeySequence::Open);

	QAction *saveFileAction = new QAction(tr("Save"), fileMenu);
	saveFileAction->setIcon(QIcon(QStringLiteral(":icons/save.svg")));
	saveFileAction->setShortcut(QKeySequence::Save);

	QAction *saveAsFileAction = new QAction(tr("Save As"), fileMenu);
	saveAsFileAction->setIcon(QIcon(QStringLiteral(":icons/save-as.svg")));
	saveAsFileAction->setShortcut(QKeySequence::SaveAs);

	m_mainMenuBar->addAction(fileMenu->menuAction());
	fileMenu->addAction(newFileAction);
	fileMenu->addAction(openFileAction);
	fileMenu->addAction(saveFileAction);
	fileMenu->addAction(saveAsFileAction);

	// edit menu
	QMenu *editMenu = new QMenu(tr("Edit"), m_mainMenuBar);

	QAction *undoEditAction = new QAction(tr("Undo"), editMenu);
	undoEditAction->setIcon(QIcon(QStringLiteral(":icons/undo.svg")));
	undoEditAction->setShortcut(QKeySequence::Undo);

	QAction *redoEditAction = new QAction(tr("Redo"), editMenu);
	redoEditAction->setIcon(QIcon(QStringLiteral(":icons/redo.svg")));
	redoEditAction->setShortcut(QKeySequence::Redo);

	QAction *cutEditAction = new QAction(tr("Cut"), editMenu);
	cutEditAction->setIcon(QIcon(QStringLiteral(":icons/cut.svg")));
	cutEditAction->setShortcut(QKeySequence::Cut);

	QAction *copyEditAction = new QAction(tr("Copy"), editMenu);
	copyEditAction->setIcon(QIcon(QStringLiteral(":icons/copy.svg")));
	copyEditAction->setShortcut(QKeySequence::Copy);

	QAction *pasteEditAction = new QAction(tr("Paste"), editMenu);
	pasteEditAction->setIcon(QIcon(QStringLiteral(":icons/paste.svg")));
	pasteEditAction->setShortcut(QKeySequence::Paste);

	QAction *deleteEditAction = new QAction(tr("Delete"), editMenu);
	deleteEditAction->setIcon(QIcon(QStringLiteral(":icons/delete.svg")));
	deleteEditAction->setShortcut(QKeySequence::Delete);

	m_mainMenuBar->addAction(editMenu->menuAction());
	editMenu->addAction(undoEditAction);
	editMenu->addAction(redoEditAction);
	editMenu->addSeparator();
	editMenu->addAction(cutEditAction);
	editMenu->addAction(copyEditAction);
	editMenu->addAction(pasteEditAction);
	editMenu->addAction(deleteEditAction);

	// object menu
	QMenu *objectMenu = new QMenu(tr("Object"), m_mainMenuBar);

	QAction *moveTopObjectAction = new QAction(tr("Move Top"), objectMenu);

	QAction *moveUpObjectAction = new QAction(tr("Move Up"), objectMenu);

	QAction *moveDownObjectAction = new QAction(tr("Move Down"), objectMenu);

	QAction *moveBottomObjectAction = new QAction(tr("Move Bottom"), objectMenu);

	m_mainMenuBar->addAction(objectMenu->menuAction());
	objectMenu->addAction(moveTopObjectAction);
	objectMenu->addAction(moveUpObjectAction);
	objectMenu->addAction(moveDownObjectAction);
	objectMenu->addAction(moveBottomObjectAction);

	// view menu
	QMenu *viewMenu = new QMenu(tr("View"), m_mainMenuBar);

	QAction *showRulesViewAction = new QAction(tr("Show Rules"), viewMenu);
	showRulesViewAction->setCheckable(true);

	m_mainMenuBar->addAction(viewMenu->menuAction());
	viewMenu->addAction(showRulesViewAction);

	// init connect
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

	QToolButton *noneButton = new QToolButton(m_leftToolBar);
	noneButton->setIcon(QIcon(QStringLiteral(":icons/none.svg")));
	noneButton->setText(tr("None"));
	noneButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	noneButton->setFixedWidth(64);

	QToolButton *pathButton = new QToolButton(m_leftToolBar);
	pathButton->setIcon(QIcon(QStringLiteral(":icons/path.svg")));
	pathButton->setText(tr("Path"));
	pathButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pathButton->setFixedWidth(64);

	QToolButton *lineButton = new QToolButton(m_leftToolBar);
	lineButton->setIcon(QIcon(QStringLiteral(":icons/line.svg")));
	lineButton->setText(tr("Line"));
	lineButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	lineButton->setFixedWidth(64);

	QToolButton *rectButton = new QToolButton(m_leftToolBar);
	rectButton->setIcon(QIcon(QStringLiteral(":icons/rect.svg")));
	rectButton->setText(tr("Rect"));
	rectButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	rectButton->setFixedWidth(64);

	QToolButton *ellipseButton = new QToolButton(m_leftToolBar);
	ellipseButton->setIcon(QIcon(QStringLiteral(":icons/ellipse.svg")));
	ellipseButton->setText(tr("Ellipse"));
	ellipseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ellipseButton->setFixedWidth(64);

	QToolButton *triangleButton = new QToolButton(m_leftToolBar);
	triangleButton->setIcon(QIcon(QStringLiteral(":icons/triangle.svg")));
	triangleButton->setText(tr("Triangle"));
	triangleButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	triangleButton->setFixedWidth(64);

	QToolButton *hexagonButton = new QToolButton(m_leftToolBar);
	hexagonButton->setIcon(QIcon(QStringLiteral(":icons/hexagon.svg")));
	hexagonButton->setText(tr("Hexagon"));
	hexagonButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	hexagonButton->setFixedWidth(64);

	QToolButton *textButton = new QToolButton(m_leftToolBar);
	textButton->setIcon(QIcon(QStringLiteral(":icons/text.svg")));
	textButton->setText(tr("Text"));
	textButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	textButton->setFixedWidth(64);

	QToolButton *paletteButton = new QToolButton(m_leftToolBar);
	paletteButton->setIcon(QIcon(QStringLiteral(":icons/palette.svg")));
	paletteButton->setText(tr("Palette"));
	paletteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	paletteButton->setFixedWidth(64);

	QToolButton *clearButton = new QToolButton(m_leftToolBar);
	clearButton->setIcon(QIcon(QStringLiteral(":icons/clear.svg")));
	clearButton->setText(tr("Clear"));
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

	QLabel *canvasColorLabel = new QLabel(tr("Canvas Color"), toolsPanelWidget);
	m_canvasColorButton = new QPushButton(toolsPanelWidget);
	m_canvasColorButton->setFixedSize(64, 32);
	m_canvasColorButton->setStyleSheet(QStringLiteral("background-color: white; border-radius: 8px"));
	QHBoxLayout *canvasColorLayout = new QHBoxLayout();
	canvasColorLayout->addWidget(canvasColorLabel);
	canvasColorLayout->addWidget(m_canvasColorButton);
	mainLayout->addLayout(canvasColorLayout);

	QLabel *canvasWidthLabel = new QLabel(tr("Canvas Width"), toolsPanelWidget);
	QSpinBox *canvasWidthSpinBox = new QSpinBox(toolsPanelWidget);
	canvasWidthSpinBox->setFixedSize(64, 32);
	canvasWidthSpinBox->setRange(100, 2000);
	canvasWidthSpinBox->setSingleStep(10);
	canvasWidthSpinBox->setValue(500);
	canvasWidthSpinBox->setContextMenuPolicy(Qt::NoContextMenu);
	QHBoxLayout *canvasWidthLayout = new QHBoxLayout();
	canvasWidthLayout->addWidget(canvasWidthLabel);
	canvasWidthLayout->addWidget(canvasWidthSpinBox);
	mainLayout->addLayout(canvasWidthLayout);

	QLabel *canvasHeightLabel = new QLabel(tr("Canvas Height"), toolsPanelWidget);
	QSpinBox *canvasHeightSpinBox = new QSpinBox(toolsPanelWidget);
	canvasHeightSpinBox->setFixedSize(64, 32);
	canvasHeightSpinBox->setRange(100, 2000);
	canvasHeightSpinBox->setSingleStep(10);
	canvasHeightSpinBox->setValue(500);
	canvasHeightSpinBox->setContextMenuPolicy(Qt::NoContextMenu);
	QHBoxLayout *canvasHeightLayout = new QHBoxLayout();
	canvasHeightLayout->addWidget(canvasHeightLabel);
	canvasHeightLayout->addWidget(canvasHeightSpinBox);
	mainLayout->addLayout(canvasHeightLayout);

	QLabel *strokeWidthLabel = new QLabel(tr("Stroke Width"), toolsPanelWidget);
	QSpinBox *strokeWidthSpinBox = new QSpinBox(toolsPanelWidget);
	strokeWidthSpinBox->setFixedSize(64, 32);
	strokeWidthSpinBox->setRange(1, 10);
	strokeWidthSpinBox->setContextMenuPolicy(Qt::NoContextMenu);
	QHBoxLayout *strokeWidthLayout = new QHBoxLayout();
	strokeWidthLayout->addWidget(strokeWidthLabel);
	strokeWidthLayout->addWidget(strokeWidthSpinBox);
	mainLayout->addLayout(strokeWidthLayout);

	connect(m_canvasColorButton, SIGNAL(clicked()), this, SLOT(setCanvasColor()));
	connect(canvasWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCanvasWidth(int)));
	connect(canvasHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCanvasHeight(int)));
	connect(strokeWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setStrokeWidth(int)));
}

void MainWindow::initBottomToolBar()
{
	m_bottomToolBar = new QToolBar(this);
	m_bottomToolBar->setMovable(false);
	this->addToolBar(Qt::BottomToolBarArea, m_bottomToolBar);

	QToolButton *paletteButton = new QToolButton(m_bottomToolBar);
	paletteButton->setIcon(QIcon(QStringLiteral(":icons/palette.svg")));
	paletteButton->setText(tr("Palette"));
	paletteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	paletteButton->setFixedWidth(64);

	m_bottomToolBar->addWidget(paletteButton);

	connect(paletteButton, SIGNAL(clicked()), this, SLOT(setFillColor()));
}

void MainWindow::onPaintNone()
{
	emit changeItemType(ItemType::NoneType);
}

void MainWindow::onPaintPath()
{
	emit changeItemType(ItemType::Path);
}

void MainWindow::onPaintLine()
{
	emit changeItemType(ItemType::Line);
}

void MainWindow::onPaintRect()
{
	emit changeItemType(ItemType::Rect);
}

void MainWindow::onPaintEllipse()
{
	emit changeItemType(ItemType::Ellipse);
}

void MainWindow::onPaintTriangle()
{
	emit changeItemType(ItemType::Triangle);
}

void MainWindow::onPaintHexagon()
{
	emit changeItemType(ItemType::Hexagon);
}

void MainWindow::onPaintText()
{
	emit changeItemType(ItemType::Text);
}

void MainWindow::onNewFile()
{
	if (!m_canvas)
		return;

	if (m_canvas->existItems())
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
				this, tr("Open File"), QStringLiteral(), tr("SVG FILES(*.svg)"));

	if (!filePath.isEmpty())
		emit sigReadItemsFromFile(filePath);
}

void MainWindow::onSaveFile()
{
	QString filePath = QFileDialog::getSaveFileName(
				this, tr("Save File"), QStringLiteral(), tr("SVG FILES(*.svg)"));

	if (!filePath.isEmpty())
		emit sigWriteItemsToFile(filePath);
}

void MainWindow::setCanvasColor()
{
	QColor color = QColorDialog::getColor();
	if (color.isValid() && m_canvasColor != color)
	{
		m_canvasColor = color;
		m_canvas->setCanvasColor(color);
		m_canvasColorButton->setStyleSheet(QStringLiteral("background-color: ") + color.name() +
										   QStringLiteral("; border-radius: 8px"));
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

void MainWindow::setFillColor()
{
	QColor color = QColorDialog::getColor();
	if (color.isValid())
		m_canvas->setFillColor(color);
}

void MainWindow::setStrokeColor()
{
	QColor color = QColorDialog::getColor();
	if (color.isValid())
		m_canvas->setStrokeColor(color);
}

void MainWindow::setStrokeWidth(int width)
{
	m_canvas->setStrokeWidth(width);
}
