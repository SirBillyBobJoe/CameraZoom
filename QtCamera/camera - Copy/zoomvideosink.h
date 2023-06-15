#ifndef ZOOMVIDEOSINK_H
#define ZOOMVIDEOSINK_H

#include <QVideoSink>

class ZoomVideoSink : public QVideoSink
{
    Q_OBJECT
public:
    ZoomVideoSink(QObject *parent = nullptr);

    void setZoomLevel(float level);
    float zoomLevel() const;

protected:
    void videoFrameProbed(const QVideoFrame &videoFrame) ;

private:
    float m_zoomLevel;
};

#endif // ZOOMVIDEOSINK_H
