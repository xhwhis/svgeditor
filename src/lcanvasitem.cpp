#include "lcanvasitem.h"

namespace lwscode {

LCanvasItem::LCanvasItem(QWidget *parent)
	: m_itemType(None)
	, m_fillColor(Qt::white)
	, m_strokeColor(Qt::black)
	, m_strokeWidth(1.0f)
	, m_bSelected(false)
	, m_lineEdit(nullptr)
{
	this->setParent(parent);
}

LCanvasItem::ItemType LCanvasItem::getItemType()
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

void LCanvasItem::addPoint(const QPoint &point)
{
	m_points.push_back(point);
}

QLineEdit *LCanvasItem::lineEdit()
{
	return m_lineEdit;
}

void LCanvasItem::resizeLineEdit()
{
	if (m_lineEdit ==  nullptr)
		return;

	QFontMetrics fontMetrics = m_lineEdit->fontMetrics();
	int width = fontMetrics.horizontalAdvance(m_lineEdit->text()) + fontMetrics.averageCharWidth();
	int height = m_lineEdit->height();
	m_lineEdit->resize(width, height);
}

void LCanvasItem::setLineEditVisible()
{
	m_lineEdit->setVisible(!m_lineEdit->isVisible());
}

LCanvasPath::LCanvasPath(QWidget *parent)
	: LCanvasItem(parent)
{
	m_itemType = Path;
}

LCanvasLine::LCanvasLine(QWidget *parent)
	: LCanvasItem(parent)
{
	m_itemType = Line;
}

int LCanvasLine::sumDistance(const QPoint &p1, const QPoint &p2)
{
	return QLineF(p1, p2).length();
}

LCanvasRect::LCanvasRect(QWidget *parent)
	: LCanvasItem(parent)
{
	m_itemType = Rect;
}

LCanvasEllipse::LCanvasEllipse(QWidget *parent)
	: LCanvasItem(parent)
{
	m_itemType = Ellipse;
}

LCanvasTriangle::LCanvasTriangle(QWidget *parent)
	: LCanvasItem(parent)
	, m_vertices(QList<QPoint>(3, QPoint()))
{
	m_itemType = Triangle;
}

LCanvasHexagon::LCanvasHexagon(QWidget *parent)
	: LCanvasItem(parent)
	, m_vertices(QList<QPoint>(6, QPoint()))
{
	m_itemType = Hexagon;
}

LCanvasText::LCanvasText(QWidget *parent)
	: LCanvasItem(parent)
{
	m_itemType = Text;
	m_lineEdit = new QLineEdit(parent);
	m_lineEdit->setAlignment(Qt::AlignLeft);
	m_lineEdit->setFont(QFont("PingFang SC", 32));
	m_font = m_lineEdit->font();
	m_lineEdit->resize(m_lineEdit->fontMetrics().averageCharWidth(),
					   m_lineEdit->fontMetrics().height());

	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(resizeLineEdit()));
	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(setLineEditVisible()));
}

void LCanvasPath::paintItem(QPainter &painter)
{
	if (m_points.size() <= 1)
		return;

	QPainterPath path;
	path.moveTo(m_points[0]);
	for (int i = 1; i < m_points.size(); ++i)
		path.lineTo(m_points[i]);

	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawPath(path);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
}

void LCanvasLine::paintItem(QPainter &painter)
{
	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawLine(m_startPos, m_endPos);
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
}

void LCanvasRect::paintItem(QPainter &painter)
{
	painter.setBrush(m_fillColor);
	painter.setPen(QPen(m_strokeColor, m_strokeWidth));
	painter.drawRect(m_startPos.x(), m_startPos.y(),
					 m_endPos.x() - m_startPos.x(), m_endPos.y() - m_startPos.y());
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
}

void LCanvasEllipse::paintItem(QPainter &painter)
{
	QPen pen;
	pen.setWidth(m_strokeWidth);
	pen.setColor(m_strokeColor);
	painter.setPen(pen);
	painter.setBrush(m_fillColor);
	painter.drawEllipse(m_startPos.x(), m_startPos.y(),
						m_endPos.x() - m_startPos.x(), m_endPos.y() - m_startPos.y());
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
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

	painter.setBrush(QBrush(m_fillColor));
	painter.setPen(QPen(m_strokeColor, m_strokeWidth + 1));
	painter.drawPolygon(polygon);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);
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

	painter.setBrush(QBrush(m_fillColor));
	painter.setPen(QPen(m_strokeColor, m_strokeWidth + 1));
	painter.drawPolygon(polygon);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);
}

void LCanvasText::paintItem(QPainter &painter)
{
	QPen pen;
	pen.setStyle(Qt::DotLine);
	pen.setColor(m_strokeColor);
	pen.setWidth((m_strokeWidth + 1));
	painter.setPen(pen);
	QFont font = m_font;
	font.setPointSize(font.pointSize());
	m_lineEdit->setFont(font);
	m_lineEdit->move(m_startPos);
	setBoundingRect();
	painter.drawRect(m_boundingRect);

	if (m_lineEdit && !m_lineEdit->isVisible())
	{
		painter.setBrush(Qt::NoBrush);
		painter.setFont(font);
		painter.drawText(m_startPos.x(),m_startPos.y()+25, m_lineEdit->text());
	}
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::NoBrush);
}

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
	setBoundingRect();
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
	setBoundingRect();
}

