#include "lcanvasview.h"

namespace lwscode {

LCanvasView::LCanvasView(QWidget *parent)
	: QWidget(parent)
	, m_rightClickMenu(nullptr)
	, m_itemType(LCanvasItem::None)
	, m_item(nullptr)
	, m_canvasColor(Qt::white)
	, m_fillColor(Qt::white)
	, m_strokeColor(Qt::black)
	, m_strokeWidth(1)
	, m_bPaintingItem(false)
	, m_bPaintingPath(false)
	, m_lineEdit(nullptr)
	, m_showFrameRect(true)
	, m_mouseFrameSelection(false)
	, m_mouseFrameSelect(false)
	, m_mouseClickSelect(false)
	, m_selectedItem(nullptr)
	, m_copyItem(nullptr)
	, m_itemHitPos(LCanvasView::None)
	, m_mouseClickOutRect(false)
{
	this->setMouseTracking(true);
	this->setAttribute(Qt::WA_StyledBackground);
	initRightClickMenu();
}

LCanvasView::~LCanvasView()
{
	if (!m_allItems.empty())
	{
		for (auto &item : m_allItems)
			delete item;
	}

	if (!m_selectedItems.isEmpty())
		deselectAllItems();

	if (m_lineEdit)
	{
		delete m_lineEdit;
		m_lineEdit = nullptr;
	}
}

void LCanvasView::setCanvasColor(const QColor &color)
{
	if (color.isValid())
		m_canvasColor = color;

	this->setStyleSheet(QStringLiteral("background-color:") + m_canvasColor.name() +
						QStringLiteral("; border-radius:10px"));
}

void LCanvasView::setStrokeColor(const QColor &color)
{
	if (color.isValid())
		m_strokeColor = color;

	if (m_selectedItem)
	{
		m_selectedItem->setStrokeColor(m_strokeColor);
		update();
	}
}

void LCanvasView::setStrokeWidth(int width)
{
	m_strokeWidth = width;
	if (m_selectedItem)
	{
		m_selectedItem->setStrokeWidth(m_strokeWidth);
		update();
	}
}

void LCanvasView::clearCanvas()
{
	if (!m_allItems.isEmpty())
	{
		for (auto &item : m_allItems)
			delete item;

		m_allItems.clear();
	}
}

bool LCanvasView::noItems()
{
	return !m_allItems.isEmpty();
}

void LCanvasView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	for (int i = 0; i < m_allItems.size() && !m_allItems.isEmpty(); ++i)
	{
		m_allItems[i]->setBoundingRect();
		m_allItems[i]->paintItem(painter);
		if (m_allItems[i]->isSelected())
		{
			if (!m_selectedItems.isEmpty())
				paintRubberBand(m_allItems[i], painter);

			if (m_selectedItems.isEmpty() || m_selectedItems.size() == 1)
				paintSelectedBox(m_allItems[i], painter);
		}
	}

	if (m_showFrameRect && !m_startPos.isNull() && !m_endPos.isNull())
	{
		QPainter paint(this);
		QPen pen(Qt::DashLine);
		pen.setColor(Qt::blue);
		pen.setWidth(2);
		paint.setPen(pen);
		m_rubberBand = QRect(m_startPos, m_endPos);
		paint.drawRect(m_rubberBand);
	}
}

