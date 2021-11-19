#ifndef LCANVAS_H
#define LCANVAS_H

#include <QtWidgets>

namespace lwscode {

class LCanvasItem;
class LCanvasShape;
class LCanvasRect;
class LCanvasPolygon;
class LCanvasEllipse;
class LCanvasText;
class LCanvasLine;
class LCanvasView;
class LCanvasChunk;
class LCanvasScene;

typedef QList<LCanvasItem *> LCanvasItemList;
typedef QList<LCanvasView *> LCanvasViewList;

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

class LCanvasWidget : public QWidget
{
public:
	LCanvasWidget(LCanvasView *view);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	void wheelEvent(QWheelEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

protected:
	LCanvasView *m_view;
};

class LCanvasView : public QScrollArea
{
	Q_OBJECT

public:
	LCanvasView(QWidget *parent = nullptr);
	LCanvasView(LCanvasScene *scene, QWidget *parent = nullptr);
	virtual ~LCanvasView();

	LCanvasScene *scene() const { return m_scene; }
	void setCanvas(LCanvasScene *scene);

	const QTransform &worldMatrix() const;
	const QTransform &inverseWorldMatrix() const;
	bool setWorldMatrix(const QTransform &matrix);

	virtual QSize sizeHint() const;

protected:
	friend class LCanvasWidget;
	virtual void drawContents(QPainter *painter, int x, int y, int width, int height);

	virtual void contentsMousePressEvent(QMouseEvent *event);
	virtual void contentsMouseReleaseEvent(QMouseEvent *event);
	virtual void contentsMouseDoubleClickEvent(QMouseEvent *event);
	virtual void contentsMouseMoveEvent(QMouseEvent *event);
	virtual void contentsDragEnterEvent(QDragEnterEvent *event);
	virtual void contentsDragMoveEvent(QDragMoveEvent *event);
	virtual void contentsDragLeaveEvent(QDragLeaveEvent *event);
	virtual void contentsDropEvent(QDropEvent *event);
	virtual void contentsWheelEvent(QWheelEvent *event);
	virtual void contentsContextMenuEvent(QContextMenuEvent *event);

private slots:
	void updateContentsSize();

private:
	void drawContents(QPainter *painter);

private:
	friend class LCanvasScene;
	LCanvasScene *m_scene;
	QTransform m_worldMatrix;
	QTransform m_inverseWorldMatrix;
};

class LCanvasItem
{
public:
	LCanvasItem(LCanvasScene *scene);
	virtual ~LCanvasItem();

	double x() const { return m_x; }
	double y() const { return m_y; }
	double z() const { return m_z; }

	virtual void moveBy(double dx, double dy);
	void move(double x, double y);
	void setX(double x) { move(x, y()); }
	void setY(double y) { move(x(), y); }
	void setZ(double z) { m_z = z; changeChunks(); }

	virtual void setCanvas(LCanvasScene *scene);

	virtual void draw(QPainter &painter) = 0;

	void show();
	void hide();

	virtual void setVisible(bool visible);
	virtual void setSelected(bool selected);
	virtual void setEnabled(bool enabled);
	virtual void setActive(bool active);

	bool isVisible() const { return m_bVisible; }
	bool isSelected() const { return m_bSelected; }
	bool isEnabled() const { return m_bEnabled; }
	bool isActive() const { return m_bActive; }

	enum ItemType
	{
		None,
		Shape,
		Line,
		Rectangle,
		Polygon,
		Ellipse,
		Text
	};

	static int m_type;
	virtual int type() const { return m_type; }

	virtual QRect boundingRect() const = 0;

	LCanvasScene *scene() const { return m_scene; }

protected:
	void update() { changeChunks(); }

	virtual QPolygon chunks() const;
	virtual void addToChunks();
	virtual void removeFromChunks();
	virtual void changeChunks();

protected:
	LCanvasScene *m_scene;
	double m_x;
	double m_y;
	double m_z;
	bool m_bVisible;
	bool m_bValid;
	bool m_bSelected;
	bool m_bEnabled;
	bool m_bActive;
};

class LCanvasShape : public LCanvasItem
{
public:
	LCanvasShape(LCanvasScene *scene);
	virtual ~LCanvasShape();

