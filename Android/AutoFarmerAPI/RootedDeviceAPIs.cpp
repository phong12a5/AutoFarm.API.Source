#include "RootedDeviceAPIs.hpp"
#include "AutoFarmerLogger.hpp"

#define DCIM_FOLDER             "/sdcard/DCIM/"

RootedDeviceAPIs::RootedDeviceAPIs(QObject *parent) : QObject(parent)
{

}

bool RootedDeviceAPIs::shellCommand(QString cmd)
{
       int retVal = QProcess::execute("su", QStringList() << "-c" << cmd);
       return retVal >= 0;
}

bool RootedDeviceAPIs::shellCommand(QString cmd, QString &output)
{
    QProcess process;
    process.start("su", QStringList() << "-c" << cmd);
    process.waitForFinished(-1);
    output = process.readAllStandardOutput();
    return process.exitCode() == 0;
}



bool RootedDeviceAPIs::installPackage(QString packagePath)
{
    return RootedDeviceAPIs::shellCommand(QString("pm install %1").arg(packagePath));
}

bool RootedDeviceAPIs::swipe(QPoint point1, QPoint point2, int delay)
{
    return RootedDeviceAPIs::shellCommand(QString("input swipe %1 %2 %3 %4 %5").arg(QString::number(point1.x())).\
                                 arg(point1.y()).\
                                 arg(point2.x()).\
                                 arg(point2.y()).\
                                 arg(delay));
}

bool RootedDeviceAPIs::doClick(QPoint point)
{
    return RootedDeviceAPIs::shellCommand(QString("input tap %1 %2").arg(point.x()).arg(point.y()));
}

bool RootedDeviceAPIs::inputText(QString text)
{
    return RootedDeviceAPIs::shellCommand(QString("input text %1").arg(text));
}

