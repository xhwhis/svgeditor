#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

#include "lcanvasitem.h"

namespace lwscode {

typedef QList<SPtrLCanvasItem> LCanvasItemList;

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
	void resizeLineEdit();
	void addText();
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
	void initLineEdit();
	void initRightClickMenu();
	void reselectItems();
	void deselectAllItems();
	void paintRubberBand(SPtrLCanvasItem item, QPainter &painter);
	void paintSelectedBox(SPtrLCanvasItem item, QPainter &painter);
	void beforePaintItem(const QPoint &pos);
	void selectItemsByClick(const QPoint &pos);
	void selectItemsByFrame(const QPoint &pos);
	void resizeSelectedItem(const QPoint &pos);
	void readItemFromXml(LCanvasItem::ItemType itemType, QXmlStreamReader &reader);

private:
	LCanvasItem::ItemType m_itemType;
	SPtrLCanvasItem m_item;
	SPtrLCanvasItem m_selectedItem;
	LCanvasItemList m_allItems;
	LCanvasItemList m_textItems;
	LCanvasItemList m_selectedItems;
	LCanvasItemList m_duplicatedItems;
	QMenu *m_rightClickMenu;
	QLineEdit *m_lineEdit;
	QColor m_canvasColor;
	QColor m_fillColor;
	QColor m_strokeColor;
	int m_strokeWidth;
	QPoint m_startPos;
	QPoint m_lastPos;
	QPoint m_endPos;
	bool m_bPaintingItem;
	bool m_bPaintingPath;
	bool m_showFrameRect;
	bool m_mouseFrameSelection;
	bool m_mouseFrameSelect;
	bool m_mouseClickSelect;
	ItemHitPos m_itemHitPos;
	QRect m_rubberBand;
	QRect m_topLeftPos;
	QRect m_topMiddlePos;
	QRect m_topRightPos;
	QRect m_middleRightPos;
	QRect m_bottomRightPos;
	QRect m_bottomMiddlePos;
	QRect m_bottomLeftPos;
	QRect m_middleLeftPos;
};

} // namespace

#endif // LCANVASVIEW_H
