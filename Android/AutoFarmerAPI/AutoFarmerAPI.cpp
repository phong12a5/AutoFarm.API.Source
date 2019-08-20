/** @file autofarmerapi.cpp
 *  @brief A set of AutoFarmer APIs.
 *  @author Dang Ba Phong (https://github.com/phong12a5)
 *  @bug No know bugs.
 **/

#include "AutoFarmerAPI.hpp"
#include "QAESEncryption.hpp"
#include "RootedDeviceAPIs.hpp"
#include "SamsungAPIs.hpp"
#include "ImageProcessing.hpp"

#define API_SERVER              "https://api.autofarmer.xyz/api/v2/"


AutoFarmerAPI::AutoFarmerAPI()
{
}

AutoFarmerAPI::~AutoFarmerAPI()
{
    // Do nothing
}

QJsonObject AutoFarmerAPI::initEnv(const QString token, const APPNAME_ID appnameId)
{
    bool retVal;
    QString message = "";

    m_token = token;
    m_appnameID = appnameId;

    if(!this->validToken()){
        message = "Token is invalid";
        retVal = false;
    }else if(!this->validAppname()){
        message = "AppName is invalid";
        retVal = false;
    }else{


        /* ----- Check deviceType ----- */
        QString deviceName = RootedDeviceAPIs::getDeviceModel();
        if(deviceName.contains("Samsung")){
            m_deviceType = DEVICE_TYPE_SAMSUNG;
            m_deviceInfo = SamsungAPIs::getDeviceInfo();
            retVal = true;
        }else if(this->isRooted()){
            m_deviceType = DEVICE_TYPE_ROOTED;
            m_deviceInfo = RootedDeviceAPIs::getDeviceInfo();
            retVal = true;
        }else{
            m_deviceType = DEVICE_TYPE_UNSUPPORTED;
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 101000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::getConfig()
{
    QJsonObject inputJson;
    QJsonObject retVal;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{

        QString url = API_SERVER + QString("config?token=%1").arg(this->token());
        QUrl serviceUrl = QUrl(url);
        QNetworkRequest request(serviceUrl);

        inputJson.insert("action", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("config")));
        inputJson.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(inputJson).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;

            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();

            if(jsonObj.isEmpty()){
                // Do nothing
            }else{
                QString data =  jsonObj["data"].toString();

                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), this->getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                retVal = QJsonDocument::fromJson(encryption.removePadding(decodeText)).object();
            }
        }

        reply->deleteLater();
        networkManager->deleteLater();
    }

    QJsonObject object{
        {"Status", status},
        {"Code", 102000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::getApk()
{
    QString url = API_SERVER + QString("config?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject json;
    QByteArray retVal;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{

        json.insert("action", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("getapk")));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(json).toJson();

        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();


            if(jsonObj.isEmpty()){
                // Do nothing
            }else{
                    QString data =  jsonObj["data"].toString();
                    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                    QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                    retVal = encryption.removePadding(decodeText);
            }
        }

        reply->deleteLater();
        networkManager->deleteLater();
    }
    QJsonObject object{
        {"Status", status},
        {"Code", 103000},
        {"Message", message},
        {"ResponseData", QString(retVal)}
    };
    return object;
}

QJsonObject AutoFarmerAPI::downloadApk(QString url)
{

    bool status;
    QString retVal = "";
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{
        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->get(QNetworkRequest(QUrl(url)));
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray content = reply->readAll();
            if(content.isEmpty())
            {
                message = "Responed content is empty";
            }else{
                if(url.split("/").last().contains("com.facebook")){
                    QString fileName = url.split("/").last();
                    QString pathName = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + fileName;;
                    QFile *file = new QFile(pathName);
                    if(file->open(QIODevice::WriteOnly | QIODevice::Text))
                    {
                        file->write(content);
                        file->flush();
                        file->close();
                    }else{
                        status = false;
                        message = "Failed to open file";
                    }
                    delete file;
                    retVal = pathName;
                }
            }
        }
    }


    QJsonObject object{
        {"Status", status},
        {"Code", 100000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::getClone()
{
    QString url = API_SERVER + QString("CloneInfo?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject retVal;
    QJsonObject json;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{
        json.insert("action", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("GetClone")));
        json.insert("country", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("VN")));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(json).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();
            if(jsonObj.isEmpty()){
                retVal = QJsonObject();
            }else{
                    QString data =  jsonObj["data"].toString();
                    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                    QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                    QJsonDocument jdoc = QJsonDocument::fromJson(encryption.removePadding(decodeText));
                    retVal = jdoc.object();
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    }

    QJsonObject object{
        {"Status", status},
        {"Code", 103000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::getCloneInfo()
{
    QString url = API_SERVER + QString("CloneInfo?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject retVal;
    QJsonObject json;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{

        json.insert("action", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("GetCloneInfo")));
        json.insert("country", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("VN")));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(json).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();
            if(jsonObj.isEmpty()){
                retVal = QJsonObject();
            }else{
                    QString data =  jsonObj["data"].toString();
                    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                    QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                    QJsonDocument jdoc = QJsonDocument::fromJson(encryption.removePadding(decodeText));
                    retVal = jdoc.object();
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    }

    QJsonObject object{
        {"Status", status},
        {"Code", 102000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::postClone(QJsonObject data)
{
    QString url = API_SERVER + QString("CloneInfo?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject retVal;
    QJsonObject json;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{
        json.insert("action", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("PostClone")));
        json.insert("country", QTextCodec::codecForMib(106)->toUnicode(getEncodedString("VN")));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("data", QTextCodec::codecForMib(106)->toUnicode(getEncodedJsonDoc(QJsonDocument(data))));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(json).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();
            if(jsonObj.isEmpty()){
                retVal = QJsonObject();
            }else{
                QString data =  jsonObj["data"].toString();
                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                QJsonDocument jdoc = QJsonDocument::fromJson(encryption.removePadding(decodeText));
                retVal = jdoc.object();
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    }

    QJsonObject object{
        {"Status", status},
        {"Code", 102000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::doAction(QString fbId)
{
    QString url = API_SERVER + QString("DoAction?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject retVal;
    QJsonObject json;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{

        json.insert("fbid", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(fbId)));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));

        QByteArray jsonData = QJsonDocument(json).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;
            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();
            if(jsonObj.isEmpty()){
                // Do nothing
            }else{
                QString data =  jsonObj["data"].toString();

                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                QJsonDocument jdoc = QJsonDocument::fromJson(encryption.removePadding(decodeText));
                retVal = jdoc.object();
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    }
    QJsonObject object{
        {"Status", status},
        {"Code", 102000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;
}

QJsonObject AutoFarmerAPI::doResult(QString fbId, QJsonObject data)
{
    QString url = API_SERVER + QString("DoResult?token=%1").arg(this->token());
    QUrl serviceUrl = QUrl(url);
    QNetworkRequest request(serviceUrl);
    QJsonObject retVal;
    QJsonObject json;

    bool status;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
        status = false;
    }else if(!this->validAppname()){
        message = "App Name is invalid";
        status = false;
    }else{

        json.insert("fbid", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(fbId)));
        json.insert("appname", QTextCodec::codecForMib(106)->toUnicode(getEncodedString(this->getAppName())));
        json.insert("info", QTextCodec::codecForMib(106)->toUnicode(getEncodedDeviceInfo()));
        json.insert("data", QTextCodec::codecForMib(106)->toUnicode(getEncodedJsonDoc(QJsonDocument(data))));

        QByteArray jsonData = QJsonDocument(json).toJson();
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(jsonData.size()));

        QEventLoop evenlopp;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager();
        QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)),
                         &evenlopp, SLOT(quit()));
        QNetworkReply* reply = networkManager->post(request, jsonData);
        evenlopp.exec();

        QNetworkReply::NetworkError error = reply->error();
        if(error != QNetworkReply::NoError){
            status = false;
            message = reply->errorString();
        }else{
            status = true;

            QByteArray responseData = reply->readAll();
            QJsonObject jsonObj = QJsonDocument::fromJson(responseData).object();
            if(jsonObj.isEmpty()){
                // Do nothing
            }else{
                QString data =  jsonObj["data"].toString();

                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
                QByteArray decodeText = encryption.decode(QByteArray::fromBase64(data.toUtf8()), getKeyByIMEI().toLocal8Bit(), getIV().toLocal8Bit());
                QJsonDocument jdoc = QJsonDocument::fromJson(encryption.removePadding(decodeText));
                retVal = jdoc.object();
            }
        }
        reply->deleteLater();
        networkManager->deleteLater();
    }
    QJsonObject object{
        {"Status", status},
        {"Code", 102000},
        {"Message", message},
        {"ResponseData", retVal},
    };
    return object;}

QJsonObject AutoFarmerAPI::doClick(int x, int y)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::doClick(x,y);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::doClick(QPoint(x,y));
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 108000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::swipe(QPoint from, QPoint to, int delay)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::swipe(from,to,delay);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::swipe(from,to,delay);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }
    QJsonObject object{
        {"Status", retVal},
        {"Code", 109000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::inputText(const QString text)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::inputText(text);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::inputText(text);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 110000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::inputKeyEvent(const QString keyCode)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::inputKeyEvent(keyCode);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::inputKeyEvent(keyCode);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 111000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::screenCapture(const QString fullPath)
{
    QString retVal = fullPath;
    if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
        retVal = SamsungAPIs::screenCapture(fullPath);
    }else if(deviceType() == DEVICE_TYPE_ROOTED){
        retVal = RootedDeviceAPIs::screenCapture(fullPath);
    }else{
        LOG_INFO << "This device is not supported!";
        retVal = "";
    }
    QJsonObject object{
        {"FilePath", retVal}
    };
    return object;
}

QJsonObject AutoFarmerAPI::installPackage(QString apkPath)
{
    bool retVal = false;
    if(!QFile(apkPath).open(QIODevice::ReadOnly)){
        retVal = false;
    }else{
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::installPackage(apkPath);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::installPackage(apkPath);
        }
    }
    QJsonObject object{
        {"Status", retVal}
    };
    return object;
}

QJsonObject AutoFarmerAPI::isExistPackage(QString packageName)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::isExistPackage(packageName);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::isExistPackage(packageName);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 114000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::uninstallPackage(QString packageName)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::uninstallPackage(packageName);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::uninstallPackage(packageName);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 115000},
        {"Message", message}
    };
    return object;}

QJsonObject AutoFarmerAPI::openPackage(QString packageName, QString activity)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::openPackage(packageName,activity);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::openPackage(packageName,activity);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 116000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::closePackage(QString packageName)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            retVal = SamsungAPIs::closePackage(packageName);
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            retVal = RootedDeviceAPIs::closePackage(packageName);
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 117000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::wipePackage(QStringList packageNames)
{
    bool retVal = false;
    QString message = "";

    if(!this->validToken()){
        message = "Token is invalid";
    }else if(!this->validAppname()){
        message = "App Name is invalid";
    }else {
        if(this->deviceType() == DEVICE_TYPE_SAMSUNG){
            foreach (QString packageName, packageNames) {
                retVal &= SamsungAPIs::wipePackage(packageName);
            }
        }else if(deviceType() == DEVICE_TYPE_ROOTED){
            foreach (QString packageName, packageNames) {
                retVal &= RootedDeviceAPIs::wipePackage(packageName);
            }
        }else{
            message = "This device is not supported!";
            retVal = false;
        }
    }

    QJsonObject object{
        {"Status", retVal},
        {"Code", 118000},
        {"Message", message}
    };
    return object;
}

QJsonObject AutoFarmerAPI::getTextFromImage(QString imagePath)
{
    Q_UNUSED(imagePath);
    return QJsonObject();
}

QJsonObject AutoFarmerAPI::findImageFromImage(QString templatePath, QString imagePath)
{
    QPoint retVal = ImageProcessing::findImageOnImage(templatePath,imagePath,this->threshold(), this->scale());
    QJsonObject object{
        {"X", retVal.x()},
        {"Y", retVal.y()}
    };
    return object;
}


/* --------------------- Private methods --------------------- */

bool AutoFarmerAPI::isRooted()
{
    return true;
}

bool AutoFarmerAPI::validToken()
{
    return this->token() != "";
}

bool AutoFarmerAPI::validAppname()
{
    return (m_appnameID >= APPNAME_ID_FACEBOOK && m_appnameID <= APPNAME_ID_FBLITE);
}

QString AutoFarmerAPI::token() const
{
    return m_token;
}

APPNAME_ID AutoFarmerAPI::appnameID() const
{
    return m_appnameID;
}

DEVICE_TYPE AutoFarmerAPI::deviceType() const
{
    return m_deviceType;
}

DEVICE_INFO AutoFarmerAPI::deviceInfo() const
{
    return m_deviceInfo;
}

QString AutoFarmerAPI::getAppName() const
{
    switch (this->appnameID()) {
    case APPNAME_ID_FACEBOOK:
        return "Facebook";
    case APPNAME_ID_INSTAGRAM:
        return "Instagram";
    case APPNAME_ID_ZALO:
        return "Zalo";
    case APPNAME_ID_PRINTERTEST:
        return "Pinterest";
    case APPNAME_ID_TWITTER:
        return "Twitter";
    case APPNAME_ID_FBLITE:
        return "FBLite";
    default:
        return "";
    }
}

QString AutoFarmerAPI::getKeyByToken() const
{
    return this->token() + "congaubeo@123";
}

QString AutoFarmerAPI::getKeyByIMEI() const
{
    return this->deviceInfo().iMEI + this->deviceInfo().iMEI + this->deviceInfo().iMEI + this->deviceInfo().iMEI;
}

QString AutoFarmerAPI::getIV() const
{
    return "0123456789012345";
}

QByteArray AutoFarmerAPI::getEncodedDeviceInfo() const
{
    QJsonObject deviceInfo;

    deviceInfo["GoogleSF"] =        this->deviceInfo().googleSF;
    deviceInfo["AndroidID"] =       this->deviceInfo().androidID;
    deviceInfo["IMEI"] =            this->deviceInfo().iMEI;
    deviceInfo["IMSI"] =            this->deviceInfo().iMSI;
    deviceInfo["SIMCardSerial"] =   this->deviceInfo().sIMCardSerial;
    deviceInfo["WifiMacAddress"] =  this->deviceInfo().wifiMacAddress;
    deviceInfo["android_verion"] =  this->deviceInfo().android_verion;
    deviceInfo["model"] =           this->deviceInfo().model;
    deviceInfo["isNox"] =           this->deviceInfo().isNox;

    QByteArray deviceInfoData = QJsonDocument(deviceInfo).toJson();

    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);

    return encryption.encode(deviceInfoData, this->getKeyByToken().toLocal8Bit(), this->getIV().toLocal8Bit()).toBase64();
}

QByteArray AutoFarmerAPI::getEncodedString(QString action) const
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    return encryption.encode(action.toLocal8Bit(), this->getKeyByToken().toLocal8Bit(), this->getIV().toLocal8Bit()).toBase64();
}

QByteArray AutoFarmerAPI::getEncodedStringByImei(QString action) const
{
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    return encryption.encode(action.toLocal8Bit(), this->getKeyByIMEI().toLocal8Bit(), this->getIV().toLocal8Bit()).toBase64();
}

QByteArray AutoFarmerAPI::getEncodedJsonDoc(QJsonDocument json) const
{
    QByteArray deviceInfoData = json.toJson();
    QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::CBC);
    return encryption.encode(deviceInfoData, this->getKeyByIMEI().toLocal8Bit(), this->getIV().toLocal8Bit()).toBase64();
}

float AutoFarmerAPI::threshold()
{
    float dpi = this->deviceInfo().dpi;
    if (dpi<160)
        return 0.83;
    else if (dpi<213)
        return 0.83;
    else if (dpi<320)
        return 0.8;
    else if (dpi<480)
        return 0.8;
    else if (dpi<640)
        return 0.8;
    else /*if (dpi>=640)*/
        return 0.8;
}

float AutoFarmerAPI::scale()
{
    float dpi = this->deviceInfo().dpi;
    if (dpi<160)
        return 0.263;
    else if (dpi<213)
        return 0.3575;
    else if (dpi<320)
        return 0.503;
    else if (dpi<480)
        return 0.705;
    else if (dpi<640)
        return 1;
    else /*if (dpi>=640)*/
        return 1.174;
}
