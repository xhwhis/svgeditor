#include "lcanvasshape.h"

namespace lwscode {

static const QPen &defaultPolygonPen()
{
	static QPen *g_pen = nullptr;
	if (!g_pen)
		g_pen = new QPen();
	return *g_pen;
}

static const QBrush& defaultPolygonBrush()
{
	static QBrush *g_brush = nullptr;
	if (!g_brush)
		g_brush = new QBrush();
	return *g_brush;
}

LCanvasShape::LCanvasShape(LCanvasScene* canvas)
	: LCanvasItem(canvas)
	, m_brush(defaultPolygonBrush())
	, m_pen(defaultPolygonPen())
{

}

LCanvasShape::~LCanvasShape()
{

}

void LCanvasShape::invalidate()
{
	m_bValid = false;
	removeFromChunks();
}

QRect LCanvasShape::boundingRect() const
{
	return areaPoints().boundingRect();
}

void LCanvasShape::draw(QPainter &painter)
{
	painter.setPen(m_pen);
	painter.setBrush(m_brush);
	drawShape(painter);
}

void LCanvasShape::setPen(QPen pen)
{
	if (m_pen != pen)
	{
		removeFromChunks();
		m_pen = pen;
		addToChunks();
	}
}

void LCanvasShape::setBrush(QBrush brush)
{
	if (m_brush != brush) {
		m_brush = brush;
		changeChunks();
	}
}

int LCanvasShape::g_type = Shape;

} // namespace
