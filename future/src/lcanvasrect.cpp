#include "lcanvasrect.h"

namespace lwscode {

LCanvasRect::LCanvasRect(LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(32)
	, m_height(32)
{

}

LCanvasRect::LCanvasRect(const QRect &rect, LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(rect.width())
	, m_height(rect.height())
{
	move(rect.left(), rect.top());
}

LCanvasRect::LCanvasRect(int x, int y, int width, int height, LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(width)
	, m_height(height)
{
	move(x, y);
}

LCanvasRect::~LCanvasRect()
{
	hide();
}

int LCanvasRect::width() const
{
	return m_width;
}

int LCanvasRect::height() const
{
	return m_height;
}

void LCanvasRect::setSize(int width, int height)
{
	if (m_width != width || m_height != height) {
		removeFromChunks();
		m_width = width;
		m_height = height;
		addToChunks();
	}
}

QPolygon LCanvasRect::areaPoints() const
{
	QPolygon polygon(4);
	int penWidth = (pen().width() + 1) / 2;
	if (penWidth < 1) penWidth = 1;
	if (pen() == Qt::NoPen) penWidth = 0;
	polygon[0] = QPoint(this->x() - penWidth, this->y() - penWidth);
	polygon[1] = polygon[0] + QPoint(m_width + penWidth * 2, 0);
	polygon[2] = polygon[1] + QPoint(0, m_height + penWidth * 2);
	polygon[3] = polygon[0] + QPoint(0, m_height + penWidth * 2);
	return polygon;
}

void LCanvasRect::drawShape(QPainter & p)
{
	p.drawRect(this->x(), this->y(), m_width, m_height);
}

int LCanvasRect::g_type = Rectangle;

} // namespace
