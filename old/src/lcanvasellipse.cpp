#include "lcanvasellipse.h"

//namespace lwscode {

LCanvasEllipse::LCanvasEllipse(LCanvas *canvas)
	: LCanvasShape(canvas)
	, m_width(32)
	, m_height(32)
	, m_angleStart(0)
	, m_angleLength(360 * 16)
{

}

LCanvasEllipse::LCanvasEllipse(int width, int height, LCanvas* canvas)
	: LCanvasShape(canvas)
	, m_width(width)
	, m_height(height)
	, m_angleStart(0)
	, m_angleLength(360 * 16)
{

}

LCanvasEllipse::LCanvasEllipse(int width, int height, int angleStart, int angle, LCanvas *canvas)
	: LCanvasShape(canvas)
	, m_width(width)
	, m_height(height)
	, m_angleStart(angleStart)
	, m_angleLength(angle)
{

}

LCanvasEllipse::~LCanvasEllipse()
{
	hide();
}

int LCanvasEllipse::width() const
{
	return m_width;
}

int LCanvasEllipse::height() const
{
	return m_height;
}

void LCanvasEllipse::setSize(int width, int height)
{
	if (m_width != width || m_height != height)
	{
		removeFromChunks();
		m_width = width;
		m_height = height;
		addToChunks();
	}
}

void LCanvasEllipse::setAngles(int start, int length)
{
	if (m_angleStart != start || m_angleLength != length) {
		removeFromChunks();
		m_angleStart = start;
		m_angleLength = length;
		addToChunks();
	}
}

QPolygon LCanvasEllipse::areaPoints() const
{
	QPainterPath path;
	int penWidth = (pen().width() + 1) / 2;
	if (penWidth < 1) penWidth = 1;
	if (pen() == Qt::NoPen) penWidth = 0;
	path.addEllipse(this->x() - penWidth, this->y() - penWidth,
					m_width + penWidth * 2, m_height + penWidth * 2);
	return path.toFillPolygon().toPolygon();
}

void LCanvasEllipse::drawShape(QPainter &painter)
{
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(this->x(), this->y(), m_width, m_height);
}

int LCanvasEllipse::g_type = Ellipse;

bool LCanvasEllipse::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, nullptr, this, nullptr);
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

bool LCanvasEllipse::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRectangle *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 this, nullptr, this, nullptr);
}

//} // namespace
