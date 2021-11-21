#include "lcanvasitem.h"

namespace lwscode {

// LCanvasItem
LCanvasItem::LCanvasItem()
	: m_itemType(ItemType::NoneType)
	, m_fillColor(Qt::white)
	, m_strokeColor(Qt::black)
	, m_strokeWidth(1.0f)
	, m_bSelected(false)
{

}

ItemType LCanvasItem::getItemType()
{
	return m_itemType;
}

QPoint LCanvasItem::startPos()
{
	return m_startPos;
}

void LCanvasItem::setStartPos(const QPoint &point)
{
	m_startPos = point;
}

void LCanvasItem::moveStartPos(int dx, int dy)
{
	m_startPos += QPoint(dx, dy);
}

QPoint LCanvasItem::endPos()
{
	return m_endPos;
}

void LCanvasItem::setEndPos(const QPoint &point)
{
	m_endPos = point;
}

void LCanvasItem::moveEndPos(int dx, int dy)
{
	m_endPos += QPoint(dx, dy);
}

void LCanvasItem::setFillColor(const QColor &color)
{
	m_fillColor = color;
}

void LCanvasItem::setStrokeColor(const QColor &color)
{
	m_strokeColor = color;
}

void LCanvasItem::setStrokeWidth(int width)
{
	m_strokeWidth = width;
}

bool LCanvasItem::isSelected()
{
	return m_bSelected;
}

void LCanvasItem::setSelected(bool selected)
{
	m_bSelected = selected;
}

QRect LCanvasItem::boundingRect()
{
	return m_boundingRect.isValid() ? m_boundingRect : QRect();
}

// LCanvasPath
LCanvasPath::LCanvasPath()
{
	m_itemType = ItemType::Path;
}

void LCanvasPath::addPoint(const QPoint &point)
{
	m_points.push_back(point);
}

// LCanvasLine
LCanvasLine::LCanvasLine()
{
	m_itemType = ItemType::Line;
}

int LCanvasLine::sumDistance(const QPoint &p1, const QPoint &p2)
{
	return QLineF(p1, p2).length();
}

// LCanvasRect
LCanvasRect::LCanvasRect()
{
	m_itemType = ItemType::Rect;
}

// LCanvasEllipse
LCanvasEllipse::LCanvasEllipse()
{
	m_itemType = ItemType::Ellipse;
}

// LCanvasTriangle
LCanvasTriangle::LCanvasTriangle()
	: m_vertices(QPoints(3, QPoint()))
{
	m_itemType = ItemType::Triangle;
}

// LCanvasHexagon
LCanvasHexagon::LCanvasHexagon()
	: m_vertices(QPoints(6, QPoint()))
{
	m_itemType = ItemType::Hexagon;
}

// LCanvasText
LCanvasText::LCanvasText()
	: m_width(0)
	, m_height(0)
	, m_text(QString::fromUtf8("text"))
{
	m_itemType = ItemType::Text;
	m_fillColor = Qt::black;
}

void LCanvasText::setFont(const QFont &font)
{
	m_font = font;

	QFontMetrics fontMetrics(m_font);
	QRect rect = fontMetrics.boundingRect(m_text);
	m_width = rect.width() < 8 ? 8 : rect.width();
	m_height = rect.height();
	m_boundingRect = QRect(m_startPos.x(), m_startPos.y(), m_width, m_height);
}

QFont LCanvasText::font() const
{
	return m_font;
}

void LCanvasText::setText(const QString &text)
{
	m_text = text;

	QFontMetrics fontMetrics(m_font);
	QRect rect = fontMetrics.boundingRect(m_text);
	m_width = rect.width() < 8 ? 8 : rect.width();
	m_height = rect.height();
	m_boundingRect = QRect(m_startPos.x(), m_startPos.y(), m_width, m_height);
}

QString LCanvasText::text() const
{
	return m_text;
}

// paintItem
void LCanvasPath::paintItem(QPainter &painter)
{
	if (m_points.size() <= 1)
		return;

	QPainterPath path;
	path.moveTo(m_points[0]);
	for (int i = 1; i < m_points.size(); ++i)
		path.lineTo(m_points[i]);

	painter.save();
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawPath(m_path);
	painter.restore();
}

void LCanvasLine::paintItem(QPainter &painter)
{
	painter.save();
	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawLine(m_startPos, m_endPos);
	painter.restore();
}

void LCanvasRect::paintItem(QPainter &painter)
{
	painter.save();
	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
//	painter.drawRect(m_startPos.x(), m_startPos.y(),
//					 m_endPos.x() - m_startPos.x(), m_endPos.y() - m_startPos.y());
	painter.drawPath(m_path);
	painter.restore();
}

void LCanvasEllipse::paintItem(QPainter &painter)
{
	painter.save();
	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawEllipse(m_startPos.x(), m_startPos.y(),
						m_endPos.x() - m_startPos.x(), m_endPos.y() - m_startPos.y());
	painter.restore();
}

void LCanvasTriangle::paintItem(QPainter &painter)
{
	int left = m_startPos.x();
	int top = m_startPos.y();
	int right = m_endPos.x();
	int bottom = m_endPos.y();
	m_vertices[0] = QPoint((left + right) / 2, top);
	m_vertices[1] = QPoint(right, bottom);
	m_vertices[2] = QPoint(left, bottom);
	QPolygon polygon(m_vertices);

	painter.save();
	painter.setBrush(QBrush(m_fillColor));
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawPolygon(polygon);
	painter.restore();
}

void LCanvasHexagon::paintItem(QPainter &painter)
{
	int left = m_startPos.x();
	int top = m_startPos.y();
	int right = m_endPos.x();
	int bottom = m_endPos.y();
	m_vertices[0] = QPoint((left * 3 + right) / 4, top);
	m_vertices[1] = QPoint((left + right * 3) / 4, top);
	m_vertices[2] = QPoint(right, (top + bottom) / 2);
	m_vertices[3] = QPoint((left + right * 3) / 4, bottom);
	m_vertices[4] = QPoint((left * 3 + right) / 4, bottom);
	m_vertices[5] = QPoint(left, (top + bottom) / 2);
	QPolygon polygon(m_vertices);

	painter.save();
	painter.setBrush(QBrush(m_fillColor));
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawPolygon(polygon);
	painter.restore();
}

void LCanvasText::paintItem(QPainter &painter)
{
	painter.save();
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.setFont(m_font);
	painter.drawText(m_boundingRect, 0, m_text);
	painter.restore();
}

// moveItem
void LCanvasPath::moveItem(int dx, int dy)
{
	QPoint offset(dx, dy);
	for (int i = 0; i < m_points.size(); ++i)
		m_points[i] += offset;
}

void LCanvasLine::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
}

