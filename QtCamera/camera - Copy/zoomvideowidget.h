#ifndef ZOOMVIDEOWIDGET_H
#define ZOOMVIDEOWIDGET_H

#include <QVideoWidget>
#include <QPinchGesture>
#include <QGestureEvent>

class ZoomVideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    ZoomVideoWidget(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
    void gestureEvent(QGestureEvent *event);

private:
    void pinchTriggered(QPinchGesture *gesture);

    qreal m_totalScaleFactor;
};

#endif // ZOOMVIDEOWIDGET_H
