#include "lcanvasview.h"

namespace lwscode {

LCanvasView::LCanvasView(QWidget *parent)
	: QWidget(parent)
	, m_rightClickMenu(nullptr)
	, m_itemType(ItemType::NoneType)
	, m_item(nullptr)
	, m_canvasColor(Qt::white)
	, m_fillColor(Qt::white)
	, m_strokeColor(Qt::black)
	, m_strokeWidth(1)
	, m_fScaleFactor(1.0f)
	, m_lineEdit(nullptr)
	, m_hitTestStatus(HitTestStatus::NoneStatus)
	, m_itemHitPos(ItemHitPos::NonePos)
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

void LCanvasView::setFillColor(const QColor &color)
{
	if (color.isValid() && m_fillColor != color)
	{
		m_fillColor = color;

		if (!m_selectedItems.isEmpty())
		{
			foreach (auto &item, m_selectedItems)
				item->setFillColor(m_fillColor);

			this->update();
		}
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
	m_textItems.clear();
	m_selectedItems.clear();
	m_duplicatedItems.clear();

	this->update();
}

bool LCanvasView::existItems()
{
	return !m_allItems.isEmpty();
}

void LCanvasView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.scale(m_fScaleFactor, m_fScaleFactor);

	foreach (auto &item, m_allItems)
	{
		item->setStrokeColor(m_strokeColor);
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

	if (m_hitTestStatus == HitTestStatus::SelectingItems && m_selectedBox.isValid())
	{
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
	m_startPos = m_lastPos = pos;
	m_lineEdit->move(event->pos());
	hitTest(pos);
	setCursorByPos(pos);

	if (m_hitTestStatus & HitTestStatus::PaintingItem)
	{
		deselectAllItems();
		m_item->setStartPos(pos);
		m_item->setEndPos(pos);
	}
	this->update();
}

void LCanvasView::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();

	if (m_hitTestStatus == HitTestStatus::ScalingItem)
	{
		resizeSelectedItem(pos);
	}
	else if (m_hitTestStatus == HitTestStatus::MovingItems)
	{
		int dx = pos.x() - m_lastPos.x();
		int dy = pos.y() - m_lastPos.y();
		foreach (auto &item, m_selectedItems)
			item->moveItem(dx, dy);
	}
	else if (m_hitTestStatus == HitTestStatus::SelectingItems)
	{
		deselectAllItems();
		m_selectedBox = QRect(m_startPos, pos).normalized();
		foreach (auto &item, m_allItems)
		{
			if (m_selectedBox.intersects(item->boundingRect()))
			{
				item->setSelected(true);
				m_selectedItems << item;
			}
		}
	}
	else if (m_hitTestStatus == HitTestStatus::PaintingPath)
	{
		m_item->addPoint(pos);
	}
	else if (m_hitTestStatus & HitTestStatus::PaintingItem)
	{
		m_item->setEndPos(pos);
	}
	this->update();
	m_lastPos = pos;
}

void LCanvasView::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_hitTestStatus & HitTestStatus::PaintingItem)
	{
		deselectAllItems();
		m_item->setSelected(true);
		m_selectedItems << m_item;
	}

	if (m_hitTestStatus == HitTestStatus::ScalingItem)
	{
		m_itemHitPos = ItemHitPos::NonePos;
		this->setCursor(Qt::ArrowCursor);
	}

	m_startPos = m_lastPos = QPoint();
	m_hitTestStatus = HitTestStatus::NoneStatus;
	m_selectedBox = QRect();

	this->update();
}

void LCanvasView::mouseDoubleClickEvent(QMouseEvent *event)
{
	deselectAllItems();

	QPoint pos = event->pos();
	for (int i = m_textItems.size() - 1; i >= 0; --i)
	{
		if (m_textItems[i]->containsPos(pos))
		{
			m_textItems[i]->setSelected(true);
			m_selectedItems << m_textItems[i];
			showLineEdit();
			break;
		}
	}
}

void LCanvasView::wheelEvent(QWheelEvent *event)
{
	int dy = event->pixelDelta().y();
	if (dy > 0)
	{
		m_fScaleFactor = 1.1f;
	}
	else if (dy < 0)
	{
		m_fScaleFactor = 0.9f;
	}

	int width = qMax(this->width() * m_fScaleFactor, 100.0f);
	int height = qMax(this->height() * m_fScaleFactor, 100.0f);
	this->resize(width, height);
}

