#include "zoomvideowidget.h"
#include <QTransform>

ZoomVideoWidget::ZoomVideoWidget(QWidget *parent)
    : QVideoWidget(parent),
    m_totalScaleFactor(1)
{
    grabGesture(Qt::PinchGesture);
}

bool ZoomVideoWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        gestureEvent(static_cast<QGestureEvent *>(event));
    return QVideoWidget::event(event);
}

void ZoomVideoWidget::gestureEvent(QGestureEvent *event)
{
    if (QGesture *pinch = event->gesture(Qt::PinchGesture))
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
}

void ZoomVideoWidget::pinchTriggered(QPinchGesture *gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        qreal scaleFactor = gesture->scaleFactor();
        m_totalScaleFactor *= scaleFactor;
        QTransform transform;
        transform.scale(m_totalScaleFactor, m_totalScaleFactor);
        setGraphicsEffect(new QGraphicsScaleEffect(transform));
    }
}