void LCanvasView::mousePressEvent(QMouseEvent *event)
{
	m_bPaintingItem = true;
	QPoint pos = event->pos();
	beforePaintItem(pos);

	m_itemHitPos = getItemHitPos(pos);
	switch (m_itemHitPos)
	{
	case ItemHitPos::None:
	{
		break;
	}
	case ItemHitPos::TopLeft:
	case ItemHitPos::BottomRight:
	{
		this->setCursor(Qt::SizeFDiagCursor);
		break;
	}
	case ItemHitPos::TopMiddle:
	case ItemHitPos::BottomMiddle:
	{
		this->setCursor(Qt::SizeVerCursor);
		break;
	}
	case ItemHitPos::TopRight:
	case ItemHitPos::BottomLeft:
	{
		this->setCursor(Qt::SizeBDiagCursor);
		break;
	}
	case ItemHitPos::MiddleRight:
	case ItemHitPos::MiddleLeft:
	{
		this->setCursor(Qt::SizeHorCursor);
		break;
	}
	}

	if (m_itemHitPos != ItemHitPos::None)
	{
		m_mouseClickOutRect = true;
		m_showFrameRect = false;
		m_lastPos = pos;
		return;
	}
	if (!m_item)
	{
		if (m_mouseFrameSelection && !m_selectedItems.isEmpty() && !m_mouseFrameSelect)
		{
			selectItemsByFrame(pos);
		}
		else if(!m_mouseFrameSelect && m_selectedItems.isEmpty())
		{
			selectItemsByClick(pos);
		}
		update();
	}
	else if (m_item && m_itemType == LCanvasItem::Path)
	{
		reselectItems();
		m_bPaintingPath = true;
		m_item->setStartPos(pos);
		m_item->addPoint(pos);
		m_allItems << m_item;
		update();
		return;
	}
	else if (m_item != nullptr)
	{
		reselectItems();
		m_bPaintingPath = false;
		m_item->setStartPos(pos);
		m_item->setEndPos(pos);
		m_allItems << m_item;
		if (m_itemType == LCanvasItem::Text)
			m_textItems << m_item;
	}

	m_endPos = pos;
}

void LCanvasView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;
	if (m_mouseClickOutRect && m_itemHitPos != ItemHitPos::None && m_itemType == LCanvasItem::None)
	{
		if (m_selectedItem)
			m_selectedItem->setSelected(true);

		resizeSelectedItem(pos);
		return;
	}
	if (event->buttons() & Qt::LeftButton && m_item == nullptr && !m_mouseClickSelect)
	{
		if (m_mouseFrameSelect)
		{
			int dx = pos.x() - m_lastPos.x();
			int dy = pos.y() - m_lastPos.y();
			foreach (auto &item, m_selectedItems)
				item->moveItem(dx, dy);

			m_lastPos = pos;
			update();
			return;
		}
		if (!m_selectedItems.isEmpty())
		{
			deselectAllItems();
		}
		for (int i = m_allItems.size() - 1; i >= 0; --i)
		{
			if (m_rubberBand.intersects(m_allItems[i]->boundingRect()))
			{
				m_allItems[i]->setSelected(true);
				m_selectedItems << m_allItems[i];
				m_showFrameRect = true;
				m_mouseFrameSelection = true;
			}
		}
	}
	else if (event->buttons() & Qt::LeftButton && !m_item && !m_showFrameRect)
	{
		if (m_selectedItem && m_selectedItem->isSelected())
		{
			int dx = pos.x() - m_lastPos.x();
			int dy = pos.y() - m_lastPos.y();
			m_selectedItem->moveItem(dx, dy);
			m_lastPos = pos;
		}
	}
	else if (event->buttons() & Qt::LeftButton && m_bPaintingPath && m_item && m_itemType == LCanvasItem::Path)
	{
		m_item->addPoint(pos);
	}
	else if (event->buttons() & Qt::LeftButton && m_item)
	{
		m_item->setEndPos(pos);
	}
	update();
}

void LCanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;

	if (event->button() & Qt::LeftButton && m_item)
	{
		m_item->setSelected(true);
		m_selectedItem = m_item;
	}

	m_startPos = QPoint();
	m_endPos = QPoint();
	m_showFrameRect = false;
	m_mouseClickOutRect = false;
	m_mouseClickSelect = false;
	m_mouseFrameSelect = false;
	update();
}

void LCanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (!m_selectedItems.isEmpty())
		deselectAllItems();

	QPoint pos = event->pos();
	for (int i = m_textItems.size() - 1; i >= 0; --i)
	{
		if (m_textItems[i]->containsPos(pos))
		{
			m_textItems[i]->setSelected(true);
			m_selectedItem = m_textItems[i];
			m_lineEdit = m_textItems[i]->lineEdit();
			m_lineEdit->setVisible(true);
			break;
		}
	}
}

void LCanvasView::contextMenuEvent(QContextMenuEvent *event)
{
	if (m_itemType == LCanvasItem::None && m_selectedItem &&
		m_selectedItem->boundingRect().contains(event->pos()))
		m_rightClickMenu->exec(event->globalPos());
}

void LCanvasView::setItemType(LCanvasItem::ItemType itemType)
{
	if (m_itemType != itemType)
		m_itemType = itemType;
}