void LCanvasRect::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
}

void LCanvasEllipse::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
}

void LCanvasTriangle::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
}

void LCanvasHexagon::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
}

void LCanvasText::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	setBoundingRect();
}

// scaleItem
void LCanvasPath::scaleItem(double sx, double sy)
{
	for (int i = 0; i < m_points.size(); i++)
	{
		m_points[i].rx() *= sx;
		m_points[i].ry() *= sy;
	}
}

void LCanvasLine::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
	m_endPos.rx() *= sx;
	m_endPos.ry() *= sy;
}

void LCanvasRect::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
	m_endPos.rx() *= sx;
	m_endPos.ry() *= sy;
}

void LCanvasEllipse::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
	m_endPos.rx() *= sx;
	m_endPos.ry() *= sy;
}

void LCanvasTriangle::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
	m_endPos.rx() *= sx;
	m_endPos.ry() *= sy;
}

void LCanvasHexagon::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
	m_endPos.rx() *= sx;
	m_endPos.ry() *= sy;
}

void LCanvasText::scaleItem(double sx, double sy)
{
	m_startPos.rx() *= sx;
	m_startPos.ry() *= sy;
}

// stretchItemTo
void LCanvasPath::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasLine::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasRect::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasEllipse::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasTriangle::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasHexagon::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

void LCanvasText::stretchItemTo(StretchItemDir dir, int x, int y)
{

}

// updatePath
void LCanvasPath::updatePath()
{
	if (m_path.isEmpty())
		m_path.moveTo(m_startPos);
	m_path.lineTo(m_endPos);
}

void LCanvasLine::updatePath()
{
	m_path.clear();
	m_path.moveTo(m_startPos);
	m_path.lineTo(m_endPos);
}

void LCanvasRect::updatePath()
{
	m_path.clear();
	m_path.addRect(QRect(m_startPos, m_endPos).normalized());
}

void LCanvasEllipse::updatePath()
{
	m_path.clear();
	m_path.addEllipse(QRect(m_startPos, m_endPos).normalized());
}

void LCanvasTriangle::updatePath()
{
	int left = m_startPos.x();
	int top = m_startPos.y();
	int right = m_endPos.x();
	int bottom = m_endPos.y();
	m_vertices[0] = QPoint((left + right) / 2, top);
	m_vertices[1] = QPoint(right, bottom);
	m_vertices[2] = QPoint(left, bottom);
	QPolygon polygon(m_vertices);

	m_path.clear();
	m_path.addPolygon(polygon);
}

