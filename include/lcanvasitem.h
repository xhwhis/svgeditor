#ifndef LCANVASITEM_H
#define LCANVASITEM_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtGlobal>

namespace lwscode {

class LCanvasItem : public QObject
{
	Q_OBJECT

public:
	enum ItemType {
		None,
		Path,
		Line,
		Rect,
		Ellipse,
		Triangle,
		Hexagon,
		Text
	};

	LCanvasItem(QWidget *parent = nullptr);
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

	QLineEdit *lineEdit();
	void addPoint(const QPoint &point);

	virtual void paintItem(QPainter &painter) = 0;
	virtual void moveItem(int dx, int dy) = 0;
	virtual void scaleItem(double sx, double sy) = 0;
	virtual void setBoundingRect() = 0;
	virtual bool containsPoint(const QPoint &point) = 0;
	virtual void writeItemToXml(QXmlStreamWriter &writer) = 0;

protected slots:
	void setLineEditVisible();
	void resizeLineEdit();

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
	QVector<QPoint> m_points;
	QLineEdit *m_lineEdit;
};

class LCanvasPath : public LCanvasItem
{
public:
	LCanvasPath(QWidget *parent = nullptr);
	virtual ~LCanvasPath() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;
};

class LCanvasLine : public LCanvasItem
{
public:
	LCanvasLine(QWidget *parent = nullptr);
	virtual ~LCanvasLine() {}

	int sumDistance(const QPoint &p1, const QPoint &p2);

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;
};

class LCanvasRect : public LCanvasItem
{
public:
	LCanvasRect(QWidget *parent = nullptr);
	virtual ~LCanvasRect() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
};

class LCanvasEllipse : public LCanvasItem
{
public:
	LCanvasEllipse(QWidget *parent = nullptr);
	virtual ~LCanvasEllipse() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
};

class LCanvasTriangle : public LCanvasItem
{
public:
	LCanvasTriangle(QWidget *parent = nullptr);
	virtual ~LCanvasTriangle() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QVector<QPoint> m_vertices;
};

class LCanvasHexagon : public LCanvasItem
{
public:
	LCanvasHexagon(QWidget *parent = nullptr);
	virtual ~LCanvasHexagon() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QVector<QPoint> m_vertices;
};

class LCanvasText : public LCanvasItem
{
public:
	LCanvasText(QWidget *parent = nullptr);
	virtual ~LCanvasText() {}

	void paintItem(QPainter &painter) override;
	void moveItem(int dx, int dy) override;
	void scaleItem(double sx, double sy) override;
	void setBoundingRect() override;
	bool containsPoint(const QPoint &point) override;
	void writeItemToXml(QXmlStreamWriter &writer) override;

private:
	int m_width;
	int m_height;
	QFont m_font;
};

} // namespace

#endif // LCANVASITEM_H
