#ifndef LCANVAS_H
#define LCANVAS_H

#include <QtWidgets>

namespace lwscode {

class LCanvasItem;
class LCanvasShape;
class LCanvasRect;
class LCanvasPolygon;
class LCanvasEllipse;
class LCanvasText;
class LCanvasLine;
class LCanvasView;
class LCanvasChunk;
class LCanvasScene;

typedef QList<LCanvasItem *> LCanvasItemList;
typedef QList<LCanvasView *> LCanvasViewList;

} // namespace

#endif // LCANVAS_H