bool RootedDeviceAPIs::isExistPackage(QString packageName)
{
    QString output;
    if(RootedDeviceAPIs::shellCommand(QString("pm list packages | grep %1").arg(packageName),output)){
        if(output.contains(packageName)){
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

bool RootedDeviceAPIs::inputKeyEvent(QString keyCode)
{
    return RootedDeviceAPIs::shellCommand(QString("input keyevent %1").arg(keyCode));
}

bool RootedDeviceAPIs::uninstallPackage(QString packageName)
{
    return RootedDeviceAPIs::shellCommand(QString("pm uninstall %1").arg(packageName));
}

bool RootedDeviceAPIs::openPackage(QString packageName, QString activity)
{
    return RootedDeviceAPIs::shellCommand(QString("am start -n %1/%2").arg(packageName).arg(activity));
}

bool RootedDeviceAPIs::closePackage(QString packageName)
{
    return RootedDeviceAPIs::shellCommand(QString("am force-stop %1").arg(packageName));
}

bool RootedDeviceAPIs::wipePackage(QString packageName)
{
    return RootedDeviceAPIs::shellCommand(QString("pm clear %1").arg(packageName));
}


QString RootedDeviceAPIs::screenCapture(QString fullPath)
{
    bool success = RootedDeviceAPIs::shellCommand(QString("screencap -p %1").arg(fullPath));
    return success? fullPath : "";
}

DEVICE_INFO RootedDeviceAPIs::getDeviceInfo()
{
    DEVICE_INFO _deviceInfo;
    _deviceInfo.googleSF = RootedDeviceAPIs::getGoogleSF();
    _deviceInfo.androidID = RootedDeviceAPIs::getAndroidID();
    _deviceInfo.iMEI = RootedDeviceAPIs::getDeviceIMEI();
    _deviceInfo.iMSI = RootedDeviceAPIs::getDeiceIMSI();
    _deviceInfo.sIMCardSerial = RootedDeviceAPIs::getSimCardSerialNo();
    _deviceInfo.wifiMacAddress = RootedDeviceAPIs::getWifiMacAdd();
    _deviceInfo.android_verion = RootedDeviceAPIs::getAndroidVersion();
    _deviceInfo.model = RootedDeviceAPIs::getDeviceModel();
    _deviceInfo.isNox = RootedDeviceAPIs::getDeviceType();
    _deviceInfo.screen_width = RootedDeviceAPIs::getScreenWidth();
    _deviceInfo.screen_width = RootedDeviceAPIs::getScreenHeight();
    _deviceInfo.screen_width = RootedDeviceAPIs::getScreenDpi();
    return _deviceInfo;
}

bool RootedDeviceAPIs::requestPermission()
{
    //Request requiered permissions at runtime

    QStringList permissions;
    permissions << "android.permission.READ_PHONE_STATE"
                << "android.permission.WRITE_EXTERNAL_STORAGE";

    foreach (QString permission, permissions) {
        auto result = QtAndroid::checkPermission(permission);
        if(result == QtAndroid::PermissionResult::Denied){
            auto resultHash = QtAndroid::requestPermissionsSync(QStringList({permission}));
            if(resultHash[permission] == QtAndroid::PermissionResult::Denied){
                LOG_DEBUG << permission << " : " << false;
                return false;
            }
        }
    }
    return true;
}

QString RootedDeviceAPIs::getDeviceIMEI()
{
    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/content/Context");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);

    jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");
    jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);

    methodId = env->GetMethodID(telephonyManagerClass, "getDeviceId", "()Ljava/lang/String;");
    jstring jstr = (jstring) env->CallObjectMethod(telephonyManager, methodId);

    return QAndroidJniObject::fromLocalRef(jstr).toString();
}

QString RootedDeviceAPIs::getGoogleSF(){

    /*Cursor query = context.getContentResolver().query(sUri, null, null, new String[] { "android_id" }, null); */

    // Get ContentResolver: context.getContentResolver()
    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    QAndroidJniObject contentResolver = qtActivityObj.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");

    // Init String[] { "android_id"}
    QAndroidJniEnvironment env;
    jobjectArray stringArrJava;
    stringArrJava = (jobjectArray)env->NewObjectArray(1,env->FindClass("java/lang/String"),env->NewStringUTF(""));
    env->SetObjectArrayElement(stringArrJava,0,env->NewStringUTF("android_id"));

    // Init sUri
    // String: "content://com.google.android.gsf.gservices"
    jstring uriString = env->NewStringUTF("content://com.google.android.gsf.gservices");

    QAndroidJniObject sUriObj;
    sUriObj = QAndroidJniObject::callStaticObjectMethod("android/net/Uri",\
                                                         "parse",\
                                                         "(Ljava/lang/String;)Landroid/net/Uri;",\
                                                         uriString);

    QAndroidJniObject query = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", sUriObj.object<jobject>(), 0, 0, stringArrJava, 0);

    QString retVal("");

    if(query == NULL){
        LOG_DEBUG << "query is NULL";
    }else{
        jint columns = query.callMethod<jint>("getColumnCount", "()I");
        LOG_DEBUG << static_cast<int>(columns);
        if(static_cast<int>(columns) >= 2){
            jboolean moveFirst = query.callMethod<jboolean>("moveToFirst", "()Z");
            Q_UNUSED(moveFirst);
            QAndroidJniObject string = query.callObjectMethod("getString", "(I)Ljava/lang/String;",0x01);
            retVal = string.toString();
        }
    }

    query.callMethod<void>("close", "()V");

    return retVal;
}


QString RootedDeviceAPIs::getDeiceIMSI()
{
    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/content/Context");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);

    jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");
    jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);

    methodId = env->GetMethodID(telephonyManagerClass, "getSubscriberId", "()Ljava/lang/String;");
    jstring jstr = (jstring) env->CallObjectMethod(telephonyManager, methodId);

    return QAndroidJniObject::fromLocalRef(jstr).toString();
}

QString RootedDeviceAPIs::getDeviceModel()
{
    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/os/Build");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "MODEL", "Ljava/lang/String;");
    jstring model = (jstring) env->GetStaticObjectField(contextClass, fieldId);
    return QAndroidJniObject::fromLocalRef(model).toString();;
}

QString RootedDeviceAPIs::getAndroidVersion()
{
#ifdef PREFER_QT
    return QSysInfo::productVersion();
#else
    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/os/Build$VERSION");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "RELEASE", "Ljava/lang/String;");
    jstring osVersion = (jstring) env->GetStaticObjectField(contextClass, fieldId);
    return QAndroidJniObject::fromLocalRef(osVersion).toString();
#endif
}

QString RootedDeviceAPIs::getAndroidID()
{
    QAndroidJniObject androidIdField = QAndroidJniObject::getStaticObjectField("android/provider/Settings$Secure", "ANDROID_ID", "Ljava/lang/String;");

    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    QAndroidJniObject contentResolver = qtActivityObj.callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");

    QString androidID = QAndroidJniObject::callStaticObjectMethod("android/provider/Settings$Secure","getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;",contentResolver.object<jobject>(),androidIdField.object<jobject>()).toString();

    return androidID;
}

QString RootedDeviceAPIs::getSimCardSerialNo()
{
   /*TelephonyManager telemamanger = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
    String getSimSerialNumber = telemamanger.getSimSerialNumber();*/

    QAndroidJniEnvironment env;
    jclass contextClass = env->FindClass("android/content/Context");
    jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);

    jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");
    jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);

    methodId = env->GetMethodID(telephonyManagerClass, "getSimSerialNumber", "()Ljava/lang/String;");
    jstring jstr = (jstring) env->CallObjectMethod(telephonyManager, methodId);

    return QAndroidJniObject::fromLocalRef(jstr).toString();;
}

