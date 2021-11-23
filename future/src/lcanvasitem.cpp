#include "lcanvasitem.h"
#include "lcanvasscene.h"

namespace lwscode {

LCanvasItem::LCanvasItem(LCanvasScene *scene)
	: m_scene(scene)
	, m_x(0)
	, m_y(0)
	, m_z(0)
	, m_bVisible(false)
	, m_bValid(false)
	, m_bSelected(false)
	, m_bEnabled(false)
	, m_bActive(false)
{
	if (m_scene)
		m_scene->addItem(this);
}

LCanvasItem::~LCanvasItem()
{
	if (m_scene)
		m_scene->removeItem(this);
}

void LCanvasItem::moveBy(double dX, double dY)
{
	if (dX || dY)
	{
		removeFromChunks();
		m_x += dX;
		m_y += dY;
		addToChunks();
	}
}

void LCanvasItem::move(double x, double y)
{
	moveBy(x - m_x, y - m_y);
}

void LCanvasItem::setCanvas(LCanvasScene *scene)
{
	bool visible = m_bVisible;
	setVisible(false);
	if (m_scene)
		m_scene->removeItem(this);

	m_scene = scene;
	if (m_scene)
		m_scene->addItem(this);

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

void LCanvasItem::addToChunks()
{
	if (m_bVisible && m_scene)
	{
		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); ++i)
			m_scene->addItemToChunk(this, polygon[i].x(), polygon[i].y());

		m_bValid = true;
	}
}

void LCanvasItem::removeFromChunks()
{
	if (m_bVisible && m_scene)
	{
		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); ++i)
			m_scene->removeItemFromChunk(this, polygon[i].x(), polygon[i].y());
	}
}

void LCanvasItem::changeChunks()
{
	if (m_bVisible && m_scene) {
		if (!m_bValid)
			addToChunks();

		QPolygon polygon = chunks();
		for (int i = 0; i < polygon.size(); i++)
			m_scene->setChangedChunk(polygon[i].x(), polygon[i].y());
	}
}

QPolygon LCanvasItem::chunks() const
{
	QPolygon polygon;
	int n = 0;
	QRect br = boundingRect();
	if (this->isVisible() && m_scene)
	{
		int chunkSize = m_scene->chunkSize();
		br &= QRect(0, 0, m_scene->width(), m_scene->height());
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

} // namespace
