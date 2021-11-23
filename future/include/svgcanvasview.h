#ifndef SVGCANVASVIEW_H
#define SVGCANVASVIEW_H

#include "lcanvasline.h"
#include "lcanvasrect.h"
#include "lcanvasellipse.h"
#include "lcanvaspolygon.h"
#include "lcanvastext.h"
#include "lcanvasview.h"
#include "lcanvasscene.h"

using namespace lwscode;

class SVGCanvasView : public LCanvasView
{
	Q_OBJECT

public:
	SVGCanvasView(LCanvasScene *scene, QWidget *parent = nullptr);
	~SVGCanvasView();

	void setItemType(int type) { m_itemType = type; }

public slots:
	void cutItem();
	void copyItem();
	void pasteItem();
	void deleteItem();
	void moveTopItem();
	void moveUpItem();
	void moveDownItem();
	void moveBottomItem();

protected:
	void contentsMousePressEvent(QMouseEvent *event);
	void contentsMouseMoveEvent(QMouseEvent *event);
	void contentsMouseReleaseEvent(QMouseEvent *event);
	void contentsContextMenuEvent(QContextMenuEvent *event);

private:
	int m_itemType;
	LCanvasLine *m_paintingLine;
	LCanvasRect *m_paintingRect;
	LCanvasEllipse *m_paintingEllipse;
	LCanvasText *m_paintingText;
	LCanvasItemList *m_selectionItems;
	QPoint m_startPos;
	QPoint m_lastPos;
	QPoint m_endPos;
	bool m_bPaintingItem;
	bool m_bTranslatingItem;
	bool m_bScalingItem;
};

#endif // SVGCANVASVIEW_H
