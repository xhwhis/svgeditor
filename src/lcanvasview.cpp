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
	, m_itemHitPos(LCanvasView::None)
{
	this->resize(500, 500);
	this->setMinimumSize(QSize(100, 100));
	this->setMaximumSize(QSize(2000, 2000));
//	this->setStyleSheet(QStringLiteral("border-radius: 8px"));

	QPalette::ColorRole role = this->backgroundRole();
	QPalette palette = this->palette();
	palette.setColor(role, m_canvasColor);
	this->setPalette(palette);

	initLineEdit();
	initRightClickMenu();
}

LCanvasView::~LCanvasView()
{

}

void LCanvasView::setCanvasColor(const QColor &color)
{
	if (color.isValid() && m_canvasColor != color)
	{
		m_canvasColor = color;

		QPalette::ColorRole role = this->backgroundRole();
		QPalette palette = this->palette();
		palette.setColor(role, m_canvasColor);
		this->setPalette(palette);
	}
}

void LCanvasView::setStrokeColor(const QColor &color)
{
	if (color.isValid() && m_strokeColor != color)
	{
		m_strokeColor = color;

		if (!m_selectedItems.isEmpty())
		{
			foreach (auto &item, m_selectedItems)
				item->setStrokeColor(m_strokeColor);

			this->update();
		}
	}
}

void LCanvasView::setStrokeWidth(int width)
{
	if (m_strokeWidth != width)
	{
		m_strokeWidth = width;

		if (!m_selectedItems.isEmpty())
		{
			foreach (auto &item, m_selectedItems)
				item->setStrokeWidth(m_strokeWidth);

			this->update();
		}
	}
}

void LCanvasView::clearCanvas()
{
	m_allItems.clear();
	this->update();
}

bool LCanvasView::noItems()
{
	return !m_allItems.isEmpty();
}

void LCanvasView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	foreach (auto &item, m_allItems)
	{
		item->paintItem(painter);

		if (item->isSelected())
		{
			item->setBoundingRect();
			if (m_selectedItems.size() > 1)
			{
				paintRubberBand(item, painter);
			}
			else
			{
				paintRubberBand(item, painter, true);
			}
		}
	}

	if (m_showFrameRect && !m_startPos.isNull() && !m_endPos.isNull())
	{
		m_selectedBox = QRect(m_startPos, m_endPos);

		painter.save();
		QPen pen(Qt::DashLine);
		pen.setColor(Qt::blue);
		pen.setWidth(2);
		painter.setPen(pen);
		painter.drawRect(m_selectedBox);
		painter.restore();
	}
}

void LCanvasView::mousePressEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	beforePaintItem(pos);
	setCursorByPos(pos);

	if (m_bPaintingItem)
	{
		deselectAllItems();

		m_item->setStartPos(pos);
		m_item->setEndPos(pos);
	}
	else
	{
		if (m_itemHitPos != ItemHitPos::None)
		{
			m_showFrameRect = false;
			m_lastPos = pos;
		}
		else if (m_mouseFrameSelection && !m_selectedItems.isEmpty() && !m_mouseFrameSelect)
		{
			selectItemsByFrame(pos);
		}
		else if(!m_mouseFrameSelect && m_selectedItems.isEmpty())
		{
			selectItemsByClick(pos);
		}
	}
	this->update();
	m_endPos = pos;
}

void LCanvasView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;

	if (m_itemHitPos != ItemHitPos::None && m_itemType == LCanvasItem::None)
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
			if (m_selectedBox.intersects(m_allItems[i]->boundingRect()))
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

	if (m_itemHitPos != ItemHitPos::None)
	{
		m_itemHitPos = ItemHitPos::None;
		this->setCursor(Qt::ArrowCursor);
	}

	m_startPos = QPoint();
	m_endPos = QPoint();
	m_showFrameRect = false;
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
	m_itemType = itemType;
	if (itemType == LCanvasItem::None)
	{
		this->setCursor(Qt::ArrowCursor);
	}
	else
	{
		this->setCursor(Qt::CrossCursor);
	}
}

void LCanvasView::resizeLineEdit()
{
	QString text = m_lineEdit->text();
	QFontMetrics fontMetrics = m_lineEdit->fontMetrics();

	QRect rect = fontMetrics.boundingRect(text);
	int width = rect.width() < 8 ? 8 : rect.width();
	int height = rect.height();
	m_lineEdit->resize(width, height);
}

