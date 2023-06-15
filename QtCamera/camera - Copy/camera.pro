TEMPLATE = app
TARGET = camera

QT += multimedia multimediawidgets

HEADERS = \
    camera.h \
    graphicsview.h \
    imagesettings.h

SOURCES = \
    graphicsview.cpp \
    main.cpp \
    camera.cpp \
    imagesettings.cpp

FORMS += \
    imagesettings.ui

android|ios {
    FORMS +=
} else {
    FORMS += \
        camera.ui
}
RESOURCES += camera.qrc

target.path = $$[QT_INSTALL_EXAMPLES]/multimedia/camera
INSTALLS += target

QT += widgets
include(../../multimedia/shared/shared.pri)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
OTHER_FILES += android/AndroidManifest.xml

INCLUDEPATH += C:/opencv/build/include
