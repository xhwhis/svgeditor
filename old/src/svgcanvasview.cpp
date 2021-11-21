#include "svgcanvasview.h"

SVGCanvasView::SVGCanvasView(LCanvasScene *scene, QWidget *parent)
	: LCanvasView(scene, parent)
{

}

SVGCanvasView::~SVGCanvasView()
{

}

void SVGCanvasView::cutItem()
{

}

void SVGCanvasView::copyItem()
{

}

void SVGCanvasView::pasteItem()
{

}

void SVGCanvasView::deleteItem()
{

}

void SVGCanvasView::moveTopItem()
{

}

void SVGCanvasView::moveUpItem()
{

}

void SVGCanvasView::moveDownItem()
{

}

void SVGCanvasView::moveBottomItem()
{

}

void SVGCanvasView::contentsMousePressEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_startPos = m_lastPos = m_endPos = pos;
	switch (m_itemType)
	{
	case LCanvasItem::Line:
	{
		m_paintingLine = new LCanvasLine(this->scene());
		m_paintingLine->setX(pos.x());
		m_paintingLine->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	case LCanvasItem::Rectangle:
	{
		m_paintingRect = new LCanvasRect(this->scene());
		m_paintingRect->setX(pos.x());
		m_paintingRect->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	case LCanvasItem::Ellipse:
	{
		m_paintingEllipse = new LCanvasEllipse(this->scene());
		m_paintingEllipse->setX(pos.x());
		m_paintingEllipse->setY(pos.y());
		m_bPaintingItem = true;
		break;
	}
	case LCanvasItem::Text:
	{
		m_paintingText = new LCanvasText(this->scene());
		m_paintingText->setX(pos.x());
		m_paintingText->setY(pos.y());
		QFont font = m_paintingText->font();
		font.setPointSize(32);
		m_paintingText->setFont(font);
		m_paintingText->setColor(Qt::red);
		m_paintingText->setText("hello world");
//		m_paintingText->setTextFlags();
		m_bPaintingItem = true;
		m_paintingText->show();
		this->scene()->update();
		break;
	}
	default:
	{
		m_bPaintingItem = false;
		break;
	}
	}
}

void SVGCanvasView::contentsMouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;
	if (m_bPaintingItem)
	{
		switch (m_itemType)
		{
		case LCanvasItem::Line:
		{
			m_paintingLine->setPoints(m_startPos.x(), m_startPos.y(), m_endPos.x(), m_endPos.y());
			m_paintingLine->show();
			this->scene()->update();
			break;
		}
		case LCanvasItem::Rectangle:
		{
			m_paintingRect->setSize(qAbs(m_endPos.x() - m_startPos.x()),
									qAbs(m_endPos.y() - m_startPos.y()));
			m_paintingRect->show();
			this->scene()->update();
			break;
		}
		case LCanvasItem::Ellipse:
		{
			m_paintingEllipse->setSize(qAbs(m_endPos.x() - m_startPos.x()),
									   qAbs(m_endPos.y() - m_startPos.y()));
			m_paintingEllipse->show();
			this->scene()->update();
			break;
		}
		default:
		{
			break;
		}
		}
	}
	m_lastPos = pos;
}

void SVGCanvasView::contentsMouseReleaseEvent(QMouseEvent *event)
{
	QPoint pos = event->pos();
	m_endPos = pos;

	if (m_bPaintingItem)
	{
		m_paintingLine = nullptr;
		m_paintingRect = nullptr;
		m_paintingEllipse = nullptr;
	}

	m_startPos = QPoint();
	m_endPos = QPoint();
	m_lastPos = QPoint();
}

void SVGCanvasView::contentsContextMenuEvent(QContextMenuEvent *event)
{

}
