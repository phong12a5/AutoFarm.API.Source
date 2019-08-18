#ifndef AUTOFARMERDEFINE_H
#define AUTOFARMERDEFINE_H

#include <QString>

enum APPNAME_ID: int {
    APPNAME_ID_FACEBOOK = 0,
    APPNAME_ID_INSTAGRAM,
    APPNAME_ID_ZALO,
    APPNAME_ID_PRINTERTEST,
    APPNAME_ID_TWITTER,
    APPNAME_ID_FBLITE
};

enum DEVICE_TYPE: int {
    DEVICE_TYPE_SAMSUNG = 0,
    DEVICE_TYPE_ROOTED,
    DEVICE_TYPE_UNSUPPORTED
};

typedef struct device_info_struct {
    QString googleSF;
    QString androidID;
    QString iMEI;
    QString iMSI;
    QString sIMCardSerial;
    QString wifiMacAddress;
    QString android_verion;
    QString model;
    QString isNox;
    int screen_width;
    int screen_height;
    int dpi;

    device_info_struct(){
        googleSF = "31b75eb8d";
        androidID = "391f3";
        iMEI = "35697865";
        iMSI = "456789987776";
        sIMCardSerial = "89876656";
        wifiMacAddress = "24:26:38";
        android_verion = "5.0.2";
        model = "Sony Z3";
        isNox = "false";
        screen_width = 1920;
        screen_height = 1080;
        dpi = 480;
    }
} DEVICE_INFO;

#endif // AUTOFARMERDEFINE_H
