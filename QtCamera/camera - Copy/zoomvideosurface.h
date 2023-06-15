#ifndef ZOOMVIDEOSURFACE_H
#define ZOOMVIDEOSURFACE_H

#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QPinchGesture>

class ZoomVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT

public:
    ZoomVideoSurface(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;
    bool present(const QVideoFrame &frame) override;

    void setZoomFactor(qreal factor);

private:
    qreal m_zoomFactor = 1.0;
};

#endif // ZOOMVIDEOSURFACE_H
