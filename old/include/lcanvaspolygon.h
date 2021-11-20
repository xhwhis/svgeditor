#ifndef LCANVASPOLYGON_H
#define LCANVASPOLYGON_H

#include "lcanvasshape.h"

namespace lwscode {

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

} // namespace

#endif // LCANVASPOLYGON_H