void LCanvasView::addText()
{

}

void LCanvasView::readItemsFromFile(const QString &filePath)
{
	if (filePath.isEmpty())
		return;

	QFile file(filePath);
	file.open(QFile::ReadOnly);

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
		{
			reader.readNext();
			continue;
		}

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
	file.open(QFile::WriteOnly);

	QXmlStreamWriter writer(&file);
	writer.setAutoFormatting(true);

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
	deleteItem();
}

void LCanvasView::copyItem()
{
	if (m_selectedItems.isEmpty())
		return;

	m_duplicatedItems.clear();
	foreach (auto &item, m_selectedItems)
	{
		SPtrLCanvasItem duplicatedItem = item->clone();
		duplicatedItem->moveItem(4, 4);
		m_duplicatedItems << duplicatedItem;
	}
}

void LCanvasView::pasteItem()
{
	if (m_duplicatedItems.isEmpty())
		return;

	m_selectedItems.clear();
	m_allItems << m_duplicatedItems;
	m_selectedItems << m_duplicatedItems;

	this->update();
}

void LCanvasView::deleteItem()
{
	m_allItems.removeOne(m_selectedItem);
	this->update();
}

void LCanvasView::moveTopItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.append(m_selectedItem);
	this->update();
}

void LCanvasView::moveUpItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.append(m_selectedItem);
	this->update();
}

void LCanvasView::moveDownItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.append(m_selectedItem);
	this->update();
}

void LCanvasView::moveBottomItem()
{
	m_allItems.removeOne(m_selectedItem);
	m_allItems.append(m_selectedItem);
	this->update();
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

void LCanvasView::initLineEdit()
{
	m_lineEdit = new QLineEdit(this);
	m_lineEdit->setVisible(false);
	m_lineEdit->resize(m_lineEdit->minimumSizeHint());
	m_lineEdit->setFont(QFont("PingFang SC", 32));
	m_lineEdit->resize(m_lineEdit->fontMetrics().averageCharWidth() * 8,
					   m_lineEdit->fontMetrics().height());

	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(resizeLineEdit()));
	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(addText()));
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

void LCanvasView::setCursorByPos(const QPoint &pos)
{
	m_itemHitPos = getItemHitPos(pos);
	switch (m_itemHitPos)
	{
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
	default:
	{
		break;
	}
	}
}

void LCanvasView::deselectAllItems()
{
	if (!m_selectedItems.isEmpty())
		return;

	foreach (auto &item, m_selectedItems)
		item->setSelected(false);

	m_selectedItems.clear();
}

void LCanvasView::paintRubberBand(SPtrLCanvasItem item, QPainter &painter, bool flag)
{
	QRect rubberBand = item->boundingRect();

	painter.save();
	painter.setPen(Qt::blue);
	painter.drawRect(rubberBand);

	if (flag)
	{
		int left = rubberBand.left();
		int top = rubberBand.top();
		int width = rubberBand.width();
		int height = rubberBand.height();
		m_topLeftPos = QRect(left - 4, top - 4, 8, 8);
		m_topMiddlePos = QRect(left + width / 2 - 4, top - 4, 8, 8);
		m_topRightPos = QRect(left + width - 4, top - 4, 8, 8);
		m_middleRightPos = QRect(left + width - 4, top + height / 2 - 4, 8, 8);
		m_bottomRightPos = QRect(left + width - 4, top + height - 4, 8, 8);
		m_bottomMiddlePos = QRect(left + width / 2 - 4, top + height - 4, 8, 8);
		m_bottomLeftPos = QRect(left - 4, top + height - 4, 8, 8);
		m_middleLeftPos = QRect(left - 4, top + height / 2 - 4, 8, 8);

		painter.setBrush(Qt::blue);
		painter.drawRect(m_topLeftPos);
		painter.drawRect(m_topMiddlePos);
		painter.drawRect(m_topRightPos);
		painter.drawRect(m_middleRightPos);
		painter.drawRect(m_bottomRightPos);
		painter.drawRect(m_bottomMiddlePos);
		painter.drawRect(m_bottomLeftPos);
		painter.drawRect(m_middleLeftPos);
	}

	painter.restore();
}

