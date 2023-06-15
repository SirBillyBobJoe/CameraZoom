// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "camera.h"
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include "ui_camera_mobile.h"
#else
#include "ui_camera.h"
#endif
#include "imagesettings.h"
#include <QVideoWidget>
#include <QCameraDevice>

#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioInput>

#include <QGesture>
#include <QGestureEvent>
#include <QPinchGesture>

#include <QMessageBox>
#include <QPalette>
#include <QImage>

#include <QtWidgets>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QHostAddress>
#include <QNetworkInterface>
#include <zoomvideosink.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#include <QGraphicsScene>

#include <QGesture>
#include <QGestureEvent>
#include <QPinchGesture>
#include <QSwipeGesture>




QGraphicsVideoItem* cameraVideoItem;

Camera::Camera()
    : ui(new Ui::Camera)
{
    ui->setupUi(this);



    //Camera devices:

    videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    updateCameras();
    connect(&m_devices, &QMediaDevices::videoInputsChanged, this, &Camera::updateCameras);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &Camera::updateCameraDevice);
    connect(ui->captureWidget, &QTabWidget::currentChanged, this, &Camera::updateCaptureMode);
    connect(ui->uploadButton, &QPushButton::clicked, this, &Camera::uploadImage);
    connect(ui->clearImage, &QPushButton::clicked, this, &Camera::clearImage);
    connect(ui->actionIP_Address, &QAction::triggered, this, &Camera::displayIpAddress);
    connect(ui->highResolution, &QPushButton::clicked, this, &Camera::setHighResolution);
    connect(ui->midResolution, &QPushButton::clicked, this, &Camera::setMidResolution);
    connect(ui->lowResolution, &QPushButton::clicked, this, &Camera::setLowResolution);

    setCamera(QMediaDevices::defaultVideoInput());

    //displays on graphics view
    scene = new QGraphicsScene(this);
    // Set video output and add item to scene
    cameraVideoItem = new QGraphicsVideoItem;
    m_captureSession.setVideoOutput(cameraVideoItem);
    // Add cameraVideoItem to scene
    scene->addItem(cameraVideoItem);

    cameraVideoItem->setScale(3);


    // Assign scene to graphicsView
    ui->graphicsView->setScene(scene);



}


void Camera::setHighResolution()
{
    if (m_camera) {
        QCameraDevice cameraDevice = m_camera->cameraDevice();
        QList<QCameraFormat> formats = cameraDevice.videoFormats();
        if (!formats.isEmpty()) {
            // Assume the first one is the highest resolution
            QCameraFormat maxResFormat = formats[0];

            // Find the format with the highest resolution
            for (const QCameraFormat &format : formats) {
                if (format.resolution().width() * format.resolution().height() >
                    maxResFormat.resolution().width() * maxResFormat.resolution().height()) {
                    maxResFormat = format;
                }
            }

            // Set the camera format
            m_camera->setCameraFormat(maxResFormat);
        }
    }

    // Restart the camera to apply the changes
    m_camera->stop();
    m_camera->start();
}

void Camera::setMidResolution()
{
    if (m_camera) {
        QCameraDevice cameraDevice = m_camera->cameraDevice();
        QList<QCameraFormat> formats = cameraDevice.videoFormats();
        if (!formats.isEmpty()) {
            // Calculate half of the max resolution
            QCameraFormat maxResFormat = formats[0];
            for (const QCameraFormat &format : formats) {
                if (format.resolution().width() * format.resolution().height() >
                    maxResFormat.resolution().width() * maxResFormat.resolution().height()) {
                    maxResFormat = format;
                }
            }
            int halfMaxRes = maxResFormat.resolution().width() * maxResFormat.resolution().height() / 2;

            // Find the format with the highest resolution below half max resolution
            QCameraFormat midResFormat = formats[0];
            for (const QCameraFormat &format : formats) {
                int currentRes = format.resolution().width() * format.resolution().height();
                if (currentRes <= halfMaxRes && currentRes > midResFormat.resolution().width() * midResFormat.resolution().height()) {
                    midResFormat = format;
                }
            }

            // Set the camera format
            m_camera->setCameraFormat(midResFormat);
        }
    }

    // Restart the camera to apply the changes
    m_camera->stop();
    m_camera->start();
}

void Camera::setLowResolution()
{
    if (m_camera) {
        QCameraDevice cameraDevice = m_camera->cameraDevice();
        QList<QCameraFormat> formats = cameraDevice.videoFormats();
        if (!formats.isEmpty()) {
            // Calculate half of the mid resolution
            QCameraFormat midResFormat = formats[0];
            for (const QCameraFormat &format : formats) {
                int currentRes = format.resolution().width() * format.resolution().height();
                if (currentRes <= midResFormat.resolution().width() * midResFormat.resolution().height() / 2) {
                    midResFormat = format;
                }
            }
            int halfMidRes = midResFormat.resolution().width() * midResFormat.resolution().height() / 2;

            // Find the format with the highest resolution below half mid resolution
            QCameraFormat lowResFormat = formats[0];
            for (const QCameraFormat &format : formats) {
                int currentRes = format.resolution().width() * format.resolution().height();
                if (currentRes <= halfMidRes && currentRes > lowResFormat.resolution().width() * lowResFormat.resolution().height()) {
                    lowResFormat = format;
                }
            }

            // Set the camera format
            m_camera->setCameraFormat(lowResFormat);
        }
    }

    // Restart the camera to apply the changes
    m_camera->stop();
    m_camera->start();
}




