#ifndef LCANVASSHAPE_H
#define LCANVASSHAPE_H

#include "lcanvasitem.h"

namespace lwscode {

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

} // namespace

#endif // LCANVASSHAPE_H
