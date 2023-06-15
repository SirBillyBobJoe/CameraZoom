#include <QApplication>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraImageCapture>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

class View : public QGraphicsView {
    Q_OBJECT
public:
    View(QGraphicsScene *scene) : QGraphicsView(scene) {
    }
protected:
    void wheelEvent(QWheelEvent *event) override {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        double scaleFactor = 1.15;
        if(event->delta() > 0) {
            // Zoom in
            scale(scaleFactor, scaleFactor);
        } else {
            // Zooming out
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCamera camera(QCameraInfo::defaultCamera());
    QCameraImageCapture imageCapture(&camera);

    QGraphicsScene scene;
    View view(&scene);
    QGraphicsPixmapItem pixmapItem;
    scene.addItem(&pixmapItem);
    view.show();

    QObject::connect(&imageCapture, &QCameraImageCapture::imageCaptured,
                     [&pixmapItem](int id, const QImage &preview) {
                         pixmapItem.setPixmap(QPixmap::fromImage(preview));
                     });

    camera.setCaptureMode(QCamera::CaptureStillImage);
    camera.start();

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &imageCapture, [&, &imageCapture]() {
        imageCapture.capture();
    });
    timer.start(100); // Capture image every 100 ms

    return app.exec();
}

#include "main.moc"
