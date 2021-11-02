#include "lcanvasitem.h"

//namespace lwscode {

LCanvasItem::LCanvasItem(LCanvas *canvas)
	: m_canvas(canvas)
	, m_x(0)
	, m_y(0)
	, m_z(0)
	, m_bVisible(false)
	, m_bValid(false)
	, m_bSelected(false)
	, m_bEnabled(false)
	, m_bActive(false)
{
	if (m_canvas)
		m_canvas->addItem(this);
}

LCanvasItem::~LCanvasItem()
{
	if (m_canvas)
		m_canvas->removeItem(this);
}

void LCanvasItem::moveBy(double deltaX, double deltaY)
{
	if (deltaX || deltaY)
	{
		removeFromChunks();
		m_x += deltaX;
		m_y += deltaY;
		addToChunks();
	}
}

void LCanvasItem::move(double x, double y)
{
	moveBy(x - m_x, y - m_y);
}

void LCanvasItem::setCanvas(LCanvas *canvas)
{
	bool visible = m_bVisible;
	setVisible(false);
	if (m_canvas)
		m_canvas->removeItem(this);

	m_canvas = canvas;
	if (m_canvas)
		m_canvas->addItem(this);

	setVisible(visible);
}

void LCanvasItem::show()
{
	setVisible(true);
}

void LCanvasItem::hide()
{
	setVisible(false);
}

void LCanvasItem::setVisible(bool visible)
{
	if (m_bVisible != visible)
	{
		if (visible)
		{
			m_bVisible = visible;
			addToChunks();
		}
		else
		{
			removeFromChunks();
			m_bVisible = visible;
		}
	}
}

void LCanvasItem::setSelected(bool selected)
{
	if (m_bSelected != selected)
	{
		m_bSelected = selected;
		changeChunks();
	}
}

void LCanvasItem::setEnabled(bool enabled)
{
	if (m_bEnabled!= enabled)
	{
		m_bEnabled = enabled;
		changeChunks();
	}
}

void LCanvasItem::setActive(bool active)
{
	if (m_bActive != active)
	{
		m_bActive = active;
		changeChunks();
	}
}

LCanvasItemList LCanvasItem::collisions(bool exact) const
{
	return m_canvas->collisions(chunks(), this, exact);
}

void LCanvasItem::addToChunks()
{
	if (m_bVisible && m_canvas)
	{
		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); ++i)
			canvas()->addItemToChunk(this, polygon[i].x(), polygon[i].y());

		m_bValid = true;
	}
}

void LCanvasItem::removeFromChunks()
{
	if (m_bVisible && m_canvas)
	{
		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); ++i)
			canvas()->removeItemFromChunk(this, polygon[i].x(), polygon[i].y());
	}
}

void LCanvasItem::changeChunks()
{
	if (m_bVisible && m_canvas) {
		if (!m_bValid)
			addToChunks();

		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); i++)
			canvas()->setChangedChunk(polygon[i].x(), polygon[i].y());
	}
}

QPolygon LCanvasItem::chunks() const
{
	QPolygon polygon;
	int n = 0;
	QRect br = boundingRect();
	if (isVisible() && canvas())
	{
		int chunkSize = m_canvas->chunkSize();
		br &= QRect(0, 0, m_canvas->width(), m_canvas->height());
		if (br.isValid())
		{
			polygon.resize((br.width() / chunkSize + 2) * (br.height() / chunkSize + 2));
			for (int j = br.top() / chunkSize; j <= br.bottom() / chunkSize; j++)
				for (int i = br.left() / chunkSize; i <= br.right() / chunkSize; i++)
					polygon[n++] = QPoint(i, j);
		}
	}
	polygon.resize(n);
	return polygon;
}

int LCanvasItem::m_type = None;

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



LCanvasLine::LCanvasLine(LCanvas* canvas)
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
