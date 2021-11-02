#ifndef LCANVASPOLYGON_H
#define LCANVASPOLYGON_H

//namespace lwscode {

class LCanvasPolygon : public LCanvasShape
{
public:
	LCanvasPolygon(LCanvas *canvas);
	virtual ~LCanvasPolygon();
	void setPoints(QPolygon polygon);
	QPolygon points() const;
	void moveBy(double detlaX, double deltaY);

	QPolygon areaPoints() const;

	static int g_type;
	int type() const
	{
		return g_type;
	}

protected:
	void drawShape(QPainter &painter);
	QPolygon m_polygon;
};

//} // namespace

#endif // LCANVASPOLYGON_H
