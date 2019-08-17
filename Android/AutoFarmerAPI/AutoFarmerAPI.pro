#-------------------------------------------------
#
# Project created by Phong.Dang 2019-08-16T22:13:59
#
#-------------------------------------------------

QT       += testlib network androidextras
QT       -= gui

TARGET = AutoFarmerAPI
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AutoFarmerAPI.cpp \
    QAESEncryption.cpp \
    RootedDeviceAPIs.cpp \
    SamsungAPIs.cpp \
    ImageProcessing.cpp

HEADERS += \
    QAESEncryption.hpp \
    RootedDeviceAPIs.hpp \
    AutoFarmerLogger.hpp \
    AutoFarmerDefine.hpp \
    AutoFarmerAPI.hpp \
    SamsungAPIs.hpp \
    ImageProcessing.hpp


    INCLUDEPATH += "$$_PRO_FILE_PWD_/OpenCV-android-sdk/sdk/native/jni/include"
    LIBS += \
        -L"$$_PRO_FILE_PWD_/OpenCV-android-sdk/sdk/native/3rdparty/libs/armeabi-v7a"\
        -L"$$_PRO_FILE_PWD_/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a"\
        -llibtiff\
        -llibjpeg\
        -llibjasper\
        -llibpng\
        -lIlmImf\
        -ltbb\
        -lopencv_core\
        -lopencv_androidcamera\
        -lopencv_flann\
        -lopencv_imgproc\
        -lopencv_highgui\
        -lopencv_features2d\
        -lopencv_calib3d\
        -lopencv_ml\
        -lopencv_objdetect\
        -lopencv_video\
        -lopencv_contrib\
        -lopencv_photo\
        -lopencv_java\
        -lopencv_legacy\
        -lopencv_ocl\
        -lopencv_stitching\
        -lopencv_superres\
        -lopencv_ts\
        -lopencv_videostab

LIBS += -L$$PWD/OpenCV-android-sdk/sdk/native/libs/armeabi-v7a/libopencv_core.a


unix {
    target.path = /libs/$$ANDROID_TARGET_ARCH
    INSTALLS += target
}
