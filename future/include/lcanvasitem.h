#ifndef LCANVASITEM_H
#define LCANVASITEM_H

#include <QtWidgets>

namespace lwscode {

class LCanvasScene;

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

} // namespace

#endif // LCANVASITEM_H
