#include "lcanvasshape.h"

//namespace lwscode {

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

LCanvasShape::LCanvasShape(LCanvas* canvas)
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

bool LCanvasShape::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, nullptr, nullptr, nullptr);
}

static bool collision_double_dispatch(
	const LCanvasShape *shape1,
	const LCanvasRectangle *rectangle1,
	const LCanvasEllipse *ellipse1,
	const LCanvasText *text1,
	const LCanvasShape *shape2,
	const LCanvasRectangle *rectangle2,
	const LCanvasEllipse *ellipse2,
	const LCanvasText *text2)
{
	const LCanvasItem *item1 = shape1 ?
				(const LCanvasItem *)shape1 : rectangle1 ?
				(const LCanvasItem *)rectangle1 : ellipse1 ?
				(const LCanvasItem *)ellipse1 : (const LCanvasItem *)text1;
	const LCanvasItem *item2 = shape2 ?
				(const LCanvasItem *)shape2 : rectangle2 ?
				(const LCanvasItem *)rectangle2 : ellipse2 ?
				(const LCanvasItem *)ellipse2 : (const LCanvasItem *)text2;

	if ((rectangle1 || text1) && (rectangle2 || text2))
	{
		QRect rect1 = item1->boundingRect();
		QRect rect2 = item2->boundingRect();
		return rect1.intersects(rect2);
	}
	else if (ellipse1 && ellipse2 &&
			 ellipse1->angleLength() >= 360 * 16 &&
			 ellipse2->angleLength() >= 360 * 16 &&
			 ellipse1->width() == ellipse1->height() &&
			 ellipse2->width() == ellipse2->height())
	{
		double dx = ellipse1->x() - ellipse2->x();
		double dy = ellipse1->y() - ellipse2->y();
		double dr = (ellipse1->width() + ellipse2->width()) / 2;
		return dx * dx +dy * dy <= dr * dr;
	}
	else if (shape1 && (shape2 || text2))
	{
		QPolygon polygon1 = shape1->areaPoints();
		QPolygon polygon2 = shape2 ? shape2->areaPoints() : QPolygon(item2->boundingRect());
		return !(QRegion(polygon1) & QRegion(polygon2, Qt::WindingFill)).isEmpty();
	}
	else
	{
		return collision_double_dispatch(shape2, rectangle2, ellipse2, text2,
										 shape1, rectangle1, ellipse1, text1);
	}
}

bool LCanvasShape::collidesWith(const LCanvasShape *shape,
								const LCanvasRectangle *rectangle,
								const LCanvasEllipse *ellipse,
								const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 this, nullptr, nullptr, nullptr);
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

//} // namespace