void LCanvasView::readItemsFromFile(const QString &filePath)
{
	if (filePath.isEmpty())
		return;

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text))
	{
		QMessageBox::critical(this, tr("Error"), tr("Read Error"));
		return;
	}

	QXmlStreamReader reader(&file);

	while(reader.name().toString() != QLatin1String("svg"))
	{
		reader.readNext();
	}

	if (reader.attributes().value(QStringLiteral("subset")).toString() != QLatin1String("lwscode"))
		return;

	while (!reader.atEnd())
	{
		if (reader.isEndElement())
			break;

		if (reader.isStartElement())
		{
			if (reader.name().toString() == QLatin1String("path"))
			{
				readItemFromXml(LCanvasItem::Path, reader);
			}
			else if (reader.name().toString() == QLatin1String("line"))
			{
				readItemFromXml(LCanvasItem::Line, reader);
			}
			else if (reader.name().toString() == QLatin1String("rect"))
			{
				readItemFromXml(LCanvasItem::Rect, reader);
			}
			else if (reader.name().toString() == QLatin1String("triangle"))
			{
				readItemFromXml(LCanvasItem::Triangle, reader);
			}
			else if (reader.name().toString() == QLatin1String("hexagon"))
			{
				readItemFromXml(LCanvasItem::Hexagon, reader);
			}
			else if (reader.name().toString() == QLatin1String("ellipse"))
			{
				readItemFromXml(LCanvasItem::Ellipse, reader);
			}
			else if (reader.name().toString() == QLatin1String("text"))
			{
				readItemFromXml(LCanvasItem::Text, reader);
			}
		}
		reader.readNext();
	}

	file.close();
}

void LCanvasView::writeItemsToFile(const QString &filePath)
{
	if (filePath.isEmpty())
		return;

	QFile file(filePath);
	if (!file.open(QFile::WriteOnly | QFile::Text))
	{
		QMessageBox::critical(this, tr("Error"), tr("Write Error"));
		return;
	}

	QXmlStreamWriter writer(&file);

	writer.writeStartDocument();
	writer.writeStartElement(QStringLiteral("svg"));
	writer.writeAttribute(QStringLiteral("subset"), QStringLiteral("lwscode"));
	writer.writeAttribute(QStringLiteral("width"), QString::number(this->width()));
	writer.writeAttribute(QStringLiteral("height"), QString::number(this->height()));
	writer.writeAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/2000/svg"));

	foreach (auto &item, m_allItems)
		item->writeItemToXml(writer);

	writer.writeEndElement();
	writer.writeEndDocument();

	file.close();
}

void LCanvasView::cutItem()
{
	copyItem();
	pasteItem();
}

void LCanvasView::copyItem()
{
	LCanvasItem::ItemType shape = m_selectedItem->getItemType();
}

void LCanvasView::pasteItem()
{

}

void LCanvasView::deleteItem()
{
	m_allItems.removeOne(m_selectedItem);
	delete m_selectedItem;
	m_selectedItem = nullptr;
}

void LCanvasView::moveTopItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.push_back(m_selectedItem);
}

void LCanvasView::moveUpItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.push_back(m_selectedItem);
}

void LCanvasView::moveDownItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.push_front(m_selectedItem);
}

void LCanvasView::moveBottomItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.push_front(m_selectedItem);
}

LCanvasView::ItemHitPos LCanvasView::getItemHitPos(const QPoint &point)
{
	if (m_topLeftPos.contains(point))
		return ItemHitPos::TopLeft;

	if (m_topMiddlePos.contains(point))
		return ItemHitPos::TopMiddle;

	if (m_topRightPos.contains(point))
		return ItemHitPos::TopRight;

	if (m_middleRightPos.contains(point))
		return ItemHitPos::MiddleRight;

	if (m_bottomRightPos.contains(point))
		return ItemHitPos::BottomRight;

	if (m_bottomMiddlePos.contains(point))
		return ItemHitPos::BottomMiddle;

	if (m_bottomLeftPos.contains(point))
		return ItemHitPos::BottomLeft;

	if (m_middleLeftPos.contains(point))
		return ItemHitPos::MiddleLeft;

	return ItemHitPos::None;
}

