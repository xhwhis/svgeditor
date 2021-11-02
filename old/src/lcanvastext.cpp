#include "lcanvastext.h"

//namespace lwscode {

LCanvasText::LCanvasText(LCanvas *canvas)
	: LCanvasItem(canvas)
	, m_text("text")
	, m_flags(0)
{
	setRect();
}

LCanvasText::LCanvasText(const QString &text, LCanvas *canvas)
	: LCanvasItem(canvas)
	, m_text(text)
	, m_flags(0)
{
	setRect();
}

LCanvasText::LCanvasText(const QString &text, const QFont &font, LCanvas *canvas)
	: LCanvasItem(canvas)
	, m_text(text)
	, m_flags(0)
	, m_font(font)
{
	setRect();
}

LCanvasText::~LCanvasText()
{
	removeFromChunks();
}

void LCanvasText::setRect()
{
	m_boundingRect = QFontMetrics(m_font).boundingRect(int(x()), int(y()), 0, 0, m_flags, m_text);
}

QRect LCanvasText::boundingRect() const
{
	return m_boundingRect;
}

void LCanvasText::setTextFlags(int flags)
{
	if (m_flags != flags)
	{
		removeFromChunks();
		m_flags = flags;
		setRect();
		addToChunks();
	}
}

QString LCanvasText::text() const
{
	return m_text;
}

void LCanvasText::setText(const QString &text)
{
	if (m_text != text)
	{
		removeFromChunks();
		m_text = text;
		setRect();
		addToChunks();
	}
}

QFont LCanvasText::font() const
{
	return m_font;
}

void LCanvasText::setFont(const QFont &font)
{
	if (m_font != font) {
		removeFromChunks();
		m_font = font;
		setRect();
		addToChunks();
	}
}

QColor LCanvasText::color() const
{
	return m_color;
}

void LCanvasText::setColor(const QColor& c)
{
	m_color = color();
	changeChunks();
}

void LCanvasText::moveBy(double deltaX, double deltaY)
{
	if (deltaX || deltaY)
	{
		removeFromChunks();
		m_x += deltaX;
		m_y += deltaY;
		m_boundingRect.translate(deltaX, deltaY);
		addToChunks();
	}
}

void LCanvasText::draw(QPainter &painter)
{
	painter.setFont(m_font);
	painter.setPen(m_color);
	painter.drawText(painter.fontMetrics().boundingRect(this->x(), this->y(), 0, 0, m_flags, m_text), m_flags, m_text);
}

void LCanvasText::changeChunks()
{
	if (this->isVisible() && this->canvas())
	{
		int chunkSize = canvas()->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				canvas()->setChangedChunk(i, j);
	}
}

void LCanvasText::addToChunks()
{
	if (this->isVisible() && this->canvas())
	{
		int chunkSize = canvas()->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				canvas()->addItemToChunk(this, i, j);
	}
}

void LCanvasText::removeFromChunks()
{
	if (this->isVisible() && this->canvas())
	{
		int chunkSize = canvas()->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				canvas()->removeItemFromChunk(this, i, j);
	}
}

int LCanvasText::g_type = Text;

bool LCanvasText::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(nullptr, nullptr, nullptr, this);
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

bool LCanvasText::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRectangle *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 nullptr, nullptr, nullptr, this);
}

//} // namespace
