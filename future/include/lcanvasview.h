#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

#include <QtWidgets>

namespace lwscode {

class LCanvasScene;

class LCanvasView : public QScrollArea
{
	Q_OBJECT

public:
	LCanvasView(QWidget *parent = nullptr);
	LCanvasView(LCanvasScene *scene, QWidget *parent = nullptr);
	virtual ~LCanvasView();

	LCanvasScene *scene() const { return m_scene; }
	void setCanvas(LCanvasScene *scene);

	const QTransform &worldMatrix() const;
	const QTransform &inverseWorldMatrix() const;
	bool setWorldMatrix(const QTransform &matrix);

	virtual QSize sizeHint() const;

protected:
	friend class LCanvasWidget;
	virtual void drawContents(QPainter *painter, int x, int y, int width, int height);

	virtual void contentsMousePressEvent(QMouseEvent *event);
	virtual void contentsMouseReleaseEvent(QMouseEvent *event);
	virtual void contentsMouseDoubleClickEvent(QMouseEvent *event);
	virtual void contentsMouseMoveEvent(QMouseEvent *event);
	virtual void contentsDragEnterEvent(QDragEnterEvent *event);
	virtual void contentsDragMoveEvent(QDragMoveEvent *event);
	virtual void contentsDragLeaveEvent(QDragLeaveEvent *event);
	virtual void contentsDropEvent(QDropEvent *event);
	virtual void contentsWheelEvent(QWheelEvent *event);
	virtual void contentsContextMenuEvent(QContextMenuEvent *event);

private slots:
	void updateContentsSize();

private:
	void drawContents(QPainter *painter);

private:
	friend class LCanvasScene;
	LCanvasScene *m_scene;
	QTransform m_worldMatrix;
	QTransform m_inverseWorldMatrix;
};

} // namespace

#endif // LCANVASVIEW_H
