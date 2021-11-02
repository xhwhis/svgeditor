#ifndef LCANVASSHAPE_H
#define LCANVASSHAPE_H

//namespace lwscode {

class LCanvasShape : public LCanvasItem
{
public:
	LCanvasShape(LCanvas *canvas);
	virtual ~LCanvasShape();

	bool collidesWith(const LCanvasItem *) const;

	virtual void setPen(QPen pen);
	virtual void setBrush(QBrush brush);

	QPen pen() const
	{
		return m_pen;
	}
	QBrush brush() const
	{
		return m_brush;
	}

	virtual QPolygon areaPoints() const = 0;
	QRect boundingRect() const;

	static int g_type;
	int type() const
	{
		return g_type;
	}

protected:
	void draw(QPainter &);
	virtual void drawShape(QPainter &) = 0;

	bool winding() const;
	void setWinding(bool);

	void invalidate();
	bool isValid() const
	{
		return LCanvasItem::m_bValid;
	}

private:
	bool collidesWith(const LCanvasShape *shape,
					  const LCanvasRectangle *rectangle,
					  const LCanvasEllipse *ellipse,
					  const LCanvasText *text) const;

private:
	QBrush m_brush;
	QPen m_pen;
};

//} // namespace

#endif // LCANVASSHAPE_H
