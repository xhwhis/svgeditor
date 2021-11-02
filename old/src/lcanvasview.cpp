#include "lcanvasview.h"

//namespace lwscode {

LCanvasWidget::LCanvasWidget(LCanvasView *view)
	: QWidget(view)
	, m_view(view)
{

}

void LCanvasWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (m_view->m_bHighQuality) {
		painter.setRenderHint(QPainter::Antialiasing);
		painter.setRenderHint(QPainter::LosslessImageRendering);
	}
	m_view->drawContents(&painter, event->rect().left(), event->rect().top(),
						 event->rect().width(), event->rect().height());
}

void LCanvasWidget::mousePressEvent(QMouseEvent *event)
{
	m_view->contentsMousePressEvent(event);
}

void LCanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
	m_view->contentsMouseMoveEvent(event);
}

void LCanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
	m_view->contentsMouseReleaseEvent(event);
}

void LCanvasWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	m_view->contentsMouseDoubleClickEvent(event);
}

void LCanvasWidget::dragEnterEvent(QDragEnterEvent *event)
{
	m_view->contentsDragEnterEvent(event);
}

void LCanvasWidget::dragMoveEvent(QDragMoveEvent *event)
{
	m_view->contentsDragMoveEvent(event);
}

void LCanvasWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	m_view->contentsDragLeaveEvent(event);
}

void LCanvasWidget::dropEvent(QDropEvent *event)
{
	m_view->contentsDropEvent(event);
}

void LCanvasWidget::wheelEvent(QWheelEvent *event)
{
	m_view->contentsWheelEvent(event);
}

void LCanvasWidget::contextMenuEvent(QContextMenuEvent *event)
{
	m_view->contentsContextMenuEvent(event);
}

LCanvasView::LCanvasView(QWidget *parent)
	: QScrollArea(parent)
	, m_bHighQuality(false)
{
	this->setWidget(new LCanvasWidget(this));
	m_canvas = nullptr;
	setCanvas(nullptr);
}

LCanvasView::LCanvasView(LCanvas *canvas, QWidget *parent)
	: QScrollArea(parent)
	, m_canvas(canvas)
	, m_bHighQuality(false)
{
	this->setWidget(new LCanvasWidget(this));
	m_canvas = nullptr;
	setCanvas(canvas);
}

LCanvasView::~LCanvasView()
{
	setCanvas(nullptr);
}

bool LCanvasView::highQualityRendering() const
{
	return m_bHighQuality;
}

void LCanvasView::setHighQualityRendering(bool enable)
{
	m_bHighQuality = enable;
	widget()->update();
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

void LCanvasView::setCanvas(LCanvas *canvas)
{
	if (m_canvas == canvas) return;

	if (m_canvas)
	{
		disconnect(m_canvas);
		m_canvas->removeView(this);
	}
	m_canvas = canvas;
	if (m_canvas) {
		connect(m_canvas, SIGNAL(resized()), this, SLOT(updateContentsSize()));
		m_canvas->addView(this);
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
	if (m_canvas)
	{
		QRect rect = m_worldMatrix.mapRect(QRect(0, 0, m_canvas->width(), m_canvas->height()));
		widget()->resize(rect.size());
	}
	else
	{
		widget()->resize(size());
	}
}

void LCanvasView::drawContents(QPainter *painter, int x, int y, int width, int height)
{
	if (!m_canvas) return;

	QPainterPath clipPath;
	clipPath.addRect(m_canvas->rect());
	painter->setClipPath(m_worldMatrix.map(clipPath), Qt::IntersectClip);
	m_canvas->drawViewArea(this, painter, QRect(x, y, width, height), false);
}

QSize LCanvasView::sizeHint() const
{
	if (!canvas())
		return QScrollArea::sizeHint();

	return (canvas()->size() + 2 * QSize(this->frameWidth(), this->frameWidth())).boundedTo(3 * QGuiApplication::primaryScreen()->size() / 4);
}

//} // namespace