void LCanvasView::initRightClickMenu()
{
	m_rightClickMenu = new QMenu(this);
	QAction *cutAction = new QAction(QStringLiteral("Cut"), m_rightClickMenu);
	QAction *pasteAction = new QAction(QStringLiteral("Paste"), m_rightClickMenu);
	QAction *copyAction = new QAction(QStringLiteral("Copy"), m_rightClickMenu);
	QAction *deleteAction = new QAction(QStringLiteral("Delete"), m_rightClickMenu);
	QAction *moveTopAction = new QAction(QStringLiteral("Move Top"), m_rightClickMenu);
	QAction *moveUpAction = new QAction(QStringLiteral("Move Up"), m_rightClickMenu);
	QAction *moveDownAction = new QAction(QStringLiteral("Move Down"), m_rightClickMenu);
	QAction *moveBottomAction = new QAction(QStringLiteral("Move Bottom"), m_rightClickMenu);

	m_rightClickMenu->addAction(cutAction);
	m_rightClickMenu->addAction(pasteAction);
	m_rightClickMenu->addAction(copyAction);
	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction(deleteAction);
	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction(moveTopAction);
	m_rightClickMenu->addAction(moveUpAction);
	m_rightClickMenu->addAction(moveDownAction);
	m_rightClickMenu->addAction(moveBottomAction);

	connect(cutAction, SIGNAL(triggered()), this, SLOT(cutItem()));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteItem()));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyItem()));
	connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));
	connect(moveTopAction, SIGNAL(triggered()), this, SLOT(moveTopItem()));
	connect(moveUpAction, SIGNAL(triggered()), this, SLOT(moveUpItem()));
	connect(moveDownAction, SIGNAL(triggered()), this, SLOT(moveDownItem()));
	connect(moveBottomAction, SIGNAL(triggered()), this, SLOT(moveBottomItem()));
}

void LCanvasView::reselectItems()
{
	if (!m_selectedItems.isEmpty())
		deselectAllItems();

	for (auto &item : m_allItems)
		item->setSelected(false);
}

void LCanvasView::deselectAllItems()
{
	foreach (auto &item, m_selectedItems)
		item->setSelected(false);

	m_selectedItems.clear();
}

void LCanvasView::paintRubberBand(LCanvasItem *item, QPainter &painter)
{
	QRect rubberBand = item->boundingRect();
	painter.setPen(Qt::blue);
	painter.drawRect(rubberBand);
	painter.setPen(Qt::NoPen);
}

void LCanvasView::paintSelectedBox(LCanvasItem *item, QPainter &painter)
{
	QRect selectedBox = item->boundingRect();
	int left = selectedBox.left();
	int top = selectedBox.top();
	int width = selectedBox.width();
	int height = selectedBox.height();
	m_topLeftPos = QRect(left - 4, top - 4, 8, 8);
	m_topMiddlePos = QRect(left + width / 2 - 4, top - 4, 8, 8);
	m_topRightPos = QRect(left + width - 4, top - 4, 8, 8);
	m_middleRightPos = QRect(left + width - 4, top + height / 2 - 4, 8, 8);
	m_bottomRightPos = QRect(left + width - 4, top + height - 4, 8, 8);
	m_bottomMiddlePos = QRect(left + width / 2 - 4, top + height - 4, 8, 8);
	m_bottomLeftPos = QRect(left - 4, top + height - 4, 8, 8);
	m_middleLeftPos = QRect(left - 4, top + height / 2 - 4, 8, 8);

	painter.setPen(Qt::blue);
	painter.drawRect(selectedBox);
	painter.setBrush(Qt::blue);
	painter.drawRect(m_topLeftPos);
	painter.drawRect(m_topMiddlePos);
	painter.drawRect(m_topRightPos);
	painter.drawRect(m_middleRightPos);
	painter.drawRect(m_bottomRightPos);
	painter.drawRect(m_bottomMiddlePos);
	painter.drawRect(m_bottomLeftPos);
	painter.drawRect(m_middleLeftPos);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);
}