void LCanvasView::contextMenuEvent(QContextMenuEvent *event)
{
	if (m_itemType == ItemType::NoneType)
		m_rightClickMenu->exec(event->globalPos());
}

void LCanvasView::setItemType(ItemType itemType)
{
	m_itemType = itemType;
	if (itemType == ItemType::NoneType)
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
				readItemFromXml(ItemType::Path, reader);
			}
			else if (reader.name().toString() == QLatin1String("line"))
			{
				readItemFromXml(ItemType::Line, reader);
			}
			else if (reader.name().toString() == QLatin1String("rect"))
			{
				readItemFromXml(ItemType::Rect, reader);
			}
			else if (reader.name().toString() == QLatin1String("polygon"))
			{
				switch (reader.attributes().value(QStringLiteral("points")).toString().count(","))
				{
				case 3:
				{
					readItemFromXml(ItemType::Triangle, reader);
					break;
				}
				case 6:
				{
					readItemFromXml(ItemType::Hexagon, reader);
					break;
				}
				default:
				{
					break;
				}
				}
			}
			else if (reader.name().toString() == QLatin1String("ellipse"))
			{
				readItemFromXml(ItemType::Ellipse, reader);
			}
			else if (reader.name().toString() == QLatin1String("text"))
			{
				readItemFromXml(ItemType::Text, reader);
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
		duplicatedItem->moveItem(8, 8);
		m_duplicatedItems << duplicatedItem;
	}
}

void LCanvasView::pasteItem()
{
	if (m_duplicatedItems.isEmpty())
		return;

	m_allItems << m_duplicatedItems;
	deselectAllItems();
	m_selectedItems << m_duplicatedItems;

	this->update();
}

void LCanvasView::deleteItem()
{
	if (m_selectedItems.isEmpty())
		return;

	foreach (auto &item, m_selectedItems)
		m_allItems.removeOne(item);

	this->update();
}

void LCanvasView::moveTopItem()
{
	if (m_selectedItems.size() != 1)
		return;

	int idx = m_allItems.indexOf(m_selectedItems[0]);
	int lastIdx = m_allItems.size() - 1;
	if (idx >= 0 && idx < lastIdx)
	{
		m_allItems.move(idx, lastIdx);
		this->update();
	}
}

void LCanvasView::moveUpItem()
{
	if (m_selectedItems.size() != 1)
		return;

	int idx = m_allItems.indexOf(m_selectedItems[0]);
	if (idx >= 0 && idx < m_allItems.size() - 1)
	{
		m_allItems.move(idx, idx + 1);
		this->update();
	}
}

void LCanvasView::moveDownItem()
{
	if (m_selectedItems.size() != 1)
		return;

	int idx = m_allItems.indexOf(m_selectedItems[0]);
	if (idx > 0)
	{
		m_allItems.move(idx, idx - 1);
		this->update();
	}
}

void LCanvasView::moveBottomItem()
{
	if (m_selectedItems.size() != 1)
		return;

	int idx = m_allItems.indexOf(m_selectedItems[0]);
	if (idx > 0)
	{
		m_allItems.move(idx, 0);
		this->update();
	}
}

ItemHitPos LCanvasView::getItemHitPos(const QPoint &point)
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

	return ItemHitPos::NonePos;
}

void LCanvasView::initLineEdit()
{
	m_lineEdit = new QLineEdit(this);
	m_lineEdit->setVisible(false);
	QFont font(QStringLiteral("PingFang SC"), 32);
	m_lineEdit->setFont(font);

	QFontMetrics fontMetrics(font);
	QRect rect = fontMetrics.boundingRect(QStringLiteral(""));
	int width = rect.width() < 8 ? 8 : rect.width();
	int height = rect.height();
	m_lineEdit->resize(width, height);

	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(resizeLineEdit()));
	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(addText()));
}

void LCanvasView::showLineEdit()
{
	QPalette palette = m_lineEdit->palette();
	palette.setBrush(QPalette::Text, m_strokeColor);
	m_lineEdit->setPalette(palette);
	m_lineEdit->show();
	m_lineEdit->setFocus();
}