void LCanvasView::beforePaintItem(const QPoint &pos)
{
	m_bPaintingItem = true;
	m_bPaintingPath = false;
	m_showFrameRect = false;

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
		m_bPaintingPath = true;
		m_item = SPtrLCanvasItem(new LCanvasPath());
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Line:
	{
		m_item = SPtrLCanvasItem(new LCanvasLine());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Rect:
	{
		m_item = SPtrLCanvasItem(new LCanvasRect());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Ellipse:
	{
		m_item = SPtrLCanvasItem(new LCanvasEllipse());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Triangle:
	{
		m_item = SPtrLCanvasItem(new LCanvasTriangle());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Hexagon:
	{
		m_item = SPtrLCanvasItem(new LCanvasHexagon());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case LCanvasItem::Text:
	{
		m_item = SPtrLCanvasItem(new LCanvasText());
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		m_textItems << m_item;
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
		if (m_allItems[i]->containsPos(pos))
		{
			m_allItems[i]->setSelected(true);
			m_selectedItems << m_allItems[i];
			m_lastPos = pos;
			m_showFrameRect = false;
			m_mouseClickSelect = true;
			m_mouseFrameSelect = false;
			return;
		}
	}
}

void LCanvasView::selectItemsByFrame(const QPoint &pos)
{
	m_showFrameRect = true;
	m_selectedItem->setSelected(false);
	m_selectedItem = nullptr;
	foreach (auto &item, m_selectedItems)
	{
		if (item->containsPos(pos))
		{
			m_lastPos = pos;
			m_showFrameRect = false;
			m_mouseFrameSelect = true;
			return;
		}
	}

	selectItemsByClick(pos);
}

void LCanvasView::resizeSelectedItem(const QPoint &pos)
{
	if (m_selectedItems.size() != 1)
		return;

	switch (m_itemHitPos)
	{
	case ItemHitPos::TopLeft:
	{
		m_selectedItems[0]->setStartPos(pos);
		break;
	}
	case ItemHitPos::TopMiddle:
	{
		int height = pos.y() - m_lastPos.y();
		m_selectedItems[0]->moveStartPos(0, height);
		break;
	}
	case ItemHitPos::TopRight:
	{
		int width = pos.x() - m_lastPos.x();
		int height = pos.y() - m_lastPos.y();
		m_selectedItems[0]->moveStartPos(0, height);
		m_selectedItems[0]->moveEndPos(width, 0);
		break;
	}
	case ItemHitPos::MiddleRight:
	{
		int width = pos.x() - m_lastPos.x();
		m_selectedItems[0]->moveEndPos(width, 0);
		break;
	}
	case ItemHitPos::BottomRight:
	{
		m_selectedItems[0]->setEndPos(pos);
		break;
	}
	case ItemHitPos::BottomMiddle:
	{
		int height = pos.y() - m_lastPos.y();
		m_selectedItems[0]->moveEndPos(0, height);
		break;
	}
	case ItemHitPos::BottomLeft:
	{
		int width = pos.x() - m_lastPos.x();
		int height = pos.y() - m_lastPos.y();
		m_selectedItems[0]->moveStartPos(width, 0);
		m_selectedItems[0]->moveEndPos(0, height);
		break;
	}
	case ItemHitPos::MiddleLeft:
	{
		int width = pos.x() - m_lastPos.x();
		m_selectedItems[0]->moveStartPos(width, 0);
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

		m_item = SPtrLCanvasItem(new LCanvasPath());
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
		m_item = SPtrLCanvasItem(new LCanvasLine());
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
		m_item = SPtrLCanvasItem(new LCanvasRect());
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
		m_item = SPtrLCanvasItem(new LCanvasEllipse());
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
		m_item = SPtrLCanvasItem(new LCanvasTriangle());
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
		m_item = SPtrLCanvasItem(new LCanvasHexagon());
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
		m_item = SPtrLCanvasItem(new LCanvasText());
		m_item->setStartPos(QPoint(reader.attributes().value(QStringLiteral("x")).toInt(),
								   reader.attributes().value(QStringLiteral("y")).toInt()));
		m_lineEdit->setText(reader.readElementText());
		m_strokeWidth = reader.attributes().value(QStringLiteral("stroke-width")).toInt();
		m_item->setStrokeWidth(m_strokeWidth);
		m_item->setFillColor(m_fillColor);
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
