#include "lcanvas.h"
#include "utility.h"

namespace lwscode {



//LCanvasItemList LCanvasScene::collisions(const QRect& rect) const
//{
//	LCanvasRect i(rect, (LCanvasScene *)this);
//	i.setPen(Qt::NoPen);
//	i.show();
//	LCanvasItemList l = i.collisions(true);
//	std::sort(l.begin(), l.end(),
//			  [](const LCanvasItem *item1, const LCanvasItem *item2) -> bool
//			{
//		if (item1->z() == item2->z())
//			return item1 > item2;

//		return (item1->z() > item2->z());
//		});
//	return l;
//}

//LCanvasItemList LCanvasScene::collisions(
//	const QPolygon& chunklist,
//	const LCanvasItem *item,
//	bool exact) const
//{
//	QSet<LCanvasItem *> seen;
//	LCanvasItemList result;
//	for (int i = 0; i < chunklist.size(); i++) {
//		int x = chunklist[i].x();
//		int y = chunklist[i].y();
//		if (validChunk(x, y)) {
//			const LCanvasItemList &l = chunk(x, y).itemList();
//			for (int i = 0; i < l.size(); ++i) {
//				LCanvasItem *g = l.at(i);
//				if (g != item) {
//					if (!seen.contains(g)) {
//						seen.insert(g);
//						if (!exact || item->collidesWith(g))
//							result.append(g);
//					}
//				}
//			}
//		}
//	}
//	return result;
//}




static bool collision_double_dispatch(
	const LCanvasShape *shape1,
	const LCanvasRect *rectangle1,
	const LCanvasEllipse *ellipse1,
	const LCanvasText *text1,
	const LCanvasShape *shape2,
	const LCanvasRect *rectangle2,
	const LCanvasEllipse *ellipse2,
	const LCanvasText *text2)
{
	const LCanvasItem *item1 = shape1 ?
				(const LCanvasItem *)shape1 : rectangle1 ?
				(const LCanvasItem *)rectangle1 : ellipse1 ?
				(const LCanvasItem *)ellipse1 : (const LCanvasItem *)text1;
	const LCanvasItem *item2 = shape2 ?
				(const LCanvasItem *)shape2 : rectangle2 ?
				(const LCanvasItem *)rectangle2 : ellipse2 ?
				(const LCanvasItem *)ellipse2 : (const LCanvasItem *)text2;

	if ((rectangle1 || text1) && (rectangle2 || text2))
	{
		QRect rect1 = item1->boundingRect();
		QRect rect2 = item2->boundingRect();
		return rect1.intersects(rect2);
	}
	else if (ellipse1 && ellipse2 &&
			 ellipse1->width() == ellipse1->height() &&
			 ellipse2->width() == ellipse2->height())
	{
		double dx = ellipse1->x() - ellipse2->x();
		double dy = ellipse1->y() - ellipse2->y();
		double dr = (ellipse1->width() + ellipse2->width()) / 2;
		return dx * dx + dy * dy <= dr * dr;
	}
	else if (shape1 && (shape2 || text2))
	{
		QPolygon polygon1 = shape1->areaPoints();
		QPolygon polygon2 = shape2 ? shape2->areaPoints() : QPolygon(item2->boundingRect());
		return QRegion(polygon1).intersects(QRegion(polygon2, Qt::WindingFill));
	}
	else
	{
		return collision_double_dispatch(shape2, rectangle2, ellipse2, text2,
										 shape1, rectangle1, ellipse1, text1);
	}
}

} // namespace
