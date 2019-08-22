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


    INCLUDEPATH += $$_PRO_FILE_PWD_/OpenCV-android-sdk/sdk/native/jni/include \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/android\
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/api                   \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/ccmain                \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/ccstruct              \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/ccutil                \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/classify              \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/cmake                 \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/contrib               \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/cube                  \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/cutil                 \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/dict                  \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/doc                   \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/java                  \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/neural_networks       \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/opencl                \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/snap                  \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/tessdata              \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/testing               \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/textord               \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/training              \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/viewer                \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src/wordrec

INCLUDEPATH += $$PWD/tess-two/include/com_googlecode_leptonica_android/src/src \
                $$PWD/tess-two/include/com_googlecode_tesseract_android/src


# install AutoFarmer API lib
    target.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    INSTALLS += target

# Coppy header files to include folder
    target_headers.files  = $$PWD/*.hpp
    target_headers.path   = /AutoFarmer.API/Android/include/
    INSTALLS              += target_headers

# Coppy opencv libs
    opencv_static_lib.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    opencv_static_lib.files = $$PWD/OpenCV-android-sdk/sdk/native/libs/$$ANDROID_TARGET_ARCH/*.a
    INSTALLS += opencv_static_lib

    opencv_shared_lib.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    opencv_shared_lib.files = $$PWD/OpenCV-android-sdk/sdk/native/libs/$$ANDROID_TARGET_ARCH/libopencv_java.so
    INSTALLS += opencv_shared_lib

# Coppy opencv 3rd-party libs

    opencv3rd_static_lib.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    opencv3rd_static_lib.files = $$PWD/OpenCV-android-sdk/sdk/native/3rdparty/libs/$$ANDROID_TARGET_ARCH/*.a
    INSTALLS += opencv3rd_static_lib

# Coppy open-ssl libs
    openssl_shared_lib.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    openssl_shared_lib.files = $$PWD/android_openssl-1.0.x/$$QT_ARCH/*.so
    INSTALLS += openssl_shared_lib

# Coppy tess-two libs
    tess_shared_lib.path = /AutoFarmer.API/Android/libs/$$ANDROID_TARGET_ARCH
    tess_shared_lib.files = $$PWD/tess-two/libs/$$ANDROID_TARGET_ARCH/*.so
    INSTALLS += tess_shared_lib
