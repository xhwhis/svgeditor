#ifndef LCANVASELLIPSE_H
#define LCANVASELLIPSE_H

//namespace lwscode {

class LCanvasEllipse : public LCanvasShape
{
public:
	LCanvasEllipse(LCanvas *canvas);
	LCanvasEllipse(int width, int height, LCanvas *canvas);
	LCanvasEllipse(int width, int height, int angleStart, int angle, LCanvas *canvas);
	virtual ~LCanvasEllipse();

	int width() const;
	int height() const;
	void setSize(int width, int height);
	void setAngles(int start, int length);
	int angleStart() const
	{
		return m_angleStart;
	}
	int angleLength() const
	{
		return m_angleLength;
	}
	QPolygon areaPoints() const;

	bool collidesWith(const LCanvasItem *item) const;

	static int g_type;
	int type() const
	{
		return g_type;
	}

protected:
	void drawShape(QPainter &painter);

private:
	bool collidesWith(LCanvasShape *shape,
					  LCanvasRectangle *rectangle,
					  LCanvasEllipse *ellipse,
					  LCanvasText *text) const;

private:
	int m_width;
	int m_height;
	int m_angleStart;
	int m_angleLength;
};

//} // namespace

#endif // LCANVASELLIPSE_H
