#include "lcanvastext.h"
#include "lcanvasscene.h"

namespace lwscode {

LCanvasText::LCanvasText(LCanvasScene *scene)
	: LCanvasItem(scene)
	, m_text("text")
	, m_flags(0)
{
	setRect();
}

LCanvasText::LCanvasText(const QString &text, LCanvasScene *scene)
	: LCanvasItem(scene)
	, m_text(text)
	, m_flags(0)
{
	setRect();
}

LCanvasText::LCanvasText(const QString &text, const QFont &font, LCanvasScene *scene)
	: LCanvasItem(scene)
	, m_text(text)
	, m_flags(0)
	, m_font(font)
{
	setRect();
}

LCanvasText::~LCanvasText()
{
	LCanvasItem::removeFromChunks();
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
	if (m_font != font)
	{
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

void LCanvasText::moveBy(double dX, double dY)
{
	if (dX || dY)
	{
		removeFromChunks();
		m_x += dX;
		m_y += dY;
		m_boundingRect.translate(dX, dY);
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
	if (this->isVisible() && this->scene())
	{
		int chunkSize = this->scene()->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				this->scene()->setChangedChunk(i, j);
	}
}

void LCanvasText::addToChunks()
{
	if (this->isVisible() && scene())
	{
		int chunkSize = m_scene->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				m_scene->addItemToChunk(this, i, j);
	}
}

void LCanvasText::removeFromChunks()
{
	if (this->isVisible() && this->scene())
	{
		int chunkSize = this->scene()->chunkSize();
		for (int j = m_boundingRect.top() / chunkSize; j <= m_boundingRect.bottom() / chunkSize; ++j)
			for (int i = m_boundingRect.left() / chunkSize; i <= m_boundingRect.right() / chunkSize; ++i)
				this->scene()->removeItemFromChunk(this, i, j);
	}
}

int LCanvasText::g_type = Text;

} // namespace