void LCanvasHexagon::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	moveEndPos(dx, dy);
	setBoundingRect();
}

void LCanvasText::moveItem(int dx, int dy)
{
	moveStartPos(dx, dy);
	setBoundingRect();
}

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
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasLine::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasRect::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasEllipse::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasTriangle::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasHexagon::setBoundingRect()
{
	int left = qMin(m_startPos.x(), m_endPos.x());
	int top = qMin(m_startPos.y(), m_endPos.y());
	int width = qAbs(m_endPos.x() - m_startPos.x());
	int height = qAbs(m_endPos.y() - m_startPos.y());
	m_boundingRect = QRect(left, top, width, height).normalized();
	m_boundingRect.adjust(-m_strokeWidth - 5, -m_strokeWidth - 5, m_strokeWidth + 5, m_strokeWidth + 5);
}

void LCanvasText::setBoundingRect()
{
	m_boundingRect = QRect(m_startPos.x(), m_startPos.y(),
						   m_lineEdit->width(), m_lineEdit->height()).normalized();
}

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
	rect.adjust(-m_strokeWidth, -m_strokeWidth, +m_strokeWidth, +m_strokeWidth);

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

void LCanvasPath::writeItemToXml(QXmlStreamWriter &writer)
{
	QString pointPath = QString("M %1 %2").arg(m_points[0].x()).arg(m_points[0].y());
	for (int i = 1; i < m_points.size(); i++)
		pointPath += QString(" L %1 %2").arg(m_points[i].x()).arg(m_points[i].y());

	writer.writeStartElement("path");
	writer.writeAttribute("d",pointPath);
	writer.writeAttribute("fill", "white");
	writer.writeAttribute("stroke", "black");
	writer.writeAttribute("stroke-width", QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasLine::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement("line");
	writer.writeAttribute("x1", QString::number(m_startPos.x()));
	writer.writeAttribute("y1", QString::number(m_startPos.y()));
	writer.writeAttribute("x2", QString::number(m_endPos.x()));
	writer.writeAttribute("y2", QString::number(m_endPos.y()));
	writer.writeAttribute("stroke", "black");
	writer.writeAttribute("stroke-width", QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasRect::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement("rect");
	writer.writeAttribute("x", QString::number(m_startPos.x()));
	writer.writeAttribute("y", QString::number(m_startPos.y()));
	writer.writeAttribute("width", QString::number(qAbs(m_endPos.x() - m_startPos.x())));
	writer.writeAttribute("height", QString::number(qAbs(m_endPos.y() - m_startPos.y())));
	writer.writeAttribute("fill", m_fillColor.name());
	writer.writeAttribute("stroke", "black");
	writer.writeAttribute("stroke-width", QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasEllipse::writeItemToXml(QXmlStreamWriter& xmlWriter)
{
	xmlWriter.writeStartElement("ellipse");
	xmlWriter.writeAttribute("cx", QString::number((m_startPos.x() + m_endPos.x()) / 2));
	xmlWriter.writeAttribute("cy", QString::number((m_startPos.y() + m_endPos.y()) / 2));
	xmlWriter.writeAttribute("rx", QString::number(qAbs(m_endPos.x() - m_startPos.x()) / 2));
	xmlWriter.writeAttribute("ry", QString::number(qAbs(m_endPos.y() - m_startPos.y()) / 2));
	xmlWriter.writeAttribute("fill", m_fillColor.name());
	xmlWriter.writeAttribute("stroke", "black");
	xmlWriter.writeAttribute("stroke-width", QString::number(m_strokeWidth));
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

	writer.writeStartElement("polygon");
	writer.writeAttribute("points", points);
	writer.writeAttribute("fill", m_fillColor.name());
	writer.writeAttribute("stroke", "black");
	writer.writeAttribute("stroke-width", QString::number(m_strokeWidth));
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

	writer.writeStartElement("polygon");
	writer.writeAttribute("points", points);
	writer.writeAttribute("fill", m_fillColor.name());
	writer.writeAttribute("stroke", "black");
	writer.writeAttribute("stroke-width", QString::number(m_strokeWidth));
	writer.writeEndElement();
}

void LCanvasText::writeItemToXml(QXmlStreamWriter &writer)
{
	writer.writeStartElement("text");
	writer.writeAttribute("font-family", "PingFang SC");
	writer.writeAttribute("font-size", QString::number(m_font.pointSize()));
	writer.writeAttribute("x", QString::number(m_startPos.x()));
	writer.writeAttribute("y", QString::number(m_startPos.y()));
	writer.writeCharacters(m_lineEdit->text());
	writer.writeEndElement();
}

} // namespace