QString RootedDeviceAPIs::getSimNumber()
{
    /*TelephonyManager telemamanger = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
     String getSimSerialNumber = telemamanger.getSimSerialNumber();*/

     QAndroidJniEnvironment env;
     jclass contextClass = env->FindClass("android/content/Context");
     jfieldID fieldId = env->GetStaticFieldID(contextClass, "TELEPHONY_SERVICE", "Ljava/lang/String;");
     jstring telephonyManagerType = (jstring) env->GetStaticObjectField(contextClass, fieldId);

     jclass telephonyManagerClass = env->FindClass("android/telephony/TelephonyManager");
     jmethodID methodId = env->GetMethodID(contextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

     QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
     jobject telephonyManager = env->CallObjectMethod(qtActivityObj.object<jobject>(), methodId, telephonyManagerType);

     methodId = env->GetMethodID(telephonyManagerClass, "getLine1Number", "()Ljava/lang/String;");
     jstring jstrNumber = (jstring) env->CallObjectMethod(telephonyManager, methodId);

     return QAndroidJniObject::fromLocalRef(jstrNumber).toString();
}

QString RootedDeviceAPIs::getWifiMacAdd()
{
    /*WifiManager wifiManager = (WifiManager) MyApplication.getInstance().getApplicationContext().getSystemService(Context.WIFI_SERVICE);
      WifiInfo wInfo = wifiManager.getConnectionInfo();
      String macAddress = wInfo.getMacAddress();*/

    QAndroidJniObject androidIdField = QAndroidJniObject::getStaticObjectField("android/content/Context", "WIFI_SERVICE", "Ljava/lang/String;");
    QAndroidJniObject qtActivityObj = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative",  "activity", "()Landroid/app/Activity;");
    QAndroidJniObject wifiManager = qtActivityObj.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",androidIdField.object<jobject>());
    QAndroidJniObject wifiInfo = wifiManager.callObjectMethod("getConnectionInfo","()Landroid/net/wifi/WifiInfo;");
    QString macAddress = wifiInfo.callObjectMethod("getMacAddress","()Ljava/lang/String;").toString();
    return macAddress;
}

QString RootedDeviceAPIs::getDeviceType()
{
    QString retVal = "false";
    if(QFile(QString(DCIM_FOLDER) + "isNox.st").exists()){
        retVal = "true";
    }else{
        // Do nothing
    }
    LOG_DEBUG << retVal;
    return retVal;
}

int RootedDeviceAPIs::getScreenWidth()
{
    QString sizeInfo;
    int screenWidth = 1080;
    RootedDeviceAPIs::shellCommand("wm size",sizeInfo);

    if(sizeInfo.contains("Physical size")){
        screenWidth = (sizeInfo.split(":").length() < 2? screenWidth : sizeInfo.split(":").at(1).simplified().split("x").at(0).simplified().toInt());
    }

    return screenWidth;
}

int RootedDeviceAPIs::getScreenHeight()
{
    QString sizeInfo;
    int screenHeight = 1920;
    RootedDeviceAPIs::shellCommand("wm size",sizeInfo);

    if(sizeInfo.contains("Physical size")){
        screenHeight = (sizeInfo.split(":").length() < 2? screenHeight : sizeInfo.split(":").at(1).simplified().split("x").at(1).simplified().toInt());
    }
    return screenHeight;
}

int RootedDeviceAPIs::getScreenDpi()
{
    QString dpiInfo;
    int dpi = 480;
    RootedDeviceAPIs::shellCommand("wm density",dpiInfo);

    if(dpiInfo.contains("Physical density")){
        dpi = (dpiInfo.split(":").length() < 2? dpi : dpiInfo.split(":").at(1).simplified().toInt());
    }
    return dpi;
}
