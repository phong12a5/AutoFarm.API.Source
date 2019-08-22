#ifndef AUTOFARMERAPI_H
#define AUTOFARMERAPI_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTextCodec>
#include "AutoFarmerDefine.hpp"
#include "AutoFarmerLogger.hpp"

class AutoFarmerAPI
{

public:
    AutoFarmerAPI();
    ~AutoFarmerAPI();

    /** @brief Set Token and AppID.
     *
     *  If any argument is invalid, the lib has no effect.
     *
     *  @param token The token is provided for customer.
     *  @param appnameId The App ID of Appliction which developer wanna develop.
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 101000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject initEnv(const QString token, const APPNAME_ID appnameId);

    /** @brief Get confirgurations from Server.
     *  @param No input.
     *  @return QJsonObject.
     */
    QJsonObject getConfig();

    /** @brief Get list apks from Server.
     *  @param No input.
     *  @return QObject object {
     *      {
     *      “appname”:”Facebook”
     *      "package": "com.facebook.liup",
     *      "apk": "https://admin.autofarmer.xyz/apks/com.facebook.liup-134.0.0.9.90.apk"
     *      },
     *      {
     *      “appname”:”Facebook”
     *      "package": "com.facebook.livt",
     *      "apk": "https://admin.autofarmer.xyz/apks/com.facebook.livt-134.0.0.9.90.apk"
     *      },
     *      }
     */
    QJsonObject getApk();

    /** @brief Download APK from Server.
     *  @param url: the link of apk.
     *  @return QString: Path to saved apk
     */
    QJsonObject downloadApk(QString url);

    /** @brief Get clone from Server.
     *  @param No input.
     *  @return QJsonObject
     */
    QJsonObject getClone();

    /** @brief Get the information of clone from Server.
     *  @param No input.
     *  @return QJsonObject
     */
    QJsonObject getCloneInfo();

    /** @brief Post clone to Server.
     *  @param data: Info of clone which is needed to post.
     *  @return QJsonObject
     */
    QJsonObject postClone(QJsonObject data);

    /** @brief Get actions for specified FB id.
     *  @param fbId: UID of specified clone.
     *  @return QJsonObject
     */
    QJsonObject doAction(QString fbId);

    /** @brief Update result to server when an anction is done
     *  @param fbId: UID of specified clone.
     *  @param data: action was done.
     *  @return QJsonObject
     */
    QJsonObject doResult(QString fbId, QJsonObject data);

    /** @brief Touch any position on the screen.
     *  @param x,y The coordinates which you want to touch.
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 108000},
     *                              {"Message", message}
     *                            };.
     */


    QJsonObject doClick(int x, int y);

    /** @brief Swipe from A point to B point around delay miliseconds.
     *  @param from: The point which you start wiping.
     *  @param to: The point which you end wiping.
     *  @param delay: The time is from starting to ending.
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 109000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject swipe(QPoint from, QPoint to, int delay);

    /** @brief Input Text.
     *  @param text: The string which you want to enter.
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 110000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject inputText(const QString text);

    /** @brief Input KEYEVENT.
     *  @param keyCode: The KEYEVENT which you want to enter.
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 111000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject inputKeyEvent(const QString keyCode);

    /** @brief Capture screen.
     *  @param fullPath: The absolutetly path of image file
     *  @return QJsonObject object{
     *                              {"FilePath", retVal}
     *                            };
     */
    QJsonObject screenCapture(const QString fullPath);

    /** @brief Intall apk file.
     *  @param apkPath: The absolutetly path of apk file
     *  @return QJsonObject object  {
     *                                  {"Status", retVal}
     *                              };.
     */
    QJsonObject installPackage(QString apkPath);

    /** @brief Check whether packageName exist or not.
     *  @param packageName: Name of package that is needed to check
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 114000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject isExistPackage(QString packageName);

    /** @brief Uninstall specified package.
     *  @param packageName: Name of package that is needed to uninstall
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 115000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject uninstallPackage(QString packageName);

    /** @brief Open specified package.
     *  @param packageName: Name of package that is needed to open
     *  @param activity: Name of activity that is needed to open
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 116000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject openPackage(QString packageName, QString activity);

    /** @brief Close specified package.
     *  @param packageName: Name of package that is needed to close
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 117000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject closePackage(QString packageName);

    /** @brief Wipe data of the specified package.
     *  @param packageNames: Name of packages that are needed to wipe
     *  @return QJsonObject object{
     *                              {"Status", retVal},
     *                              {"Code", 118000},
     *                              {"Message", message}
     *                            };.
     */
    QJsonObject wipePackage(QStringList packageNames);

    /** @brief Get all texts which appear on the image.
     *  @param imagePath: The absolutetly path of image file
     *  @note: THIS METHOD HAS NOT IMPELEMTED YET.
     */
    QJsonObject getTextFromImage(QString imagePath, QString lang = "eng");

    /** @brief Find a template on the image .
     *  @param templatePath: The absolutetly path of template file
     *  @param imagePath: The absolutetly path of image file
     *  @return QJsonObject object{
     *                              {"X", 0},
     *                              {"Y", 0}
     *                            };.
     */
    QJsonObject findImageFromImage(QString templatePath, QString imagePath);

private:
    QString token() const;
    APPNAME_ID appnameID() const;
    DEVICE_TYPE deviceType() const;
    DEVICE_INFO deviceInfo() const;
    QString getAppName() const;
    QString getKeyByToken() const;
    QString getKeyByIMEI() const;
    QString getIV() const;
    QByteArray getEncodedDeviceInfo() const;
    QByteArray getEncodedString(QString action) const;
    QByteArray getEncodedStringByImei(QString action) const;
    QByteArray getEncodedJsonDoc(QJsonDocument json) const;
    float threshold();
    float scale();
    bool isRooted();
    bool validToken();
    bool validAppname();

private:
    APPNAME_ID m_appnameID;
    QString m_token;
    DEVICE_INFO m_deviceInfo;
    DEVICE_TYPE m_deviceType;
};

#endif // AUTOFARMERAPI_H
