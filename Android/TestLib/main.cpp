#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <AutoFarmerAPI.hpp>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;
    AutoFarmerAPI apis;
    apis.initEnv("",APPNAME_ID_FACEBOOK);

    return app.exec();
}