void LCanvasView::beforePaintItem(const QPoint &pos)
{
	switch (m_itemType)
	{
	case LCanvasItem::None:
	{
		m_item = nullptr;
		m_bPaintingItem = false;
		m_showFrameRect = true;
		m_startPos = pos;
		break;
	}
	case LCanvasItem::Path:
	{
		m_item = new LCanvasPath();
		m_item->setStrokeWidth(m_strokeWidth);
		break;
	}
	case LCanvasItem::Line:
	{
		m_item = new LCanvasLine();
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		break;
	}
	case LCanvasItem::Rect:
	{
		m_item = new LCanvasRect();
		if (m_item)
		{
			m_item->setFillColor(m_fillColor);
			m_item->setStrokeWidth(m_strokeWidth);
		}
		break;
	}
	case LCanvasItem::Ellipse:
	{
		m_item = new LCanvasEllipse();
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		break;
	}
	case LCanvasItem::Triangle:
	{
		m_item = new LCanvasTriangle();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(m_fillColor);
		break;
	}
	case LCanvasItem::Hexagon:
	{
		m_item = new LCanvasHexagon();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(m_fillColor);
		break;
	}
	case LCanvasItem::Text:
	{
		m_item = new LCanvasText(this);
		m_lineEdit = m_item->lineEdit();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(m_fillColor);
		connect(m_lineEdit, SIGNAL(textChanged()), this, SLOT(update()));
		break;
	}
	}
}

void LCanvasView::selectItemsByClick(const QPoint &pos)
{
	if (!m_selectedItems.isEmpty())
		deselectAllItems();

	for (int i = m_allItems.size() - 1; i >= 0; i--)
	{
		if (m_allItems[i]->isSelected())
			m_allItems[i]->setSelected(false);

		if (m_allItems[i]->containsPos(pos))
		{
			if (m_selectedItem && m_selectedItem != m_allItems[i])
				m_selectedItem->setSelected(false);

			m_selectedItem = m_allItems[i];
			m_selectedItem->setSelected(true);
			m_lastPos = pos;
			m_showFrameRect = false;
			m_mouseClickSelect = true;
			m_mouseFrameSelect = false;
			return;
		}
	}

	if (m_selectedItem)
		m_selectedItem->setSelected(false);
}

void LCanvasView::selectItemsByFrame(const QPoint &pos)
{
	m_showFrameRect = true;
	m_selectedItem->setSelected(false);
	m_selectedItem = nullptr;
	bool containsFlag = false;
	foreach (auto &item, m_selectedItems)
	{
		if (item->containsPos(pos))
		{
			m_lastPos = pos;
			containsFlag = true;
			m_showFrameRect = false;
			m_mouseFrameSelect = true;
			break;
		}
	}

	if (containsFlag == false)
	{
		selectItemsByClick(pos);
		deselectAllItems();
	}
}

void LCanvasView::resizeSelectedItem(const QPoint& pos)
{
	if (!m_selectedItem)
		return;

	switch (m_itemHitPos)
	{
	case ItemHitPos::TopLeft:
	{
		m_selectedItem->setStartPos(pos);
		break;
	}
	case ItemHitPos::TopMiddle:
	{
		int height = pos.y() - m_lastPos.y();
		m_selectedItem->moveStartPos(0, height);
		break;
	}
	case ItemHitPos::TopRight:
	{
		int width = pos.x() - m_lastPos.x();
		int height = pos.y() - m_lastPos.y();
		m_selectedItem->moveStartPos(0, height);
		m_selectedItem->moveEndPos(width, 0);
		break;
	}
	case ItemHitPos::MiddleRight:
	{
		int width = pos.x() - m_lastPos.x();
		m_selectedItem->moveEndPos(width, 0);
		break;
	}
	case ItemHitPos::BottomRight:
	{
		m_selectedItem->setEndPos(pos);
		break;
	}
	case ItemHitPos::BottomMiddle:
	{
		int height = pos.y() - m_lastPos.y();
		m_selectedItem->moveEndPos(0, height);
		break;
	}
	case ItemHitPos::BottomLeft:
	{
		int width = pos.x() - m_lastPos.x();
		int height = pos.y() - m_lastPos.y();
		m_selectedItem->moveStartPos(width, 0);
		m_selectedItem->moveEndPos(0, height);
		break;
	}
	case ItemHitPos::MiddleLeft:
	{
		int width = pos.x() - m_lastPos.x();
		m_selectedItem->moveStartPos(width, 0);
		break;
	}
	default:
	{
		break;
	}
	}
	m_lastPos = pos;
	update();
}