void LCanvasHexagon::updatePath()
{
	int left = m_startPos.x();
	int top = m_startPos.y();
	int right = m_endPos.x();
	int bottom = m_endPos.y();
	m_vertices[0] = QPoint((left * 3 + right) / 4, top);
	m_vertices[1] = QPoint((left + right * 3) / 4, top);
	m_vertices[2] = QPoint(right, (top + bottom) / 2);
	m_vertices[3] = QPoint((left + right * 3) / 4, bottom);
	m_vertices[4] = QPoint((left * 3 + right) / 4, bottom);
	m_vertices[5] = QPoint(left, (top + bottom) / 2);
	QPolygon polygon(m_vertices);

	m_path.clear();
	m_path.addPolygon(polygon);
}

void LCanvasText::updatePath()
{
	m_path.clear();
	m_path.addText(m_startPos, m_font, m_text);
}

// setBoundingRect
void LCanvasPath::setBoundingRect()
{
	int left = m_points[0].x();
	int top = m_points[0].y();
	int right = m_points[0].x();
	int bottom = m_points[0].y();
	for (int i = 1; i < m_points.size(); ++i)
	{
		left = qMin(left, m_points[i].x());
		top = qMin(top, m_points[i].y());
		right = qMax(right, m_points[i].x());
		bottom = qMax(bottom, m_points[i].y());
	}
	m_boundingRect = QRect(QPoint(left, top), QPoint(right, bottom)).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasLine::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasRect::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasEllipse::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasTriangle::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasHexagon::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	int d = (m_strokeWidth + 1) / 2 + 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

void LCanvasText::setBoundingRect()
{
	m_boundingRect = QRect(m_startPos.x(), m_startPos.y(), m_width, m_height).normalized();
	int d = 4;
	m_boundingRect.adjust(-d, -d, d, d);
}

// containsPos
bool LCanvasPath::containsPos(const QPoint &point)
{
	for (int i = 0; i < m_points.size(); ++i)
	{
		if (point.x() - m_points[i].x() < 6 &&
			point.x() - m_points[i].x() > -6 &&
			point.y() - m_points[i].y() < 6 &&
			point.y() - m_points[i].y() > -6)
			return true;
	}

	return false;
}

bool LCanvasLine::containsPos(const QPoint &point)
{
	int length = sumDistance(m_startPos, m_endPos);
	int lenOne = sumDistance(m_startPos, point);
	int lenTwo = sumDistance(m_endPos, point);

	return length == (lenOne + lenTwo);
}

bool LCanvasRect::containsPos(const QPoint &point)
{
	QRect rect = QRect(m_startPos, m_endPos);
	rect.adjust(-m_strokeWidth, -m_strokeWidth, m_strokeWidth, m_strokeWidth);

	return rect.contains(point);
}

bool LCanvasEllipse::containsPos(const QPoint &point)
{
	m_width = abs(m_endPos.x() - m_startPos.x()) / 2;
	m_height = abs(m_endPos.y() - m_startPos.y()) / 2;
	QPoint centerPoint = QPoint((m_startPos.x() + m_endPos.x())/2,(m_startPos.y() + m_endPos.y())/2);
	return pow(point.x() - centerPoint.x(), 2) / pow(m_width, 2) + pow(point.y() - centerPoint.y(), 2) / pow(m_height, 2) <= 1;
}

bool LCanvasTriangle::containsPos(const QPoint &point)
{
	QPolygon polygon(m_vertices);
	if (polygon.containsPoint(point, Qt::WindingFill))
		return true;

	return false;
}

bool LCanvasHexagon::containsPos(const QPoint &point)
{
	QPolygon polygon(m_vertices);
	if (polygon.containsPoint(point, Qt::WindingFill))
		return true;

	return false;
}

bool LCanvasText::containsPos(const QPoint &point)
{
	if (m_boundingRect.isValid())
		return m_boundingRect.contains(point);

	return false;
}

// clone
SPtrLCanvasItem LCanvasPath::clone()
{
	return SPtrLCanvasItem(new LCanvasPath(*this));
}

SPtrLCanvasItem LCanvasLine::clone()
{
	return SPtrLCanvasItem(new LCanvasLine(*this));
}

SPtrLCanvasItem LCanvasRect::clone()
{
	return SPtrLCanvasItem(new LCanvasRect(*this));
}

SPtrLCanvasItem LCanvasEllipse::clone()
{
	return SPtrLCanvasItem(new LCanvasEllipse(*this));
}

SPtrLCanvasItem LCanvasTriangle::clone()
{
	return SPtrLCanvasItem(new LCanvasTriangle(*this));
}

SPtrLCanvasItem LCanvasHexagon::clone()
{
	return SPtrLCanvasItem(new LCanvasHexagon(*this));
}

SPtrLCanvasItem LCanvasText::clone()
{
	return SPtrLCanvasItem(new LCanvasText(*this));
}

// writeItemToXml
void LCanvasPath::writeItemToXml(QXmlStreamWriter &writer)
{
	QString pointPath = QString("M%1 %2").arg(m_points[0].x()).arg(m_points[0].y());
	for (int i = 1; i < m_points.size(); i++)
		pointPath += QString(" L%1 %2").arg(m_points[i].x()).arg(m_points[i].y());

	writer.writeStartElement(QString::fromUtf8("path"));
	writer.writeAttribute(QString::fromUtf8("d"), pointPath);
	writer.writeAttribute(QString::fromUtf8("fill"), QString::fromUtf8("none"));
	writer.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	writer.writeEndElement();
}

void LCanvasLine::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement(QString::fromUtf8("line"));
	writer.writeAttribute(QString::fromUtf8("x1"), QString::number(m_startPos.x()));
	writer.writeAttribute(QString::fromUtf8("y1"), QString::number(m_startPos.y()));
	writer.writeAttribute(QString::fromUtf8("x2"), QString::number(m_endPos.x()));
	writer.writeAttribute(QString::fromUtf8("y2"), QString::number(m_endPos.y()));
	writer.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke-width"), QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasRect::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement(QString::fromUtf8("rect"));
	writer.writeAttribute(QString::fromUtf8("x"), QString::number(m_startPos.x()));
	writer.writeAttribute(QString::fromUtf8("y"), QString::number(m_startPos.y()));
	writer.writeAttribute(QString::fromUtf8("width"), QString::number(qAbs(m_endPos.x() - m_startPos.x())));
	writer.writeAttribute(QString::fromUtf8("height"), QString::number(qAbs(m_endPos.y() - m_startPos.y())));
	writer.writeAttribute(QString::fromUtf8("fill"), m_fillColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke-width"), QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasEllipse::writeItemToXml(QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement(QString::fromUtf8("ellipse"));
	xmlWriter.writeAttribute(QString::fromUtf8("cx"), QString::number((m_startPos.x() + m_endPos.x()) / 2));
	xmlWriter.writeAttribute(QString::fromUtf8("cy"), QString::number((m_startPos.y() + m_endPos.y()) / 2));
	xmlWriter.writeAttribute(QString::fromUtf8("rx"), QString::number(qAbs(m_endPos.x() - m_startPos.x()) / 2));
	xmlWriter.writeAttribute(QString::fromUtf8("ry"), QString::number(qAbs(m_endPos.y() - m_startPos.y()) / 2));
	xmlWriter.writeAttribute(QString::fromUtf8("fill"), m_fillColor.name());
	xmlWriter.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	xmlWriter.writeAttribute(QString::fromUtf8("stroke-width"), QString::number(m_strokeWidth));
	xmlWriter.writeEndElement();
}

void LCanvasTriangle::writeItemToXml(QXmlStreamWriter &writer)
{
	QString points;
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		if (i) points += " ";
		points += QString("%1,%2").arg(m_vertices[i].x()).arg(m_vertices[i].y());
	}

	writer.writeStartElement(QString::fromUtf8("polygon"));
	writer.writeAttribute(QString::fromUtf8("points"), points);
	writer.writeAttribute(QString::fromUtf8("fill"), m_fillColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke-width"), QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasHexagon::writeItemToXml(QXmlStreamWriter &writer)
{
	QString points;
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		if (i) points += " ";
		points += QString("%1,%2").arg(m_vertices[i].x()).arg(m_vertices[i].y());
	}

	writer.writeStartElement(QString::fromUtf8("polygon"));
	writer.writeAttribute(QString::fromUtf8("points"), points);
	writer.writeAttribute(QString::fromUtf8("fill"), m_fillColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke"), m_strokeColor.name());
	writer.writeAttribute(QString::fromUtf8("stroke-width"), QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasText::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement(QString::fromUtf8("text"));
	writer.writeAttribute(QString::fromUtf8("font-family"), m_font.family());
	writer.writeAttribute(QString::fromUtf8("font-size"), QString::number(m_font.pointSize()));
	writer.writeAttribute(QString::fromUtf8("x"), QString::number(m_startPos.x()));
	writer.writeAttribute(QString::fromUtf8("y"), QString::number(m_startPos.y()));
	writer.writeCharacters(m_text);
	writer.writeEndElement();
}

} // namespace
