#ifndef LCANVASVIEW_H
#define LCANVASVIEW_H

//namespace lwscode {

class LCanvasWidget : public QWidget
{
public:
	LCanvasWidget(LCanvasView *view);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);
	void wheelEvent(QWheelEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);

protected:
	LCanvasView *m_view;
};

class LCanvasView : public QScrollArea
{
	Q_OBJECT

public:
	LCanvasView(QWidget *parent = nullptr);
	LCanvasView(LCanvas *canvas, QWidget *parent = nullptr);
	virtual ~LCanvasView();

	LCanvas *canvas() const
	{
		return m_canvas;
	}
	void setCanvas(LCanvas *canvas);

	const QTransform &worldMatrix() const;
	const QTransform &inverseWorldMatrix() const;
	bool setWorldMatrix(const QTransform &matrix);

	virtual QSize sizeHint() const;

	bool highQualityRendering() const;

public slots:
	void setHighQualityRendering(bool enable);

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
	friend class LCanvas;
	LCanvas *m_canvas;
	QTransform m_worldMatrix;
	QTransform m_inverseWorldMatrix;
	bool m_bHighQuality;
};

//} // namespace

#endif // LCANVASVIEW_H
