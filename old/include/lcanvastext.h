#ifndef LCANVASTEXT_H
#define LCANVASTEXT_H

//namespace lwscode {

class LCanvasText : public LCanvasItem
{
public:
	LCanvasText(LCanvas *canvas);
	LCanvasText(const QString &text, LCanvas *canvas);
	LCanvasText(const QString &text, const QFont &font, LCanvas *canvas);
	virtual ~LCanvasText();

	void setText(const QString &text);
	void setFont(const QFont &font);
	void setColor(const QColor &color);
	QString text() const;
	QFont font() const;
	QColor color() const;

	void moveBy(double deltaX, double deltaY);

	int textFlags() const
	{
		return m_flags;
	}
	void setTextFlags(int flags);

	QRect boundingRect() const;

	bool collidesWith(const LCanvasItem *item) const;

	static int g_type;
	int type() const
	{
		return g_type;
	}

protected:
	virtual void draw(QPainter &painter);

private:
	void addToChunks();
	void removeFromChunks();
	void changeChunks();

	void setRect();
	bool collidesWith(
		const LCanvasShape *shape,
		const LCanvasRectangle *rectangle,
		const LCanvasEllipse *ellipse,
		const LCanvasText *text) const;

private:
	QRect m_boundingRect;
	QString m_text;
	int m_flags;
	QFont m_font;
	QColor m_color;
};

//} // namespace

#endif // LCANVASTEXT_H