void LCanvasView::readItemFromXml(LCanvasItem::ItemType itemType, QXmlStreamReader &reader)
{
	switch (itemType)
	{
	case LCanvasItem::Path:
	{
		QString path = reader.attributes().value(QStringLiteral("d")).toString();
		QStringList points = path.split(QRegularExpression(QStringLiteral("[ML]")));

		m_item = new LCanvasPath();
		m_bPaintingPath = true;
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setStartPos(QPoint(points[0].toInt(), points[1].toInt()));
		int size = points.size();
		m_item->setEndPos(QPoint(points[size - 2].toInt(), points[size - 1].toInt()));

		for (int i = 0; i < points.size() - 1; i += 2)
			m_item->addPoint(QPoint(points[i].toInt(), points[i + 1].toInt()));

		break;
	}
	case LCanvasItem::Line:
	{
		m_item = new LCanvasLine();
		m_item->setStartPos(QPoint(reader.attributes().value(QStringLiteral("x1")).toInt(),
								   reader.attributes().value(QStringLiteral("y1")).toInt()));
		m_item->setEndPos(QPoint(reader.attributes().value(QStringLiteral("x2")).toInt(),
								 reader.attributes().value(QStringLiteral("y2")).toInt()));
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		break;
	}
	case LCanvasItem::Rect:
	{
		m_item = new LCanvasRect();
		int x = reader.attributes().value(QStringLiteral("x")).toInt();
		int y = reader.attributes().value(QStringLiteral("y")).toInt();
		int width = reader.attributes().value(QStringLiteral("width")).toInt();
		int height = reader.attributes().value(QStringLiteral("height")).toInt();
		m_item->setStartPos(QPoint(x, y));
		m_item->setEndPos(QPoint(x + width, y + height));
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(reader.attributes().value(QStringLiteral("fill")).toString());
		break;
	}
	case LCanvasItem::Ellipse:
	{
		m_item = new LCanvasEllipse();
		int cx = reader.attributes().value(QStringLiteral("cx")).toInt();
		int cy = reader.attributes().value(QStringLiteral("cy")).toInt();
		int rx = reader.attributes().value(QStringLiteral("rx")).toInt();
		int ry = reader.attributes().value(QStringLiteral("ry")).toInt();
		m_item->setStartPos(QPoint(cx - rx, cy - ry));
		m_item->setEndPos(QPoint(cx + rx, cy + cy));
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(reader.attributes().value(QStringLiteral("fill")).toString());
		break;
	}
	case LCanvasItem::Triangle:
	{
		m_item = new LCanvasTriangle();
		QString polygon = reader.attributes().value(QStringLiteral("points")).toString();
		QStringList points = polygon.split(QRegularExpression(QStringLiteral("[,]")));
		m_item->setStartPos(QPoint(points[4].toInt(), points[1].toInt()));
		m_item->setEndPos(QPoint(points[2].toInt(), points[3].toInt()));
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(reader.attributes().value(QStringLiteral("fill")).toString());
		break;
	}
	case LCanvasItem::Hexagon:
	{
		m_item = new LCanvasHexagon();
		QString polygon = reader.attributes().value(QStringLiteral("points")).toString();
		QStringList points = polygon.split(QRegularExpression(QStringLiteral("[,]")));
		m_item->setStartPos(QPoint(points[10].toInt(), points[1].toInt()));
		m_item->setEndPos(QPoint(points[4].toInt(), points[7].toInt()));
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(reader.attributes().value(QStringLiteral("fill")).toString());
		break;
	}
	case LCanvasItem::Text:
	{
		m_item = new LCanvasText(this);
		m_item->setStartPos(QPoint(reader.attributes().value(QStringLiteral("x")).toInt(),
								   reader.attributes().value(QStringLiteral("y")).toInt()));
		m_lineEdit = m_item->lineEdit();
		m_lineEdit->setText(reader.readElementText());
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(m_fillColor);
		connect(m_lineEdit, SIGNAL(textChanged()), this, SLOT(update()));
		m_textItems << m_item;
		break;
	}
	default:
	{
		break;
	}
	}
	m_allItems << m_item;
}

} // namespace
