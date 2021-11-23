#include "lcanvasview.h"
#include "lcanvasscene.h"

namespace lwscode {

class LCanvasWidget : public QWidget
{
public:
	LCanvasWidget(LCanvasView *view) : QWidget(view), m_view(view) {}

protected:
	void paintEvent(QPaintEvent *event)
	{
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		m_view->drawContents(&painter, event->rect().left(), event->rect().top(),
							 event->rect().width(), event->rect().height());
	}

	void mousePressEvent(QMouseEvent *event)
	{
		m_view->contentsMousePressEvent(event);
	}

	void mouseMoveEvent(QMouseEvent *event)
	{
		m_view->contentsMouseMoveEvent(event);
	}

	void mouseReleaseEvent(QMouseEvent *event)
	{
		m_view->contentsMouseReleaseEvent(event);
	}

	void mouseDoubleClickEvent(QMouseEvent *event)
	{
		m_view->contentsMouseDoubleClickEvent(event);
	}

	void dragEnterEvent(QDragEnterEvent *event)
	{
		m_view->contentsDragEnterEvent(event);
	}

	void dragMoveEvent(QDragMoveEvent *event)
	{
		m_view->contentsDragMoveEvent(event);
	}

	void dragLeaveEvent(QDragLeaveEvent *event)
	{
		m_view->contentsDragLeaveEvent(event);
	}

	void dropEvent(QDropEvent *event)
	{
		m_view->contentsDropEvent(event);
	}

	void wheelEvent(QWheelEvent *event)
	{
		m_view->contentsWheelEvent(event);
	}

	void contextMenuEvent(QContextMenuEvent *event)
	{
		m_view->contentsContextMenuEvent(event);
	}

protected:
	LCanvasView *m_view;
};

LCanvasView::LCanvasView(QWidget *parent)
	: QScrollArea(parent)
{
	this->setWidget(new LCanvasWidget(this));
	m_scene = nullptr;
	setCanvas(nullptr);
}

LCanvasView::LCanvasView(LCanvasScene *scene, QWidget *parent)
	: QScrollArea(parent)
	, m_scene(scene)
{
	this->setWidget(new LCanvasWidget(this));
	m_scene = nullptr;
	setCanvas(scene);
}

LCanvasView::~LCanvasView()
{
	setCanvas(nullptr);
}

void LCanvasView::contentsMousePressEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseReleaseEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseDoubleClickEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsMouseMoveEvent(QMouseEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsDragEnterEvent(QDragEnterEvent *event)
{
}

void LCanvasView::contentsDragMoveEvent(QDragMoveEvent *event)
{
}

void LCanvasView::contentsDragLeaveEvent(QDragLeaveEvent *event)
{
}

void LCanvasView::contentsDropEvent(QDropEvent *event)
{
}

void LCanvasView::contentsWheelEvent(QWheelEvent *event)
{
	event->ignore();
}

void LCanvasView::contentsContextMenuEvent(QContextMenuEvent *event)
{
	event->ignore();
}

void LCanvasView::setCanvas(LCanvasScene *scene)
{
	if (m_scene == scene) return;

	if (m_scene)
	{
		disconnect(m_scene);
		m_scene->removeView(this);
	}
	m_scene = scene;
	if (m_scene)
	{
		connect(m_scene, SIGNAL(resized()), this, SLOT(updateContentsSize()));
		m_scene->addView(this);
	}
	updateContentsSize();
	update();
}

const QTransform &LCanvasView::worldMatrix() const
{
	return m_worldMatrix;
}

const QTransform &LCanvasView::inverseWorldMatrix() const
{
	return m_inverseWorldMatrix;
}

bool LCanvasView::setWorldMatrix(const QTransform &matrix)
{
	bool enable = matrix.isInvertible();
	if (enable)
	{
		m_worldMatrix = matrix;
		m_inverseWorldMatrix = matrix.inverted();
		updateContentsSize();
		widget()->update();
	}
	return enable;
}

void LCanvasView::updateContentsSize()
{
	if (m_scene)
	{
		QRect rect = m_worldMatrix.mapRect(QRect(0, 0, m_scene->width(), m_scene->height()));
		widget()->resize(rect.size());
	}
	else
	{
		widget()->resize(size());
	}
}

void LCanvasView::drawContents(QPainter *painter, int x, int y, int width, int height)
{
	if (!m_scene) return;

	QPainterPath clipPath;
	clipPath.addRect(m_scene->rect());
	painter->setClipPath(m_worldMatrix.map(clipPath), Qt::IntersectClip);
	m_scene->drawViewArea(this, painter, QRect(x, y, width, height));
}

QSize LCanvasView::sizeHint() const
{
	if (!m_scene) return QScrollArea::sizeHint();

	return (m_scene->size() + 2 * QSize(this->frameWidth(), this->frameWidth())).boundedTo(3 * QApplication::primaryScreen()->size() / 4);
}

} // namespace
