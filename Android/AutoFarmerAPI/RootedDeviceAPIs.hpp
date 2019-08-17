#ifndef SHELLOPERATION_H
#define SHELLOPERATION_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <AutoFarmerDefine.hpp>
#include <QPoint>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>

class RootedDeviceAPIs : public QObject
{
    Q_OBJECT
public:
    explicit RootedDeviceAPIs(QObject *parent = nullptr);

public:
    static bool shellCommand(QString cmd);
    static bool shellCommand(QString cmd, QString& output);
    static bool installPackage(QString packagePath);
    static QString screenCapture(QString fullPath);
    static bool swipe(QPoint point1, QPoint point2, int delay);
    static bool findAndClick(QString iconPath, bool repeat = false);
    static bool doClick(QPoint point);
    static bool inputText(QString text);
    static bool isExistPackage(QString packageName);
    static bool inputKeyEvent(QString keyCode);
    static bool uninstallPackage(QString packageName);
    static bool openPackage(QString packageName, QString activity);
    static bool closePackage(QString packageName);
    static bool wipePackage(QString packageName);
    static DEVICE_INFO getDeviceInfo();

    static bool requestPermission();
    static QString getGoogleSF();
    static QString getDeviceIMEI();
    static QString getDeiceIMSI();
    static QString getDeviceModel();
    static QString getAndroidVersion();
    static QString getAndroidID();
    static QString getSimCardSerialNo();
    static QString getSimNumber();
    static QString getWifiMacAdd();
    static QString getDeviceType();
    static int getScreenWidth();
    static int getScreenHeight();
    static int getScreenDpi();
};

#endif // SHELLOPERATION_H
