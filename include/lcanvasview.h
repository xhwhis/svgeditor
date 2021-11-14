#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

#include "lcanvasitem.h"

namespace lwscode {

typedef QList<SPtrLCanvasItem> LCanvasItemList;

class LCanvasView : public QWidget
{
	Q_OBJECT

public:
	LCanvasView(QWidget *parent = nullptr);
	virtual ~LCanvasView();

	void setCanvasColor(const QColor &color);
	void setStrokeColor(const QColor &color);
	void setStrokeWidth(int width);
	void clearCanvas();
	bool existItems();

	enum ItemHitPos
	{
		None = -1,
		TopLeft,
		TopMiddle,
		TopRight,
		MiddleRight,
		BottomRight,
		BottomMiddle,
		BottomLeft,
		MiddleLeft
	};

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
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
	void showLineEdit();
	void initRightClickMenu();
	void setCursorByPos(const QPoint &pos);
	void deselectAllItems();
	void paintRubberBand(SPtrLCanvasItem item, QPainter &painter, bool flag = false);
	void hitTest(const QPoint &pos);
	void resizeSelectedItem(const QPoint &pos);
	void readItemFromXml(LCanvasItem::ItemType itemType, QXmlStreamReader &reader);

private:
	LCanvasItem::ItemType m_itemType;
	SPtrLCanvasItem m_item;
	LCanvasItemList m_allItems;
	LCanvasItemList m_textItems;
	LCanvasItemList m_selectedItems;
	LCanvasItemList m_duplicatedItems;
	QLineEdit *m_lineEdit;
	QMenu *m_rightClickMenu;
	QColor m_canvasColor;
	float m_fScaleFactor;
	QColor m_fillColor;
	QColor m_strokeColor;
	int m_strokeWidth;
	QPoint m_startPos;
	QPoint m_lastPos;
	bool m_bPaintingItem;
	bool m_bPaintingPath;
	bool m_bMovingItems;
	bool m_bScalingItem;
	bool m_bSelectingItems;
	ItemHitPos m_itemHitPos;
	QRect m_topLeftPos;
	QRect m_topMiddlePos;
	QRect m_topRightPos;
	QRect m_middleRightPos;
	QRect m_bottomRightPos;
	QRect m_bottomMiddlePos;
	QRect m_bottomLeftPos;
	QRect m_middleLeftPos;
	QRect m_selectedBox;
};

} // namespace

#endif // LCANVASVIEW_H
