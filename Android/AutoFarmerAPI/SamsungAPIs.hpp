#ifndef SAMSUNGAPIS_HPP
#define SAMSUNGAPIS_HPP

#include "AutoFarmerDefine.hpp"
#include <QString>
#include <QPoint>

class SamsungAPIs
{
public:
    SamsungAPIs();
    ~SamsungAPIs();

    static DEVICE_INFO getDeviceInfo();
    static bool doClick(int x, int y);
    static bool swipe(QPoint from, QPoint to, int delay);
    static bool inputText(QString text);
    static bool inputKeyEvent(QString keyCode);
    static QString screenCapture(QString fullPath);
    static bool installPackage(QString apkPath);
    static bool isExistPackage(QString packageName);
    static bool uninstallPackage(QString packageName);
    static bool openPackage(QString packageName, QString activity);
    static bool closePackage(QString packageName);
    static bool wipePackage(QString packageName);
};

#endif // SAMSUNGAPIS_HPP
