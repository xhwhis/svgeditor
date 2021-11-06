#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

#include "lcanvasitem.h"

namespace lwscode {

class LCanvasView : public QWidget
{
	Q_OBJECT

public:
	enum ItemHitPos
	{
		None,
		TopLeft,
		TopMiddle,
		TopRight,
		MiddleRight,
		BottomRight,
		BottomMiddle,
		BottomLeft,
		MiddleLeft
	};

	LCanvasView(QWidget *parent = nullptr);
	virtual ~LCanvasView();

	void setCanvasColor(const QColor &color);
	void setStrokeColor(const QColor &color);
	void setStrokeWidth(int width);
	void clearCanvas();
	bool noItems();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

protected slots:
	void setItemType(LCanvasItem::ItemType itemType);
	void readItemsFromFile(const QString &filePath);
	void writeItemsToFile(const QString &filePath);
	void cutItem();
	void copyItem();
	void pasteItem();
	void deleteItem();
	void moveTopItem();
	void moveUpItem();
	void moveDownItem();
	void moveBottomItem();

private:
	ItemHitPos getItemHitPos(const QPoint &point);
	void initRightClickMenu();
	void reselectItems();
	void deselectAllItems();
	void paintRubberBand(LCanvasItem *item, QPainter &painter);
	void paintSelectedBox(LCanvasItem *item, QPainter &painter);
	void beforePaintItem(const QPoint &pos);
	void selectItemsByClick(const QPoint &pos);
	void selectItemsByFrame(const QPoint &pos);
	void resizeSelectedItem(const QPoint &pos);
	void readItemFromXml(LCanvasItem::ItemType itemType, QXmlStreamReader &reader);

private:
	QMenu *m_rightClickMenu;
	LCanvasItem::ItemType m_itemType;
	LCanvasItem *m_item;
	QList<LCanvasItem *> m_allItems;
	QSet<LCanvasItem *> m_selectedItems;
	QList<LCanvasItem *> m_textItems;
	LCanvasItem *m_copyItem;
	QColor m_canvasColor;
	QColor m_fillColor;
	QColor m_strokeColor;
	int m_strokeWidth;
	bool m_bPaintingItem;
	bool m_bPaintingPath;
	QPoint m_startPos;
	QPoint m_lastPos;
	QPoint m_endPos;
	QRect m_rubberBand;
	QRect m_topLeftPos;
	QRect m_topMiddlePos;
	QRect m_topRightPos;
	QRect m_middleRightPos;
	QRect m_bottomRightPos;
	QRect m_bottomMiddlePos;
	QRect m_bottomLeftPos;
	QRect m_middleLeftPos;
	QLineEdit *m_lineEdit;
	bool m_showFrameRect;
	bool m_mouseFrameSelection;
	bool m_mouseFrameSelect;
	bool m_mouseClickSelect;
	LCanvasItem *m_selectedItem;
	ItemHitPos m_itemHitPos;
	bool m_mouseClickOutRect;
};

} // namespace

#endif // LCANVASVIEW_H
