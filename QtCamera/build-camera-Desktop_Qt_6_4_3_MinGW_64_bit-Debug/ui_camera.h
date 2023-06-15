/********************************************************************************
** Form generated from reading UI file 'camera.ui'
**
** Created by: Qt User Interface Compiler version 6.4.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERA_H
#define UI_CAMERA_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>
#include "graphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_Camera
{
public:
    QAction *actionExit;
    QAction *actionStartCamera;
    QAction *actionStopCamera;
    QAction *actionSettings;
    QAction *actionIP_Address;
    QWidget *centralwidget;
    QGridLayout *gridLayout_3;
    QStackedWidget *stackedWidget;
    QWidget *viewfinderPage;
    QGridLayout *gridLayout_5;
    GraphicsView *graphicsView;
    QWidget *previewPage;
    QGridLayout *gridLayout_4;
    QLabel *lastImagePreviewLabel;
    QTabWidget *captureWidget;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QPushButton *lowResolution;
    QPushButton *midResolution;
    QPushButton *highResolution;
    QSpacerItem *verticalSpacer_2;
    QPushButton *clearImage;
    QPushButton *takeImageButton;
    QPushButton *uploadButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuDevices;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Camera)
    {
        if (Camera->objectName().isEmpty())
            Camera->setObjectName("Camera");
        Camera->resize(668, 429);
        actionExit = new QAction(Camera);
        actionExit->setObjectName("actionExit");
        actionStartCamera = new QAction(Camera);
        actionStartCamera->setObjectName("actionStartCamera");
        actionStopCamera = new QAction(Camera);
        actionStopCamera->setObjectName("actionStopCamera");
        actionSettings = new QAction(Camera);
        actionSettings->setObjectName("actionSettings");
        actionIP_Address = new QAction(Camera);
        actionIP_Address->setObjectName("actionIP_Address");
        centralwidget = new QWidget(Camera);
        centralwidget->setObjectName("centralwidget");
        gridLayout_3 = new QGridLayout(centralwidget);
        gridLayout_3->setObjectName("gridLayout_3");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        QBrush brush1(QColor(145, 145, 145, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        stackedWidget->setPalette(palette);
        viewfinderPage = new QWidget();
        viewfinderPage->setObjectName("viewfinderPage");
        gridLayout_5 = new QGridLayout(viewfinderPage);
        gridLayout_5->setObjectName("gridLayout_5");
        graphicsView = new GraphicsView(nullptr, viewfinderPage);
          // "this" is a pointer to the current QWidget

        graphicsView->setObjectName("graphicsView");

        gridLayout_5->addWidget(graphicsView, 0, 0, 1, 1);

        stackedWidget->addWidget(viewfinderPage);
        previewPage = new QWidget();
        previewPage->setObjectName("previewPage");
        gridLayout_4 = new QGridLayout(previewPage);
        gridLayout_4->setObjectName("gridLayout_4");
        lastImagePreviewLabel = new QLabel(previewPage);
        lastImagePreviewLabel->setObjectName("lastImagePreviewLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lastImagePreviewLabel->sizePolicy().hasHeightForWidth());
        lastImagePreviewLabel->setSizePolicy(sizePolicy1);
        lastImagePreviewLabel->setFrameShape(QFrame::Box);

        gridLayout_4->addWidget(lastImagePreviewLabel, 1, 0, 1, 1);

        stackedWidget->addWidget(previewPage);

        gridLayout_3->addWidget(stackedWidget, 0, 1, 2, 1);

        captureWidget = new QTabWidget(centralwidget);
        captureWidget->setObjectName("captureWidget");
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        gridLayout = new QGridLayout(tab_2);
        gridLayout->setObjectName("gridLayout");
        lowResolution = new QPushButton(tab_2);
        lowResolution->setObjectName("lowResolution");

        gridLayout->addWidget(lowResolution, 5, 0, 1, 1);

        midResolution = new QPushButton(tab_2);
        midResolution->setObjectName("midResolution");

        gridLayout->addWidget(midResolution, 4, 0, 1, 1);

        highResolution = new QPushButton(tab_2);
        highResolution->setObjectName("highResolution");

        gridLayout->addWidget(highResolution, 3, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 161, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 6, 0, 1, 1);

        clearImage = new QPushButton(tab_2);
        clearImage->setObjectName("clearImage");

        gridLayout->addWidget(clearImage, 2, 0, 1, 1);

        takeImageButton = new QPushButton(tab_2);
        takeImageButton->setObjectName("takeImageButton");
        takeImageButton->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/shutter.svg"), QSize(), QIcon::Normal, QIcon::Off);
        takeImageButton->setIcon(icon);

        gridLayout->addWidget(takeImageButton, 0, 0, 1, 1);

        uploadButton = new QPushButton(tab_2);
        uploadButton->setObjectName("uploadButton");

        gridLayout->addWidget(uploadButton, 1, 0, 1, 1);

        captureWidget->addTab(tab_2, QString());

        gridLayout_3->addWidget(captureWidget, 1, 2, 1, 2);

        Camera->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Camera);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 668, 17));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName("menuFile");
        menuDevices = new QMenu(menubar);
        menuDevices->setObjectName("menuDevices");
        Camera->setMenuBar(menubar);
        statusbar = new QStatusBar(Camera);
        statusbar->setObjectName("statusbar");
        Camera->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuDevices->menuAction());
        menuFile->addAction(actionStartCamera);
        menuFile->addAction(actionStopCamera);
        menuFile->addSeparator();
        menuFile->addAction(actionSettings);
        menuFile->addSeparator();
        menuFile->addAction(actionIP_Address);
        menuFile->addAction(actionExit);

        retranslateUi(Camera);
        QObject::connect(actionExit, &QAction::triggered, Camera, qOverload<>(&QMainWindow::close));
        QObject::connect(takeImageButton, SIGNAL(clicked()), Camera, SLOT(takeImage()));
        QObject::connect(actionSettings, SIGNAL(triggered()), Camera, SLOT(configureCaptureSettings()));
        QObject::connect(actionStartCamera, SIGNAL(triggered()), Camera, SLOT(startCamera()));
        QObject::connect(actionStopCamera, SIGNAL(triggered()), Camera, SLOT(stopCamera()));

        stackedWidget->setCurrentIndex(0);
        captureWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Camera);
    } // setupUi

    void retranslateUi(QMainWindow *Camera)
    {
        Camera->setWindowTitle(QCoreApplication::translate("Camera", "Camera", nullptr));
        actionExit->setText(QCoreApplication::translate("Camera", "Close", nullptr));
        actionStartCamera->setText(QCoreApplication::translate("Camera", "Start Camera", nullptr));
        actionStopCamera->setText(QCoreApplication::translate("Camera", "Stop Camera", nullptr));
        actionSettings->setText(QCoreApplication::translate("Camera", "Change Settings", nullptr));
        actionIP_Address->setText(QCoreApplication::translate("Camera", "IP Address", nullptr));
        lastImagePreviewLabel->setText(QString());
        lowResolution->setText(QCoreApplication::translate("Camera", "Low Resolution", nullptr));
        midResolution->setText(QCoreApplication::translate("Camera", "Mid Resolution", nullptr));
        highResolution->setText(QCoreApplication::translate("Camera", "High Resolution", nullptr));
        clearImage->setText(QCoreApplication::translate("Camera", "Clear Image", nullptr));
        takeImageButton->setText(QCoreApplication::translate("Camera", "Capture Photo", nullptr));
        uploadButton->setText(QCoreApplication::translate("Camera", "Upload Photo", nullptr));
        captureWidget->setTabText(captureWidget->indexOf(tab_2), QCoreApplication::translate("Camera", "Image", nullptr));
        menuFile->setTitle(QCoreApplication::translate("Camera", "File", nullptr));
        menuDevices->setTitle(QCoreApplication::translate("Camera", "Devices", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Camera: public Ui_Camera {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERA_H
