#include "lcanvaspolygon.h"

//namespace lwscode {

LCanvasPolygon::LCanvasPolygon(LCanvas* canvas)
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

void LCanvasPolygon::moveBy(double deltaX, double deltaY)
{
	if (deltaX || deltaY) {
		removeFromChunks();
		m_polygon.translate(deltaX, deltaY);
		m_x += deltaX;
		m_y += deltaY;
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

//} // namespace
