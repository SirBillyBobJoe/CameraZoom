#include "zoomvideosink.h"
#include "qvideoframe.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QAbstractVideoBuffer>


ZoomVideoSink::ZoomVideoSink(QObject *parent)
    : QVideoSink(parent),
    m_zoomLevel(1.0f)  // initialize zoom level to 1.0
{
}

void ZoomVideoSink::setZoomLevel(float level)
{
    m_zoomLevel = level;
}

float ZoomVideoSink::zoomLevel() const
{
    return m_zoomLevel;
}

void ZoomVideoSink::videoFrameProbed(const QVideoFrame &frame)
{
    QVideoFrame copyFrame(frame);
    copyFrame.map(QAbstractVideoBuffer::ReadOnly);
    int zoomLevel = m_zoomLevel;

    QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(copyFrame.pixelFormat());
    QImage img(copyFrame.bits(), copyFrame.width(), copyFrame.height(), imageFormat);

    // Create an OpenCV image from the QImage
    cv::Mat mat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine());

    // Apply the zoom
    cv::Mat resized;
    cv::resize(mat, resized, cv::Size(), zoomLevel, zoomLevel, cv::INTER_NEAREST);

    // Convert the OpenCV image back to a QImage
    QImage zoomed((uchar*) resized.data, resized.cols, resized.rows, resized.step, QImage::Format_RGB32);

    copyFrame.unmap();

    emit frameAvailable(zoomed);
}


