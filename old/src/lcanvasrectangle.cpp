#include "lcanvasrectangle.h"

//namespace lwscode {

LCanvasRectangle::LCanvasRectangle(LCanvas *canvas)
	: LCanvasShape(canvas)
	, m_width(32)
	, m_height(32)
{

}

LCanvasRectangle::LCanvasRectangle(const QRect &rect, LCanvas *canvas)
	: LCanvasShape(canvas)
	, m_width(rect.width())
	, m_height(rect.height())
{
	move(rect.left(), rect.top());
}

LCanvasRectangle::LCanvasRectangle(int x, int y, int width, int height, LCanvas *canvas)
	: LCanvasShape(canvas)
	, m_width(width)
	, m_height(height)
{
	move(x, y);
}

LCanvasRectangle::~LCanvasRectangle()
{
	hide();
}

int LCanvasRectangle::width() const
{
	return m_width;
}

int LCanvasRectangle::height() const
{
	return m_height;
}

void LCanvasRectangle::setSize(int width, int height)
{
	if (m_width != width || m_height != height) {
		removeFromChunks();
		m_width = width;
		m_height = height;
		addToChunks();
	}
}

QPolygon LCanvasRectangle::areaPoints() const
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

void LCanvasRectangle::drawShape(QPainter & p)
{
	p.drawRect(this->x(), this->y(), m_width, m_height);
}

int LCanvasRectangle::g_type = Rectangle;

bool LCanvasRectangle::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, this, nullptr, nullptr);
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

bool LCanvasRectangle::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRectangle *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 this, this, nullptr, nullptr);
}

QPolygon LCanvasRectangle::chunks() const
{
	return LCanvasItem::chunks();
}

//} // namespace
