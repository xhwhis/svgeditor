#ifndef LCANVASLINE_H
#define LCANVASLINE_H

#include "lcanvasshape.h"

namespace lwscode {

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

} // namespace

#endif // LCANVASLINE_H
