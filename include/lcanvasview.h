#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

#include "lcanvasitem.h"

namespace lwscode {

typedef QList<SPtrLCanvasItem> LCanvasItemList;

enum HitTestStatus
{
	NoneStatus = 0x00000000,
	PaintingItem = 0x00000010,
	PaintingPath = 0x00000011,
	MovingItems = 0x00000020,
	ScalingItem = 0x00000040,
	SelectingItems = 0x00000080
};

enum ItemHitPos
{
	NonePos = -1,
	TopLeft,
	TopMiddle,
	TopRight,
	MiddleRight,
	BottomRight,
	BottomMiddle,
	BottomLeft,
	MiddleLeft
};

class LCanvasView : public QWidget
{
	Q_OBJECT

public:
	LCanvasView(QWidget *parent = nullptr);
	virtual ~LCanvasView();

	void setCanvasColor(const QColor &color);
	void setFillColor(const QColor &color);
	void setStrokeColor(const QColor &color);
	void setStrokeWidth(int width);
	void clearCanvas();
	bool existItems();

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

protected slots:
	void setItemType(ItemType itemType);
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
	void readItemFromXml(ItemType itemType, QXmlStreamReader &reader);

private:
	ItemType m_itemType;
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
	HitTestStatus m_hitTestStatus;
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
