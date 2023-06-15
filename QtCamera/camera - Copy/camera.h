// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef CAMERA_H
#define CAMERA_H

#include <QCamera>
#include <QImageCapture>
#include <QScopedPointer>

#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QAudioInput>
#include <QMainWindow>

#include <QHostAddress>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QHostAddress>

#include <QGraphicsVideoItem>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QGesture>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QSwipeGesture>

QT_BEGIN_NAMESPACE
namespace Ui { class Camera; }
class QActionGroup;
QT_END_NAMESPACE


class Camera : public QMainWindow
{
    Q_OBJECT

public:
    Camera();

public slots:
    void uploadImage();
    void clearImage();
    void displayIpAddress();
    void setHighResolution();
    void setMidResolution();
    void setLowResolution();




private slots:
    void setCamera(const QCameraDevice &cameraDevice);

    void startCamera();
    void stopCamera();

    void takeImage();
    void displayCaptureError(int, QImageCapture::Error, const QString &errorString);

    void configureCaptureSettings();
    void configureImageSettings();

    void displayCameraError();

    void updateCameraDevice(QAction *action);

    void updateCameraActive(bool active);
    void updateCaptureMode();


    void processCapturedImage(int requestId, const QImage &img);

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);
    void imageSaved(int id, const QString &fileName);

    void updateCameras();




protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::Camera *ui;

    QActionGroup *videoDevicesGroup  = nullptr;;
    QMediaDevices m_devices;
    QScopedPointer<QImageCapture> m_imageCapture;
    QMediaCaptureSession m_captureSession;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QAudioInput> m_audioInput;


    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;
    bool m_doImageCapture = true;

\
    QGraphicsScene* scene;
\

};

#endif