	virtual void setPen(QPen pen);
	virtual void setBrush(QBrush brush);

	QPen pen() const { return m_pen; }
	QBrush brush() const { return m_brush; }

	virtual QPolygon areaPoints() const = 0;
	QRect boundingRect() const;

	static int g_type;
	int type() const { return g_type; }

protected:
	void draw(QPainter &);
	virtual void drawShape(QPainter &) = 0;

	void invalidate();
	bool isValid() const { return LCanvasItem::m_bValid; }

private:
	QBrush m_brush;
	QPen m_pen;
};

class LCanvasRect : public LCanvasShape
{
public:
	LCanvasRect(LCanvasScene *scene);
	LCanvasRect(const QRect &rect, LCanvasScene *scene);
	LCanvasRect(int x, int y, int width, int height, LCanvasScene *scene);
	virtual ~LCanvasRect();

	int width() const;
	int height() const;
	void setSize(int width, int height);
	QSize size() const { return QSize(m_width, m_height); }
	QPolygon areaPoints() const;
	QRect rect() const { return QRect(this->x(), this->y(), m_width, m_height); }

	static int g_type;
	int type() const { return g_type; }

protected:
	void drawShape(QPainter &painter);
	QPolygon chunks() const;

private:
	int m_width, m_height;
};

class LCanvasPolygon : public LCanvasShape
{
public:
	LCanvasPolygon(LCanvasScene *scene);
	virtual ~LCanvasPolygon();
	void setPoints(QPolygon polygon);
	QPolygon points() const;
	void moveBy(double detlaX, double dY);

	QPolygon areaPoints() const;

	static int g_type;
	int type() const { return g_type; }

protected:
	void drawShape(QPainter &painter);
	QPolygon m_polygon;
};

class LCanvasLine : public LCanvasShape
{
public:
	LCanvasLine(LCanvasScene *scene);
	virtual ~LCanvasLine();
	void setPoints(int x1, int y1, int x2, int y2);

	QPoint startPoint() const { return QPoint(m_x1, m_y1); }
	QPoint endPoint() const { return QPoint(m_x2, m_y2); }

	static int g_type;
	int type() const { return g_type; }

protected:
	void drawShape(QPainter &painter);
	QPolygon areaPoints() const;

private:
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
};

class LCanvasEllipse : public LCanvasShape
{
public:
	LCanvasEllipse(LCanvasScene *scene);
	LCanvasEllipse(int width, int height, LCanvasScene *scene);
	virtual ~LCanvasEllipse();

	int width() const;
	int height() const;
	void setSize(int width, int height);
	QPolygon areaPoints() const;

	static int g_type;
	int type() const { return g_type; }

protected:
	void drawShape(QPainter &painter);

private:
	int m_width;
	int m_height;
};

class LCanvasText : public LCanvasItem
{
public:
	LCanvasText(LCanvasScene *scene);
	LCanvasText(const QString &text, LCanvasScene *scene);
	LCanvasText(const QString &text, const QFont &font, LCanvasScene *scene);
	virtual ~LCanvasText();

	void setText(const QString &text);
	void setFont(const QFont &font);
	void setColor(const QColor &color);
	QString text() const;
	QFont font() const;
	QColor color() const;

	void moveBy(double dX, double dY);

	int textFlags() const { return m_flags; }
	void setTextFlags(int flags);

	QRect boundingRect() const;

	static int g_type;
	int type() const { return g_type; }

protected:
	virtual void draw(QPainter &painter);

private:
	void addToChunks();
	void removeFromChunks();
	void changeChunks();

	void setRect();

private:
	QRect m_boundingRect;
	QString m_text;
	int m_flags;
	QFont m_font;
	QColor m_color;
};

} // namespace

#endif // LCANVAS_H
