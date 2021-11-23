#include "lcanvaspolygon.h"

namespace lwscode {

LCanvasPolygon::LCanvasPolygon(LCanvasScene* canvas)
	: LCanvasShape(canvas)
{

}

LCanvasPolygon::~LCanvasPolygon()
{
	hide();
}

void LCanvasPolygon::drawShape(QPainter &painter)
{
	painter.setPen(Qt::NoPen);
	painter.drawPolygon(m_polygon);
}

void LCanvasPolygon::setPoints(QPolygon polygon)
{
	removeFromChunks();
	m_polygon = polygon;
	m_polygon.detach();
	m_polygon.translate(x(), y());
	addToChunks();
}

void LCanvasPolygon::moveBy(double dX, double dY)
{
	if (dX || dY) {
		removeFromChunks();
		m_polygon.translate(dX, dY);
		m_x += dX;
		m_y += dY;
		addToChunks();
	}
}

QPolygon LCanvasPolygon::points() const
{
	QPolygon polygon = areaPoints();
	polygon.translate(int(-(this->x())), int(-(this->y())));
	return polygon;
}

QPolygon LCanvasPolygon::areaPoints() const
{
	return m_polygon;
}

int LCanvasPolygon::g_type = Polygon;

} // namespace
