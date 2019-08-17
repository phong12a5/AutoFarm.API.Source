#include "SamsungAPIs.hpp"

SamsungAPIs::SamsungAPIs()
{

}

SamsungAPIs::~SamsungAPIs()
{
    // Do nothing
}

DEVICE_INFO SamsungAPIs::getDeviceInfo()
{
    DEVICE_INFO info;
    return info;
}

bool SamsungAPIs::doClick(int x, int y)
{
    Q_UNUSED(x);
    Q_UNUSED(y);

    return true;
}

QString SamsungAPIs::screenCapture(QString fullPath)
{
    return fullPath;
}

bool SamsungAPIs::installPackage(QString apkPath)
{
    Q_UNUSED(apkPath);
    return true;
}

bool SamsungAPIs::isExistPackage(QString packageName)
{
    Q_UNUSED(packageName);
    return true;
}

bool SamsungAPIs::openPackage(QString packageName, QString activity)
{
    Q_UNUSED(packageName);
    Q_UNUSED(activity);
    return true;
}

bool SamsungAPIs::closePackage(QString packageName)
{
    Q_UNUSED(packageName);
    return true;
}

bool SamsungAPIs::wipePackage(QString packageName)
{
    Q_UNUSED(packageName);
    return true;
}

bool SamsungAPIs::uninstallPackage(QString packageName)
{
    Q_UNUSED(packageName);
    return true;
}

bool SamsungAPIs::swipe(QPoint from, QPoint to, int delay)
{
    Q_UNUSED(from);
    Q_UNUSED(to);
    Q_UNUSED(delay);
    return true;
}

bool SamsungAPIs::inputText(QString text)
{
    Q_UNUSED(text);
    return true;
}

bool SamsungAPIs::inputKeyEvent(QString keyCode)
{
    Q_UNUSED(keyCode);
    return true;
}
