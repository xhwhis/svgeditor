#ifndef LCANVASTEXT_H
#define LCANVASTEXT_H

#include "lcanvasitem.h"

namespace lwscode {

class LCanvasText : public LCanvasItem
{
public:
	LCanvasText(LCanvasScene *scene);
	LCanvasText(const QString &text, LCanvasScene *scene);
	LCanvasText(const QString &text, const QFont &font, LCanvasScene *scene);
	virtual ~LCanvasText();

	void setText(const QString &text);
	void setFont(const QFont &font);
	void setColor(const QColor &color);
	QString text() const;
	QFont font() const;
	QColor color() const;

	void moveBy(double dX, double dY);

	int textFlags() const { return m_flags; }
	void setTextFlags(int flags);

	QRect boundingRect() const;

	static int g_type;
	int type() const { return g_type; }

protected:
	virtual void draw(QPainter &painter);

private:
	void addToChunks();
	void removeFromChunks();
	void changeChunks();

	void setRect();

private:
	QRect m_boundingRect;
	QString m_text;
	int m_flags;
	QFont m_font;
	QColor m_color;
};

} // namespace

#endif // LCANVASTEXT_H
