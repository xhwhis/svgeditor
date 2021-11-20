#ifndef LCANVASELLIPSE_H
#define LCANVASELLIPSE_H

#include "lcanvasshape.h"

namespace lwscode {

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

} // namespace

#endif // LCANVASELLIPSE_H
