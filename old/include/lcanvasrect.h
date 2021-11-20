#ifndef LCANVASRECT_H
#define LCANVASRECT_H

#include "lcanvasshape.h"

namespace lwscode {

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

private:
	int m_width, m_height;
};

} // namespace

#endif // LCANVASRECT_H