void LCanvasView::initRightClickMenu()
{
	m_rightClickMenu = new QMenu(this);

	QAction *cutAction = new QAction(tr("Cut"), m_rightClickMenu);
	cutAction->setIcon(QIcon(QStringLiteral(":icons/cut.svg")));
	cutAction->setShortcut(QKeySequence::Cut);

	QAction *copyAction = new QAction(tr("Copy"), m_rightClickMenu);
	copyAction->setIcon(QIcon(QStringLiteral(":icons/copy.svg")));
	copyAction->setShortcut(QKeySequence::Copy);

	QAction *pasteAction = new QAction(tr("Paste"), m_rightClickMenu);
	pasteAction->setIcon(QIcon(QStringLiteral(":icons/paste.svg")));
	pasteAction->setShortcut(QKeySequence::Paste);

	QAction *deleteAction = new QAction(tr("Delete"), m_rightClickMenu);
	deleteAction->setIcon(QIcon(QStringLiteral(":icons/delete.svg")));
	deleteAction->setShortcut(QKeySequence::Delete);

	QAction *moveTopAction = new QAction(tr("Move Top"), m_rightClickMenu);

	QAction *moveUpAction = new QAction(tr("Move Up"), m_rightClickMenu);

	QAction *moveDownAction = new QAction(tr("Move Down"), m_rightClickMenu);

	QAction *moveBottomAction = new QAction(tr("Move Bottom"), m_rightClickMenu);

	m_rightClickMenu->addAction(cutAction);
	m_rightClickMenu->addAction(copyAction);
	m_rightClickMenu->addAction(pasteAction);
	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction(deleteAction);
	m_rightClickMenu->addSeparator();
	m_rightClickMenu->addAction(moveTopAction);
	m_rightClickMenu->addAction(moveUpAction);
	m_rightClickMenu->addAction(moveDownAction);
	m_rightClickMenu->addAction(moveBottomAction);

	connect(cutAction, SIGNAL(triggered()), this, SLOT(cutItem()));
	connect(copyAction, SIGNAL(triggered()), this, SLOT(copyItem()));
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(pasteItem()));
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
	if (m_selectedItems.isEmpty())
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

void LCanvasView::hitTest(const QPoint &pos)
{
	switch (m_itemType)
	{
	case ItemType::NoneType:
	{
		m_item = nullptr;
		m_itemHitPos = getItemHitPos(pos);
		if (m_itemHitPos != ItemHitPos::NonePos)
		{
			m_hitTestStatus = HitTestStatus::ScalingItem;
		}
		else
		{
			for (int i = m_allItems.size() - 1; i >= 0; --i)
			{
				if (m_allItems[i]->containsPos(pos))
				{
					m_hitTestStatus = HitTestStatus::MovingItems;
					if (m_allItems[i]->isSelected())
						break;

					deselectAllItems();
					m_allItems[i]->setSelected(true);
					m_selectedItems << m_allItems[i];
					break;
				}
			}

			if (m_hitTestStatus != HitTestStatus::MovingItems)
			{
				m_hitTestStatus = HitTestStatus::SelectingItems;
				deselectAllItems();
			}
		}
		m_startPos = pos;
		break;
	}
	case ItemType::Path:
	{
		m_hitTestStatus = HitTestStatus::PaintingPath;
		m_item = SPtrLCanvasItem(new LCanvasPath());
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Line:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasLine());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Rect:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasRect());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Ellipse:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasEllipse());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Triangle:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasTriangle());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Hexagon:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasHexagon());
		m_item->setFillColor(m_fillColor);
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		break;
	}
	case ItemType::Text:
	{
		m_hitTestStatus = HitTestStatus::PaintingItem;
		m_item = SPtrLCanvasItem(new LCanvasText());
		m_item->setStrokeWidth(m_strokeWidth);
		m_allItems << m_item;
		m_textItems << m_item;
		showLineEdit();
		break;
	}
	}
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
}

