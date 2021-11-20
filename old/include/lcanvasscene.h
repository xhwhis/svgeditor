#ifndef LCANVASSCENE_H
#define LCANVASSCENE_H

#include <QtWidgets>

namespace lwscode {

class LCanvasItem;
class LCanvasView;

typedef QList<LCanvasItem *> LCanvasItemList;
typedef QList<LCanvasView *> LCanvasViewList;

class LCanvasChunk;

class LCanvasScene : public QObject
{
	Q_OBJECT

public:
	LCanvasScene(QObject *parent = nullptr);
	LCanvasScene(int width, int height);
	LCanvasScene(int hTiles, int vTiles, int tileWidth, int tileHeight);
	virtual ~LCanvasScene();

	virtual void setTiles(int hTiles, int vTiles, int tileWidth, int tileHeight);

	virtual void setBackgroundColor(const QColor &color);
	QColor backgroundColor() const;

	virtual void setTile(int x, int y, int tileNum);
	int tile(int x, int y) const { return m_grid[x + y * m_hTiles]; }

	int tilesHorizontally() const { return m_hTiles; }
	int tilesVertically() const { return m_vTiles; }

	int tileWidth() const { return m_tileWidth; }
	int tileHeight() const { return m_tileHeight; }

	virtual void resize(int width, int height);
	int width() const { return m_width; }
	int height() const { return m_height; }
	QSize size() const { return QSize(m_width, m_height); }
	QRect rect() const { return QRect(0, 0, m_width, m_height); }
	bool onCanvas(int x, int y) const { return x >= 0 && y >= 0 && x < m_width && y < m_height; }
	bool onCanvas(const QPoint &point) const { return onCanvas(point.x(), point.y()); }
	bool validChunk(int x, int y) const { return x >= 0 && y >= 0 && x < m_chunkWidth && y < m_chunkHeight; }
	bool validChunk(const QPoint &point) const { return validChunk(point.x(), point.y()); }

	int chunkSize() const { return m_chunkSize; }
	virtual void retune(int chunkSize);

	bool sameChunk(int x1, int y1, int x2, int y2) const
	{
		return x1 / m_chunkSize == x2 / m_chunkSize && y1 / m_chunkSize == y2 / m_chunkSize;
	}
	virtual void setChangedChunk(int x, int y);
	virtual void setChangedChunkContaining(int x, int y);
	virtual void setAllChanged();
	virtual void setChanged(const QRect &rect);
	virtual void setUnchanged(const QRect &rect);

	void addItemToChunk(LCanvasItem *item, int x, int y);
	void removeItemFromChunk(LCanvasItem *item, int x, int y);
	void addItemToChunkContaining(LCanvasItem *item, int x, int y);
	void removeItemFromChunkContaining(LCanvasItem *item, int x, int y);

	LCanvasItemList allItems();
	LCanvasItemList collisions(const QPoint &) const;

	void drawArea(const QRect &rect, QPainter *painter);

	virtual void addView(LCanvasView *view);
	virtual void removeView(LCanvasView *view);

	void drawCanvasArea(const QRect &rect, QPainter *painter = nullptr);
	void drawViewArea(LCanvasView *view, QPainter *painter, const QRect &rect);

	virtual void addItem(LCanvasItem *item);
	virtual void removeItem(LCanvasItem *item);

signals:
	void resized();

public slots:
	virtual void update();

protected:
	virtual void drawBackground(QPainter &painter, const QRect &rect);

private:
	void init(int width, int height, int chunkSize = 16);

	LCanvasChunk &chunk(int x, int y) const;
	LCanvasChunk &chunkContaining(int x, int y) const;

	QRect changeBounds();

	void initTiles(int hTiles, int vTiles, int tileWidth, int tileHeight);

private:
	int m_width;
	int m_height;
	int m_chunkSize;
	int m_chunkWidth;
	int m_chunkHeight;
	LCanvasChunk *m_chunks;

	LCanvasViewList m_viewList;
	LCanvasItemList m_itemList;

	int *m_grid;
	int m_hTiles;
	int m_vTiles;
	int m_tileWidth;
	int m_tileHeight;
	QColor m_backgroundColor;
};

} // namespace

#endif // LCANVASSCENE_H
