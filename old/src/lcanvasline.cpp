#include "lcanvasline.h"

namespace lwscode {

LCanvasLine::LCanvasLine(LCanvasScene* canvas)
	: LCanvasShape(canvas)
	, m_x1(0)
	, m_y1(0)
	, m_x2(0)
	, m_y2(0)
{

}

LCanvasLine::~LCanvasLine()
{
	hide();
}

void LCanvasLine::setPoints(int x1, int y1, int x2, int y2)
{
	if (m_x1 != x1 || m_y1 != y1 || m_x2 != x2 || m_y2 != y2)
	{
		removeFromChunks();
		m_x1 = x1;
		m_y1 = y1;
		m_x2 = x2;
		m_y2 = y2;
		addToChunks();
	}
}

void LCanvasLine::drawShape(QPainter &painter)
{
	painter.drawLine((int)(this->x() + m_x1), (int)(this->y() + m_y1),
					 (int)(this->x() + m_x2), (int)(this->y() + m_y2));
}

QPolygon LCanvasLine::areaPoints() const
{
	QPolygon polygon(4);
	int x = int(this->x());
	int y = int(this->y());
	int penWidth = pen().width();
	int dx = qAbs(m_x1 - m_x2);
	int dy = qAbs(m_y1 - m_y2);
	penWidth = penWidth * 4 / 3 + 2;
	int px = m_x1 < m_x2 ? -penWidth : penWidth;
	int py = m_y1 < m_y2 ? -penWidth : penWidth;
	if (dx && dy && (dx > dy ? (dx * 2 / dy <= 2) : (dy * 2 / dx <= 2)))
	{
		if (px == py)
		{
			polygon[0] = QPoint(m_x1 + x, m_y1 + y + py);
			polygon[1] = QPoint(m_x2 + x - px, m_y2 + y);
			polygon[2] = QPoint(m_x2 + x, m_y2 + y - py);
			polygon[3] = QPoint(m_x1 + x + px, m_y1 + y);
		}
		else
		{
			polygon[0] = QPoint(m_x1 + x + px, m_y1 + y);
			polygon[1] = QPoint(m_x2 + x, m_y2 + y - py);
			polygon[2] = QPoint(m_x2 + x - px, m_y2 + y);
			polygon[3] = QPoint(m_x1 + x, m_y1 + y + py);
		}
	}
	else if (dx > dy)
	{
		polygon[0] = QPoint(m_x1 + x + px, m_y1 + y + py);
		polygon[1] = QPoint(m_x2 + x - px, m_y2 + y + py);
		polygon[2] = QPoint(m_x2 + x - px, m_y2 + y - py);
		polygon[3] = QPoint(m_x1 + x + px, m_y1 + y - py);
	}
	else
	{
		polygon[0] = QPoint(m_x1 + x + px, m_y1 + y + py);
		polygon[1] = QPoint(m_x2 + x + px, m_y2 + y - py);
		polygon[2] = QPoint(m_x2 + x - px, m_y2 + y - py);
		polygon[3] = QPoint(m_x1 + x - px, m_y1 + y + py);
	}
	return polygon;
}

int LCanvasLine::g_type = Line;

} // namespace
