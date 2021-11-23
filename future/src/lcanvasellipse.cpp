#include "lcanvasellipse.h"

namespace lwscode {

LCanvasEllipse::LCanvasEllipse(LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(32)
	, m_height(32)
{

}

LCanvasEllipse::LCanvasEllipse(int width, int height, LCanvasScene* canvas)
	: LCanvasShape(canvas)
	, m_width(width)
	, m_height(height)
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
	painter.drawEllipse(QRect(this->x(), this->y(), m_width, m_height).normalized());
}

int LCanvasEllipse::g_type = Ellipse;

} // namespace
