#include "lcanvas.h"
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
	return collisions(QRect(p, QSize(1, 1)));
}

LCanvasItemList LCanvasScene::collisions(const QRect& rect) const
{
	LCanvasRect i(rect, (LCanvasScene *)this);
	i.setPen(Qt::NoPen);
	i.show();
	LCanvasItemList l = i.collisions(true);
	std::sort(l.begin(), l.end(),
			  [](const LCanvasItem *item1, const LCanvasItem *item2) -> bool
			{
		if (item1->z() == item2->z())
			return item1 > item2;

		return (item1->z() > item2->z());
		});
	return l;
}

LCanvasItemList LCanvasScene::collisions(
	const QPolygon& chunklist,
	const LCanvasItem *item,
	bool exact) const
{
	QSet<LCanvasItem *> seen;
	LCanvasItemList result;
	for (int i = 0; i < chunklist.size(); i++) {
		int x = chunklist[i].x();
		int y = chunklist[i].y();
		if (validChunk(x, y)) {
			const LCanvasItemList &l = chunk(x, y).itemList();
			for (int i = 0; i < l.size(); ++i) {
				LCanvasItem *g = l.at(i);
				if (g != item) {
					if (!seen.contains(g)) {
						seen.insert(g);
						if (!exact || item->collidesWith(g))
							result.append(g);
					}
				}
			}
		}
	}
	return result;
}

LCanvasWidget::LCanvasWidget(LCanvasView *view)
	: QWidget(view)
	, m_view(view)
{

}

void LCanvasWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	m_view->drawContents(&painter, event->rect().left(), event->rect().top(),
						 event->rect().width(), event->rect().height());
}

void LCanvasWidget::mousePressEvent(QMouseEvent *event)
{
	m_view->contentsMousePressEvent(event);
}

void LCanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
	m_view->contentsMouseMoveEvent(event);
}

void LCanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_view->contentsMouseReleaseEvent(event);
}

void LCanvasWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	m_view->contentsMouseDoubleClickEvent(event);
}

void LCanvasWidget::dragEnterEvent(QDragEnterEvent *event)
{
	m_view->contentsDragEnterEvent(event);
}

void LCanvasWidget::dragMoveEvent(QDragMoveEvent *event)
{
	m_view->contentsDragMoveEvent(event);
}

void LCanvasWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	m_view->contentsDragLeaveEvent(event);
}

void LCanvasWidget::dropEvent(QDropEvent *event)
{
	m_view->contentsDropEvent(event);
}

void LCanvasWidget::wheelEvent(QWheelEvent *event)
{
	m_view->contentsWheelEvent(event);
}

void LCanvasWidget::contextMenuEvent(QContextMenuEvent *event)
{
	m_view->contentsContextMenuEvent(event);
}

LCanvasView::LCanvasView(QWidget *parent)
	: QScrollArea(parent)
{
	this->setWidget(new LCanvasWidget(this));
	m_scene = nullptr;
	setCanvas(nullptr);
}

LCanvasView::LCanvasView(LCanvasScene *scene, QWidget *parent)
	: QScrollArea(parent)
	, m_scene(scene)
{
	this->setWidget(new LCanvasWidget(this));
	m_scene = nullptr;
	setCanvas(scene);
}

LCanvasView::~LCanvasView()
{
	setCanvas(nullptr);
}

void LCanvasView::contentsMousePressEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseReleaseEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseDoubleClickEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseMoveEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsDragEnterEvent(QDragEnterEvent *event)
{
}

void LCanvasView::contentsDragMoveEvent(QDragMoveEvent *event)
{
}

void LCanvasView::contentsDragLeaveEvent(QDragLeaveEvent *event)
{
}

void LCanvasView::contentsDropEvent(QDropEvent *event)
{
}