//clears the image
void Camera::clearImage()
{
    // Clear the pixmap
    ui->lastImagePreviewLabel->clear();

    // Switch back to the viewfinder
    displayViewfinder();
}
QString getIpAddress() {
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}
void Camera::displayIpAddress() {
    QString ipAddress = getIpAddress();
    QMessageBox::information(this, tr("IP Address"), tr("Your IP address is: ") + ipAddress);
}

//uploads an image
void Camera::uploadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if(fileName.isEmpty())
        return;

    QImage image;
    if(!image.load(fileName))
        return;

    // scale image to fit the viewfinder
    QImage scaledImage = image.scaled(ui->graphicsView->size(),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));
    // display captured image
    displayCapturedImage();
}


void Camera::setCamera(const QCameraDevice &cameraDevice)
{
    m_camera.reset(new QCamera(cameraDevice));
    m_captureSession.setCamera(m_camera.data());

    connect(m_camera.data(), &QCamera::activeChanged, this, &Camera::updateCameraActive);
    connect(m_camera.data(), &QCamera::errorOccurred, this, &Camera::displayCameraError);



    if (!m_imageCapture) {
        m_imageCapture.reset(new QImageCapture);
        m_captureSession.setImageCapture(m_imageCapture.get());
        connect(m_imageCapture.get(), &QImageCapture::readyForCaptureChanged, this,
                &Camera::readyForCapture);
        connect(m_imageCapture.get(), &QImageCapture::imageCaptured, this,
                &Camera::processCapturedImage);
        connect(m_imageCapture.get(), &QImageCapture::imageSaved, this, &Camera::imageSaved);
        connect(m_imageCapture.get(), &QImageCapture::errorOccurred, this,
                &Camera::displayCaptureError);
    }

    m_captureSession.setVideoOutput(cameraVideoItem);

    updateCameraActive(m_camera->isActive());

    readyForCapture(m_imageCapture->isReadyForCapture());

    updateCaptureMode();

    if (m_camera->cameraFormat().isNull()) {
        auto formats = cameraDevice.videoFormats();
        if (!formats.isEmpty()) {
            QCameraFormat selectedFormat;
            for (const auto &fmt : formats) {
                if (fmt.resolution() == m_imageCapture->resolution()) {
                    selectedFormat = fmt;
                    break;
                }
            }

            if (!selectedFormat.isNull()) {
                m_camera->setCameraFormat(selectedFormat);
            }

        }
    }


    m_camera->start();


}

void Camera::keyPressEvent(QKeyEvent * event)
{
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_doImageCapture) {
            takeImage();
        }
        event->accept();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}


void Camera::processCapturedImage(int requestId, const QImage& img)
{
    Q_UNUSED(requestId);
    QImage scaledImage = img.scaled(ui->graphicsView->size(),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

    ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 1 seconds.
    displayCapturedImage();
    QTimer::singleShot(1000, this, &Camera::displayViewfinder);
}

void Camera::configureCaptureSettings()
{
    if (m_doImageCapture)
        configureImageSettings();

}

void Camera::configureImageSettings()
{
    ImageSettings settingsDialog(m_imageCapture.get());
    settingsDialog.setWindowFlags(settingsDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if (settingsDialog.exec()) {
        settingsDialog.applyImageSettings();
    }
}

void Camera::takeImage()
{
    m_isCapturingImage = true;
    m_imageCapture->captureToFile();
}

void Camera::displayCaptureError(int id, const QImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    m_isCapturingImage = false;
}

void Camera::startCamera()
{
    m_camera->start();
}

void Camera::stopCamera()
{
    m_camera->stop();
}

void Camera::updateCaptureMode()
{
    int tabIndex = ui->captureWidget->currentIndex();
    m_doImageCapture = (tabIndex == 0);
}

void Camera::updateCameraActive(bool active)
{
    if (active) {
        ui->actionStartCamera->setEnabled(false);
        ui->actionStopCamera->setEnabled(true);
        ui->captureWidget->setEnabled(true);
        ui->actionSettings->setEnabled(true);
        ui->actionIP_Address->setEnabled(true);
    } else {
        ui->actionStartCamera->setEnabled(true);
        ui->actionStopCamera->setEnabled(false);
        ui->captureWidget->setEnabled(false);
        ui->actionSettings->setEnabled(false);
        ui->actionIP_Address->setEnabled(true);
    }
}





void Camera::displayCameraError()
{
    if (m_camera->error() != QCamera::NoError)
        QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void Camera::updateCameraDevice(QAction *action)
{
    setCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void Camera::displayViewfinder()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Camera::displayCapturedImage()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Camera::readyForCapture(bool ready)
{
    ui->takeImageButton->setEnabled(ready);
}

void Camera::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    ui->statusbar->showMessage(tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName)));

    m_isCapturingImage = false;
    if (m_applicationExiting)
        close();
}

void Camera::closeEvent(QCloseEvent *event)
{
    if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}

void Camera::updateCameras()
{
    ui->menuDevices->clear();
    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraDevice.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraDevice));
        if (cameraDevice == QMediaDevices::defaultVideoInput())
            videoDeviceAction->setChecked(true);

        ui->menuDevices->addAction(videoDeviceAction);
    }
}
