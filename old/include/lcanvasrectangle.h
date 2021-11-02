#ifndef LCANVASRECTANGLE_H
#define LCANVASRECTANGLE_H

//namespace lwscode {

class LCanvasRectangle : public LCanvasShape
{
public:
	LCanvasRectangle(LCanvas *canvas);
	LCanvasRectangle(const QRect &rect, LCanvas *canvas);
	LCanvasRectangle(int x, int y, int width, int height, LCanvas *canvas);
	virtual ~LCanvasRectangle();

	int width() const;
	int height() const;
	void setSize(int width, int height);
	QSize size() const
	{
		return QSize(m_width, m_height);
	}
	QPolygon areaPoints() const;
	QRect rect() const
	{
		return QRect(this->x(), this->y(), m_width, m_height);
	}

	bool collidesWith(const LCanvasItem *item) const;

	static int g_type;
	int type() const
	{
		return g_type;
	}

protected:
	void drawShape(QPainter &painter);
	QPolygon chunks() const;

private:
	bool collidesWith(
		const LCanvasShape *shape,
		const LCanvasRectangle *rectangle,
		const LCanvasEllipse *ellipse,
		const LCanvasText *text) const;

private:
	int m_width, m_height;
};

//} // namespace

#endif // LCANVASRECTANGLE_H
