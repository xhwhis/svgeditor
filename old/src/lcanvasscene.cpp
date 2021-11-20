#include "lcanvasscene.h"
#include "lcanvasitem.h"
#include "lcanvasview.h"
#include "utility.h"

namespace lwscode {

class LCanvasChunk {
public:
	LCanvasChunk() : m_bChanged(true) {}

	void sort()
	{
		std::sort(m_itemList.begin(), m_itemList.end(),
			[](const LCanvasItem *item1, const LCanvasItem *item2) -> bool
			{
				if (item1->z() == item2->z())
					return item1 > item2;

				return (item1->z() > item2->z());
			}
		);
	}

	const LCanvasItemList &itemList() const
	{
		return m_itemList;
	}

	void add(LCanvasItem *item)
	{
		m_itemList.prepend(item);
		m_bChanged = true;
	}

	void remove(LCanvasItem *item)
	{
		m_itemList.removeAll(item);
		m_bChanged = true;
	}

	void change()
	{
		m_bChanged = true;
	}

	bool hasChanged() const
	{
		return m_bChanged;
	}

	bool takeChange()
	{
		bool changed = m_bChanged;
		m_bChanged = false;
		return changed;
	}

private:
	LCanvasItemList m_itemList;
	bool m_bChanged;
};

LCanvasScene::LCanvasScene(QObject *parent)
	: QObject(parent)
{
	init(0, 0);
}

LCanvasScene::LCanvasScene(int width, int height)
{
	init(width, height);
}

LCanvasScene::LCanvasScene(int hTiles, int vTiles, int tileWidth, int tileHeight)
{
	init(hTiles * tileWidth, vTiles * tileHeight, scm(tileWidth, tileHeight));
	setTiles(hTiles, vTiles, tileWidth, tileHeight);
}

LCanvasScene::~LCanvasScene()
{
	for (int i = 0; i < m_viewList.size(); ++i)
		m_viewList[i]->setCanvas(nullptr);

	LCanvasItemList items = allItems();
	for (auto it = items.begin(); it != items.end(); ++it)
		delete *it;

	delete[] m_chunks;
	delete[] m_grid;
}

void LCanvasScene::init(int width, int height, int chunkSize)
{
	m_width = width;
	m_height = height;
	m_chunkSize = chunkSize;
	m_chunkWidth = (width + chunkSize - 1) / chunkSize;
	m_chunkHeight = (height + chunkSize - 1) / chunkSize;
	m_chunks = new LCanvasChunk[m_chunkWidth * m_chunkHeight];
	m_backgroundColor = Qt::white;
	m_grid = nullptr;
	m_hTiles = 0;
	m_vTiles = 0;
}

LCanvasChunk &LCanvasScene::chunk(int i, int j) const
{
	return m_chunks[i + m_chunkWidth * j];
}

LCanvasChunk &LCanvasScene::chunkContaining(int x, int y) const
{
	return chunk(x / m_chunkSize, y / m_chunkSize);
}

LCanvasItemList LCanvasScene::allItems()
{
	return m_itemList;
}

void LCanvasScene::resize(int width, int height)
{
	if (m_width == width && m_height == height)
		return;

	LCanvasItemList hiddenItems;
	for (auto it = m_itemList.begin(); it != m_itemList.end(); ++it)
	{
		if ((*it)->isVisible())
		{
			(*it)->hide();
			hiddenItems.append(*it);
		}
	}

	int chunkWidth = (width + m_chunkSize - 1) / m_chunkSize;
	int chunkHeight = (height + m_chunkSize - 1) / m_chunkSize;

	LCanvasChunk *chunks = new LCanvasChunk[chunkWidth * chunkHeight];

	m_width = width;
	m_height = height;
	m_chunkWidth = chunkWidth;
	m_chunkHeight = chunkHeight;
	delete[] chunks;
	m_chunks = chunks;

	for (int i = 0; i < hiddenItems.size(); ++i)
		hiddenItems.at(i)->show();

	setAllChanged();

	emit resized();
}

void LCanvasScene::retune(int chunkSize)
{
	if (m_chunkSize != chunkSize)
	{
		LCanvasItemList hiddenItems;
		for (auto it = m_itemList.begin(); it != m_itemList.end(); ++it)
		{
			if ((*it)->isVisible())
			{
				(*it)->hide();
				hiddenItems.append(*it);
			}
		}

		m_chunkSize = chunkSize;

		int chunkWidth = (m_width + chunkSize - 1) / chunkSize;
		int chunkHeight = (m_height + chunkSize - 1) / chunkSize;

		LCanvasChunk *chunks = new LCanvasChunk[chunkWidth * chunkHeight];

		m_chunkWidth = chunkWidth;
		m_chunkHeight = chunkHeight;
		delete[] m_chunks;
		m_chunks = chunks;

		for (int i = 0; i < hiddenItems.size(); ++i)
			hiddenItems.at(i)->show();
	}
}

void LCanvasScene::addItem(LCanvasItem *item)
{
	m_itemList.append(item);
}

void LCanvasScene::removeItem(LCanvasItem* item)
{
	m_itemList.removeAll(item);
}

void LCanvasScene::addView(LCanvasView *view)
{
	m_viewList.append(view);
	if (m_hTiles > 1 || m_vTiles > 1)
	{
		QPalette::ColorRole role = view->widget()->backgroundRole();
		QPalette palette = view->widget()->palette();
		palette.setColor(role, backgroundColor());
		view->widget()->setPalette(palette);
	}
}

void LCanvasScene::removeView(LCanvasView *view)
{
	m_viewList.removeAll(view);
}

void LCanvasScene::drawViewArea(LCanvasView *view, QPainter *painter, const QRect &rect)
{
	QTransform wm = view->worldMatrix();
	QTransform iwm = wm.inverted();

	QRect ivr = iwm.mapRect(rect);

	painter->setTransform(wm);
	drawCanvasArea(ivr, painter);
}

void LCanvasScene::update()
{
	QRect rect = changeBounds();
	for (int i = 0; i < m_viewList.size(); ++i)
	{
		LCanvasView *view = m_viewList.at(i);
		if (!rect.isEmpty())
		{
			QRect tr = view->worldMatrix().mapRect(rect);
			view->widget()->update(tr);
		}
	}
	setUnchanged(rect);
}

void LCanvasScene::setAllChanged()
{
	setChanged(QRect(0, 0, width(), height()));
}

void LCanvasScene::setChanged(const QRect &rect)
{
	QRect area = rect.intersected(QRect(0, 0, width(), height()));

	int chunkWidth = (area.left() + area.width() + m_chunkSize) / m_chunkSize;
	int chunkHeight = (area.top() + area.height() + m_chunkSize) / m_chunkSize;
	if (chunkWidth > m_chunkWidth)
		chunkWidth = m_chunkWidth;
	if (chunkHeight > m_chunkHeight)
		chunkHeight = m_chunkHeight;

	int i = area.left() / m_chunkSize;
	while(i < chunkWidth)
	{
		int j = area.top() / m_chunkSize;
		while(j < chunkHeight)
		{
			chunk(i, j).change();
			++j;
		}
		++i;
	}
}

void LCanvasScene::setUnchanged(const QRect &rect)
{
	QRect area = rect.intersected(QRect(0, 0, width(), height()));

	int chunkWidth = (area.left() + area.width() + m_chunkSize) / m_chunkSize;
	int chunkHeight = (area.top() + area.height() + m_chunkSize) / m_chunkSize;
	if (chunkWidth > m_chunkWidth)
		chunkWidth = m_chunkWidth;
	if (chunkHeight > m_chunkHeight)
		chunkHeight = m_chunkHeight;

	int i = area.left() / m_chunkSize;
	while(i < chunkWidth)
	{
		int j = area.top() / m_chunkSize;
		while(j < chunkHeight)
		{
			chunk(i, j).takeChange();
			++j;
		}
		++i;
	}
}

QRect LCanvasScene::changeBounds()
{
	QRect rect = QRect(0, 0, width(), height());

	int chunkWidth = (rect.left() + rect.width() + m_chunkSize) / m_chunkSize;
	int chunkHeight = (rect.top() + rect.height() + m_chunkSize) / m_chunkSize;
	if (chunkWidth > m_chunkWidth)
		chunkWidth = m_chunkWidth;
	if (chunkHeight > m_chunkHeight)
		chunkHeight = m_chunkHeight;

	QRect result;
	int i = rect.left() / m_chunkSize;
	while(i < chunkWidth)
	{
		int j = rect.top() / m_chunkSize;
		while(j < chunkHeight)
		{
			if (chunk(i, j).hasChanged())
				result |= QRect(i * m_chunkSize, j * m_chunkSize, m_chunkSize + 1, m_chunkSize + 1);
			++j;
		}
		++i;
	}

	return result;
}

void LCanvasScene::drawArea(const QRect &rect, QPainter *painter)
{
	if (painter)
		drawCanvasArea(rect, painter);
}

void LCanvasScene::drawCanvasArea(const QRect &rect, QPainter *painter)
{
	if (!painter)
		return;

	QRect area = rect.intersected(QRect(0, 0, width(), height()));

	int left = area.left() / m_chunkSize;
	int top = area.top() / m_chunkSize;
	int right = area.right() / m_chunkSize;
	int bottom = area.bottom() / m_chunkSize;
	if (right >= m_chunkWidth)
		right = m_chunkWidth - 1;
	if (bottom >= m_chunkHeight)
		bottom = m_chunkHeight - 1;

	LCanvasItemList visibleItems;
	QRegion region;
	for (int i = left; i <= right; ++i) {
		for (int j = top; j <= bottom; ++j) {
			if (!painter) {
				if (chunk(i, j).takeChange()) {
					region |= QRegion(i * m_chunkSize - area.left(), j * m_chunkSize - area.top(),
									  m_chunkSize, m_chunkSize);
					visibleItems += chunk(i, j).itemList();
				}
			} else {
				visibleItems += chunk(i, j).itemList();
			}
		}
	}
	std::sort(visibleItems.begin(), visibleItems.end(),
		  [](const LCanvasItem *item1, const LCanvasItem *item2) -> bool
		{
			if (item1->z() == item2->z())
				return item1 > item2;

			return (item1->z() > item2->z());
		}
	);

	drawBackground(*painter, area);
	if (!visibleItems.isEmpty())
	{
		LCanvasItem *pre = nullptr;
		for (int i = visibleItems.size() - 1; i >= 0; --i)
		{
			LCanvasItem *item = visibleItems[i];
			if (item != pre)
			{
				item->draw(*painter);
				pre = item;
			}
		}
	}
}

void LCanvasScene::setChangedChunk(int x, int y)
{
	if (validChunk(x, y))
		chunk(x, y).change();
}

void LCanvasScene::setChangedChunkContaining(int x, int y)
{
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
		chunkContaining(x, y).change();
}

void LCanvasScene::addItemToChunk(LCanvasItem *item, int x, int y)
{
	if (validChunk(x, y))
		chunk(x, y).add(item);
}

void LCanvasScene::removeItemFromChunk(LCanvasItem *item, int x, int y)
{
	if (validChunk(x, y))
		chunk(x, y).remove(item);
}

void LCanvasScene::addItemToChunkContaining(LCanvasItem *item, int x, int y)
{
	if (x >= 0 && x < width() && y >= 0 && y < height())
		chunkContaining(x, y).add(item);
}

void LCanvasScene::removeItemFromChunkContaining(LCanvasItem *item, int x, int y)
{
	if (x >= 0 && x < width() && y >= 0 && y < height())
		chunkContaining(x, y).remove(item);
}

QColor LCanvasScene::backgroundColor() const
{
	return m_backgroundColor;
}

void LCanvasScene::setBackgroundColor(const QColor &color)
{
	if (m_backgroundColor != color)
	{
		m_backgroundColor = color;
		for (int i = 0; i < m_viewList.size(); ++i)
		{
			LCanvasView *view = m_viewList.at(i);
			QPalette::ColorRole role = view->widget()->backgroundRole();
			QPalette palette = view->widget()->palette();
			palette.setColor(role, m_backgroundColor);
			view->widget()->setPalette(palette);
		}
		setAllChanged();
	}
}

void LCanvasScene::drawBackground(QPainter &painter, const QRect &rect)
{
	painter.fillRect(rect, m_backgroundColor);
}

void LCanvasScene::setTiles(int hTiles, int vTiles, int tileWidth, int tileHeight)
{
	if (!tileWidth || !tileHeight)
		return;

	m_hTiles = hTiles;
	m_vTiles = vTiles;
	delete[] m_grid;
	if (hTiles && vTiles)
	{
		m_grid = new int[hTiles * vTiles];
		memset(m_grid, 0, hTiles * hTiles * sizeof(int));
		m_tileWidth = tileWidth;
		m_tileHeight = tileHeight;
	}
	else
	{
		m_grid = nullptr;
	}
	if (hTiles + vTiles > 10)
	{
		int s = scm(tileWidth, tileHeight);
		retune(s < 128 ? s : qMax(tileWidth, tileHeight));
	}
	setAllChanged();
}

void LCanvasScene::setTile(int x, int y, int tileNum)
{
	int &num = m_grid[x + y * m_hTiles];
	if (num != tileNum) {
		num = tileNum;
		if (m_tileWidth == m_tileHeight && m_tileWidth == m_chunkSize)
		{
			setChangedChunk(x, y);
		}
		else
		{
			setChanged(QRect(x * m_tileWidth, y * m_tileHeight, m_tileWidth, m_tileHeight));
		}
	}
}

LCanvasItemList LCanvasScene::collisions(const QPoint &p) const
{
//	return collisions(QRect(p, QSize(1, 1)));
	return LCanvasItemList();
}

} // namespace