void LCanvasView::contentsWheelEvent(QWheelEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsContextMenuEvent(QContextMenuEvent *event)
{
	event->ignore();
}

void LCanvasView::setCanvas(LCanvasScene *scene)
{
	if (m_scene == scene) return;

	if (m_scene)
	{
		disconnect(m_scene);
		m_scene->removeView(this);
	}
	m_scene = scene;
	if (m_scene)
	{
		connect(m_scene, SIGNAL(resized()), this, SLOT(updateContentsSize()));
		m_scene->addView(this);
	}
	updateContentsSize();
	update();
}

const QTransform &LCanvasView::worldMatrix() const
{
	return m_worldMatrix;
}

const QTransform &LCanvasView::inverseWorldMatrix() const
{
	return m_inverseWorldMatrix;
}

bool LCanvasView::setWorldMatrix(const QTransform &matrix)
{
	bool enable = matrix.isInvertible();
	if (enable)
	{
		m_worldMatrix = matrix;
		m_inverseWorldMatrix = matrix.inverted();
		updateContentsSize();
		widget()->update();
	}
	return enable;
}

void LCanvasView::updateContentsSize()
{
	if (m_scene)
	{
		QRect rect = m_worldMatrix.mapRect(QRect(0, 0, m_scene->width(), m_scene->height()));
		widget()->resize(rect.size());
	}
	else
	{
		widget()->resize(size());
	}
}

void LCanvasView::drawContents(QPainter *painter, int x, int y, int width, int height)
{
	if (!m_scene) return;

	QPainterPath clipPath;
	clipPath.addRect(m_scene->rect());
	painter->setClipPath(m_worldMatrix.map(clipPath), Qt::IntersectClip);
	m_scene->drawViewArea(this, painter, QRect(x, y, width, height));
}

QSize LCanvasView::sizeHint() const
{
	if (!m_scene) return QScrollArea::sizeHint();

	return (m_scene->size() + 2 * QSize(this->frameWidth(), this->frameWidth())).boundedTo(3 * QApplication::primaryScreen()->size() / 4);
}

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

LCanvasItemList LCanvasItem::collisions(bool exact) const
{
	return m_scene->collisions(chunks(), this, exact);
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

static bool collision_double_dispatch(
	const LCanvasShape *shape1,
	const LCanvasRect *rectangle1,
	const LCanvasEllipse *ellipse1,
	const LCanvasText *text1,
	const LCanvasShape *shape2,
	const LCanvasRect *rectangle2,
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
			 ellipse1->width() == ellipse1->height() &&
			 ellipse2->width() == ellipse2->height())
	{
		double dx = ellipse1->x() - ellipse2->x();
		double dy = ellipse1->y() - ellipse2->y();
		double dr = (ellipse1->width() + ellipse2->width()) / 2;
		return dx * dx + dy * dy <= dr * dr;
	}
	else if (shape1 && (shape2 || text2))
	{
		QPolygon polygon1 = shape1->areaPoints();
		QPolygon polygon2 = shape2 ? shape2->areaPoints() : QPolygon(item2->boundingRect());
		return QRegion(polygon1).intersects(QRegion(polygon2, Qt::WindingFill));
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

bool LCanvasShape::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, nullptr, nullptr, nullptr);
}

bool LCanvasShape::collidesWith(const LCanvasShape *shape,
								const LCanvasRect *rectangle,
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


LCanvasRect::LCanvasRect(LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(32)
	, m_height(32)
{

}

LCanvasRect::LCanvasRect(const QRect &rect, LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(rect.width())
	, m_height(rect.height())
{
	move(rect.left(), rect.top());
}

LCanvasRect::LCanvasRect(int x, int y, int width, int height, LCanvasScene *scene)
	: LCanvasShape(scene)
	, m_width(width)
	, m_height(height)
{
	move(x, y);
}

LCanvasRect::~LCanvasRect()
{
	hide();
}

int LCanvasRect::width() const
{
	return m_width;
}

int LCanvasRect::height() const
{
	return m_height;
}

void LCanvasRect::setSize(int width, int height)
{
	if (m_width != width || m_height != height) {
		removeFromChunks();
		m_width = width;
		m_height = height;
		addToChunks();
	}
}

QPolygon LCanvasRect::areaPoints() const
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

void LCanvasRect::drawShape(QPainter & p)
{
	p.drawRect(this->x(), this->y(), m_width, m_height);
}

int LCanvasRect::g_type = Rectangle;

bool LCanvasRect::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, this, nullptr, nullptr);
}

bool LCanvasRect::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRect *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 this, this, nullptr, nullptr);
}

QPolygon LCanvasRect::chunks() const
{
	return LCanvasItem::chunks();
}

LCanvasPolygon::LCanvasPolygon(LCanvasScene* canvas)
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

void LCanvasPolygon::moveBy(double dX, double dY)
{
	if (dX || dY) {
		removeFromChunks();
		m_polygon.translate(dX, dY);
		m_x += dX;
		m_y += dY;
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

LCanvasLine::LCanvasLine(LCanvasScene* canvas)
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

bool LCanvasEllipse::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(this, nullptr, this, nullptr);
}

bool LCanvasEllipse::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRect *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 this, nullptr, this, nullptr);
}

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

bool LCanvasText::collidesWith(const LCanvasItem *item) const
{
	return item->collidesWith(nullptr, nullptr, nullptr, this);
}

bool LCanvasText::collidesWith(
	const LCanvasShape *shape,
	const LCanvasRect *rectangle,
	const LCanvasEllipse *ellipse,
	const LCanvasText *text) const
{
	return collision_double_dispatch(shape, rectangle, ellipse, text,
									 nullptr, nullptr, nullptr, this);
}

} // namespace