void LCanvasView::readItemFromXml(ItemType itemType, QXmlStreamReader &reader)
{
	switch (itemType)
	{
	case ItemType::Path:
	{
		m_item = SPtrLCanvasItem(new LCanvasPath());
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		QString path = reader.attributes().value(QStringLiteral("d")).toString();
		QStringList points = path.split(QRegularExpression(QStringLiteral("\\D")), Qt::SkipEmptyParts);
		int size = points.size();
		m_item->setStartPos(QPoint(points[0].toInt(), points[1].toInt()));
		m_item->setEndPos(QPoint(points[size - 2].toInt(), points[size - 1].toInt()));

		for (int i = 0; i < points.size() - 1; i += 2)
			m_item->addPoint(QPoint(points[i].toInt(), points[i + 1].toInt()));

		m_allItems << m_item;
		break;
	}
	case ItemType::Line:
	{
		m_item = SPtrLCanvasItem(new LCanvasLine());
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		m_item->setStartPos(QPoint(reader.attributes().value(QStringLiteral("x1")).toInt(),
								   reader.attributes().value(QStringLiteral("y1")).toInt()));
		m_item->setEndPos(QPoint(reader.attributes().value(QStringLiteral("x2")).toInt(),
								 reader.attributes().value(QStringLiteral("y2")).toInt()));

		m_allItems << m_item;
		break;
	}
	case ItemType::Rect:
	{
		m_item = SPtrLCanvasItem(new LCanvasRect());
		m_item->setFillColor(QColor(reader.attributes().value(QStringLiteral("fill")).toString()));
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		int x = reader.attributes().value(QStringLiteral("x")).toInt();
		int y = reader.attributes().value(QStringLiteral("y")).toInt();
		int width = reader.attributes().value(QStringLiteral("width")).toInt();
		int height = reader.attributes().value(QStringLiteral("height")).toInt();
		m_item->setStartPos(QPoint(x, y));
		m_item->setEndPos(QPoint(x + width, y + height));

		m_allItems << m_item;
		break;
	}
	case ItemType::Ellipse:
	{
		m_item = SPtrLCanvasItem(new LCanvasEllipse());
		m_item->setFillColor(QColor(reader.attributes().value(QStringLiteral("fill")).toString()));
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		int cx = reader.attributes().value(QStringLiteral("cx")).toInt();
		int cy = reader.attributes().value(QStringLiteral("cy")).toInt();
		int rx = reader.attributes().value(QStringLiteral("rx")).toInt();
		int ry = reader.attributes().value(QStringLiteral("ry")).toInt();
		m_item->setStartPos(QPoint(cx - rx, cy - ry));
		m_item->setEndPos(QPoint(cx + rx, cy + cy));

		m_allItems << m_item;
		break;
	}
	case ItemType::Triangle:
	{
		m_item = SPtrLCanvasItem(new LCanvasTriangle());
		m_item->setFillColor(QColor(reader.attributes().value(QStringLiteral("fill")).toString()));
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		QString polygon = reader.attributes().value(QStringLiteral("points")).toString();
		QStringList points = polygon.split(QRegularExpression(QStringLiteral("\\D")), Qt::SkipEmptyParts);
		m_item->setStartPos(QPoint(points[4].toInt(), points[1].toInt()));
		m_item->setEndPos(QPoint(points[2].toInt(), points[3].toInt()));

		m_allItems << m_item;
		break;
	}
	case ItemType::Hexagon:
	{
		m_item = SPtrLCanvasItem(new LCanvasHexagon());
		m_item->setFillColor(QColor(reader.attributes().value(QStringLiteral("fill")).toString()));
		m_item->setStrokeColor(QColor(reader.attributes().value(QStringLiteral("stroke")).toString()));
		m_item->setStrokeWidth(reader.attributes().value(QStringLiteral("stroke-width")).toInt());

		QString polygon = reader.attributes().value(QStringLiteral("points")).toString();
		QStringList points = polygon.split(QRegularExpression(QStringLiteral("\\D")), Qt::SkipEmptyParts);
		m_item->setStartPos(QPoint(points[10].toInt(), points[1].toInt()));
		m_item->setEndPos(QPoint(points[4].toInt(), points[7].toInt()));

		m_allItems << m_item;
		break;
	}
	case ItemType::Text:
	{
		m_item = SPtrLCanvasItem(new LCanvasText());
		m_item->setFillColor(QColor(reader.attributes().value(QStringLiteral("fill")).toString()));

		m_item->setStartPos(QPoint(reader.attributes().value(QStringLiteral("x")).toInt(),
								   reader.attributes().value(QStringLiteral("y")).toInt()));
		m_item->setText(reader.readElementText());

		m_allItems << m_item;
		m_textItems << m_item;
		break;
	}
	default:
	{
		break;
	}
	}
}

} // namespace
