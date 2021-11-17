#ifndef LCANVASITEM_H
#define LCANVASITEM_H

#include <QtWidgets>

namespace lwscode {

class LCanvasItem;
typedef QSharedPointer<LCanvasItem> SPtrLCanvasItem;

enum ItemType {
	NoneType = -1,
	Path,
	Line,
	Rect,
	Ellipse,
	Triangle,
	Hexagon,
	Text
};

class LCanvasItem
{
public:
	LCanvasItem();
	virtual ~LCanvasItem() {}

	ItemType getItemType();

	QPoint startPos();
	void setStartPos(const QPoint &point);
	void moveStartPos(int dx, int dy);
	QPoint endPos();
	void setEndPos(const QPoint &point);
	void moveEndPos(int dx, int dy);

	void setFillColor(const QColor &color);
	void setStrokeColor(const QColor &color);
	void setStrokeWidth(int width);

	bool isSelected();
	void setSelected(bool selected);

	QRect boundingRect();

	virtual void paintItem(QPainter &painter) = 0;
	virtual void moveItem(int dx, int dy) = 0;
	virtual void scaleItem(double sx, double sy) = 0;
	virtual void setBoundingRect() = 0;
	virtual bool containsPos(const QPoint &point) = 0;
	virtual SPtrLCanvasItem clone() = 0;
	virtual void writeItemToXml(QXmlStreamWriter &writer) = 0;

	virtual void addPoint(const QPoint &point) {}

	virtual void setFont(const QFont &font) {}
	virtual QFont font() const { return QFont(); }
	virtual void setText(const QString &text) {}
	virtual QString text() const { return QString(); }

protected:
	ItemType m_itemType;
	QPoint m_startPos;
	QPoint m_endPos;
	QColor m_fillColor;
	QColor m_strokeColor;
	double m_strokeWidth;
	bool m_bSelected;
	QRect m_boundingRect;
	QPainterPath m_path;
};

class LCanvasPath : public LCanvasItem
{
public:
	LCanvasPath();
	virtual ~LCanvasPath() {}

	void addPoint(const QPoint &point) override;

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	QVector<QPoint> m_points;
};

class LCanvasLine : public LCanvasItem
{
public:
	LCanvasLine();
	virtual ~LCanvasLine() {}

	int sumDistance(const QPoint &p1, const QPoint &p2);

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;
};

class LCanvasRect : public LCanvasItem
{
public:
	LCanvasRect();
	virtual ~LCanvasRect() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
};

class LCanvasEllipse : public LCanvasItem
{
public:
	LCanvasEllipse();
	virtual ~LCanvasEllipse() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
};

class LCanvasTriangle : public LCanvasItem
{
public:
	LCanvasTriangle();
	virtual ~LCanvasTriangle() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QVector<QPoint> m_vertices;
};

class LCanvasHexagon : public LCanvasItem
{
public:
	LCanvasHexagon();
	virtual ~LCanvasHexagon() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QVector<QPoint> m_vertices;
};

class LCanvasText : public LCanvasItem
{
public:
	LCanvasText();
	virtual ~LCanvasText() {}

	void setFont(const QFont &font) override;
	QFont font() const override;
	void setText(const QString &text) override;
	QString text() const override;

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPos(const QPoint &point) override;
	SPtrLCanvasItem clone() override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QFont m_font;
	QString m_text;
};

} // namespace

#endif // LCANVASITEM_H
